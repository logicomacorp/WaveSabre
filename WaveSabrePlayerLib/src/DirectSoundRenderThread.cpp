#include <WaveSabrePlayerLib/DirectSoundRenderThread.h>

namespace WaveSabrePlayerLib
{
	DirectSoundRenderThread::DirectSoundRenderThread(RenderCallback callback, void *callbackData, int sampleRate, int bufferSizeMs)
		: callback(callback)
		, callbackData(callbackData)
		, sampleRate(sampleRate)
		, bufferSizeMs(bufferSizeMs)
	{
		shutdown = false;

		InitializeCriticalSection(&criticalSection);

		thread = CreateThread(0, 0, threadProc, (LPVOID)this, 0, 0);
		SetThreadPriority(thread, THREAD_PRIORITY_HIGHEST);
	}

	DirectSoundRenderThread::~DirectSoundRenderThread()
	{
		// We don't need to enter/leave a critical section here since we're the only writer at this point.
		shutdown = true;

		WaitForSingleObject(thread, INFINITE);
		DeleteCriticalSection(&criticalSection);
	}

	DWORD WINAPI DirectSoundRenderThread::threadProc(LPVOID lpParameter)
	{
		auto renderThread = (DirectSoundRenderThread *)lpParameter;

		LPDIRECTSOUND8 device;
		DirectSoundCreate8(0, &device, 0);
		device->SetCooperativeLevel(GetForegroundWindow(), DSSCL_NORMAL);

		int sampleRate = renderThread->sampleRate;
		int bitsPerSample = 16;
		int blockAlign = SongRenderer::NumChannels * bitsPerSample / 8;
		int bytesPerSec = sampleRate * blockAlign;

		int bufferSizeBytes = sampleRate * blockAlign * renderThread->bufferSizeMs / 1000;

		LPDIRECTSOUNDBUFFER buffer;
		WAVEFORMATEX bufferFormat =
		{
			WAVE_FORMAT_PCM,
			SongRenderer::NumChannels,
			sampleRate,
			bytesPerSec,
			blockAlign,
			bitsPerSample,
			0
		};
		DSBUFFERDESC bufferDesc =
		{
			sizeof(DSBUFFERDESC),
			DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2,
			bufferSizeBytes,
			0,
			&bufferFormat,
			GUID_NULL
		};
		device->CreateSoundBuffer(&bufferDesc, &buffer, 0);

		int oldPlayCursorPos = 0;
		buffer->Play(0, 0, DSBPLAY_LOOPING);

		// We don't need to enter/leave a critical section here since there's only one writer for this value.
		while (!renderThread->shutdown)
		{
			EnterCriticalSection(&renderThread->criticalSection);

			int playCursorPos;
			buffer->GetCurrentPosition((LPDWORD)&playCursorPos, 0);
			int bytesToRender = playCursorPos - oldPlayCursorPos;
			if (bytesToRender)
			{
				if (bytesToRender < 0) bytesToRender += bufferSizeBytes;
				if (bytesToRender >= 1000)
				{
					SongRenderer::Sample *p1, *p2;
					int b1, b2;
					buffer->Lock(oldPlayCursorPos, bytesToRender, (LPVOID *)&p1, (LPDWORD)&b1, (LPVOID *)&p2, (LPDWORD)&b2, 0);
					renderThread->callback(p1, b1 / sizeof(SongRenderer::Sample), renderThread->callbackData);
					if (b2) renderThread->callback(p2, b2 / sizeof(SongRenderer::Sample), renderThread->callbackData);
					buffer->Unlock(p1, b1, p2, b2);
					oldPlayCursorPos = playCursorPos;
				}
			}

			LeaveCriticalSection(&renderThread->criticalSection);

			Sleep(3);
		}

		buffer->Stop();
		buffer->Release();
		device->Release();

		return 0;
	}
}