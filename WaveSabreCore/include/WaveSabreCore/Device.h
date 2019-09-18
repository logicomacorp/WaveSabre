#ifndef __WAVESABRECORE_DEVICE_H__
#define __WAVESABRECORE_DEVICE_H__

namespace WaveSabreCore
{
	class Device
	{
	public:
		Device(int numParams);
		virtual ~Device();

		virtual void Run(double songPosition, float **inputs, float **outputs, int numSamples) = 0;

		virtual void AllNotesOff();
		virtual void NoteOn(int note, int velocity, int deltaSamples);
		virtual void NoteOff(int note, int deltaSamples);

		virtual void SetSampleRate(float sampleRate);
		virtual void SetTempo(int tempo);

		virtual void SetParam(int index, float value);
		virtual float GetParam(int index) const;

		virtual void SetChunk(void *data, int size);
		virtual int GetChunk(void **data);

	protected:
		virtual void clearOutputs(float **outputs, int numSamples);

		int numParams;
		void *chunkData;
	};
}

#endif
