#include <WaveSabrePlayerLib/DirectSoundRenderThread.h>

namespace WaveSabrePlayerLib
{
	DirectSoundRenderThread::DirectSoundRenderThread(RenderCallback callback, void *callbackData, int sampleRate, int bufferSizeMs)
		: callback(callback)
		, callbackData(callbackData)
		, sampleRate(sampleRate)
		, bufferSizeMs(bufferSizeMs)
		, shutdown(false)
		, oldPlayCursorPos(0)
		, bytesRendered(0)
		, buffer(nullptr)
	{
		bufferSizeBytes = sampleRate * SongRenderer::BlockAlign * bufferSizeMs / 1000;

		thread = CreateThread(0, 0, threadProc, (LPVOID)this, 0, 0);
		SetThreadPriority(thread, THREAD_PRIORITY_HIGHEST);
	}

	DirectSoundRenderThread::~DirectSoundRenderThread()
	{
		// We don't need to enter/leave a critical section here since we're the only writer at this point.
		shutdown = true;

		WaitForSingleObject(thread, INFINITE);
	}

	double DirectSoundRenderThread::GetPlayPositionMs()
	{
		if (!buffer)
			return 0;

		int playCursorPos;
		buffer->GetCurrentPosition((LPDWORD)&playCursorPos, 0);

		int currentOldPlayCursorPos;
		double currentBytesRendered;

		{
			auto playPositionCriticalSectionGuard = playPositionCriticalSection.Enter();

			currentOldPlayCursorPos = oldPlayCursorPos;
			currentBytesRendered = bytesRendered;
		}

		double totalBytesRead = playCursorPos - currentOldPlayCursorPos;
		if (totalBytesRead < 0)
			totalBytesRead += bufferSizeBytes;
		totalBytesRead += currentBytesRendered;

		return totalBytesRead / SongRenderer::BlockAlign * 1000 / sampleRate;
	}

	DWORD WINAPI DirectSoundRenderThread::threadProc(LPVOID lpParameter)
	{
		auto renderThread = (DirectSoundRenderThread *)lpParameter;

		LPDIRECTSOUND8 device;
		DirectSoundCreate8(0, &device, 0);
		device->SetCooperativeLevel(GetForegroundWindow(), DSSCL_NORMAL);

		int sampleRate = renderThread->sampleRate;
		int bytesPerSec = sampleRate * SongRenderer::BlockAlign;

		WAVEFORMATEX bufferFormat =
		{
			WAVE_FORMAT_PCM,
			SongRenderer::NumChannels,
			(DWORD)sampleRate,
			(DWORD)bytesPerSec,
			(WORD)SongRenderer::BlockAlign,
			(WORD)SongRenderer::BitsPerSample,
			0
		};
		DSBUFFERDESC bufferDesc =
		{
			sizeof(DSBUFFERDESC),
			DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2,
			(DWORD)renderThread->bufferSizeBytes,
			0,
			&bufferFormat,
			GUID_NULL
		};
		device->CreateSoundBuffer(&bufferDesc, &renderThread->buffer, 0);

		renderThread->buffer->Play(0, 0, DSBPLAY_LOOPING);

		// We don't need to enter/leave a critical section here since there's only one writer for this value.
		while (!renderThread->shutdown)
		{
			{
				auto criticalSectionGuard = renderThread->criticalSection.Enter();

				int playCursorPos;
				renderThread->buffer->GetCurrentPosition((LPDWORD)&playCursorPos, 0);
				int bytesToRender = playCursorPos - renderThread->oldPlayCursorPos;
				if (bytesToRender)
				{
					if (bytesToRender < 0) bytesToRender += renderThread->bufferSizeBytes;
					if (bytesToRender >= 1000)
					{
						SongRenderer::Sample *p1, *p2;
						int b1, b2;
						renderThread->buffer->Lock(renderThread->oldPlayCursorPos, bytesToRender, (LPVOID *)&p1, (LPDWORD)&b1, (LPVOID *)&p2, (LPDWORD)&b2, 0);
						renderThread->callback(p1, b1 / sizeof(SongRenderer::Sample), renderThread->callbackData);
						if (b2) renderThread->callback(p2, b2 / sizeof(SongRenderer::Sample), renderThread->callbackData);
						renderThread->buffer->Unlock(p1, b1, p2, b2);

						auto playPositionCriticalSectionGuard = renderThread->playPositionCriticalSection.Enter();

						renderThread->oldPlayCursorPos = playCursorPos;
						renderThread->bytesRendered += bytesToRender;
					}
				}
			}

			Sleep(3);
		}

		renderThread->buffer->Stop();
		renderThread->buffer->Release();
		device->Release();

		return 0;
	}
}