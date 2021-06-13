#include <WaveSabrePlayerLib/RealtimePlayer.h>

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

	void RealtimePlayer::Play()
	{
		if (renderThread)
			delete renderThread;
		if (songRenderer)
			delete songRenderer;

		songRenderer = new SongRenderer(song, numRenderThreads);
		renderThread = new DirectSoundRenderThread(renderCallback, this, songRenderer->GetSampleRate(), bufferSizeMs);
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

		return max((renderThread->GetPlayPositionMs() - (double)bufferSizeMs) / 1000.0, 0.0);
	}

	void RealtimePlayer::renderCallback(SongRenderer::Sample *buffer, int numSamples, void *data)
	{
		auto player = (RealtimePlayer *)data;
		const int stepSize = 100 * SongRenderer::NumChannels;
		for (int i = 0; i < numSamples; i += stepSize)
			player->songRenderer->RenderSamples(buffer + i, min(numSamples - i, stepSize));
	}
}
