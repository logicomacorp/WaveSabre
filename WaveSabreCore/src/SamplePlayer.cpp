#include <WaveSabreCore/SamplePlayer.h>
#include <WaveSabreCore/Helpers.h>

#include <math.h>

namespace WaveSabreCore
{
	SamplePlayer::SamplePlayer()
	{
		SampleStart = 0.0f;
		Reverse = false;
		LoopMode = LoopMode::Repeat;
		LoopBoundaryMode = LoopBoundaryMode::FromSample;
		LoopStart = 0.0f;
		LoopLength = 1.0f;
		SampleLoopStart = 0;
		SampleLoopLength = 0;

		InterpolationMode = InterpolationMode::Linear;

		SampleData = nullptr;
		SampleLength = 0;

		IsActive = false;
	}

	void SamplePlayer::CalcPitch(double note)
	{
		double freqDelta = Helpers::Exp2(note / 12.0);
		if (!reverse)
		{
			sampleDelta = freqDelta;
		}
		else
		{
			sampleDelta = -freqDelta;
		}
	}

	void SamplePlayer::InitPos()
	{
		reverse = Reverse;
		IsActive = true;
		if (!reverse)
		{
			samplePos = (double)SampleStart * (double)(SampleLength - 1);
		}
		else
		{
			samplePos = (1.0 - (double)SampleStart) * (double)(SampleLength - 1);
		}
	}

	void SamplePlayer::RunPrep()
	{
		switch (LoopBoundaryMode)
		{
		case LoopBoundaryMode::FromSample:
			roundedLoopStart = SampleLoopStart;
			roundedLoopLength = SampleLoopLength;
			break;

		case LoopBoundaryMode::Manual:
			roundedLoopStart = (int)((float)SampleLength * LoopStart);
			roundedLoopLength = (int)((float)SampleLength * LoopLength);
			break;
		}

		if (roundedLoopLength < 1)
			roundedLoopLength = 1;

		if (roundedLoopStart >= SampleLength)
			roundedLoopStart = SampleLength - 1;

		roundedLoopEnd = roundedLoopStart + roundedLoopLength;
		if (roundedLoopEnd > SampleLength)
		{
			roundedLoopEnd = SampleLength;
			roundedLoopLength = roundedLoopEnd - roundedLoopStart;
		}
	}

	float SamplePlayer::Next()
	{
		double samplePosFloor = floor(samplePos);
		double samplePosFract = samplePos - samplePosFloor;

		int roundedSamplePos = (int)samplePosFloor;
		if (roundedSamplePos < 0 || roundedSamplePos >= SampleLength)
		{
			IsActive = false;
			return 0.0f;
		}

		float sample = 0.0f;
		switch (InterpolationMode)
		{
		case InterpolationMode::Nearest:
			sample = SampleData[roundedSamplePos];
			break;

		case InterpolationMode::Linear:
			float left = SampleData[roundedSamplePos];
			int rightIndex = roundedSamplePos + 1;
			if (LoopMode == LoopMode::Repeat && rightIndex == roundedLoopEnd)
				rightIndex = roundedLoopStart;
			float right = rightIndex < SampleLength ? SampleData[rightIndex] : 0.0f;
			sample = (float)((double)left * (1.0 - samplePosFract) + (double)right * samplePosFract);
			break;
		}

		samplePos += sampleDelta;

		switch (LoopMode)
		{
		case LoopMode::Repeat:
			if (sampleDelta > 0.0)
			{
				while (samplePos >= (double)roundedLoopEnd)
					samplePos -= (double)roundedLoopLength;
			}
			else
			{
				while (samplePos < (double)roundedLoopStart)
					samplePos += (double)roundedLoopLength;
			}
			break;

		case LoopMode::PingPong:
			if (sampleDelta > 0.0 && samplePos >= (double)roundedLoopEnd)
			{
				samplePos = (double)(roundedLoopEnd - 1);
				sampleDelta = -sampleDelta;
				reverse = !reverse;
			}
			else if (sampleDelta < 0.0 && samplePos < (double)roundedLoopStart)
			{
				samplePos = (double)roundedLoopStart;
				sampleDelta = -sampleDelta;
				reverse = !reverse;
			}
			break;
		}

		return sample;
	}
}
