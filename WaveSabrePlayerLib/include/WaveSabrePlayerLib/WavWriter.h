#ifndef __WAVESABREPLAYERLIB_WAVWRITER_H__
#define __WAVESABREPLAYERLIB_WAVWRITER_H__

#include <stdio.h>

#include "SongRenderer.h"

namespace WaveSabrePlayerLib
{
	class WavWriter
	{
	public:
		typedef void (*ProgressCallback)(double progress, void *data);

		WavWriter(const SongRenderer::Song *song, int numRenderThreads);
		~WavWriter();

		void Write(const char *fileName, ProgressCallback callback, void *data);

	private:
		static void writeInt(int i, FILE *file);
		static void writeShort(short s, FILE *file);

		SongRenderer *songRenderer;
	};
}

#endif
