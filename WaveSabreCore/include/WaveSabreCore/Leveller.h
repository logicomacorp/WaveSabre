#ifndef __WAVESABRECORE_LEVELLER_H__
#define __WAVESABRECORE_LEVELLER_H__

#include "Device.h"
#include "BiquadFilter.h"

namespace WaveSabreCore
{
	class Leveller : public Device
	{
	public:
		enum class ParamIndices
		{
			LowCutFreq,
			LowCutQ,

			Peak1Freq,
			Peak1Gain,
			Peak1Q,

			Peak2Freq,
			Peak2Gain,
			Peak2Q,

			Peak3Freq,
			Peak3Gain,
			Peak3Q,

			HighCutFreq,
			HighCutQ,

			Master,

			NumParams,
		};

		Leveller();

		virtual void Run(double songPosition, float **inputs, float **outputs, int numSamples);

		virtual void SetParam(int index, float value);
		virtual float GetParam(int index) const;

	private:
		float lowCutFreq, lowCutQ;
		float peak1Freq, peak1Gain, peak1Q;
		float peak2Freq, peak2Gain, peak2Q;
		float peak3Freq, peak3Gain, peak3Q;
		float highCutFreq, highCutQ;
		float master;

		BiquadFilter highpass[2];
		BiquadFilter peak1[2];
		BiquadFilter peak2[2];
		BiquadFilter peak3[2];
		BiquadFilter lowpass[2];
	};
}

#endif
