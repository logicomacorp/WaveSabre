#ifndef __WAVESABRECORE_BIQUADFILTER_H__
#define __WAVESABRECORE_BIQUADFILTER_H__

namespace WaveSabreCore
{
	enum class BiquadFilterType
	{
		Lowpass,
		Highpass,
		Peak,
	};

	class BiquadFilter
	{
	public:
		BiquadFilter();

		float Next(float input);

		void SetType(BiquadFilterType type);
		void SetFreq(float freq);
		void SetQ(float q);
		void SetGain(float gain);

	private:
		bool recalculate;

		BiquadFilterType type;
		float freq;
		float q;
		float gain;

		float c1, c2, c3, c4, c5;

		float lastInput, lastLastInput;
		float lastOutput, lastLastOutput;
	};
}

#endif
