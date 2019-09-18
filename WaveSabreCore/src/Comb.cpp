#include <WaveSabreCore/Comb.h>
#include <WaveSabreCore/Helpers.h>

#include <math.h>

namespace WaveSabreCore
{
	Comb::Comb()
	{
		buffer = nullptr;
		filterStore = 0;
		bufferIndex = 0;
	}

	Comb::~Comb()
	{
		if (buffer) delete[] buffer;
	}

	void Comb::SetBufferSize(int size)
	{
		if (size < 1) size = 1;
		bufferSize = size;
		auto newBuffer = new float[size];
		for (int i = 0; i < size; i++) newBuffer[i] = 0.0f;
		bufferIndex = 0;
		auto oldBuffer = buffer;
		buffer = newBuffer;
		if (oldBuffer) delete[] oldBuffer;
	}

	void Comb::SetDamp(float val)
	{
		damp1 = val;
		damp2 = 1 - val;
	}

	float Comb::GetDamp()
	{
		return damp1;
	}

	void Comb::SetFeedback(float val)
	{
		feedback = val;
	}

	float Comb::GetFeedback()
	{
		return feedback;
	}

	float Comb::Process(float input)
	{
		float output;

		output = buffer[bufferIndex];

		filterStore = (output * damp2) + (filterStore * damp1);

		buffer[bufferIndex] = input + (filterStore * feedback);

		bufferIndex = (bufferIndex + 1) % bufferSize;

		return output;
	}
}
