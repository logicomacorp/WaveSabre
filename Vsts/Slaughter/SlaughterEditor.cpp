#include "SlaughterEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

SlaughterEditor::SlaughterEditor(AudioEffect *audioEffect)
	: VstEditor(audioEffect, 520, 600, "SLAUGHTER")
{
}

SlaughterEditor::~SlaughterEditor()
{
}

void SlaughterEditor::Open()
{
	addKnob((VstInt32)Slaughter::ParamIndices::Osc1Waveform, "WAVEFORM");
	addKnob((VstInt32)Slaughter::ParamIndices::Osc1PulseWidth, "PW");
	addSpacer();
	addKnob((VstInt32)Slaughter::ParamIndices::Osc1DetuneCoarse, "DETUNE");
	addKnob((VstInt32)Slaughter::ParamIndices::Osc1DetuneFine, "DET FINE");
	addSpacer();
	addKnob((VstInt32)Slaughter::ParamIndices::Osc1Volume, "VOLUME");
	
	addSpacer();
	addSpacer();
	addSpacer();
	addSpacer();
	addSpacer();

	addKnob((VstInt32)Slaughter::ParamIndices::MasterLevel, "MASTER");
	
	startNextRow();

	addKnob((VstInt32)Slaughter::ParamIndices::Osc2Waveform, "WAVEFORM");
	addKnob((VstInt32)Slaughter::ParamIndices::Osc2PulseWidth, "PW");
	addSpacer();
	addKnob((VstInt32)Slaughter::ParamIndices::Osc2DetuneCoarse, "DETUNE");
	addKnob((VstInt32)Slaughter::ParamIndices::Osc2DetuneFine, "DET FINE");
	addSpacer();
	addKnob((VstInt32)Slaughter::ParamIndices::Osc2Volume, "VOLUME");

	startNextRow();

	addKnob((VstInt32)Slaughter::ParamIndices::Osc3Waveform, "WAVEFORM");
	addKnob((VstInt32)Slaughter::ParamIndices::Osc3PulseWidth, "PW");
	addSpacer();
	addKnob((VstInt32)Slaughter::ParamIndices::Osc3DetuneCoarse, "DETUNE");
	addKnob((VstInt32)Slaughter::ParamIndices::Osc3DetuneFine, "DET FINE");
	addSpacer();
	addKnob((VstInt32)Slaughter::ParamIndices::Osc3Volume, "VOLUME");

	startNextRow();

	addKnob((VstInt32)Slaughter::ParamIndices::NoiseVolume, "NOISE");

	startNextRow();

	addKnob((VstInt32)Slaughter::ParamIndices::FilterType, "FLT TYPE");
	addKnob((VstInt32)Slaughter::ParamIndices::FilterFreq, "FLT FREQ");
	addKnob((VstInt32)Slaughter::ParamIndices::FilterResonance, "FLT RES");
	addKnob((VstInt32)Slaughter::ParamIndices::FilterModAmt, "M ENV AMT");

	startNextRow();

	addKnob((VstInt32)Slaughter::ParamIndices::ModAttack, "M ATTACK");
	addSpacer();
	addKnob((VstInt32)Slaughter::ParamIndices::ModDecay, "M DECAY");
	addSpacer();
	addKnob((VstInt32)Slaughter::ParamIndices::ModSustain, "M SUSTAIN");
	addSpacer();
	addKnob((VstInt32)Slaughter::ParamIndices::ModRelease, "M RELEASE");

	startNextRow();

	addKnob((VstInt32)Slaughter::ParamIndices::AmpAttack, "A ATTACK");
	addSpacer();
	addKnob((VstInt32)Slaughter::ParamIndices::AmpDecay, "A DECAY");
	addSpacer();
	addKnob((VstInt32)Slaughter::ParamIndices::AmpSustain, "A SUSTAIN");
	addSpacer();
	addKnob((VstInt32)Slaughter::ParamIndices::AmpRelease, "A RELEASE");

	startNextRow();
	addKnob((VstInt32)Slaughter::ParamIndices::PitchAttack, "P ATTACK");
	addSpacer();
	addKnob((VstInt32)Slaughter::ParamIndices::PitchDecay, "P DECAY");
	addSpacer();
	addKnob((VstInt32)Slaughter::ParamIndices::PitchSustain, "P SUSTAIN");
	addSpacer();
	addKnob((VstInt32)Slaughter::ParamIndices::PitchRelease, "P RELEASE");
	addSpacer();
	addKnob((VstInt32)Slaughter::ParamIndices::PitchEnvAmt, "P ENV AMT");

	startNextRow();

	addKnob((VstInt32)Slaughter::ParamIndices::VoicesUnisono, "UNISONO");
	addKnob((VstInt32)Slaughter::ParamIndices::VoicesDetune, "DETUNE");
	addKnob((VstInt32)Slaughter::ParamIndices::VoicesPan, "PAN");

	addSpacer();

	addKnob((VstInt32)Slaughter::ParamIndices::VibratoFreq, "VIB FREQ");
	addKnob((VstInt32)Slaughter::ParamIndices::VibratoAmount, "VIB AMT");

	addKnob((VstInt32)Slaughter::ParamIndices::Rise, "RISE");

	addSpacer();

	addKnob((VstInt32)Slaughter::ParamIndices::VoiceMode, "MODE");
	addKnob((VstInt32)Slaughter::ParamIndices::SlideTime, "SLIDE");


	VstEditor::Open();
}
