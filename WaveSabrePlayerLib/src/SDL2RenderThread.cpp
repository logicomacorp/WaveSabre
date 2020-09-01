#include <WaveSabrePlayerLib/SDL2RenderThread.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

namespace WaveSabrePlayerLib
{
	SDL2RenderThread::SDL2RenderThread(RenderCallback callback,
			void *callbackData, int sampleRate, int bufferSizeMs, bool pthread)
		: callback(callback)
		, callbackData(callbackData)
		, sampleRate(sampleRate)
		, bufferSizeMs(bufferSizeMs)
		, bytesWritten(0)
		, bufferBytesLeft(0)
#if HAVE_PTHREAD
		, writerStarted(false)
#endif
	{
#if !defined(HAVE_PTHREAD) || !HAVE_PTHREAD
		pthread = false;
#endif

		bufferSizeBytes = sampleRate * SongRenderer::BlockAlign * bufferSizeMs / 1000;
		sampleBuffer = (SongRenderer::Sample*)malloc(bufferSizeBytes);
#if HAVE_PTHREAD
		backupBuffer = (SongRenderer::Sample*)malloc(bufferSizeBytes);
#endif

		int rv = SDL_Init(SDL_INIT_AUDIO);
		assert(rv >= 0 && "Can't init SDL2 audio");

		spec.freq = sampleRate;
		spec.format = AUDIO_S16SYS;
		spec.channels = 2;
		spec.samples = bufferSizeBytes / (sizeof(SongRenderer::Sample) * spec.channels);
		spec.callback = SDL2Callback;
		spec.userdata = this;

		dev = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
#ifndef NDEBUG
		if (dev <= 0) {
			printf("SDL2 error: %s\n", SDL_GetError());
		}
#endif
		assert(rv > 0 && "Can't open SDL2 audio");

		// pre-buffer stuff a bit, SDL2 tends to be finnicky
		callback(sampleBuffer, bufferSizeBytes/sizeof(SongRenderer::Sample), callbackData);
		bufferBytesLeft = bufferSizeBytes;
		bufferToWrite = sampleBuffer;

#if HAVE_PTHREAD
		if (pthread) {
			sem_init(&needbuf_sem, 0, 0);

			rv = pthread_create(&writer, NULL, SDL2WriterProc, this);
			assert(rv == 0 && "Couldn't create background writer thread");

			sem_post(&needbuf_sem); // start filling in the backup buffer..

			writerStarted = true;
		}
#endif

		SDL_PauseAudioDevice(dev, 0);
	}

	SDL2RenderThread::~SDL2RenderThread()
	{
		SDL_CloseAudioDevice(dev);

#if HAVE_PTHREAD
		if (writerStarted) {
			pthread_cancel(writer);
			pthread_join(writer, NULL);
			sem_destroy(&needbuf_sem);
			writerStarted = false;
		}
#endif

		free(sampleBuffer);
#if HAVE_PTHREAD
		free(backupBuffer);
#endif
	}

	void SDL2RenderThread::DoForegroundWork()
	{
		// nop.
	}

	int SDL2RenderThread::GetPlayPositionMs()
	{
		return (int)(bytesWritten / SongRenderer::BlockAlign * 1000 / sampleRate);
	}

#if HAVE_PTHREAD
	void* SDL2RenderThread::SDL2WriterProc(void* ud)
	{
		auto self = (SDL2RenderThread*)ud;

		while (true) {
			sem_wait(&self->needbuf_sem);

			self->callback(self->backupBuffer,
				self->bufferSizeBytes/sizeof(SongRenderer::Sample),
				self->callbackData);
		}

		self->writerStarted = false;
		return NULL;
	}
#endif

	void SDL2RenderThread::SDL2Callback2(uint8_t* buf, int len)
	{
		if (bufferBytesLeft <= 0) {
#if HAVE_PTHREAD
			if (writerStarted) {
				// TODO: pingpong backupBuffer<->sampleBuffer?
				//       (needs an atomic exchange)
				SDL_memcpy(sampleBuffer, backupBuffer, bufferSizeBytes);
				sem_post(&needbuf_sem);
			} else {
#else
				callback(sampleBuffer, bufferSizeBytes/sizeof(SongRenderer::Sample), callbackData);
#endif
			}

			bufferToWrite = sampleBuffer;
			bufferBytesLeft = bufferSizeBytes;
		}

		if (len == 0)
			return;

		if (len > bufferBytesLeft)
			len = bufferBytesLeft;

		SDL_memcpy(buf, bufferToWrite, len);

		bufferToWrite   += len / sizeof(SongRenderer::Sample);
		bufferBytesLeft -= len;
		bytesWritten    += len;
	}

	void SDL2RenderThread::SDL2Callback(void* userdata, uint8_t* buf, int len)
	{
		auto self = (SDL2RenderThread*)userdata;
		self->SDL2Callback2(buf, len);
	}
}
