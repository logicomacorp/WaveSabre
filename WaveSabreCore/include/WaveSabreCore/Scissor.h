#ifndef __WAVESABRECORE_SCISSOR_H__
#define __WAVESABRECORE_SCISSOR_H__

#include "Device.h"

namespace WaveSabreCore
{
	class Scissor : public Device
	{
	public:
		enum class ParamIndices
		{
			Drive,
			Threshold,
			Foldover,

			DryWet,

			Type,

			Oversampling,

			NumParams,
		};

		Scissor();

		virtual void Run(double songPosition, float **inputs, float **outputs, int numSamples);

		virtual void SetParam(int index, float value);
		virtual float GetParam(int index) const;

	private:
		enum class ShaperType
		{
			Clipper,
			Sine,
			Parabola,
		};

		enum class Oversampling
		{
			X1,
			X2,
			X4,
		};

		float distort(float v, float driveScalar);

		ShaperType type;
		float drive, threshold, foldover, dryWet;
		Oversampling oversampling;

		float lastSample[2];
	};
}

#endif
