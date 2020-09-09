#ifndef __WAVESABREPLAYERLIB_REALTIMEPLAYER_H__
#define __WAVESABREPLAYERLIB_REALTIMEPLAYER_H__

#include "IPlayer.h"
#include "IRenderThread.h"
#include "SongRenderer.h"

namespace WaveSabrePlayerLib
{
	class RealtimePlayer : public IPlayer
	{
	public:
		RealtimePlayer(const SongRenderer::Song *song, int numRenderThreads, int bufferSizeMs = 1000);
		virtual ~RealtimePlayer();

		virtual void Play();
		virtual void DoForegroundWork();

		virtual int GetTempo() const;
		virtual int GetSampleRate() const;
		virtual double GetLength() const;
		virtual double GetSongPos() const;

	private:
		static void renderCallback(SongRenderer::Sample *buffer, int numSamples, void *data);

		const SongRenderer::Song *song;
		int numRenderThreads;
		int bufferSizeMs;

		SongRenderer *songRenderer;
		IRenderThread *renderThread;
	};
}

#endif
