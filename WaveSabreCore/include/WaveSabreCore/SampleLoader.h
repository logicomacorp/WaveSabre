#ifndef __WAVESABRECORE_SAMPLELOADER_H__
#define __WAVESABRECORE_SAMPLELOADER_H__

#include <string.h>

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

#ifdef _MSC_VER
		__forceinline
#else
		__attribute__((__always_inline__))
#endif
		inline static LoadedSample LoadSampleGSM(char *data, int compressedSize, int uncompressedSize, WAVEFORMATEX *waveFormat)
		{
			LoadedSample ret;

			ret.compressedSize = compressedSize;
			ret.uncompressedSize = uncompressedSize;

			ret.waveFormatData = new char[sizeof(WAVEFORMATEX) + waveFormat->cbSize];
			memcpy(ret.waveFormatData, waveFormat, sizeof(WAVEFORMATEX) + waveFormat->cbSize);
			ret.compressedData = new char[compressedSize];
			memcpy(ret.compressedData, data, compressedSize);

			acmDriverEnum(driverEnumCallback, NULL, NULL);
			HACMDRIVER driver = NULL;
			acmDriverOpen(&driver, driverId, 0);

			WAVEFORMATEX dstWaveFormat =
			{
				WAVE_FORMAT_PCM,
				1,
				waveFormat->nSamplesPerSec,
				waveFormat->nSamplesPerSec * 2,
				sizeof(short),
				sizeof(short) * 8,
				0
			};

			HACMSTREAM stream = NULL;
			acmStreamOpen(&stream, driver, waveFormat, &dstWaveFormat, NULL, NULL, NULL, ACM_STREAMOPENF_NONREALTIME);

			ACMSTREAMHEADER streamHeader;
			memset(&streamHeader, 0, sizeof(ACMSTREAMHEADER));
			streamHeader.cbStruct = sizeof(ACMSTREAMHEADER);
			streamHeader.pbSrc = (LPBYTE)ret.compressedData;
			streamHeader.cbSrcLength = compressedSize;
			auto uncompressedData = new short[uncompressedSize * 2];
			streamHeader.pbDst = (LPBYTE)uncompressedData;
			streamHeader.cbDstLength = uncompressedSize * 2;
			acmStreamPrepareHeader(stream, &streamHeader, 0);

			acmStreamConvert(stream, &streamHeader, 0);

			acmStreamClose(stream, 0);
			acmDriverClose(driver, 0);

			ret.sampleLength = streamHeader.cbDstLengthUsed / sizeof(short);
			ret.sampleData = new float[ret.sampleLength];
			for (int i = 0; i < ret.sampleLength; i++)
				ret.sampleData[i] = (float)((double)uncompressedData[i] / 32768.0);

			delete [] uncompressedData;

			return ret;
		}

	private:
		static BOOL __stdcall driverEnumCallback(HACMDRIVERID driverId, DWORD_PTR dwInstance, DWORD fdwSupport);
		static BOOL __stdcall formatEnumCallback(HACMDRIVERID driverId, LPACMFORMATDETAILS formatDetails, DWORD_PTR dwInstance, DWORD fdwSupport);

		static HACMDRIVERID driverId;
	};
}

#endif
