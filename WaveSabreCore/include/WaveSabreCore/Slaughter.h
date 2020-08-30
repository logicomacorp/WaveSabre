#ifndef __WAVESABRECORE_SLAUGHTER_H__
#define __WAVESABRECORE_SLAUGHTER_H__

#include "SynthDevice.h"
#include "Envelope.h"
#include "StateVariableFilter.h"

namespace WaveSabreCore
{
	class Slaughter : public SynthDevice
	{
	public:
		enum class ParamIndices
		{
			Osc1Waveform,
			Osc1PulseWidth,
			Osc1Volume,
			Osc1DetuneCoarse,
			Osc1DetuneFine,

			Osc2Waveform,
			Osc2PulseWidth,
			Osc2Volume,
			Osc2DetuneCoarse,
			Osc2DetuneFine,

			Osc3Waveform,
			Osc3PulseWidth,
			Osc3Volume,
			Osc3DetuneCoarse,
			Osc3DetuneFine,

			NoiseVolume,

			FilterType,
			FilterFreq,
			FilterResonance,
			FilterModAmt,

			AmpAttack,
			AmpDecay,
			AmpSustain,
			AmpRelease,

			ModAttack,
			ModDecay,
			ModSustain,
			ModRelease,

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
			PitchEnvAmt,

			VoiceMode,
			SlideTime,

			NumParams,
		};

		Slaughter();

		virtual void SetParam(int index, float value);
		virtual float GetParam(int index) const;

	protected:
		class SlaughterVoice : public Voice
		{
		public:
			SlaughterVoice(Slaughter *slaughter);
			virtual WaveSabreCore::SynthDevice *GetSynthDevice() const;

			virtual void Run(double songPosition, float **outputs, int numSamples);

			virtual void NoteOn(int note, int velocity, float detune, float pan);
			virtual void NoteOff();

		private:
			class Oscillator
			{
			public:
				float Next(double note, float waveform, float pulseWidth);

				double Phase;
				double Integral;
			};

			double coarseDetune(float detune);

			Slaughter *slaughter;

			Envelope ampEnv, modEnv, pitchEnv;

			Oscillator osc1, osc2, osc3;
			StateVariableFilter filter;
		};

		float osc1Waveform, osc1PulseWidth, osc1Volume, osc1DetuneCoarse, osc1DetuneFine;
		float osc2Waveform, osc2PulseWidth, osc2Volume, osc2DetuneCoarse, osc2DetuneFine;
		float osc3Waveform, osc3PulseWidth, osc3Volume, osc3DetuneCoarse, osc3DetuneFine;
		float noiseVolume;
		StateVariableFilterType filterType;
		float filterFreq, filterResonance, filterModAmt;
		float ampAttack, ampDecay, ampSustain, ampRelease;
		float modAttack, modDecay, modSustain, modRelease;
		float pitchAttack, pitchDecay, pitchSustain, pitchRelease, pitchEnvAmt;
		float masterLevel;
	};
}

#endif
