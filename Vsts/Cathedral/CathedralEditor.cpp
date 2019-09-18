#include "CathedralEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

CathedralEditor::CathedralEditor(AudioEffect *audioEffect)
	: VstEditor(audioEffect, 560, 100, "CATHEDRAL")
{
}

CathedralEditor::~CathedralEditor()
{
}

void CathedralEditor::Open()
{
	addKnob((VstInt32)Cathedral::ParamIndices::Freeze, "FREEZE");
	addKnob((VstInt32)Cathedral::ParamIndices::RoomSize, "ROOM SIZE");
	addKnob((VstInt32)Cathedral::ParamIndices::Damp, "DAMP");
	addKnob((VstInt32)Cathedral::ParamIndices::Width, "WIDTH");
	addKnob((VstInt32)Cathedral::ParamIndices::PreDelay, "PRE DLY");
	addSpacer();
	addKnob((VstInt32)Cathedral::ParamIndices::LowCutFreq, "LC FREQ");
	addKnob((VstInt32)Cathedral::ParamIndices::HighCutFreq, "HC FREQ");
	addSpacer();
	addKnob((VstInt32)Cathedral::ParamIndices::DryWet, "DRY/WET");

	VstEditor::Open();
}
