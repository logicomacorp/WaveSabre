#include "FalconEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

FalconEditor::FalconEditor(AudioEffect *audioEffect)
	: VstEditor(audioEffect, 820, 220, "FALCON")
{
}

FalconEditor::~FalconEditor()
{
}

void FalconEditor::Open()
{
	addKnob((VstInt32)Falcon::ParamIndices::Osc1Waveform, "WAVEFORM");
	addSpacer();
	addKnob((VstInt32)Falcon::ParamIndices::Osc1RatioCoarse, "RATIO COARSE");
	addSpacer();
	addKnob((VstInt32)Falcon::ParamIndices::Osc1RatioFine, "RATIO SEMI");
	addSpacer();
	addKnob((VstInt32)Falcon::ParamIndices::Osc1Feedback, "FEEDBACK");
	addSpacer();
	addKnob((VstInt32)Falcon::ParamIndices::Osc1FeedForward, "FEEDFORWARD");

	addSpacer();
	addSpacer();
	addSpacer();
	addSpacer();
	addSpacer();

	addKnob((VstInt32)Falcon::ParamIndices::Osc1Attack, "ATTACK");
	addKnob((VstInt32)Falcon::ParamIndices::Osc1Decay, "DECAY");
	addKnob((VstInt32)Falcon::ParamIndices::Osc1Sustain, "SUSTAIN");
	addKnob((VstInt32)Falcon::ParamIndices::Osc1Release, "RELEASE");

	addSpacer();
	addSpacer();
	addSpacer();

	addKnob((VstInt32)Falcon::ParamIndices::MasterLevel, "MASTER");

	startNextRow();
	addSpacer();
	addSpacer();

	addKnob((VstInt32)Falcon::ParamIndices::Osc2Waveform, "WAVEFORM");
	addSpacer();
	addKnob((VstInt32)Falcon::ParamIndices::Osc2RatioCoarse, "RATIO COARSE");
	addSpacer();
	addKnob((VstInt32)Falcon::ParamIndices::Osc2RatioFine, "RATIO SEMI");
	addSpacer();
	addKnob((VstInt32)Falcon::ParamIndices::Osc2Feedback, "FEEDBACK");

	addSpacer();
	addSpacer();
	addSpacer();
	addSpacer();
	addSpacer();
	addSpacer();
	addSpacer();
	addSpacer();

	addKnob((VstInt32)Falcon::ParamIndices::Osc2Attack, "ATTACK");
	addKnob((VstInt32)Falcon::ParamIndices::Osc2Decay, "DECAY");
	addKnob((VstInt32)Falcon::ParamIndices::Osc2Sustain, "SUSTAIN");
	addKnob((VstInt32)Falcon::ParamIndices::Osc2Release, "RELEASE");

	startNextRow();

	addKnob((VstInt32)Falcon::ParamIndices::VoicesUnisono, "UNISONO");
	addKnob((VstInt32)Falcon::ParamIndices::VoicesDetune, "DETUNE");
	addKnob((VstInt32)Falcon::ParamIndices::VoicesPan, "PAN");

	addKnob((VstInt32)Falcon::ParamIndices::VibratoFreq, "VIB FREQ");
	addKnob((VstInt32)Falcon::ParamIndices::VibratoAmount, "VIB AMT");

	addKnob((VstInt32)Falcon::ParamIndices::VoiceMode, "MODE");
	addKnob((VstInt32)Falcon::ParamIndices::SlideTime, "SLIDE");

	addKnob((VstInt32)Falcon::ParamIndices::Rise, "RISE");

	addSpacer();
	addKnob((VstInt32)Falcon::ParamIndices::PitchAttack, "P ATTACK");
	addKnob((VstInt32)Falcon::ParamIndices::PitchDecay, "P DECAY");
	addKnob((VstInt32)Falcon::ParamIndices::PitchSustain, "P SUSTAIN");
	addKnob((VstInt32)Falcon::ParamIndices::PitchRelease, "P RELEASE");
	addKnob((VstInt32)Falcon::ParamIndices::PitchEnvAmt1, "P AMT 1");
	addKnob((VstInt32)Falcon::ParamIndices::PitchEnvAmt2, "P AMT 2");

	VstEditor::Open();
}
