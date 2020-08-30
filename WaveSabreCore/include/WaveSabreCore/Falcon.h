#ifndef __WAVESABRECORE_FALCON_H__
#define __WAVESABRECORE_FALCON_H__

#include "SynthDevice.h"
#include "Envelope.h"

namespace WaveSabreCore
{
	class Falcon : public SynthDevice
	{
	public:
		enum class ParamIndices
		{
			Osc1Waveform,
			Osc1RatioCoarse,
			Osc1RatioFine,
			Osc1Feedback,
			Osc1FeedForward,

			Osc1Attack,
			Osc1Decay,
			Osc1Sustain,
			Osc1Release,

			Osc2Waveform,
			Osc2RatioCoarse,
			Osc2RatioFine,
			Osc2Feedback,

			Osc2Attack,
			Osc2Decay,
			Osc2Sustain,
			Osc2Release,

			MasterLevel,

			VoicesUnisono,
			VoicesDetune,
			VoicesPan,

			VibratoFreq,
			VibratoAmount,

			Rise,

			PitchAttack,
			PitchDecay,
			PitchSustain,
			PitchRelease,
			PitchEnvAmt1,
			PitchEnvAmt2,

			VoiceMode,
			SlideTime,

			NumParams,
		};

		Falcon();

		virtual void SetParam(int index, float value);
		virtual float GetParam(int index) const;

	protected:
		class FalconVoice : public Voice
		{
		public:
			FalconVoice(Falcon *falcon);
			virtual WaveSabreCore::SynthDevice *GetSynthDevice() const;

			virtual void Run(double songPosition, float **outputs, int numSamples);

			virtual void NoteOn(int note, int velocity, float detune, float pan);
			virtual void NoteOff();

		private:
			Falcon *falcon;

			Envelope osc1Env, osc2Env, pitchEnv;

			double osc1Phase, osc2Phase;
			double osc1Output, osc2Output;
		};

		static double ratioScalar(double coarse, double fine);

		float osc1Waveform, osc1RatioCoarse, osc1RatioFine, osc1Feedback, osc1FeedForward;
		float osc1Attack, osc1Decay, osc1Sustain, osc1Release;
		float osc2Waveform, osc2RatioCoarse, osc2RatioFine, osc2Feedback;
		float osc2Attack, osc2Decay, osc2Sustain, osc2Release;
		float masterLevel;
		float pitchAttack, pitchDecay, pitchSustain, pitchRelease, pitchEnvAmt1, pitchEnvAmt2;
	};
}

#endif
