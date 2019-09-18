#include "ScissorVst.h"
#include "ScissorEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
	Helpers::Init();
	return new ScissorVst(audioMaster);
}

ScissorVst::ScissorVst(audioMasterCallback audioMaster)
	: VstPlug(audioMaster, (int)Scissor::ParamIndices::NumParams, 2, 2, 'Scsr', new Scissor())
{
	setEditor(new ScissorEditor(this));
}

void ScissorVst::getParameterName(VstInt32 index, char *text)
{
	switch ((Scissor::ParamIndices)index)
	{
	case Scissor::ParamIndices::Drive: vst_strncpy(text, "Drive", kVstMaxParamStrLen); break;
	case Scissor::ParamIndices::Threshold: vst_strncpy(text, "Threshld", kVstMaxParamStrLen); break;
	case Scissor::ParamIndices::Foldover: vst_strncpy(text, "Foldover", kVstMaxParamStrLen); break;
	case Scissor::ParamIndices::DryWet: vst_strncpy(text, "Dry/Wet", kVstMaxParamStrLen); break;
	case Scissor::ParamIndices::Type: vst_strncpy(text, "Type", kVstMaxParamStrLen); break;
	case Scissor::ParamIndices::Oversampling: vst_strncpy(text, "Oversmpl", kVstMaxParamStrLen); break;
	}
}

bool ScissorVst::getEffectName(char *name)
{
	vst_strncpy(name, "WaveSabre - Scissor", kVstMaxEffectNameLen);
	return true;
}

bool ScissorVst::getProductString(char *text)
{
	vst_strncpy(text, "WaveSabre - Scissor", kVstMaxProductStrLen);
	return true;
}
