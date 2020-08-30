#ifndef __WAVESABREPLAYERLIB_IRENDERTHREAD_H__
#define __WAVESABREPLAYERLIB_IRENDERTHREAD_H__

#include "SongRenderer.h"

namespace WaveSabrePlayerLib
{
	class IRenderThread
	{
	public:
		typedef void (*RenderCallback)(SongRenderer::Sample *buffer, int numSamples, void *data);

		virtual ~IRenderThread();

		virtual int GetPlayPositionMs() = 0;
	};
}

#endif
