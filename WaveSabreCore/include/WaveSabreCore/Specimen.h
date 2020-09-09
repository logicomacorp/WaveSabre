#ifndef __WAVESABRECORE_SPECIMEN_H__
#define __WAVESABRECORE_SPECIMEN_H__

#include "SynthDevice.h"
#include "Envelope.h"
#include "StateVariableFilter.h"
#include "SampleLoader.h"
#include "SamplePlayer.h"

namespace WaveSabreCore
{
	class Specimen : public SynthDevice
	{
	public:
		static const int SampleRate = SampleLoader::SampleRate;

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

		Specimen();
		virtual ~Specimen();

		virtual void SetParam(int index, float value);
		virtual float GetParam(int index) const;

		virtual void SetChunk(void *data, int size);
		virtual int GetChunk(void **data);

		inline void LoadSample(char *compressedDataPtr, int compressedSize,
				int uncompressedSize, WAVEFORMATEX *waveFormatPtr)
		{
			auto sample = SampleLoader::LoadSampleGSM(compressedDataPtr,
					compressedSize, uncompressedSize, waveFormatPtr);

			this->compressedSize = sample.compressedSize;
			this->uncompressedSize = sample.uncompressedSize;

			if (waveFormatData) delete [] waveFormatData;
			waveFormatData = sample.waveFormatData;
			if (compressedData) delete [] compressedData;
			compressedData = sample.compressedData;
			if (sampleData) delete [] sampleData;
			sampleData = sample.sampleData;

			sampleLength = sample.sampleLength;
			sampleLoopStart = 0;
			sampleLoopLength = sampleLength;
		}
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

		char *waveFormatData;
		int compressedSize, uncompressedSize;

		char *compressedData;
		float *sampleData;

		float ampAttack, ampDecay, ampSustain, ampRelease;
		float sampleStart;
		bool reverse;
		LoopMode loopMode;
		LoopBoundaryMode loopBoundaryMode;
		float loopStart, loopLength;

		InterpolationMode interpolationMode;

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
