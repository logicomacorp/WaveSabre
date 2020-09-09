#ifndef __WAVESABRECORE_GSMSAMPLE_H__
#define __WAVESABRECORE_GSMSAMPLE_H__

#include "Helpers.h"

#include <Windows.h>

#ifdef UNICODE
#define _UNICODE
#endif

#include <mmreg.h>
#include <MSAcm.h>

namespace WaveSabreCore
{
	class GsmSample
	{
	public:
		GsmSample(char *data, int compressedSize, int uncompressedSize, WAVEFORMATEX *waveFormat);
		~GsmSample();

		char *WaveFormatData;
		int CompressedSize, UncompressedSize;

		char *CompressedData;
		float *SampleData;

		int SampleLength;

	private:
		static BOOL __stdcall driverEnumCallback(HACMDRIVERID driverId, DWORD_PTR dwInstance, DWORD fdwSupport);
		static BOOL __stdcall formatEnumCallback(HACMDRIVERID driverId, LPACMFORMATDETAILS formatDetails, DWORD_PTR dwInstance, DWORD fdwSupport);

		static HACMDRIVERID driverId;
	};
}

#endif
