#include <WaveSabreCore/GsmSample.h>

#include <string.h>

namespace WaveSabreCore
{
	GsmSample::GsmSample(char *data, int compressedSize, int uncompressedSize, WAVEFORMATEX *waveFormat)
		: CompressedSize(compressedSize)
		, UncompressedSize(uncompressedSize)
	{
		WaveFormatData = new char[sizeof(WAVEFORMATEX) + waveFormat->cbSize];
		memcpy(WaveFormatData, waveFormat, sizeof(WAVEFORMATEX) + waveFormat->cbSize);
		CompressedData = new char[compressedSize];
		memcpy(CompressedData, data, compressedSize);

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
		streamHeader.pbSrc = (LPBYTE)CompressedData;
		streamHeader.cbSrcLength = compressedSize;
		auto uncompressedData = new short[uncompressedSize * 2];
		streamHeader.pbDst = (LPBYTE)uncompressedData;
		streamHeader.cbDstLength = uncompressedSize * 2;
		acmStreamPrepareHeader(stream, &streamHeader, 0);

		acmStreamConvert(stream, &streamHeader, 0);

		acmStreamClose(stream, 0);
		acmDriverClose(driver, 0);

		SampleLength = streamHeader.cbDstLengthUsed / sizeof(short);
		SampleData = new float[SampleLength];
		for (int i = 0; i < SampleLength; i++)
			SampleData[i] = (float)((double)uncompressedData[i] / 32768.0);

		delete [] uncompressedData;
	}

	GsmSample::~GsmSample()
	{
		delete [] WaveFormatData;
		delete [] CompressedData;
		delete [] SampleData;
	}

	HACMDRIVERID GsmSample::driverId = NULL;

	BOOL __stdcall GsmSample::driverEnumCallback(HACMDRIVERID driverId, DWORD_PTR dwInstance, DWORD fdwSupport)
	{
		if (GsmSample::driverId) return 1;

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

	BOOL __stdcall GsmSample::formatEnumCallback(HACMDRIVERID driverId, LPACMFORMATDETAILS formatDetails, DWORD_PTR dwInstance, DWORD fdwSupport)
	{
		if (formatDetails->pwfx->wFormatTag == WAVE_FORMAT_GSM610 &&
			formatDetails->pwfx->nChannels == 1 &&
			formatDetails->pwfx->nSamplesPerSec == 44100)
		{
			GsmSample::driverId = driverId;
		}
		return 1;
	}
}
