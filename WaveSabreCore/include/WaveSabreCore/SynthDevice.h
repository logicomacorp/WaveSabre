#ifndef __WAVESABRECORE_SYNTHDEVICE_H__
#define __WAVESABRECORE_SYNTHDEVICE_H__

#include "Device.h"

namespace WaveSabreCore
{
	class SynthDevice : public Device
	{
	public:
		SynthDevice(int numParams);
		virtual ~SynthDevice();

		virtual void Run(double songPosition, float **inputs, float **outputs, int numSamples);

		virtual void AllNotesOff();
		virtual void NoteOn(int note, int velocity, int deltaSamples);
		virtual void NoteOff(int note, int deltaSamples);

		int VoicesUnisono;
		float VoicesDetune;
		float VoicesPan;

		double VibratoFreq;
		float VibratoAmount;

		float Rise;

	protected:
		static const int maxVoices = 256;
		static const int maxEvents = 256;

		class Voice
		{
		public:
			Voice();
			virtual ~Voice();

			virtual void Run(double songPosition, float **outputs, int numSamples) = 0;

			virtual void NoteOn(int note, int velocity, float detune, float pan);
			virtual void NoteOff();

			bool IsOn;
			int Note;

			float Detune;
			float Pan;

			double vibratoPhase;
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

		void clearEvents();

		Voice *voices[maxVoices];

		Event events[maxEvents];
	};
}

#endif
