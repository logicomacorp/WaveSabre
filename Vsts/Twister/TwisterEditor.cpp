#include "TwisterEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

TwisterEditor::TwisterEditor(AudioEffect *audioEffect)
	: VstEditor(audioEffect, 400, 160, "TWISTER")
{
}

TwisterEditor::~TwisterEditor()
{
}

void TwisterEditor::Open()
{
	addKnob((VstInt32)Twister::ParamIndices::Type, "TYPE");
	addKnob((VstInt32)Twister::ParamIndices::Amount, "AMOUNT");
	addKnob((VstInt32)Twister::ParamIndices::Feedback, "FEEDBACK");
	addKnob((VstInt32)Twister::ParamIndices::Spread, "SPREAD");
	addSpacer();
	addKnob((VstInt32)Twister::ParamIndices::VibratoFreq, "VIB FREQ");
	addKnob((VstInt32)Twister::ParamIndices::VibratoAmount, "VIB AMT");

	startNextRow();
	addKnob((VstInt32)Twister::ParamIndices::LowCutFreq, "LC FREQ");
	addKnob((VstInt32)Twister::ParamIndices::HighCutFreq, "HC FREQ");
	addSpacer();
	addKnob((VstInt32)Twister::ParamIndices::DryWet, "DRY/WET");

	VstEditor::Open();
}
