#ifndef __WAVESABRECORE_GMDLS_H__
#define __WAVESABRECORE_GMDLS_H__

namespace WaveSabreCore
{
	class GmDls
	{
	public:
		static const int WaveListOffset = 0x00044602;
		static const int NumSamples = 495;

		static unsigned char *Load();
	};
}

#endif
