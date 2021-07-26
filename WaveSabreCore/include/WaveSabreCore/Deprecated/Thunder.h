#ifndef __WAVESABRECORE_THUNDER_H__
#define __WAVESABRECORE_THUNDER_H__

#include "../SynthDevice.h"
#include "../GsmSample.h"

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

		void LoadSample(char *compressedDataPtr, int compressedSize, int uncompressedSize, WAVEFORMATEX *waveFormatPtr);

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

		GsmSample *sample;
	};
}

#endif
