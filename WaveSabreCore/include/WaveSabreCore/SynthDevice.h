#ifndef __WAVESABRECORE_SYNTHDEVICE_H__
#define __WAVESABRECORE_SYNTHDEVICE_H__

#include "Device.h"

namespace WaveSabreCore
{
	enum class VoiceMode
	{
		Polyphonic,
		MonoLegatoTrill,
	};

	class SynthDevice : public Device
	{
	public:
		SynthDevice(int numParams);
		virtual ~SynthDevice();

		virtual void Run(double songPosition, float **inputs, float **outputs, int numSamples);

		virtual void AllNotesOff();
		virtual void NoteOn(int note, int velocity, int deltaSamples);
		virtual void NoteOff(int note, int deltaSamples);

		void SetVoiceMode(VoiceMode voiceMode);
		VoiceMode GetVoiceMode() const;

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
		static const int maxActiveNotes = 128;

		class Voice
		{
		public:
			Voice();
			virtual ~Voice();

			virtual SynthDevice *GetSynthDevice() const = 0;

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
		int noteLog[maxActiveNotes];
		int noteCount;
		bool activeNotes[maxActiveNotes];

		void clearEvents();

		Voice *voices[maxVoices];
		VoiceMode voiceMode;
		Event events[maxEvents];
	};
}

#endif
