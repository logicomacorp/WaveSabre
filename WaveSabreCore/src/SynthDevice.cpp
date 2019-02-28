#include <WaveSabreCore/SynthDevice.h>

#include <WaveSabreCore/Helpers.h>

#include <Windows.h>

namespace WaveSabreCore
{
	SynthDevice::SynthDevice(int numParams)
		: Device(numParams)
	{
		VoicesUnisono = 1;
		VoicesDetune = 0.0f;
		VoicesPan = .5f;

		VibratoFreq = Helpers::ParamToVibratoFreq(0.0f);
		VibratoAmount = 0.0f;

		Rise = 0.0f;

		clearEvents();
	}

	SynthDevice::~SynthDevice()
	{
		for (int i = 0; i < maxVoices; i++) delete voices[i];
	}

	void SynthDevice::Run(double songPosition, float **inputs, float **outputs, int numSamples)
	{
		int originalNumSamples = numSamples;
		clearOutputs(outputs, numSamples);

		float *runningOutputs[2];
		runningOutputs[0] = outputs[0];
		runningOutputs[1] = outputs[1];

		while (numSamples)
		{
			int samplesToNextEvent = numSamples;
			for (int i = 0; i < maxEvents; i++)
			{
				Event *e = &events[i];
				if (e->Type != EventType::None)
				{
					if (!e->DeltaSamples)
					{
						switch (e->Type)
						{
						case EventType::NoteOn:
							{
								int j = VoicesUnisono;
								for (int k = 0; j && k < maxVoices; k++)
								{
									if (!voices[k]->IsOn)
									{
										j--;
										float f = (float)j / (VoicesUnisono > 1 ? (float)(VoicesUnisono - 1) : 1.0f);
										voices[k]->NoteOn(e->Note, e->Velocity, f * VoicesDetune, (f - .5f) * (VoicesPan * 2.0f - 1.0f) + .5f);
									}
								}
							}
							break;

						case EventType::NoteOff:
							for (int j = 0; j < maxVoices; j++)
							{
								if (voices[j]->IsOn && voices[j]->Note == e->Note) voices[j]->NoteOff();
							}
						}
						events[i].Type = EventType::None;
					}
					else if (e->DeltaSamples < samplesToNextEvent)
					{
						samplesToNextEvent = e->DeltaSamples;
					}
				}
			}

			for (int i = 0; i < maxVoices; i++)
			{
				if (voices[i]->IsOn) voices[i]->Run(songPosition, runningOutputs, samplesToNextEvent);
			}
			for (int i = 0; i < maxEvents; i++)
			{
				if (events[i].Type != EventType::None) events[i].DeltaSamples -= samplesToNextEvent;
			}
			songPosition += (double)samplesToNextEvent / Helpers::CurrentSampleRate;
			runningOutputs[0] += samplesToNextEvent;
			runningOutputs[1] += samplesToNextEvent;
			numSamples -= samplesToNextEvent;
		}
	}

	void SynthDevice::AllNotesOff()
	{
		for (int i = 0; i < maxVoices; i++)
		{
			if (voices[i]->IsOn) voices[i]->NoteOff();
		}
		clearEvents();
	}

	void SynthDevice::NoteOn(int note, int velocity, int deltaSamples)
	{
		for (int i = 0; i < maxEvents; i++)
		{
			if (events[i].Type == EventType::None)
			{
				events[i].Type = EventType::NoteOn;
				events[i].DeltaSamples = deltaSamples;
				events[i].Note = note;
				events[i].Velocity = velocity;
				break;
			}
		}
	}

	void SynthDevice::NoteOff(int note, int deltaSamples)
	{
		for (int i = 0; i < maxEvents; i++)
		{
			if (events[i].Type == EventType::None)
			{
				events[i].Type = EventType::NoteOff;
				events[i].DeltaSamples = deltaSamples;
				events[i].Note = note;
				break;
			}
		}
	}

	SynthDevice::Voice::Voice()
	{
		IsOn = false;
		vibratoPhase = 0.0;
	}

	SynthDevice::Voice::~Voice() { }

	void SynthDevice::Voice::NoteOn(int note, int velocity, float detune, float pan)
	{
		IsOn = true;
		Note = note;
		Detune = detune;
		Pan = pan;
	}

	void SynthDevice::Voice::NoteOff()
	{
	}

	void SynthDevice::clearEvents()
	{
		for (int i = 0; i < maxEvents; i++) events[i].Type = EventType::None;
	}
}
