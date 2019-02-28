#ifndef __WAVESABRECORE_ECHO_H__
#define __WAVESABRECORE_ECHO_H__

#include "Device.h"
#include "DelayBuffer.h"
#include "StateVariableFilter.h"

namespace WaveSabreCore
{
	class Echo : public Device
	{
	public:
		enum class ParamIndices
		{
			LeftDelayCoarse,
			LeftDelayFine,

			RightDelayCoarse,
			RightDelayFine,

			LowCutFreq,
			HighCutFreq,

			Feedback,
			Cross,

			DryWet,

			NumParams,
		};

		Echo();
		virtual ~Echo();

		virtual void Run(double songPosition, float **inputs, float **outputs, int numSamples);

		virtual void SetParam(int index, float value);
		virtual float GetParam(int index) const;

	private:
		int leftDelayCoarse, leftDelayFine;
		int rightDelayCoarse, rightDelayFine;
		float lowCutFreq, highCutFreq;
		float feedback, cross;
		float dryWet;

		DelayBuffer leftBuffer;
		DelayBuffer rightBuffer;

		StateVariableFilter lowCutFilter[2], highCutFilter[2];
	};
}

#endif
