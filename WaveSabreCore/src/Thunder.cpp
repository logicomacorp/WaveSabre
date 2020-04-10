#include <WaveSabreCore/Thunder.h>
#include <WaveSabreCore/Helpers.h>

#include <math.h>

namespace WaveSabreCore
{
	HACMDRIVERID Thunder::driverId = NULL;

	Thunder::Thunder()
		: SynthDevice(0)
	{
		for (int i = 0; i < maxVoices; i++) voices[i] = new ThunderVoice(this);

		chunkData = nullptr;

		waveFormatData = nullptr;
		compressedSize = uncompressedSize = 0;
		compressedData = nullptr;
		sampleData = nullptr;

		sampleLength = 0;
	}

	Thunder::~Thunder()
	{
		if (chunkData) delete [] chunkData;
		if (waveFormatData) delete [] waveFormatData;
		if (compressedData) delete [] compressedData;
		if (sampleData) delete [] sampleData;
	}

	typedef struct
	{
		int CompressedSize;
		int UncompressedSize;
	} ChunkHeader;

	void Thunder::SetChunk(void *data, int size)
	{
		if (!size) return;
		auto h = (ChunkHeader *)data;
		auto waveFormat = (WAVEFORMATEX *)((char *)data + sizeof(ChunkHeader));
		auto compressedData = (char *)waveFormat + sizeof(WAVEFORMATEX) + waveFormat->cbSize;
		LoadSample(compressedData, h->CompressedSize, h->UncompressedSize, waveFormat);
	}

	int Thunder::GetChunk(void **data)
	{
		if (!compressedData) return 0;
		ChunkHeader h;
		h.CompressedSize = compressedSize;
		h.UncompressedSize = uncompressedSize;
		if (chunkData) delete [] chunkData;
		int chunkSize = sizeof(ChunkHeader) + sizeof(WAVEFORMATEX) + ((WAVEFORMATEX *)waveFormatData)->cbSize + compressedSize + sizeof(int);
		chunkData = new char[chunkSize];
		memcpy(chunkData, &h, sizeof(ChunkHeader));
		memcpy(chunkData + sizeof(ChunkHeader), waveFormatData, sizeof(WAVEFORMATEX) + ((WAVEFORMATEX *)waveFormatData)->cbSize);
		memcpy(chunkData + sizeof(ChunkHeader) + sizeof(WAVEFORMATEX) + ((WAVEFORMATEX *)waveFormatData)->cbSize, compressedData, compressedSize);
		*(int *)(chunkData + chunkSize - sizeof(int)) = chunkSize;
		*data = chunkData;
		return chunkSize;
	}

	void Thunder::LoadSample(char *data, int compressedSize, int uncompressedSize, WAVEFORMATEX *waveFormat)
	{
		this->compressedSize = compressedSize;
		this->uncompressedSize = uncompressedSize;

		if (waveFormatData) delete [] waveFormatData;
		waveFormatData = new char[sizeof(WAVEFORMATEX) + waveFormat->cbSize];
		memcpy(waveFormatData, waveFormat, sizeof(WAVEFORMATEX) + waveFormat->cbSize);
		if (compressedData) delete [] compressedData;
		compressedData = new char[compressedSize];
		memcpy(compressedData, data, compressedSize);

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
		streamHeader.pbSrc = (LPBYTE)compressedData;
		streamHeader.cbSrcLength = compressedSize;
		auto uncompressedData = new short[uncompressedSize * 2];
		streamHeader.pbDst = (LPBYTE)uncompressedData;
		streamHeader.cbDstLength = uncompressedSize * 2;
		acmStreamPrepareHeader(stream, &streamHeader, 0);

		acmStreamConvert(stream, &streamHeader, 0);
		
		acmStreamClose(stream, 0);
		acmDriverClose(driver, 0);

		sampleLength = streamHeader.cbDstLengthUsed / sizeof(short);
		if (sampleData) delete [] sampleData;
		sampleData = new float[sampleLength];
		for (int i = 0; i < sampleLength; i++) sampleData[i] = (float)((double)uncompressedData[i] / 32768.0);

		delete [] uncompressedData;
	}

	Thunder::ThunderVoice::ThunderVoice(Thunder *thunder)
	{
		this->thunder = thunder;
	}

	SynthDevice *Thunder::ThunderVoice::SynthDevice() const
	{
		return thunder;
	}

	void Thunder::ThunderVoice::Run(double songPosition, float **outputs, int numSamples)
	{
		for (int i = 0; i < numSamples; i++)
		{
			if (samplePos >= thunder->sampleLength)
			{
				IsOn = false;
				break;
			}
			float sample = thunder->sampleData[samplePos];
			outputs[0][i] += sample;
			outputs[1][i] += sample;
			samplePos++;
		}
	}

	void Thunder::ThunderVoice::NoteOn(int note, int velocity, float detune, float pan)
	{
		Voice::NoteOn(note, velocity, detune, pan);
		samplePos = 0;
	}

	BOOL __stdcall Thunder::driverEnumCallback(HACMDRIVERID driverId, DWORD_PTR dwInstance, DWORD fdwSupport)
	{
		if (Thunder::driverId) return 1;

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

	BOOL __stdcall Thunder::formatEnumCallback(HACMDRIVERID driverId, LPACMFORMATDETAILS formatDetails, DWORD_PTR dwInstance, DWORD fdwSupport)
	{
		if (formatDetails->pwfx->wFormatTag == WAVE_FORMAT_GSM610 &&
			formatDetails->pwfx->nChannels == 1 &&
			formatDetails->pwfx->nSamplesPerSec == SampleRate)
		{
			Thunder::driverId = driverId;
		}
		return 1;
	}
}
