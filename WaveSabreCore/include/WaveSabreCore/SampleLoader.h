#ifndef __WAVESABRECORE_SAMPLELOADER_H__
#define __WAVESABRECORE_SAMPLELOADER_H__

#include <Windows.h>
#include <mmreg.h>

#ifdef UNICODE
#define _UNICODE
#endif

#include <MSAcm.h>

namespace WaveSabreCore
{
	class SampleLoader
	{
	public:
		static const int SampleRate = 44100;

		struct LoadedSample {
			char *chunkData;

			char *waveFormatData;
			int compressedSize, uncompressedSize;

			char *compressedData;
			float *sampleData;

			int sampleLength;
		};

		static LoadedSample LoadSampleGSM(char *data, int compressedSize, int uncompressedSize, WAVEFORMATEX *waveFormat);

	private:
		static BOOL __stdcall driverEnumCallback(HACMDRIVERID driverId, DWORD_PTR dwInstance, DWORD fdwSupport);
		static BOOL __stdcall formatEnumCallback(HACMDRIVERID driverId, LPACMFORMATDETAILS formatDetails, DWORD_PTR dwInstance, DWORD fdwSupport);

		static HACMDRIVERID driverId;

	};
}

#endif
