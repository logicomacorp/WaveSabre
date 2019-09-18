#include <WaveSabreCore/StateVariableFilter.h>

#include <WaveSabreCore/Helpers.h>

#include <math.h>

namespace WaveSabreCore
{
	StateVariableFilter::StateVariableFilter()
	{
		recalculate = true;

		type = StateVariableFilterType::Lowpass;
		freq = 20.0f;
		q = 1.0f;

		lastInput = 0.0f;
		low = band = 0.0f;
	}

	float StateVariableFilter::Next(float input)
	{
		if (recalculate)
		{
			f = (float)(1.5 * Helpers::FastSin(3.141592 * (double)freq / 2.0 / Helpers::CurrentSampleRate));

			recalculate = false;
		}

		float ret = (run((lastInput + input) / 2.0f) + run(input)) / 2.0f;
		lastInput = input;
		return ret;
	}

	void StateVariableFilter::SetType(StateVariableFilterType type)
	{
		if (type == this->type)
			return;

		this->type = type;
		recalculate = true;
	}

	void StateVariableFilter::SetFreq(float freq)
	{
		if (freq == this->freq)
			return;

		this->freq = freq;
		recalculate = true;
	}

	void StateVariableFilter::SetQ(float q)
	{
		if (q == this->q)
			return;

		this->q = q;
		recalculate = true;
	}

	float StateVariableFilter::run(float input)
	{
		low = low + f * band;
		float high = q * (input - band) - low;
		band = band + f * high;
		switch (type)
		{
		case StateVariableFilterType::Lowpass:
		default:
			return low;

		case StateVariableFilterType::Highpass: return high;
		case StateVariableFilterType::Bandpass: return band;
		case StateVariableFilterType::Notch: return low + high;
		}
	}
}
