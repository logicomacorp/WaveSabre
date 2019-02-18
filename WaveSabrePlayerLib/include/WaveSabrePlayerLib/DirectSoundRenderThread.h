#ifndef __WAVESABREPLAYERLIB_DIRECTSOUNDRENDERTHREAD_H__
#define __WAVESABREPLAYERLIB_DIRECTSOUNDRENDERTHREAD_H__

#include "SongRenderer.h"

#include <Windows.h>
#include <dsound.h>

namespace WaveSabrePlayerLib
{
	class DirectSoundRenderThread
	{
	public:
		typedef void (*RenderCallback)(SongRenderer::Sample *buffer, int numSamples, void *data);

		DirectSoundRenderThread(RenderCallback callback, void *callbackData, int sampleRate, int bufferSizeMs = 1000);
		~DirectSoundRenderThread();

	private:
		static DWORD WINAPI threadProc(LPVOID lpParameter);

		RenderCallback callback;
		void *callbackData;
		int sampleRate;
		int bufferSizeMs;

		HANDLE thread;
		CRITICAL_SECTION criticalSection;
		bool shutdown;
	};
}

#endif
