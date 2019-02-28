#include <WaveSabreCore/Smasher.h>
#include <WaveSabreCore/Helpers.h>

#include <math.h>

namespace WaveSabreCore
{
	const float Smasher::lookaheadMs = 2.0f;

	Smasher::Smasher()
		: Device((int)ParamIndices::NumParams)
	{
		sidechain = false;
		inputGain = 0.0f;
		threshold = 0.0f;
		ratio = 2.0f;
		attack = 1.0f;
		release = 200.0f;
		outputGain = 0.0f;

		peak = 0.0f;
	}

	Smasher::~Smasher()
	{
	}

	void Smasher::Run(double songPosition, float **inputs, float **outputs, int numSamples)
	{
		leftBuffer.SetLength(lookaheadMs);
		rightBuffer.SetLength(lookaheadMs);

		float inputGainScalar = Helpers::DbToScalar(inputGain);
		float outputGainScalar = Helpers::DbToScalar(outputGain);
		int inputChannelOffset = sidechain ? 2 : 0;

		float envCoeff = (float)(1000.0 / Helpers::CurrentSampleRate);
		float attackScalar = envCoeff / attack;
		float releaseScalar = envCoeff / release;

		float thresholdScalar = Helpers::DbToScalar(threshold);

		for (int i = 0; i < numSamples; i++)
		{
			leftBuffer.WriteSample(inputs[0][i] * inputGainScalar);
			rightBuffer.WriteSample(inputs[1][i] * inputGainScalar);
			float inputLeft = inputs[inputChannelOffset][i] * inputGainScalar;
			float inputRight = inputs[inputChannelOffset + 1][i] * inputGainScalar;
			float inputLeftLevel = fabsf(inputLeft);
			float inputRightLevel = fabsf(inputRight);
			float inputLevel = inputLeftLevel >= inputRightLevel ? inputLeftLevel : inputRightLevel;

			if (inputLevel > peak)
			{
				peak += attackScalar;
				if (peak > inputLevel) peak = inputLevel;
			}
			else
			{
				peak -= releaseScalar;
				if (peak < inputLevel) peak = inputLevel;
			}

			float gainScalar = outputGainScalar;
			if (peak > thresholdScalar) gainScalar *= (thresholdScalar + (peak - thresholdScalar) / ratio) / peak;

			outputs[0][i] = leftBuffer.ReadSample() * gainScalar;
			outputs[1][i] = rightBuffer.ReadSample() * gainScalar;
		}
	}

	void Smasher::SetParam(int index, float value)
	{
		switch ((ParamIndices)index)
		{
		case ParamIndices::Sidechain: sidechain = Helpers::ParamToBoolean(value); break;
		case ParamIndices::InputGain: inputGain = Helpers::ParamToDb(value, 12.0f); break;
		case ParamIndices::Threshold: threshold = Helpers::ParamToDb(value / 2.0f, 36.0f); break;
		case ParamIndices::Attack: attack = Helpers::ScalarToEnvValue(value) / 5.0f; break;
		case ParamIndices::Release: release = Helpers::ScalarToEnvValue(value); break;
		case ParamIndices::Ratio: ratio = value * value * 18.0f + 2.0f; break;
		case ParamIndices::OutputGain: outputGain = Helpers::ParamToDb(value, 12.0f); break;
		}
	}

	float Smasher::GetParam(int index) const
	{
		switch ((ParamIndices)index)
		{
		case ParamIndices::Sidechain:
		default:
			return Helpers::BooleanToParam(sidechain);

		case ParamIndices::InputGain: return Helpers::DbToParam(inputGain, 12.0f);
		case ParamIndices::Threshold: return Helpers::DbToParam(threshold, 36.0f) * 2.0f;
		case ParamIndices::Attack: return Helpers::EnvValueToScalar(attack * 5.0f); break;
		case ParamIndices::Release: return Helpers::EnvValueToScalar(release); break;
		case ParamIndices::Ratio: return sqrtf((ratio - 2.0f) / 18.0f);
		case ParamIndices::OutputGain: return Helpers::DbToParam(outputGain, 12.0f);
		}
		return 0.0f;
	}
}
