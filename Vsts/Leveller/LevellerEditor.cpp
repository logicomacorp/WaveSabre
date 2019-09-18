#include "LevellerEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

LevellerEditor::LevellerEditor(AudioEffect *audioEffect)
	: VstEditor(audioEffect, 210, 340, "LEVELLER")
{
}

LevellerEditor::~LevellerEditor()
{
}

void LevellerEditor::Open()
{
	addKnob((VstInt32)Leveller::ParamIndices::LowCutFreq, "LC FREQ");
	addKnob((VstInt32)Leveller::ParamIndices::LowCutQ, "LC Q");

	startNextRow();

	addKnob((VstInt32)Leveller::ParamIndices::Peak1Freq, "P1 FREQ");
	addKnob((VstInt32)Leveller::ParamIndices::Peak1Gain, "P1 GAIN");
	addKnob((VstInt32)Leveller::ParamIndices::Peak1Q, "P1 Q");

	startNextRow();

	addKnob((VstInt32)Leveller::ParamIndices::Peak2Freq, "P2 FREQ");
	addKnob((VstInt32)Leveller::ParamIndices::Peak2Gain, "P2 GAIN");
	addKnob((VstInt32)Leveller::ParamIndices::Peak2Q, "P2 Q");

	startNextRow();

	addKnob((VstInt32)Leveller::ParamIndices::Peak3Freq, "P3 FREQ");
	addKnob((VstInt32)Leveller::ParamIndices::Peak3Gain, "P3 GAIN");
	addKnob((VstInt32)Leveller::ParamIndices::Peak3Q, "P3 Q");

	startNextRow();

	addKnob((VstInt32)Leveller::ParamIndices::HighCutFreq, "HC FREQ");
	addKnob((VstInt32)Leveller::ParamIndices::HighCutQ, "HC Q");
	addKnob((VstInt32)Leveller::ParamIndices::Master, "MASTER");

	VstEditor::Open();
}
