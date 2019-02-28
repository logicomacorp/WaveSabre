#include <WaveSabreCore/AllPass.h>
#include <WaveSabreCore/Helpers.h>

#include <stdlib.h>
#include <math.h>

namespace WaveSabreCore
{
	AllPass::AllPass()
	{
		buffer = nullptr;
		bufferIndex = 0;
	}

	AllPass::~AllPass()
	{
		free(buffer);
	}

	void AllPass::SetBufferSize(int size)
	{
		if (size < 1) size = 1;
		bufferSize = size;
		auto newBuffer = (float *)malloc(sizeof(float) * size);
		for (int i = 0; i < size; i++) newBuffer[i] = 0.0f;
		bufferIndex = 0;
		auto oldBuffer = buffer;
		buffer = newBuffer;
		free(oldBuffer);
	}

	void AllPass::SetFeedback(float value)
	{
		feedback = value;
	}

	float AllPass::GetFeedback()
	{
		return feedback;
	}

	float AllPass::Process(float input)
	{
		float output;
		float bufferOut;

		bufferOut = buffer[bufferIndex];

		output = -input + bufferOut;
		buffer[bufferIndex] = input + (bufferOut * feedback);

		bufferIndex = (bufferIndex + 1) % bufferSize;

		return output;
	}
}
