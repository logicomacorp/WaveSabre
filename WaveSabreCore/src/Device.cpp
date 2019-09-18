#include <WaveSabreCore/Device.h>
#include <WaveSabreCore/Helpers.h>

namespace WaveSabreCore
{
	Device::Device(int numParams)
	{
		this->numParams = numParams;
		chunkData = nullptr;
	}

	Device::~Device()
	{
		if (chunkData) delete (char *)chunkData;
	}

	void Device::AllNotesOff() { }
	void Device::NoteOn(int note, int velocity, int deltaSamples) { }
	void Device::NoteOff(int note, int deltaSamples) { }

	void Device::SetSampleRate(float sampleRate)
	{
		Helpers::CurrentSampleRate = (double)sampleRate;
	}

	void Device::SetTempo(int tempo)
	{
		Helpers::CurrentTempo = tempo;
	}

	void Device::SetParam(int index, float value) { }

	float Device::GetParam(int index) const
	{
		return 0.0f;
	}

	void Device::SetChunk(void *data, int size)
	{
		auto params = (float *)data;
		// This may be different than our internal numParams value if this chunk was
		//  saved with an earlier version of the plug for example. It's important we
		//  don't read past the chunk data, so we set as many parameters as the
		//  chunk contains, not the amount of parameters we have available. The
		//  remaining parameters will retain their default values in that case, which
		//  if we've done our job right, shouldn't change the sound with respect to
		//  the parameters we read here.
		auto numChunkParams = (int)((size - sizeof(int)) / sizeof(float));
		for (int i = 0; i < numChunkParams; i++)
			SetParam(i, params[i]);
	}

	int Device::GetChunk(void **data)
	{
		int chunkSize = numParams * sizeof(float) + sizeof(int);
		if (!chunkData) chunkData = new char[chunkSize];

		for (int i = 0; i < numParams; i++)
			((float *)chunkData)[i] = GetParam(i);
		*(int *)((char *)chunkData + chunkSize - sizeof(int)) = chunkSize;

		*data = chunkData;
		return chunkSize;
	}

	void Device::clearOutputs(float **outputs, int numSamples)
	{
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < numSamples; j++) outputs[i][j] = 0.0f;
		}
	}
}
