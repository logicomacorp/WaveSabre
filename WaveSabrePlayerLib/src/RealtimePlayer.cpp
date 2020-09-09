#include <WaveSabrePlayerLib/RealtimePlayer.h>

#if defined(WIN32) || defined(_WIN32)
#include <WaveSabrePlayerLib/DirectSoundRenderThread.h>
#elif HAVE_SDL2
#include <WaveSabrePlayerLib/SDL2RenderThread.h>
#elif HAVE_APLAY
#include <WaveSabrePlayerLib/AplayRenderThread.h>
#endif

namespace WaveSabrePlayerLib
{
	RealtimePlayer::RealtimePlayer(const SongRenderer::Song *song, int numRenderThreads, int bufferSizeMs)
		: song(song)
		, numRenderThreads(numRenderThreads)
		, bufferSizeMs(bufferSizeMs)
		, songRenderer(new SongRenderer(song, numRenderThreads))
		, renderThread(nullptr)
	{
	}

	RealtimePlayer::~RealtimePlayer()
	{
		if (renderThread)
			delete renderThread;
		if (songRenderer)
			delete songRenderer;
	}

	void RealtimePlayer::DoForegroundWork()
	{
		if (renderThread)
			renderThread->DoForegroundWork();
	}

	void RealtimePlayer::Play()
	{
		if (renderThread)
			delete renderThread;
		if (songRenderer)
			delete songRenderer;

		songRenderer = new SongRenderer(song, numRenderThreads);
#if defined(WIN32) || defined(_WIN32)
		renderThread = new DirectSoundRenderThread(renderCallback, this, songRenderer->GetSampleRate(), bufferSizeMs);
#elif HAVE_SDL2
		renderThread = new SDL2RenderThread(renderCallback, this, songRenderer->GetSampleRate(), bufferSizeMs, true);
#elif HAVE_APLAY
		renderThread = new AplayRenderThread(renderCallback, this, songRenderer->GetSampleRate(), bufferSizeMs, true);
#endif
	}

	int RealtimePlayer::GetTempo() const
	{
		return songRenderer->GetTempo();
	}

	int RealtimePlayer::GetSampleRate() const
	{
		return songRenderer->GetSampleRate();
	}

	double RealtimePlayer::GetLength() const
	{
		return songRenderer->GetLength();
	}

	double RealtimePlayer::GetSongPos() const
	{
		if (!renderThread)
			return 0.0;

		double v = ((double)renderThread->GetPlayPositionMs() - (double)bufferSizeMs) / 1000.0;
#if defined(WIN32) || defined(_WIN32)
		return max(v, 0.0);
#else
		return (v > 0.0) ? v : 0.0;
#endif
	}

	void RealtimePlayer::renderCallback(SongRenderer::Sample *buffer, int numSamples, void *data)
	{
		auto player = (RealtimePlayer *)data;
		const int stepSize = 100 * SongRenderer::NumChannels;
		for (int i = 0; i < numSamples; i += stepSize) {
			int v = numSamples - i;
			if (v > stepSize) v = stepSize;

			player->songRenderer->RenderSamples(buffer + i, v);
		}
	}
}
