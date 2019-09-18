#include "SmasherEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

SmasherEditor::SmasherEditor(AudioEffect *audioEffect)
	: VstEditor(audioEffect, 330, 160, "SMASHER")
{
}

SmasherEditor::~SmasherEditor()
{
}

void SmasherEditor::Open()
{
	addKnob((VstInt32)Smasher::ParamIndices::InputGain, "INPUT GAIN");
	addSpacer();
	addKnob((VstInt32)Smasher::ParamIndices::Threshold, "THRESHOLD");
	addSpacer();
	addKnob((VstInt32)Smasher::ParamIndices::Attack, "ATTACK");
	addSpacer();
	addKnob((VstInt32)Smasher::ParamIndices::OutputGain, "OUTPUT GAIN");

	startNextRow();

	addKnob((VstInt32)Smasher::ParamIndices::Sidechain, "SIDECHAIN");
	addSpacer();
	addKnob((VstInt32)Smasher::ParamIndices::Ratio, "RATIO");
	addSpacer();
	addKnob((VstInt32)Smasher::ParamIndices::Release, "RELEASE");

	VstEditor::Open();
}
