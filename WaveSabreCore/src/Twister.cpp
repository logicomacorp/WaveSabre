#include <WaveSabreCore/Twister.h>
#include <WaveSabreCore/Helpers.h>

int const lookAhead = 4;

namespace WaveSabreCore
{
	Twister::Twister()
		: Device((int)ParamIndices::NumParams)
	{
		type = 0;
		amount = 0;
		feedback = 0.0f;
		spread = Spread::Mono;
		vibratoFreq = Helpers::ParamToVibratoFreq(0.0f);
		vibratoAmount = 0.0f;
		
		vibratoPhase = 0.0;
		
		lowCutFreq = 20.0f;
		highCutFreq = 20000.0f- 20.0f;

		dryWet = .5f;

		leftBuffer.SetLength(1000);
		rightBuffer.SetLength(1000);

		lastLeft = 0.0f;
		lastRight = 0.0f;

		for (int i = 0; i < 2; i++)
		{
			lowCutFilter[i].SetType(StateVariableFilterType::Highpass);
			highCutFilter[i].SetType(StateVariableFilterType::Lowpass);
		}
	}

	Twister::~Twister()
	{
	}

	void Twister::Run(double songPosition, float **inputs, float **outputs, int numSamples)
	{
		double vibratoDelta = (vibratoFreq / Helpers::CurrentSampleRate) * 0.25f;
		float outputLeft = 0.0f;
		float outputRight = 0.0f;
		float positionLeft = 0.0f;
		float positionRight = 0.0f;

		for (int i = 0; i < 2; i++)
		{
			lowCutFilter[i].SetFreq(lowCutFreq);
			highCutFilter[i].SetFreq(highCutFreq);
		}

		for (int i = 0; i < numSamples; i++)
		{
			float leftInput = inputs[0][i];
			float rightInput = inputs[1][i];

			double freq = Helpers::FastSin(vibratoPhase) * vibratoAmount;

			switch (spread)
			{
			case Spread::Mono: 
			default:
				positionLeft = Helpers::Clamp((amount + (float)freq), 0.0f, 1.0f);
				positionRight = positionLeft;
				break;
			case Spread::FullInvert:
				positionLeft = Helpers::Clamp((amount + (float)freq), 0.0f, 1.0f);
				positionRight = (1.0f - Helpers::Clamp((amount + (float)freq), 0.0f, 1.0f));
				break;
			case Spread::ModInvert:
				positionLeft = Helpers::Clamp((amount + (float)freq), 0.0f, 1.0f);
				positionRight = Helpers::Clamp((amount - (float)freq), 0.0f, 1.0f);
				break;
			}

			switch (type)
			{
			case 0:
				positionLeft *= 132.0f;
				positionRight *= 132.0f;
				outputLeft = highCutFilter[0].Next(lowCutFilter[0].Next(leftBuffer.ReadPosition(positionLeft + 2)));
				outputRight = highCutFilter[1].Next(lowCutFilter[1].Next(rightBuffer.ReadPosition(positionRight + 2)));
				leftBuffer.WriteSample(leftInput + (outputLeft * feedback));
				rightBuffer.WriteSample(rightInput + (outputRight * feedback));
				break;
			case 1:
				positionLeft *= 132.0f;
				positionRight *= 132.0f;
				outputLeft = highCutFilter[0].Next(lowCutFilter[0].Next(leftBuffer.ReadPosition(positionLeft + 2)));
				outputRight = highCutFilter[1].Next(lowCutFilter[1].Next(rightBuffer.ReadPosition(positionRight + 2)));
				leftBuffer.WriteSample(leftInput - (outputLeft * feedback));
				rightBuffer.WriteSample(rightInput - (outputRight * feedback));
				break;
			case 2:
				for (int i = 0; i<6; i++) allPassLeft[i].Delay(positionLeft);
				for (int i = 0; i<6; i++) allPassRight[i].Delay(positionRight);
				outputLeft = highCutFilter[0].Next(lowCutFilter[0].Next(AllPassUpdateLeft(leftInput + lastLeft * feedback)));
				outputRight = highCutFilter[1].Next(lowCutFilter[1].Next(AllPassUpdateRight(rightInput + lastRight * feedback)));
				lastLeft = outputLeft;
				lastRight = outputRight;
				break;
			case 3:
				for (int i = 0; i<6; i++) allPassLeft[i].Delay(positionLeft);
				for (int i = 0; i<6; i++) allPassRight[i].Delay(positionRight);
				outputLeft = highCutFilter[0].Next(lowCutFilter[0].Next(AllPassUpdateLeft(leftInput - lastLeft * feedback)));
				outputRight = highCutFilter[1].Next(lowCutFilter[1].Next(AllPassUpdateRight(rightInput - lastRight * feedback)));
				lastLeft = outputLeft;
				lastRight = outputRight;
				break;
			default:
				outputLeft = 0.0f;
				outputRight = 0.0f;
				break;
			}

			outputs[0][i] = (leftInput * (1.0f - dryWet)) + (outputLeft * dryWet);
			outputs[1][i] = (rightInput * (1.0f - dryWet)) + (outputRight * dryWet);

			vibratoPhase += vibratoDelta;
		}
	}

	float Twister::AllPassUpdateLeft(float input)
	{
		return(
			allPassLeft[0].Update(
			allPassLeft[1].Update(
			allPassLeft[2].Update(
			allPassLeft[3].Update(
			allPassLeft[4].Update(
			allPassLeft[5].Update(input)))))));
	}

	float Twister::AllPassUpdateRight(float input)
	{
		return(
			allPassRight[0].Update(
			allPassRight[1].Update(
			allPassRight[2].Update(
			allPassRight[3].Update(
			allPassRight[4].Update(
			allPassRight[5].Update(input)))))));
	}

	void Twister::SetParam(int index, float value)
	{
		switch ((ParamIndices)index)
		{
		case ParamIndices::Type: type = (int)(value * 3.0f); break;
		case ParamIndices::Amount: amount = value; break;
		case ParamIndices::Feedback: feedback = value; break;
		case ParamIndices::Spread: spread = Helpers::ParamToSpread(value); break;
		case ParamIndices::VibratoFreq: vibratoFreq = Helpers::ParamToVibratoFreq(value); break;
		case ParamIndices::VibratoAmount: vibratoAmount = value; break;
		case ParamIndices::LowCutFreq: lowCutFreq = Helpers::ParamToFrequency(value); break;
		case ParamIndices::HighCutFreq: highCutFreq = Helpers::ParamToFrequency(value); break;
		case ParamIndices::DryWet: dryWet = value; break;
		}
	}

	float Twister::GetParam(int index) const
	{
		switch ((ParamIndices)index)
		{
		case ParamIndices::Type: 
		default: 
			return type / 3.0f;

		case ParamIndices::Amount: return amount;
		case ParamIndices::Feedback: return feedback;
		case ParamIndices::Spread: return Helpers::SpreadToParam(spread);
		case ParamIndices::VibratoFreq: return Helpers::VibratoFreqToParam(vibratoFreq);
		case ParamIndices::VibratoAmount: return vibratoAmount;
		case ParamIndices::LowCutFreq: return Helpers::FrequencyToParam(lowCutFreq);
		case ParamIndices::HighCutFreq: return Helpers::FrequencyToParam(highCutFreq);
		case ParamIndices::DryWet: return dryWet;
		}
	}
}
