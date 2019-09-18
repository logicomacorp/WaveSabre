#include "CrusherEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

CrusherEditor::CrusherEditor(AudioEffect *audioEffect)
	: VstEditor(audioEffect, 250, 100, "CRUSHER")
{
}

CrusherEditor::~CrusherEditor()
{
}

void CrusherEditor::Open()
{
	addKnob((VstInt32)Crusher::ParamIndices::Vertical, "VERTICAL");
	addSpacer();
	addKnob((VstInt32)Crusher::ParamIndices::Horizontal, "HORIZONTAL");
	addSpacer();
	addKnob((VstInt32)Crusher::ParamIndices::DryWet, "DRY/WET");

	VstEditor::Open();
}
