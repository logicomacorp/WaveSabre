#include <WaveSabreCore/Leveller.h>
#include <WaveSabreCore/Helpers.h>

#include <math.h>

namespace WaveSabreCore
{
	Leveller::Leveller()
		: Device((int)ParamIndices::NumParams)
	{
		lowCutFreq = 20.0f;
		lowCutQ = 1.0f;

		peak1Freq = 1000.0f;
		peak1Gain = 0.0f;
		peak1Q = 1.0f;

		peak2Freq = 3000.0f;
		peak2Gain = 0.0f;
		peak2Q = 1.0f;

		peak3Freq = 7000.0f;
		peak3Gain = 0.0f;
		peak3Q = 1.0f;

		highCutFreq = 20000.0f;
		highCutQ = 1.0f;

		for (int i = 0; i < 2; i++)
		{
			highpass[i].SetType(BiquadFilterType::Highpass);
			peak1[i].SetType(BiquadFilterType::Peak);
			peak2[i].SetType(BiquadFilterType::Peak);
			peak3[i].SetType(BiquadFilterType::Peak);
		}

		master = 1.0f;
	}

	void Leveller::Run(double songPosition, float **inputs, float **outputs, int numSamples)
	{
		for (int i = 0; i < 2; i++)
		{
			highpass[i].SetFreq(lowCutFreq);
			highpass[i].SetQ(lowCutQ);

			lowpass[i].SetFreq(highCutFreq);
			lowpass[i].SetQ(highCutQ);

			peak1[i].SetFreq(peak1Freq);
			peak1[i].SetGain(peak1Gain);
			peak1[i].SetQ(peak1Q);

			peak2[i].SetFreq(peak2Freq);
			peak2[i].SetGain(peak2Gain);
			peak2[i].SetQ(peak2Q);

			peak3[i].SetFreq(peak3Freq);
			peak3[i].SetGain(peak3Gain);
			peak3[i].SetQ(peak3Q);

			for (int j = 0; j < numSamples; j++)
			{
				float sample = inputs[i][j];

				sample = highpass[i].Next(sample);
				if (peak1Gain != 0.0f) sample = peak1[i].Next(sample);
				if (peak2Gain != 0.0f) sample = peak2[i].Next(sample);
				if (peak3Gain != 0.0f) sample = peak3[i].Next(sample);
				sample = lowpass[i].Next(sample);

				outputs[i][j] = sample * master;
			}
		}
	}

	void Leveller::SetParam(int index, float value)
	{
		switch ((ParamIndices)index)
		{
		case ParamIndices::LowCutFreq: lowCutFreq = Helpers::ParamToFrequency(value); break;
		case ParamIndices::LowCutQ: lowCutQ = Helpers::ParamToQ(value); break;

		case ParamIndices::Peak1Freq: peak1Freq = Helpers::ParamToFrequency(value); break;
		case ParamIndices::Peak1Gain: peak1Gain = Helpers::ParamToDb(value); break;
		case ParamIndices::Peak1Q: peak1Q = Helpers::ParamToQ(value); break;

		case ParamIndices::Peak2Freq: peak2Freq = Helpers::ParamToFrequency(value); break;
		case ParamIndices::Peak2Gain: peak2Gain = Helpers::ParamToDb(value); break;
		case ParamIndices::Peak2Q: peak2Q = Helpers::ParamToQ(value); break;

		case ParamIndices::Peak3Freq: peak3Freq = Helpers::ParamToFrequency(value); break;
		case ParamIndices::Peak3Gain: peak3Gain = Helpers::ParamToDb(value); break;
		case ParamIndices::Peak3Q: peak3Q = Helpers::ParamToQ(value); break;

		case ParamIndices::HighCutFreq: highCutFreq = Helpers::ParamToFrequency(value); break;
		case ParamIndices::HighCutQ: highCutQ = Helpers::ParamToQ(value);; break;

		case ParamIndices::Master: master = value; break;
		}
	}

	float Leveller::GetParam(int index) const
	{
		switch ((ParamIndices)index)
		{
		case ParamIndices::LowCutFreq:
		default:
			return Helpers::FrequencyToParam(lowCutFreq);

		case ParamIndices::LowCutQ: return Helpers::QToParam(lowCutQ);

		case ParamIndices::Peak1Freq: return Helpers::FrequencyToParam(peak1Freq);
		case ParamIndices::Peak1Gain: return Helpers::DbToParam(peak1Gain);
		case ParamIndices::Peak1Q: return Helpers::QToParam(peak1Q);

		case ParamIndices::Peak2Freq: return Helpers::FrequencyToParam(peak2Freq);
		case ParamIndices::Peak2Gain: return Helpers::DbToParam(peak2Gain);
		case ParamIndices::Peak2Q: return Helpers::QToParam(peak2Q);

		case ParamIndices::Peak3Freq: return Helpers::FrequencyToParam(peak3Freq);
		case ParamIndices::Peak3Gain: return Helpers::DbToParam(peak3Gain);
		case ParamIndices::Peak3Q: return Helpers::QToParam(peak3Q);

		case ParamIndices::HighCutFreq: return Helpers::FrequencyToParam(highCutFreq);
		case ParamIndices::HighCutQ: return Helpers::QToParam(highCutQ);
		
		case ParamIndices::Master: return master;
		}
	}
}
