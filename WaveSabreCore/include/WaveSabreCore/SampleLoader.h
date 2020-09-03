#ifndef __WAVESABRECORE_SAMPLELOADER_H__
#define __WAVESABRECORE_SAMPLELOADER_H__

#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#include <mmreg.h>

#ifdef UNICODE
#define _UNICODE
#endif
#include <MSAcm.h>
#else /* not WIN32 */
#include <stdint.h>

typedef struct __attribute__((__packed__)) tWAVEFORMATEX {
	int16_t wFormatTag;
	int16_t nChannels;
	int32_t nSamplesPerSec;
	int32_t nAvgBytesPerSec;
	int16_t nBlockAlign;
	int16_t wBitsPerSample;
	int16_t cbSize;
} WAVEFORMATEX;
#endif /* WIN32 */

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
#if defined(WIN32) || defined(_WIN32)
		static BOOL __stdcall driverEnumCallback(HACMDRIVERID driverId, DWORD_PTR dwInstance, DWORD fdwSupport);
		static BOOL __stdcall formatEnumCallback(HACMDRIVERID driverId, LPACMFORMATDETAILS formatDetails, DWORD_PTR dwInstance, DWORD fdwSupport);

		static HACMDRIVERID driverId;
#endif

	};
}

#endif
