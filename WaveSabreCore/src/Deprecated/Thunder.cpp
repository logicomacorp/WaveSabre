#include <WaveSabreCore/Deprecated/Thunder.h>
#include <WaveSabreCore/Helpers.h>

namespace WaveSabreCore
{
	Thunder::Thunder()
		: SynthDevice(0)
		, sample(nullptr)
	{
		for (int i = 0; i < maxVoices; i++) voices[i] = new ThunderVoice(this);

		chunkData = nullptr;
	}

	Thunder::~Thunder()
	{
		if (chunkData) delete [] chunkData;
		if (sample) delete sample;
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
		if (!sample) return 0;
		ChunkHeader h;
		h.CompressedSize = sample->CompressedSize;
		h.UncompressedSize = sample->UncompressedSize;
		if (chunkData) delete [] chunkData;
		int chunkSize = sizeof(ChunkHeader) + sizeof(WAVEFORMATEX) + ((WAVEFORMATEX *)sample->WaveFormatData)->cbSize + sample->CompressedSize + sizeof(int);
		chunkData = new char[chunkSize];
		memcpy(chunkData, &h, sizeof(ChunkHeader));
		memcpy(chunkData + sizeof(ChunkHeader), sample->WaveFormatData, sizeof(WAVEFORMATEX) + ((WAVEFORMATEX *)sample->WaveFormatData)->cbSize);
		memcpy(chunkData + sizeof(ChunkHeader) + sizeof(WAVEFORMATEX) + ((WAVEFORMATEX *)sample->WaveFormatData)->cbSize, sample->CompressedData, sample->CompressedSize);
		*(int *)(chunkData + chunkSize - sizeof(int)) = chunkSize;
		*data = chunkData;
		return chunkSize;
	}

	void Thunder::LoadSample(char *compressedDataPtr, int compressedSize, int uncompressedSize, WAVEFORMATEX *waveFormatPtr)
	{
		if (sample) delete sample;

		sample = new GsmSample(compressedDataPtr, compressedSize, uncompressedSize, waveFormatPtr);
	}

	Thunder::ThunderVoice::ThunderVoice(Thunder *thunder)
	{
		this->thunder = thunder;
	}

	SynthDevice *Thunder::ThunderVoice::GetSynthDevice() const
	{
		return thunder;
	}

	void Thunder::ThunderVoice::Run(double songPosition, float **outputs, int numSamples)
	{
		for (int i = 0; i < numSamples; i++)
		{
			if (samplePos >= thunder->sample->SampleLength)
			{
				IsOn = false;
				break;
			}
			float sample = thunder->sample->SampleData[samplePos];
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
}
