#ifndef __WAVESABRECORE_ADULTERY_H__
#define __WAVESABRECORE_ADULTERY_H__

#include "SynthDevice.h"
#include "Envelope.h"
#include "StateVariableFilter.h"
#include "SamplePlayer.h"

namespace WaveSabreCore
{
	class Adultery : public SynthDevice
	{
	public:
		enum class ParamIndices
		{
			SampleIndex,

			AmpAttack,
			AmpDecay,
			AmpSustain,
			AmpRelease,

			SampleStart,
			LoopMode,
			LoopBoundaryMode,
			LoopStart,
			LoopLength,
			Reverse,

			InterpolationMode,

			CoarseTune,
			FineTune,

			FilterType,
			FilterFreq,
			FilterResonance,
			FilterModAmt,

			ModAttack,
			ModDecay,
			ModSustain,
			ModRelease,

			VoicesUnisono,
			VoicesDetune,
			VoicesPan,

			Master,

			VoiceMode,
			SlideTime,

			NumParams,
		};

		Adultery();
		virtual ~Adultery();

		virtual void SetParam(int index, float value);
		virtual float GetParam(int index) const;

	private:
		class AdulteryVoice : public Voice
		{
		public:
			AdulteryVoice(Adultery *adultery);
			virtual WaveSabreCore::SynthDevice *GetSynthDevice() const;

			virtual void Run(double songPosition, float **outputs, int numSamples);

			virtual void NoteOn(int note, int velocity, float detune, float pan);
			virtual void NoteOff();

		private:
			double coarseDetune(float detune);
			void calcPitch();

			Adultery *adultery;

			Envelope ampEnv, modEnv;

			SamplePlayer samplePlayer;

			StateVariableFilter filter;

			float velocity;
		};

		int sampleIndex;

		float ampAttack, ampDecay, ampSustain, ampRelease;
		float sampleStart;
		bool reverse;
		LoopMode loopMode;
		LoopBoundaryMode loopBoundaryMode;
		float loopStart, loopLength;

		InterpolationMode interpolationMode;

		float *sampleData;
		int sampleLength;
		int sampleLoopStart, sampleLoopLength;
		float coarseTune, fineTune;
		float masterLevel;
		StateVariableFilterType filterType;
		float filterFreq, filterResonance, filterModAmt;
		float modAttack, modDecay, modSustain, modRelease;
	};
}

#endif
