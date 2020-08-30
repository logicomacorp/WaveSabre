#include <WaveSabreCore/Slaughter.h>
#include <WaveSabreCore/Helpers.h>

#include <math.h>

namespace WaveSabreCore
{
	Slaughter::Slaughter()
		: SynthDevice((int)ParamIndices::NumParams)
	{
		for (int i = 0; i < maxVoices; i++) voices[i] = new SlaughterVoice(this);

		osc1Waveform = osc2Waveform = osc3Waveform = 0.0f;
		osc1PulseWidth = osc2PulseWidth = osc3PulseWidth = .5f;
		osc1DetuneCoarse = osc2DetuneCoarse = osc3DetuneCoarse = 0.0f;
		osc1DetuneFine = osc2DetuneFine = osc3DetuneFine = 0.0f;
		osc1Volume = 1.0f;
		osc2Volume = osc3Volume = noiseVolume = 0.0f;

		filterType = StateVariableFilterType::Lowpass;
		filterFreq = 20000.0f - 20.0f;
		filterResonance = 1.0f;
		filterModAmt = .5f;

		ampAttack = 1.0f;
		ampDecay = 5.0f;
		ampSustain = .5f;
		ampRelease = 1.5f;

		modAttack = 1.0f;
		modDecay = 5.0f;
		modSustain = 1.0f;
		modRelease = 1.5f;

		pitchAttack = 1.0f;
		pitchDecay = 5.0f;
		pitchSustain = .5f;
		pitchRelease = 1.5f;
		pitchEnvAmt = 0.0f;

		masterLevel = .5f;
	}

	void Slaughter::SetParam(int index, float value)
	{
		switch ((ParamIndices)index)
		{
		case ParamIndices::Osc1Waveform: osc1Waveform = value; break;
		case ParamIndices::Osc1PulseWidth: osc1PulseWidth = 1.0f - value; break;
		case ParamIndices::Osc1Volume: osc1Volume = value; break;
		case ParamIndices::Osc1DetuneCoarse: osc1DetuneCoarse = value; break;
		case ParamIndices::Osc1DetuneFine: osc1DetuneFine = value; break;

		case ParamIndices::Osc2Waveform: osc2Waveform = value; break;
		case ParamIndices::Osc2PulseWidth: osc2PulseWidth = 1.0f - value; break;
		case ParamIndices::Osc2Volume: osc2Volume = value; break;
		case ParamIndices::Osc2DetuneCoarse: osc2DetuneCoarse = value; break;
		case ParamIndices::Osc2DetuneFine: osc2DetuneFine = value; break;

		case ParamIndices::Osc3Waveform: osc3Waveform = value; break;
		case ParamIndices::Osc3PulseWidth: osc3PulseWidth = 1.0f - value; break;
		case ParamIndices::Osc3Volume: osc3Volume = value; break;
		case ParamIndices::Osc3DetuneCoarse: osc3DetuneCoarse = value; break;
		case ParamIndices::Osc3DetuneFine: osc3DetuneFine = value; break;

		case ParamIndices::NoiseVolume: noiseVolume = value; break;

		case ParamIndices::FilterType: filterType = Helpers::ParamToStateVariableFilterType(value); break;
		case ParamIndices::FilterFreq: filterFreq = Helpers::ParamToFrequency(value); break;
		case ParamIndices::FilterResonance: filterResonance = 1.0f - value; break;
		case ParamIndices::FilterModAmt: filterModAmt = value; break;

		case ParamIndices::AmpAttack: ampAttack = Helpers::ScalarToEnvValue(value); break;
		case ParamIndices::AmpDecay: ampDecay = Helpers::ScalarToEnvValue(value); break;
		case ParamIndices::AmpSustain: ampSustain = value; break;
		case ParamIndices::AmpRelease: ampRelease = Helpers::ScalarToEnvValue(value); break;

		case ParamIndices::ModAttack: modAttack = Helpers::ScalarToEnvValue(value); break;
		case ParamIndices::ModDecay: modDecay = Helpers::ScalarToEnvValue(value); break;
		case ParamIndices::ModSustain: modSustain = value; break;
		case ParamIndices::ModRelease: modRelease = Helpers::ScalarToEnvValue(value); break;

		case ParamIndices::PitchAttack: pitchAttack = Helpers::ScalarToEnvValue(value); break;
		case ParamIndices::PitchDecay: pitchDecay = Helpers::ScalarToEnvValue(value); break;
		case ParamIndices::PitchSustain: pitchSustain = value; break;
		case ParamIndices::PitchRelease: pitchRelease = Helpers::ScalarToEnvValue(value); break;
		case ParamIndices::PitchEnvAmt: pitchEnvAmt = (value - .5f) * 2.0f * 36.0f; break;

		case ParamIndices::MasterLevel: masterLevel = value; break;

		case ParamIndices::VoicesUnisono: VoicesUnisono = Helpers::ParamToUnisono(value); break;
		case ParamIndices::VoicesDetune: VoicesDetune = value; break;
		case ParamIndices::VoicesPan: VoicesPan = value; break;

		case ParamIndices::VibratoFreq: VibratoFreq = Helpers::ParamToVibratoFreq(value); break;
		case ParamIndices::VibratoAmount: VibratoAmount = value; break;

		case ParamIndices::Rise: Rise = value; break;

		case ParamIndices::VoiceMode: SetVoiceMode(Helpers::ParamToVoiceMode(value)); break;
		case ParamIndices::SlideTime: Slide = value; break;
		}
	}

	float Slaughter::GetParam(int index) const
	{
		switch ((ParamIndices)index)
		{
		case ParamIndices::Osc1Waveform:
		default:
			return osc1Waveform;

		case ParamIndices::Osc1PulseWidth: return 1.0f - osc1PulseWidth;
		case ParamIndices::Osc1Volume: return osc1Volume;
		case ParamIndices::Osc1DetuneCoarse: return osc1DetuneCoarse;
		case ParamIndices::Osc1DetuneFine: return osc1DetuneFine;

		case ParamIndices::Osc2Waveform: return osc2Waveform;
		case ParamIndices::Osc2PulseWidth: return 1.0f - osc2PulseWidth;
		case ParamIndices::Osc2Volume: return osc2Volume;
		case ParamIndices::Osc2DetuneCoarse: return osc2DetuneCoarse;
		case ParamIndices::Osc2DetuneFine: return osc2DetuneFine;

		case ParamIndices::Osc3Waveform: return osc3Waveform;
		case ParamIndices::Osc3PulseWidth: return 1.0f - osc3PulseWidth;
		case ParamIndices::Osc3Volume: return osc3Volume;
		case ParamIndices::Osc3DetuneCoarse: return osc3DetuneCoarse;
		case ParamIndices::Osc3DetuneFine: return osc3DetuneFine;

		case ParamIndices::NoiseVolume: return noiseVolume;

		case ParamIndices::FilterType: return Helpers::StateVariableFilterTypeToParam(filterType);
		case ParamIndices::FilterFreq: return Helpers::FrequencyToParam(filterFreq);
		case ParamIndices::FilterResonance: return 1.0f - filterResonance;
		case ParamIndices::FilterModAmt: return filterModAmt;

		case ParamIndices::AmpAttack: return Helpers::EnvValueToScalar(ampAttack);
		case ParamIndices::AmpDecay: return Helpers::EnvValueToScalar(ampDecay);
		case ParamIndices::AmpSustain: return ampSustain;
		case ParamIndices::AmpRelease: return Helpers::EnvValueToScalar(ampRelease);

		case ParamIndices::ModAttack: return Helpers::EnvValueToScalar(modAttack);
		case ParamIndices::ModDecay: return Helpers::EnvValueToScalar(modDecay);
		case ParamIndices::ModSustain: return modSustain;
		case ParamIndices::ModRelease: return Helpers::EnvValueToScalar(modRelease);

		case ParamIndices::PitchAttack: return Helpers::EnvValueToScalar(pitchAttack);
		case ParamIndices::PitchDecay: return Helpers::EnvValueToScalar(pitchDecay);
		case ParamIndices::PitchSustain: return pitchSustain;
		case ParamIndices::PitchRelease: return Helpers::EnvValueToScalar(pitchRelease);
		case ParamIndices::PitchEnvAmt: return pitchEnvAmt / 36.0f / 2.0f + .5f;

		case ParamIndices::MasterLevel: return masterLevel;

		case ParamIndices::VoicesUnisono: return Helpers::UnisonoToParam(VoicesUnisono);
		case ParamIndices::VoicesDetune: return VoicesDetune;
		case ParamIndices::VoicesPan: return VoicesPan;

		case ParamIndices::VibratoFreq: return Helpers::VibratoFreqToParam(VibratoFreq);
		case ParamIndices::VibratoAmount: return VibratoAmount;

		case ParamIndices::Rise: return Rise;

		case ParamIndices::VoiceMode: return Helpers::VoiceModeToParam(GetVoiceMode());
		case ParamIndices::SlideTime: return Slide;
		}
	}

	Slaughter::SlaughterVoice::SlaughterVoice(Slaughter *slaughter)
	{
		this->slaughter = slaughter;

		osc1.Phase = (double)Helpers::RandFloat() * 2.0 * 3.141592;
		osc2.Phase = (double)Helpers::RandFloat() * 2.0 * 3.141592;
		osc3.Phase = (double)Helpers::RandFloat() * 2.0 * 3.141592;
		osc1.Integral = osc2.Integral = osc3.Integral = 0.0;
	}

	SynthDevice *Slaughter::SlaughterVoice::GetSynthDevice() const
	{
		return slaughter;
	}

	void Slaughter::SlaughterVoice::Run(double songPosition, float **outputs, int numSamples)
	{
		double vibratoFreq = slaughter->VibratoFreq / Helpers::CurrentSampleRate;

		filter.SetType(slaughter->filterType);
		filter.SetQ(slaughter->filterResonance);

		float amp = -16.0f * Helpers::VolumeToScalar(slaughter->masterLevel);
		float panLeft = Helpers::PanToScalarLeft(Pan);
		float panRight = Helpers::PanToScalarRight(Pan);

		double osc1Detune = coarseDetune(slaughter->osc1DetuneCoarse) + (double)slaughter->osc1DetuneFine;
		double osc2Detune = coarseDetune(slaughter->osc2DetuneCoarse) + (double)slaughter->osc2DetuneFine;
		double osc3Detune = coarseDetune(slaughter->osc3DetuneCoarse) + (double)slaughter->osc3DetuneFine;

		float osc1VolumeScalar = slaughter->osc1Volume * slaughter->osc1Volume;
		float osc2VolumeScalar = slaughter->osc2Volume * slaughter->osc2Volume;
		float osc3VolumeScalar = slaughter->osc3Volume * slaughter->osc3Volume;
		float noiseScalar = slaughter->noiseVolume * slaughter->noiseVolume;

		for (int i = 0; i < numSamples; i++)
		{
			filter.SetFreq(Helpers::Clamp(slaughter->filterFreq + modEnv.GetValue() * (20000.0f - 20.0f) * (slaughter->filterModAmt * 2.0f - 1.0f), 0.0f, 20000.0f - 20.0f));

			double baseNote = GetNote() + Detune + pitchEnv.GetValue() * slaughter->pitchEnvAmt + Helpers::FastSin(vibratoPhase) * slaughter->VibratoAmount + slaughter->Rise * 24.0f;
			float oscMix = 0.0;
			if (osc1VolumeScalar > 0.0f) oscMix += (float)(osc1.Next(baseNote + osc1Detune, slaughter->osc1Waveform, slaughter->osc1PulseWidth) * osc1VolumeScalar);
			if (osc2VolumeScalar > 0.0f) oscMix += (float)(osc2.Next(baseNote + osc2Detune, slaughter->osc2Waveform, slaughter->osc2PulseWidth) * osc2VolumeScalar);
			if (osc3VolumeScalar > 0.0f) oscMix += (float)(osc3.Next(baseNote + osc3Detune, slaughter->osc3Waveform, slaughter->osc3PulseWidth) * osc3VolumeScalar);
			if (noiseScalar > 0.0f) oscMix += Helpers::RandFloat() * noiseScalar;
			float out = filter.Next(oscMix) * ampEnv.GetValue() * amp;
			outputs[0][i] += out * panLeft;
			outputs[1][i] += out * panRight;

			ampEnv.Next();
			if (ampEnv.State == EnvelopeState::Finished)
			{
				IsOn = false;
				break;
			}
			vibratoPhase += vibratoFreq;
			modEnv.Next();
			pitchEnv.Next();
		}
	}

	void Slaughter::SlaughterVoice::NoteOn(int note, int velocity, float detune, float pan)
	{
		Voice::NoteOn(note, velocity, detune, pan);

		ampEnv.Attack = slaughter->ampAttack;
		ampEnv.Decay = slaughter->ampDecay;
		ampEnv.Sustain = slaughter->ampSustain;
		ampEnv.Release = slaughter->ampRelease;
		ampEnv.Trigger();

		modEnv.Attack = slaughter->modAttack;
		modEnv.Decay = slaughter->modDecay;
		modEnv.Sustain = slaughter->modSustain;
		modEnv.Release = slaughter->modRelease;
		modEnv.Trigger();

		pitchEnv.Attack = slaughter->pitchAttack;
		pitchEnv.Decay = slaughter->pitchDecay;
		pitchEnv.Sustain = slaughter->pitchSustain;
		pitchEnv.Release = slaughter->pitchRelease;
		pitchEnv.Trigger();
	}

	void Slaughter::SlaughterVoice::NoteOff()
	{
		ampEnv.Off();
		modEnv.Off();
		pitchEnv.Off();
	}

	float Slaughter::SlaughterVoice::Oscillator::Next(double note, float waveform, float pulseWidth)
	{
		double phaseMax = Helpers::CurrentSampleRate * .5 / Helpers::NoteToFreq(note);
		double dcOffset = -.498 / phaseMax;

		double phase2 = fmod(Phase + 2.0 * phaseMax * (double)pulseWidth, phaseMax * 2.0) - phaseMax;
		Phase = fmod(Phase + 1.0, phaseMax * 2.0);
		double tmpPhase = Phase - phaseMax;

		double blit1, blit2;
		const double epsilon = .0000001;
		if (tmpPhase > epsilon || tmpPhase < -epsilon)
		{
			tmpPhase *= 3.141592;
			blit1 = Helpers::FastSin(tmpPhase) / tmpPhase;
		}
		else blit1 = 1.0;
		if (phase2 > epsilon || phase2 < -epsilon)
		{
			phase2 *= 3.141592;
			blit2 = Helpers::FastSin(phase2) / phase2;
		}
		else blit2 = 1.0;

		Integral = .998 * Integral + dcOffset * (1.0 - (double)waveform) + blit1 - blit2 * (double)waveform;
		return (float)Integral;
	}

	double Slaughter::SlaughterVoice::coarseDetune(float detune)
	{
		return floor(detune * 24.99f);
	}
}
