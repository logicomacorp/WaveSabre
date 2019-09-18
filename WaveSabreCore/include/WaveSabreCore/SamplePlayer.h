#ifndef __WAVESABRECORE_SAMPLEPLAYER_H__
#define __WAVESABRECORE_SAMPLEPLAYER_H__

namespace WaveSabreCore
{
	enum class InterpolationMode
	{
		Nearest,
		Linear,

		NumInterpolationModes,
	};

	enum class LoopMode
	{
		Disabled,
		Repeat,
		PingPong,

		NumLoopModes,
	};

	enum class LoopBoundaryMode
	{
		FromSample,
		Manual,

		NumLoopBoundaryModes,
	};

	class SamplePlayer
	{
	public:
		SamplePlayer();

		void CalcPitch(double note);
		void InitPos();
		void RunPrep();
		float Next();

		bool IsActive;

		float SampleStart;
		bool Reverse;
		WaveSabreCore::LoopMode LoopMode;
		WaveSabreCore::LoopBoundaryMode LoopBoundaryMode;
		float LoopStart, LoopLength;

		WaveSabreCore::InterpolationMode InterpolationMode;

		float *SampleData;
		int SampleLength;
		int SampleLoopStart, SampleLoopLength;

	private:
		double samplePos, sampleDelta;
		int roundedLoopStart, roundedLoopLength, roundedLoopEnd;
		bool reverse;
	};
}

#endif
