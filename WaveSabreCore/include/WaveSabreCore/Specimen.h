#ifndef __WAVESABRECORE_SPECIMEN_H__
#define __WAVESABRECORE_SPECIMEN_H__

#include "SynthDevice.h"
#include "Envelope.h"
#include "StateVariableFilter.h"
#include "SamplePlayer.h"
#include "GsmSample.h"

namespace WaveSabreCore
{
	class Specimen : public SynthDevice
	{
	public:
		enum class ParamIndices
		{
			AmpAttack,
			AmpDecay,
			AmpSustain,
			AmpRelease,

			SampleStart,
			LoopMode,
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

		static const int SampleRate = 44100;

		Specimen();
		virtual ~Specimen();

		virtual void SetParam(int index, float value);
		virtual float GetParam(int index) const;

		virtual void SetChunk(void *data, int size);
		virtual int GetChunk(void **data);

		void LoadSample(char *compressedDataPtr, int compressedSize, int uncompressedSize, WAVEFORMATEX *waveFormatPtr);

	private:
		class SpecimenVoice : public Voice
		{
		public:
			SpecimenVoice(Specimen *thunder);
			virtual WaveSabreCore::SynthDevice *GetSynthDevice() const;

			virtual void Run(double songPosition, float **outputs, int numSamples);

			virtual void NoteOn(int note, int velocity, float detune, float pan);
			virtual void NoteOff();

		private:
			double coarseDetune(float detune);
			void calcPitch();

			Specimen *specimen;

			Envelope ampEnv, modEnv;

			SamplePlayer samplePlayer;

			StateVariableFilter filter;

			float velocity;
		};

		char *chunkData;

		GsmSample* sample;

		float ampAttack, ampDecay, ampSustain, ampRelease;
		float sampleStart;
		bool reverse;
		LoopMode loopMode;
		LoopBoundaryMode loopBoundaryMode;
		float loopStart, loopLength;

		InterpolationMode interpolationMode;

		int sampleLoopStart, sampleLoopLength;
		float coarseTune, fineTune;
		float masterLevel;
		StateVariableFilterType filterType;
		float filterFreq, filterResonance, filterModAmt;
		float modAttack, modDecay, modSustain, modRelease;
	};
}

#endif
