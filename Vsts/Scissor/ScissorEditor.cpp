#include "ScissorEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

ScissorEditor::ScissorEditor(AudioEffect *audioEffect)
	: VstEditor(audioEffect, 480, 100, "SCISSOR")
{
}

ScissorEditor::~ScissorEditor()
{
}

void ScissorEditor::Open()
{
	addKnob((VstInt32)Scissor::ParamIndices::Type, "TYPE");
	addSpacer();
	addKnob((VstInt32)Scissor::ParamIndices::Drive, "DRIVE");
	addSpacer();
	addKnob((VstInt32)Scissor::ParamIndices::Threshold, "THRESHOLD");
	addSpacer();
	addKnob((VstInt32)Scissor::ParamIndices::Foldover, "FOLDOVER");
	addSpacer();
	addKnob((VstInt32)Scissor::ParamIndices::DryWet, "DRY/WET");
	addSpacer();
	addKnob((VstInt32)Scissor::ParamIndices::Oversampling, "OVERSAMPLING");

	VstEditor::Open();
}
