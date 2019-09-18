#include "ChamberEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

ChamberEditor::ChamberEditor(AudioEffect *audioEffect)
	: VstEditor(audioEffect, 460, 100, "CHAMBER")
{
}

ChamberEditor::~ChamberEditor()
{
}

void ChamberEditor::Open()
{
	addKnob((VstInt32)Chamber::ParamIndices::Mode, "MODE");
	addSpacer();
	addKnob((VstInt32)Chamber::ParamIndices::Feedback, "FEEDBACK");
	addSpacer();
	addKnob((VstInt32)Chamber::ParamIndices::PreDelay, "PRE DELAY");
	addSpacer();
	addKnob((VstInt32)Chamber::ParamIndices::LowCutFreq, "LC FREQ");
	addSpacer();
	addKnob((VstInt32)Chamber::ParamIndices::HighCutFreq, "HC FREQ");
	addSpacer();
	addKnob((VstInt32)Chamber::ParamIndices::DryWet, "DRY/WET");

	VstEditor::Open();
}
