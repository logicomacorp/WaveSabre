#ifndef __WAVESABRECORE_SYNTHDEVICE_H__
#define __WAVESABRECORE_SYNTHDEVICE_H__

#include "Device.h"

namespace WaveSabreCore
{
	class SynthDevice : public Device
	{
	public:
		enum class VoiceMode
		{
			Polyphonic,
			MonoLegatoTrill,
			MonoLegatoAlways,
			NumParams,
		};

		SynthDevice(int numParams);
		virtual ~SynthDevice();

		virtual void Run(double songPosition, float **inputs, float **outputs, int numSamples);
		void RunPolyVoice(double songPosition, float **runningOutputs, int numSamples);
		void RunMonoVoice(double songPosition, float **runningOutputs, int numSamples);

		virtual void AllNotesOff();
		virtual void NoteOn(int note, int velocity, int deltaSamples);
		virtual void NoteOff(int note, int deltaSamples);

		void SetVoiceMode(VoiceMode voiceMode);

		int VoicesUnisono;
		float VoicesDetune;
		float VoicesPan;

		double VibratoFreq;
		float VibratoAmount;

		float Rise;
		float Slide;

	protected:
		static const int maxVoices = 256;
		static const int maxEvents = 256;

		class Voice
		{
		public:
			Voice(SynthDevice *synthDevice);
			virtual ~Voice();

			virtual SynthDevice *synthDevice() const = 0;

			virtual void Run(double songPosition, float **outputs, int numSamples) = 0;

			virtual void NoteOn(int note, int velocity, float detune, float pan);
			virtual void NoteOff();
			virtual void NoteSlide(int note);
			virtual double GetNote();

			bool IsOn;
			int Note;

			float Detune;
			float Pan;

			double vibratoPhase;
		private:
			bool slideActive;
			double slideDelta;
			int slideSamples;
			int destinationNote;
			double currentNote;
		};

		enum class EventType
		{
			None,
			NoteOn,
			NoteOff,
		};

		struct Event
		{
			EventType Type;
			int DeltaSamples;

			int Note, Velocity;
		};

		bool monoActive;
		int noteLog[127];
		int noteCount;
		bool activeNotes[127];
		VoiceMode voiceMode;

		void clearEvents();

		Voice *voices[maxVoices];

		Event events[maxEvents];
	};
}

#endif
