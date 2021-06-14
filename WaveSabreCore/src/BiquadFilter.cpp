#include <WaveSabreCore/BiquadFilter.h>
#include <WaveSabreCore/Helpers.h>

#include <math.h>

namespace WaveSabreCore
{
	BiquadFilter::BiquadFilter()
	{
		recalculate = true;

		type = BiquadFilterType::Lowpass;

		freq = 1000.0f;
		q = 1.0f;
		gain = 0.0f;

		lastInput = lastLastInput = 0.0f;
		lastOutput = lastLastOutput = 0.0f;
	}

	float BiquadFilter::Next(float input)
	{
		if (recalculate)
		{
			float w0 = 2.0f * 3.141592f * freq / (float)Helpers::CurrentSampleRate;

			float alpha = (float)Helpers::FastSin(w0) / (2.0f * q);

			float a0, a1, a2;
			float b0, b1, b2;
			switch (type)
			{
			case BiquadFilterType::Lowpass:
				a0 = 1.0f + alpha;
				a1 = -2.0f * (float)Helpers::FastCos(w0);
				a2 = 1.0f - alpha;
				b0 = (1.0f - (float)Helpers::FastCos(w0)) / 2.0f;
				b1 = 1.0f - (float)Helpers::FastCos(w0);
				b2 = (1.0f - (float)Helpers::FastCos(w0)) / 2.0f;
				break;

			case BiquadFilterType::Highpass:
				a0 = 1.0f + alpha;
				a1 = -2.0f * (float)Helpers::FastCos(w0);
				a2 = 1.0f - alpha;
				b0 = (1.0f + (float)Helpers::FastCos(w0)) / 2.0f;
				b1 = -(1.0f + (float)Helpers::FastCos(w0));
				b2 = (1.0f + (float)Helpers::FastCos(w0)) / 2.0f;
				break;

			case BiquadFilterType::Peak:
				{
					float A = Helpers::Exp10F(gain / 40.0f);
					a0 = 1.0f + alpha / A;
					a1 = -2.0f * (float)Helpers::FastCos(w0);
					a2 = 1.0f - alpha / A;
					b0 = 1.0f + alpha * A;
					b1 = -2.0f * (float)Helpers::FastCos(w0);
					b2 = 1.0f - alpha * A;
				}
				break;
			}

			c1 = b0 / a0;
			c2 = b1 / a0;
			c3 = b2 / a0;
			c4 = a1 / a0;
			c5 = a2 / a0;

			recalculate = false;
		}

		float output = c1 * input + c2 * lastInput + c3 * lastLastInput - c4 * lastOutput - c5 * lastLastOutput;

		lastLastInput = lastInput;
		lastInput = input;
		lastLastOutput = lastOutput;
		lastOutput = output;

		return output;
	}

	void BiquadFilter::SetType(BiquadFilterType type)
	{
		if (type == this->type)
			return;

		this->type = type;
		recalculate = true;
	}

	void BiquadFilter::SetFreq(float freq)
	{
		if (freq == this->freq)
			return;

		this->freq = freq;
		recalculate = true;
	}

	void BiquadFilter::SetQ(float q)
	{
		if (q == this->q)
			return;

		this->q = q;
		recalculate = true;
	}

	void BiquadFilter::SetGain(float gain)
	{
		if (gain == this->gain)
			return;

		this->gain = gain;
		recalculate = true;
	}
}
