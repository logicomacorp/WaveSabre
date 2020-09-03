#include <WaveSabreCore/SampleLoader.h>

#include <string.h>

namespace WaveSabreCore
{
#if defined(WIN32) || defined(_WIN32)
	HACMDRIVERID SampleLoader::driverId = NULL;
#endif

	SampleLoader::LoadedSample SampleLoader::LoadSampleGSM(char *data, int compressedSize, int uncompressedSize, WAVEFORMATEX *waveFormat)
	{
		LoadedSample ret;

		ret.compressedSize = compressedSize;
		ret.uncompressedSize = uncompressedSize;

		//if (waveFormatData) delete [] waveFormatData;
		ret.waveFormatData = new char[sizeof(WAVEFORMATEX) + waveFormat->cbSize];
		memcpy(ret.waveFormatData, waveFormat, sizeof(WAVEFORMATEX) + waveFormat->cbSize);
		//if (compressedData) delete [] compressedData;
		ret.compressedData = new char[compressedSize];
		memcpy(ret.compressedData, data, compressedSize);

		//if (sampleData) delete [] sampleData;

#if defined(WIN32) || defined(_WIN32)
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
#elif HAVE_FFMPEG_GSM
#else
		ret.sampleLength = 1;
		ret.sampleData = new float[1];
		ret.sampleData[0] = 0;
#endif

		return ret;
	}

#if defined(WIN32) || defined(_WIN32)
	BOOL __stdcall SampleLoader::driverEnumCallback(HACMDRIVERID driverId, DWORD_PTR dwInstance, DWORD fdwSupport)
	{
		if (SampleLoader::driverId) return 1;

		HACMDRIVER driver = NULL;
		acmDriverOpen(&driver, driverId, 0);

		int waveFormatSize = 0;
		acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &waveFormatSize);
		auto waveFormat = (WAVEFORMATEX *)(new char[waveFormatSize]);
		memset(waveFormat, 0, waveFormatSize);
		ACMFORMATDETAILS formatDetails;
		memset(&formatDetails, 0, sizeof(formatDetails));
		formatDetails.cbStruct = sizeof(formatDetails);
		formatDetails.pwfx = waveFormat;
		formatDetails.cbwfx = waveFormatSize;
		formatDetails.dwFormatTag = WAVE_FORMAT_UNKNOWN;
		acmFormatEnum(driver, &formatDetails, formatEnumCallback, NULL, NULL);

		delete [] (char *)waveFormat;

		acmDriverClose(driver, 0);

		return 1;
	}

	BOOL __stdcall SampleLoader::formatEnumCallback(HACMDRIVERID driverId, LPACMFORMATDETAILS formatDetails, DWORD_PTR dwInstance, DWORD fdwSupport)
	{
		if (formatDetails->pwfx->wFormatTag == WAVE_FORMAT_GSM610 &&
			formatDetails->pwfx->nChannels == 1 &&
			formatDetails->pwfx->nSamplesPerSec == SampleRate)
		{
			SampleLoader::driverId = driverId;
		}
		return 1;
	}
#endif
}
