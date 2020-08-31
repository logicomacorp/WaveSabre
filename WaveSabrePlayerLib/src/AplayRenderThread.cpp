#include <WaveSabrePlayerLib/AplayRenderThread.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <signal.h>

#if HAVE_PTHREAD
#include <pthread.h>
#endif

static unsigned is_le(void)
{
	const union { unsigned u; unsigned char c[4]; } one = { 1 };
	return one.c[0];
}

namespace WaveSabrePlayerLib
{
	AplayRenderThread::AplayRenderThread(RenderCallback callback,
			void *callbackData, int sampleRate, int bufferSizeMs,
			bool pthread)
		: callback(callback)
		, callbackData(callbackData)
		, sampleRate(sampleRate)
		, bufferSizeMs(bufferSizeMs)
		, samplesWritten(0)
		, writeBytesMax(PIPE_BUF)
#if HAVE_PTHREAD
		, writerStarted(false)
#endif
	{
#if !defined(HAVE_PTHREAD) || !HAVE_PTHREAD
		pthread = false;
#endif

		bufferSizeBytes = sampleRate * SongRenderer::BlockAlign * bufferSizeMs / 1000;
		bufferToWrite = (SongRenderer::Sample*)malloc(bufferSizeBytes);

		int fdpair[2];
		int rv = pipe(fdpair);
		assert(rv == 0 && "Couldn't set up a pipe for aplay.");

		int readend = fdpair[0], writeend = fdpair[1];

		this->aupipe = writeend;

		if (!pthread) {
			// set the write end of the pipe to nonblocking, so we can limit the
			// number of bytes to write to the pipe to the pipe size
			int flags = fcntl(writeend, F_GETFL);
			assert(flags != -1 && "Couldn't get pipe write-end fd flags.");
			flags = fcntl(writeend, F_SETFL, flags|O_NONBLOCK);
			assert(flags == 0 && "Couldn't set pipe write-end fd NONBLOCK flag.");
		}

		pid_t child = fork(); // TODO: use vfork, needs shuffling stuff around
		assert(child >= 0 && "Couldn't fork.");

		if (child == 0) {
			// child process
			close(writeend); // don't need this
			AplayProc(readend, sampleRate);
		} else {
			// parent process
			close(readend); // don't need this
			this->aplay = child;
		}

#if HAVE_PTHREAD
		if (pthread) {
			rv = pthread_create(&writer, NULL, AplayWriterProc, this);
			assert(rv == 0 && "Couldn't create background writer thread");

			writerStarted = true;
		}
#endif
	}

	AplayRenderThread::~AplayRenderThread()
	{
		struct timespec ts;
		ts.tv_sec = 1;
		ts.tv_nsec = 0;

#if HAVE_PTHREAD
		if (writerStarted) {
			pthread_cancel(writer);
			if (pthread_timedjoin_np(writer, NULL, &ts) != 0) {
				pthread_kill(writer, SIGTERM);
				if (pthread_timedjoin_np(writer, NULL, &ts) != 0) {
					pthread_kill(writer, SIGKILL);
					pthread_join(writer, NULL);
				}
			}
			writerStarted = false;
		}
#endif

		if (this->aplay > 1) {
			int stat;
			kill(this->aplay, SIGTERM);

			// wait a second for aplay to ack the SIGTERM
			sigset_t sset;
			sigemptyset(&sset);
			sigaddset(&sset, SIGCHLD);
			if (sigtimedwait(&sset, NULL, &ts) < 0) {
				// not acked? then you shall die.
				kill(this->aplay, SIGKILL);
			}

			waitpid(this->aplay, &stat, 0);

			this->aupipe  = -1;
			this->aplay = -1;
		}
	}

	void AplayRenderThread::DoForegroundWork()
	{
		GetBufferTick(false);
	}

	int AplayRenderThread::GetPlayPositionMs()
	{
		return (samplesWritten * 1000) / sampleRate;
	}

	void AplayRenderThread::AplayProc(int readend, int rate)
	{
		// stdin becomes readend (properly sets up the pipe)
		// doing this instead of using /proc/$pid/fd is more portable
		dup2(readend, STDIN_FILENO);

		// format aplay args
		char* progpath = "/usr/bin/aplay";
		char* arg0 = progpath;
		char arg1[strlen("-r48000")+1];
		char arg2[strlen("-fS16_LE")+1];

		snprintf(arg1, sizeof(arg1), "-r%d", rate);
		snprintf(arg2, sizeof(arg2), "-fS%d_%sE",
				(int)sizeof(SongRenderer::Sample)*CHAR_BIT,
				(is_le() ? "L" : "B"));

		// now we're ready to start aplay!
		char *const args[] = {arg0, arg1, arg2, NULL};
		int rv = execve("/usr/bin/aplay", args, environ);
		assert(rv >= 0 && "Failed to run aplay!");
	}

	void AplayRenderThread::GetBufferTick(bool block)
	{
		while (true) {
			struct pollfd pfd;
			pfd.fd = this->aupipe;
			pfd.events = POLLOUT;
			pfd.revents = 0;

			int res = poll(&pfd, 1, (block ? (-1) : 0));
			assert(res >= 0 && "poll(2) failed.");

			// can't write? let's try again later then
			if (!res || (pfd.revents & POLLERR))
				break;

			assert(!(pfd.revents & POLLHUP) && "aplay suddenly stopped?");

			if (bufferBytesLeft <= 0) {
				callback(sampleBuffer, bufferSizeBytes/sizeof(SongRenderer::Sample), callbackData);
				bufferToWrite = sampleBuffer;
				bufferBytesLeft = bufferSizeBytes;
			}

			size_t toWrite = bufferBytesLeft;
			if (toWrite > writeBytesMax)
				toWrite = writeBytesMax;

			bool haderr = false;
			while (true) {
				ssize_t ret = write(aupipe, bufferToWrite, toWrite);

				if (ret < 0) {
					haderr = true;
					int err = errno;
					if (err == EAGAIN || err == EWOULDBLOCK) {
						toWrite >>= 1; // AIMD
						continue;
					} else {
						assert(0 && "Couldn't write to the aplay pipe.");
					}
				} else if (toWrite > (size_t)ret) {
					haderr = true; // inhibit AI
					toWrite >>= 1; // MD
				}

				bufferBytesLeft -= (size_t)ret;
				bufferToWrite   += ret / sizeof(SongRenderer::Sample);
				samplesWritten  += ret / sizeof(SongRenderer::Sample);

				break;
			}

			writeBytesMax = toWrite;
			if (!haderr) {
				// AIMD
				writeBytesMax += 0x100;
				if (writeBytesMax > PIPE_BUF)
					writeBytesMax = PIPE_BUF;
			}
		}
	}

#if HAVE_PTHREAD
	void* AplayRenderThread::AplayWriterProc(void* ud)
	{
		auto self = (AplayRenderThread*)ud;

		while (self->aupipe >= 0) {
			self->GetBufferTick(true);
			//pthread_yield(); // poll and write are cancellation points anyway
		}

		self->writerStarted = false;
		return NULL;
	}
#endif
}
