#ifndef __WAVESABREPLAYERLIB_SDL2RENDERTHREAD_H__
#define __WAVESABREPLAYERLIB_SDL2RENDERTHREAD_H__

#include "SongRenderer.h"
#include "CriticalSection.h"
#include "IRenderThread.h"

#include <stddef.h>
#include <stdint.h>

#include <SDL2/SDL.h>

#if HAVE_PTHREAD
#include <pthread.h>
#include <semaphore.h>
#endif

namespace WaveSabrePlayerLib
{
	class SDL2RenderThread : public IRenderThread
	{
	public:
		SDL2RenderThread(RenderCallback callback, void *callbackData,
				int sampleRate, int bufferSizeMs = 1000, bool pthread = false);
		virtual ~SDL2RenderThread();

		virtual int GetPlayPositionMs();

		virtual void DoForegroundWork();

	private:
#if HAVE_PTHREAD
		static void* SDL2WriterProc(void* ud);
#endif

		void SDL2Callback2(uint8_t* buf, int len);
		static void SDL2Callback(void* userdata, uint8_t* buf, int len);

		RenderCallback callback;
		void *callbackData;
		int sampleRate;
		int bufferSizeMs;
		int bufferSizeBytes;
		int bytesWritten;
		SongRenderer::Sample *sampleBuffer;
#if HAVE_PTHREAD
		SongRenderer::Sample *backupBuffer;
#endif
		const SongRenderer::Sample *bufferToWrite;
		int bufferBytesLeft;

		SDL_AudioSpec spec;
		SDL_AudioDeviceID dev;
#if HAVE_PTHREAD
		sem_t needbuf_sem;
		pthread_t writer;
		bool writerStarted;
#endif
	};
}

#endif
