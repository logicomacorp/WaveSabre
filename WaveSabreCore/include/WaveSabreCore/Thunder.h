#ifndef __WAVESABRECORE_THUNDER_H__
#define __WAVESABRECORE_THUNDER_H__

#include "SynthDevice.h"
#include "SampleLoader.h"

namespace WaveSabreCore
{
	class Thunder : public SynthDevice
	{
	public:
		static const int SampleRate = 44100;

		Thunder();
		virtual ~Thunder();

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
		}

	private:
		class ThunderVoice : public Voice
		{
		public:
			ThunderVoice(Thunder *thunder);
			virtual WaveSabreCore::SynthDevice *GetSynthDevice() const;

			virtual void Run(double songPosition, float **outputs, int numSamples);

			virtual void NoteOn(int note, int velocity, float detune, float pan);

		private:
			Thunder *thunder;

			int samplePos;
		};

		char *chunkData;

		char *waveFormatData;
		int compressedSize, uncompressedSize;

		char *compressedData;
		float *sampleData;

		int sampleLength;
	};
}

#endif
