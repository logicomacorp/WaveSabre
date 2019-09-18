#ifndef __WAVESABREPLAYERLIB_DIRECTSOUNDRENDERTHREAD_H__
#define __WAVESABREPLAYERLIB_DIRECTSOUNDRENDERTHREAD_H__

#include "SongRenderer.h"
#include "CriticalSection.h"

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

		int GetPlayPositionMs();

	private:
		static DWORD WINAPI threadProc(LPVOID lpParameter);

		RenderCallback callback;
		void *callbackData;
		int sampleRate;
		int bufferSizeMs;
		int bufferSizeBytes;

		HANDLE thread;
		CriticalSection criticalSection;
		CriticalSection playPositionCriticalSection;
		bool shutdown;

		int oldPlayCursorPos;
		long long bytesRendered;
		LPDIRECTSOUNDBUFFER buffer;
	};
}

#endif
