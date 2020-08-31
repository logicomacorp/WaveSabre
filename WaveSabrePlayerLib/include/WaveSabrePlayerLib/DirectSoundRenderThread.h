#ifndef __WAVESABREPLAYERLIB_DIRECTSOUNDRENDERTHREAD_H__
#define __WAVESABREPLAYERLIB_DIRECTSOUNDRENDERTHREAD_H__

#include "SongRenderer.h"
#include "CriticalSection.h"
#include "IRenderThread.h"

#include <Windows.h>
#include <dsound.h>

namespace WaveSabrePlayerLib
{
	class DirectSoundRenderThread : public IRenderThread
	{
	public:
		DirectSoundRenderThread(RenderCallback callback, void *callbackData, int sampleRate, int bufferSizeMs = 1000);
		virtual ~DirectSoundRenderThread();

		virtual int GetPlayPositionMs();

		virtual void DoForegroundWork();

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
