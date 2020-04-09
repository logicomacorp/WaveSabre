#include <WaveSabreCore/Scissor.h>
#include <WaveSabreCore/Helpers.h>

namespace WaveSabreCore
{
	Scissor::Scissor()
		: Device((int)ParamIndices::NumParams)
	{
		type = ShaperType::Clipper;
		drive = .2f;
		threshold = .8f;
		foldover = 0.0f;
		dryWet = 1.0f;
		oversampling = Oversampling::X1;

		lastSample[0] = lastSample[1] = 0.0f;
	}

	void Scissor::Run(double songPosition, float **inputs, float **outputs, int numSamples)
	{
		float driveScalar;
		if (drive < .2f)
		{
			driveScalar = 1.0f - (.2f - drive);
		}
		else
		{
			driveScalar = 1.0f + Helpers::Pow2F((drive - .2f) * 5.0f) * 5.0f;
		}

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < numSamples; j++)
			{
				float input = inputs[i][j];

				float v = distort(input, driveScalar);

				switch (oversampling)
				{
				case Oversampling::X2:
					{
						float inputMid = (lastSample[i] + input) * 0.5f;
						float vMid = distort(inputMid, driveScalar);
						v = (vMid + v) * 0.5f;
					}
					break;

				case Oversampling::X4:
					{
						float inputMid = (lastSample[i] + input) * 0.5f;
						float inputQ1 = (lastSample[i] + inputMid) * 0.5f;
						float inputQ2 = (inputMid + input) * 0.5f;
						float vQ1 = distort(inputQ1, driveScalar);
						float vMid = distort(inputMid, driveScalar);
						float vQ2 = distort(inputQ2, driveScalar);
						v = (vQ1 + vMid + vQ2 + v) * 0.25f;
					}
					break;
				}

				outputs[i][j] = Helpers::Mix(input, v, dryWet);

				lastSample[i] = input;
			}
		}
	}

	void Scissor::SetParam(int index, float value)
	{
		switch ((ParamIndices)index)
		{
		case ParamIndices::Drive: drive = value; break;
		case ParamIndices::Threshold: threshold = value; break;
		case ParamIndices::Foldover: foldover = value; break;
		case ParamIndices::DryWet: dryWet = value; break;
		case ParamIndices::Type: type = (ShaperType)(int)(value * 2.0f); break;
		case ParamIndices::Oversampling: oversampling = (Oversampling)(int)(value * 2.0f); break;
		}
	}

	float Scissor::GetParam(int index) const
	{
		switch ((ParamIndices)index)
		{
		case ParamIndices::Drive:
		default:
			return drive;

		case ParamIndices::Threshold: return threshold;
		case ParamIndices::Foldover: return foldover;
		case ParamIndices::DryWet: return dryWet;
		case ParamIndices::Type: return (float)type / 2.0f;
		case ParamIndices::Oversampling: return (float)oversampling / 2.0f;
		}
	}

	float Scissor::distort(float v, float driveScalar)
	{
		v /= threshold;

		v *= driveScalar;

		switch (type)
		{
		case ShaperType::Clipper:
			if (foldover > 0.0f)
			{
				if (v < -1.0f) v = -1.0f + (-1.0f - v) * foldover;
				else if (v > 1.0f) v = 1.0f + (1.0f - v) * foldover;
			}
			break;

		case ShaperType::Sine: v = (v * 3.141592f / 2.0f); break;
		case ShaperType::Parabola: v = v * v * v;
		}

		if (v < -1.0f) v = -1.0f;
		else if (v > 1.0f) v = 1.0f;

		v *= threshold;

		return v;
	}
}
