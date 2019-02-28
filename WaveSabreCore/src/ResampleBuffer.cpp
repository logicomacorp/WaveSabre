#include <WaveSabreCore/ResampleBuffer.h>
#include <WaveSabreCore/Helpers.h>
#include <math.h>

namespace WaveSabreCore
{
	ResampleBuffer::ResampleBuffer(float lengthMs)
	{
		buffer = nullptr;
		SetLength(lengthMs);
	}

	ResampleBuffer::~ResampleBuffer()
	{
		if (buffer) delete [] buffer;
	}

	void ResampleBuffer::SetLength(float lengthMs)
	{
		int newLength = (int)((double)lengthMs * Helpers::CurrentSampleRate / 1000.0);
		SetLengthSamples(newLength);
	}

	void ResampleBuffer::SetLengthSamples(int samples)
	{
		if (samples < 1) samples = 1;
		if (samples != length || !buffer)
		{
			auto newBuffer = new float[samples];
			for (int i = 0; i < samples; i++) newBuffer[i] = 0.0f;
			currentPosition = 0;
			auto oldBuffer = buffer;
			buffer = newBuffer;
			length = samples;
			if (oldBuffer) delete[] oldBuffer;
		}
	}

	void ResampleBuffer::WriteSample(float sample)
	{
		buffer[currentPosition] = sample;
		currentPosition = currentPosition - 1;
		if (currentPosition < 0)
		{
			currentPosition = length - 1;
		}
	}

	float ResampleBuffer::ReadPosition(float position) const
	{
		int samplePos = (currentPosition + (int)position) % length; // actual sample position determined
		float fraction = position - floorf(position);  // fractional
		
		float s0 = buffer[samplePos];
		float s1 = (samplePos > 0) ? buffer[samplePos - 1] : buffer[length - 1];
		return s0 + fraction * (s1 - s0);
	}
}
