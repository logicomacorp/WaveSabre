#include "EchoEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

EchoEditor::EchoEditor(AudioEffect *audioEffect)
	: VstEditor(audioEffect, 400, 160, "ECHO")
{
}

EchoEditor::~EchoEditor()
{
}

void EchoEditor::Open()
{
	addKnob((VstInt32)Echo::ParamIndices::LeftDelayCoarse, "LEFT COARSE");
	addSpacer();
	addKnob((VstInt32)Echo::ParamIndices::LeftDelayFine, "LEFT FINE");
	addSpacer();
	addKnob((VstInt32)Echo::ParamIndices::Feedback, "FEEDBACK");
	addSpacer();
	addKnob((VstInt32)Echo::ParamIndices::Cross, "CROSS");
	addSpacer();
	addKnob((VstInt32)Echo::ParamIndices::DryWet, "DRY/WET");

	startNextRow();
	addKnob((VstInt32)Echo::ParamIndices::RightDelayCoarse, "RIGHT COARSE");
	addSpacer();
	addKnob((VstInt32)Echo::ParamIndices::RightDelayFine, "RIGHT FINE");
	addSpacer();
	addKnob((VstInt32)Echo::ParamIndices::LowCutFreq, "LC FREQ");
	addSpacer();
	addKnob((VstInt32)Echo::ParamIndices::HighCutFreq, "HC FREQ");

	VstEditor::Open();
}
