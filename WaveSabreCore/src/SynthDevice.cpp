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
		Slide = 0.0f;

		voiceMode = VoiceMode::Polyphonic;
		monoActive = false;
		noteCount = 0;

		for (int i = 0; i < 127; i++)
		{
			activeNotes[i] = false;
			noteLog[i] = 0;
		}

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

		switch (voiceMode)
		{
			case VoiceMode::Polyphonic:
			default:
				RunPolyVoice(songPosition, runningOutputs, numSamples);
				break;
			case VoiceMode::MonoLegatoTrill:
			case VoiceMode::MonoLegatoAlways:
				RunMonoVoice(songPosition, runningOutputs, numSamples);
				break;
		}
	}

	void SynthDevice::RunPolyVoice(double songPosition, float **runningOutputs, int numSamples)
	{
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

	void SynthDevice::RunMonoVoice(double songPosition, float **runningOutputs, int numSamples)
	{
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
							if (!monoActive)  // no current note active, start new one
							{
								monoActive = true;
								activeNotes[e->Note] = true;
								noteLog[noteCount] = e->Note;
								noteCount++;
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
							else   // note active, slide to new note
							{
								activeNotes[e->Note] = true;
								noteLog[noteCount] = e->Note;
								noteCount++;
								for (int j = 0; j < maxVoices; j++)
								{
									if (voices[j]->IsOn)
									{
										voices[j]->NoteSlide(e->Note);
									}
								}
							}
						}
							break;

						case EventType::NoteOff:
							activeNotes[e->Note] = false;
							if (e->Note == noteLog[noteCount - 1])	// note off is last note played, find last active note
							{
								for (; noteCount > 0; noteCount--)
								{
									if (activeNotes[noteLog[noteCount - 1]])
									{
										for (int j = 0; j < maxVoices; j++)
										{
											if (voices[j]->IsOn)
											{
												voices[j]->NoteSlide(noteLog[noteCount - 1]);
											}
										}
										break;
									}
								}

								if (noteCount <= 0)   // no notes left, switch of the voices
								{
									monoActive = false;
									noteCount = 0;
									for (int j = 0; j < 127; j++) activeNotes[j] = false;
									for (int j = 0; j < maxVoices; j++)
									{
										if (voices[j]->IsOn) voices[j]->NoteOff();
									}
								}
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
		monoActive = false;
		noteCount = 0;
		for (int i = 0; i < 127; i++) activeNotes[i] = false;
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

	void SynthDevice::SetVoiceMode(VoiceMode voiceMode)
	{
		if (this->voiceMode != voiceMode)
		{
			AllNotesOff();
			this->voiceMode = voiceMode;
		}
	}

	SynthDevice::Voice::Voice() { }
	
	SynthDevice::Voice::Voice(SynthDevice *synth)
	{
		this->synth = synth;
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
		
		switch (synth->voiceMode)
		{
		case VoiceMode::Polyphonic:
		default:
			currentNote = (double)note;
			break;
		case VoiceMode::MonoLegatoTrill:
			currentNote = (double)note;
			slideActive = false;
			break;
		case VoiceMode::MonoLegatoAlways:
			break;
		}
	}

	void SynthDevice::Voice::NoteOff()
	{
	}

	void SynthDevice::Voice::NoteSlide(int note)
	{
		slideActive = true;
		destinationNote = note;
		slideDelta = ((double)note - currentNote) / (Helpers::CurrentSampleRate * 0.2f);
		slideSamples = (int)(Helpers::CurrentSampleRate * 0.2f);
	}

	double SynthDevice::Voice::GetNote()
	{
		if (slideActive)
		{
			currentNote += slideDelta;
			slideSamples--;
			if (slideSamples < 0)
			{
				Note = (int)destinationNote;
				slideActive = false;
				currentNote = (double)destinationNote;
			}
		}
		return currentNote;
	}

	void SynthDevice::clearEvents()
	{
		for (int i = 0; i < maxEvents; i++) events[i].Type = EventType::None;
	}
}
