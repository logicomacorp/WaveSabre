#ifndef __WAVESABREPLAYERLIB_APLAYRENDERTHREAD_H__
#define __WAVESABREPLAYERLIB_APLAYRENDERTHREAD_H__

#include "SongRenderer.h"
#include "CriticalSection.h"
#include "IRenderThread.h"

#include <stddef.h>

#include <sys/types.h>
#include <unistd.h>

#if HAVE_PTHREADS
#include <pthread.h>
#endif

namespace WaveSabrePlayerLib
{
	class AplayRenderThread : public IRenderThread
	{
	public:
		// 'pthread': if true, spawn a separate thread using pthreads and do
		// the poll/select/send-to-alsa loop there, if not, you have to
		// periodically call DoForegroundWork, but, pthread won't be needed
		AplayRenderThread(RenderCallback callback, void *callbackData,
				int sampleRate, int bufferSizeMs = 1000, bool pthread = false);
		virtual ~AplayRenderThread();

		virtual int GetPlayPositionMs();

		virtual void DoForegroundWork();

	private:
		static void  AplayProc(int readend, int rate);
		static void* AplayWriterProc(void* ud);

		void GetBufferTick(bool block = false);

		RenderCallback callback;
		void *callbackData;
		int sampleRate;
		int bufferSizeMs;
		int bufferSizeBytes;
		int bytesWritten;
		SongRenderer::Sample *sampleBuffer;
		const SongRenderer::Sample *bufferToWrite;
		size_t bufferBytesLeft;
		size_t writeBytesMax;

		int aupipe;
		pid_t aplay;
#if HAVE_PTHREAD
		pthread_t writer;
		bool writerStarted;
#endif
	};
}

#endif
