#include <WaveSabreCore/Adultery.h>
#include <WaveSabreCore/Helpers.h>
#include <WaveSabreCore/GmDls.h>

#include <string.h>
#include <math.h>

typedef struct
{
	char tag[4];
	unsigned int size;
	short wChannels;
	int dwSamplesPerSec;
	int dwAvgBytesPerSec;
	short wBlockAlign;
} Fmt;

typedef struct
{
	char tag[4];
	unsigned int size;
	unsigned short unityNote;
	short fineTune;
	int gain;
	int attenuation;
	unsigned int fulOptions;
	unsigned int loopCount;
	unsigned int loopSize;
	unsigned int loopType;
	unsigned int loopStart;
	unsigned int loopLength;
} Wsmp;

namespace WaveSabreCore
{
	Adultery::Adultery()
		: SynthDevice((int)ParamIndices::NumParams)
	{
		for (int i = 0; i < maxVoices; i++) voices[i] = new AdulteryVoice(this);

		sampleIndex = -1;

		ampAttack = 1.0f;
		ampDecay = 1.0f;
		ampSustain = 1.0f;
		ampRelease = 1.0f;

		sampleStart = 0.0f;
		reverse = false;
		loopMode = LoopMode::Repeat;
		loopBoundaryMode = LoopBoundaryMode::FromSample;
		loopStart = 0.0f;
		loopLength = 1.0f;
		sampleLoopStart = 0;
		sampleLoopLength = 0;

		interpolationMode = InterpolationMode::Linear;

		sampleData = nullptr;
		sampleLength = 0;

		coarseTune = 0.5f;
		fineTune = 0.5f;

		filterType = StateVariableFilterType::Lowpass;
		filterFreq = 20000.0f - 20.0f;
		filterResonance = 1.0f;
		filterModAmt = .5f;

		modAttack = 1.0f;
		modDecay = 5.0f;
		modSustain = 1.0f;
		modRelease = 1.5f;

		masterLevel = 0.5f;
	}

	Adultery::~Adultery()
	{
		if (sampleData)
			delete [] sampleData;
	}

	void Adultery::SetParam(int index, float value)
	{
		switch ((ParamIndices)index)
		{
		case ParamIndices::SampleIndex:
			sampleIndex = (int)value - 1;

			if (sampleData)
			{
				delete [] sampleData;
				sampleData = nullptr;
				sampleLength = 0;
			}

			if (sampleIndex >= 0)
			{
				auto gmDls = GmDls::Load();

				// Seek to wave pool chunk's data
				auto ptr = gmDls + GmDls::WaveListOffset;

				// Walk wave pool entries
				for (int i = 0; i <= sampleIndex; i++)
				{
					// Walk wave list
					auto waveListTag = *((unsigned int *)ptr); // Should be 'LIST'
					ptr += 4;
					auto waveListSize = *((unsigned int *)ptr);
					ptr += 4;

					// Skip entries until we've reached the index we're looking for
					if (i != sampleIndex)
					{
						ptr += waveListSize;
						continue;
					}

					// Walk wave entry
					auto wave = ptr;
					auto waveTag = *((unsigned int *)wave); // Should be 'wave'
					wave += 4;

					// Read fmt chunk
					Fmt fmt;
					memcpy(&fmt, wave, sizeof(Fmt));
					wave += fmt.size + 8; // size field doesn't account for tag or length fields

					// Read wsmp chunk
					Wsmp wsmp;
					memcpy(&wsmp, wave, sizeof(Wsmp));
					wave += wsmp.size + 8; // size field doesn't account for tag or length fields

					// Read data chunk
					auto dataChunkTag = *((unsigned int *)wave); // Should be 'data'
					wave += 4;
					auto dataChunkSize = *((unsigned int *)wave);
					wave += 4;

					// Data format is assumed to be mono 16-bit signed PCM
					sampleLength = dataChunkSize / 2;
					sampleData = new float[sampleLength];
					for (int j = 0; j < sampleLength; j++)
					{
						auto sample = *((short *)wave);
						wave += 2;
						sampleData[j] = (float)((double)sample / 32768.0);
					}

					if (wsmp.loopCount)
					{
						sampleLoopStart = wsmp.loopStart;
						sampleLoopLength = wsmp.loopLength;
					}
					else
					{
						sampleLoopStart = 0;
						sampleLoopLength = sampleLength;
					}
				}

				delete [] gmDls;
			}
			break;

		case ParamIndices::AmpAttack: ampAttack = Helpers::ScalarToEnvValue(value); break;
		case ParamIndices::AmpDecay: ampDecay = Helpers::ScalarToEnvValue(value); break;
		case ParamIndices::AmpSustain: ampSustain = value; break;
		case ParamIndices::AmpRelease: ampRelease = Helpers::ScalarToEnvValue(value); break;

		case ParamIndices::SampleStart: sampleStart = value; break;
		case ParamIndices::Reverse: reverse = Helpers::ParamToBoolean(value); break;
		case ParamIndices::LoopMode: loopMode = (LoopMode)(int)(value * (float)((int)LoopMode::NumLoopModes - 1)); break;
		case ParamIndices::LoopBoundaryMode: loopBoundaryMode = (LoopBoundaryMode)(int)(value * (float)((int)LoopBoundaryMode::NumLoopBoundaryModes - 1)); break;
		case ParamIndices::LoopStart: loopStart = value; break;
		case ParamIndices::LoopLength: loopLength = value; break;

		case ParamIndices::InterpolationMode: interpolationMode = (InterpolationMode)(int)(value * (float)((int)InterpolationMode::NumInterpolationModes - 1)); break;

		case ParamIndices::CoarseTune: coarseTune = value; break;
		case ParamIndices::FineTune: fineTune = value; break;

		case ParamIndices::FilterType: filterType = Helpers::ParamToStateVariableFilterType(value); break;
		case ParamIndices::FilterFreq: filterFreq = Helpers::ParamToFrequency(value); break;
		case ParamIndices::FilterResonance: filterResonance = 1.0f - value; break;
		case ParamIndices::FilterModAmt: filterModAmt = value; break;

		case ParamIndices::ModAttack: modAttack = Helpers::ScalarToEnvValue(value); break;
		case ParamIndices::ModDecay: modDecay = Helpers::ScalarToEnvValue(value); break;
		case ParamIndices::ModSustain: modSustain = value; break;
		case ParamIndices::ModRelease: modRelease = Helpers::ScalarToEnvValue(value); break;

		case ParamIndices::VoicesUnisono: VoicesUnisono = Helpers::ParamToUnisono(value); break;
		case ParamIndices::VoicesDetune: VoicesDetune = value; break;
		case ParamIndices::VoicesPan: VoicesPan = value; break;

		case ParamIndices::VoiceMode: SetVoiceMode(Helpers::ParamToVoiceMode(value)); break;
		case ParamIndices::SlideTime: Slide = value; break;

		case ParamIndices::Master: masterLevel = value; break;
		}
	}

	float Adultery::GetParam(int index) const
	{
		switch ((ParamIndices)index)
		{
		case ParamIndices::SampleIndex:
		default:
			return (float)(sampleIndex + 1);

		case ParamIndices::AmpAttack: return Helpers::EnvValueToScalar(ampAttack);
		case ParamIndices::AmpDecay: return Helpers::EnvValueToScalar(ampDecay);
		case ParamIndices::AmpSustain: return ampSustain;
		case ParamIndices::AmpRelease: return Helpers::EnvValueToScalar(ampRelease);

		case ParamIndices::SampleStart: return sampleStart;
		case ParamIndices::Reverse: return Helpers::BooleanToParam(reverse);
		case ParamIndices::LoopMode: return (float)loopMode / (float)((int)LoopMode::NumLoopModes - 1);
		case ParamIndices::LoopBoundaryMode: return (float)loopBoundaryMode / (float)((int)LoopBoundaryMode::NumLoopBoundaryModes - 1);
		case ParamIndices::LoopStart: return loopStart;
		case ParamIndices::LoopLength: return loopLength;
		case ParamIndices::InterpolationMode: return (float)interpolationMode / (float)((int)InterpolationMode::NumInterpolationModes - 1);

		case ParamIndices::CoarseTune: return coarseTune;
		case ParamIndices::FineTune: return fineTune;

		case ParamIndices::FilterType: return Helpers::StateVariableFilterTypeToParam(filterType);
		case ParamIndices::FilterFreq: return Helpers::FrequencyToParam(filterFreq);
		case ParamIndices::FilterResonance: return 1.0f - filterResonance;
		case ParamIndices::FilterModAmt: return filterModAmt;

		case ParamIndices::ModAttack: return Helpers::EnvValueToScalar(modAttack);
		case ParamIndices::ModDecay: return Helpers::EnvValueToScalar(modDecay);
		case ParamIndices::ModSustain: return modSustain;
		case ParamIndices::ModRelease: return Helpers::EnvValueToScalar(modRelease);

		case ParamIndices::VoicesUnisono: return Helpers::UnisonoToParam(VoicesUnisono);
		case ParamIndices::VoicesDetune: return VoicesDetune;
		case ParamIndices::VoicesPan: return VoicesPan;
		
		case ParamIndices::VoiceMode: return Helpers::VoiceModeToParam(GetVoiceMode());
		case ParamIndices::SlideTime: return Slide;

		case ParamIndices::Master: return masterLevel;
		}
	}

	Adultery::AdulteryVoice::AdulteryVoice(Adultery *adultery)
	{
		this->adultery = adultery;
	}

	SynthDevice *Adultery::AdulteryVoice::GetSynthDevice() const
	{
		return adultery;
	}

	void Adultery::AdulteryVoice::Run(double songPosition, float **outputs, int numSamples)
	{
		filter.SetType(adultery->filterType);
		filter.SetQ(adultery->filterResonance);

		samplePlayer.SampleStart = adultery->sampleStart;
		samplePlayer.LoopStart = adultery->loopStart;
		samplePlayer.LoopLength = adultery->loopLength;
		samplePlayer.LoopMode = adultery->loopMode;
		samplePlayer.LoopBoundaryMode = adultery->loopBoundaryMode;
		samplePlayer.InterpolationMode = adultery->interpolationMode;
		samplePlayer.Reverse = adultery->reverse;

		samplePlayer.RunPrep();

		float amp = Helpers::VolumeToScalar(adultery->masterLevel);
		float panLeft = Helpers::PanToScalarLeft(Pan);
		float panRight = Helpers::PanToScalarRight(Pan);

		for (int i = 0; i < numSamples; i++)
		{
			calcPitch();

			filter.SetFreq(Helpers::Clamp(adultery->filterFreq + modEnv.GetValue() * (20000.0f - 20.0f) * (adultery->filterModAmt * 2.0f - 1.0f), 0.0f, 20000.0f - 20.0f));

			float sample = samplePlayer.Next();
			if (!samplePlayer.IsActive)
			{
				IsOn = false;
				break;
			}

			sample = filter.Next(sample) * ampEnv.GetValue() * velocity * amp;
			outputs[0][i] += sample * panLeft;
			outputs[1][i] += sample * panRight;

			modEnv.Next();
			ampEnv.Next();
			if (ampEnv.State == EnvelopeState::Finished)
			{
				IsOn = false;
				break;
			}
		}
	}

	void Adultery::AdulteryVoice::NoteOn(int note, int velocity, float detune, float pan)
	{
		Voice::NoteOn(note, velocity, detune, pan);

		ampEnv.Attack = adultery->ampAttack;
		ampEnv.Decay = adultery->ampDecay;
		ampEnv.Sustain = adultery->ampSustain;
		ampEnv.Release = adultery->ampRelease;
		ampEnv.Trigger();

		modEnv.Attack = adultery->modAttack;
		modEnv.Decay = adultery->modDecay;
		modEnv.Sustain = adultery->modSustain;
		modEnv.Release = adultery->modRelease;
		modEnv.Trigger();
		
		samplePlayer.SampleData = adultery->sampleData;
		samplePlayer.SampleLength = adultery->sampleLength;
		samplePlayer.SampleLoopStart = adultery->sampleLoopStart;
		samplePlayer.SampleLoopLength = adultery->sampleLoopLength;

		samplePlayer.SampleStart = adultery->sampleStart;
		samplePlayer.LoopStart = adultery->loopStart;
		samplePlayer.LoopLength = adultery->loopLength;
		samplePlayer.LoopMode = adultery->loopMode;
		samplePlayer.LoopBoundaryMode = adultery->loopBoundaryMode;
		samplePlayer.InterpolationMode = adultery->interpolationMode;
		samplePlayer.Reverse = adultery->reverse;

		calcPitch();
		samplePlayer.InitPos();

		this->velocity = (float)velocity / 128.0f;
	}

	void Adultery::AdulteryVoice::NoteOff()
	{
		ampEnv.Off();
		modEnv.Off();
	}

	double Adultery::AdulteryVoice::coarseDetune(float detune)
	{
		return floor((detune * 2.0f - 1.0f) * 12.0f);
	}

	void Adultery::AdulteryVoice::calcPitch()
	{
		samplePlayer.CalcPitch(GetNote() - 60 + Detune + adultery->fineTune * 2.0f - 1.0f + AdulteryVoice::coarseDetune(adultery->coarseTune));
	}
}
