#include <WaveSabreCore/DelayBuffer.h>
#include <WaveSabreCore/Helpers.h>

#include <stdlib.h>

namespace WaveSabreCore
{
	DelayBuffer::DelayBuffer(float lengthMs)
	{
		buffer = nullptr;
		SetLength(lengthMs);
	}

	DelayBuffer::~DelayBuffer()
	{
		free(buffer);
	}

	void DelayBuffer::SetLength(float lengthMs)
	{
		int newLength = (int)((double)lengthMs * Helpers::CurrentSampleRate / 1000.0);
		if (newLength < 1) newLength = 1;
		if (newLength != length || !buffer)
		{
			auto newBuffer = (float *)malloc(sizeof(float) * newLength);
			for (int i = 0; i < newLength; i++) newBuffer[i] = 0.0f;
			currentPosition = 0;
			auto oldBuffer = buffer;
			buffer = newBuffer;
			length = newLength;
			free(oldBuffer);
		}
	}

	void DelayBuffer::WriteSample(float sample)
	{
		buffer[currentPosition] = sample;
		currentPosition = (currentPosition + 1) % length;
	}

	float DelayBuffer::ReadSample() const
	{
		return buffer[currentPosition];
	}
}