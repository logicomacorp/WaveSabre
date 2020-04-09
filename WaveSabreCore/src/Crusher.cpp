#include <WaveSabreCore/Crusher.h>
#include <WaveSabreCore/Helpers.h>

#include <math.h>

namespace WaveSabreCore
{
	Crusher::Crusher()
		: Device((int)ParamIndices::NumParams)
	{
		vertical = 0.0f;
		horizontal = 0.0f;
		dryWet = 1.0f;

		for (int i = 0; i < 2; i++)
		{
			phase[i] = 0.0f;
			hold[i] = 0.0f;
		}
	}

	void Crusher::Run(double songPosition, float **inputs, float **outputs, int numSamples)
	{
		float step = 1.0f / Helpers::Exp2F((1.0f - vertical) * 15.0f + 1.0f);
		float freq = Helpers::Pow2F(1.0f - horizontal);
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < numSamples; j++)
			{
				float input = inputs[i][j];

				phase[i] += freq;
				if (phase[i] >= 1.0f)
				{
					phase[i] -= 1.0f;
					hold[i] = floorf(input / step + .5f) * step;
				}

				outputs[i][j] = Helpers::Mix(input, hold[i], dryWet);
			}
		}
	}

	void Crusher::SetParam(int index, float value)
	{
		switch ((ParamIndices)index)
		{
		case ParamIndices::Vertical: vertical = value; break;
		case ParamIndices::Horizontal: horizontal = value; break;
		case ParamIndices::DryWet: dryWet = value; break;
		}
	}

	float Crusher::GetParam(int index) const
	{
		switch ((ParamIndices)index)
		{
		case ParamIndices::Vertical:
		default:
			return vertical;

		case ParamIndices::Horizontal: return horizontal;
		case ParamIndices::DryWet: return dryWet;
		}
	}
}
