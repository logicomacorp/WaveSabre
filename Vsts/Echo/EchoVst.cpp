#include "EchoVst.h"
#include "EchoEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
	Helpers::Init();
	return new EchoVst(audioMaster);
}

EchoVst::EchoVst(audioMasterCallback audioMaster)
	: VstPlug(audioMaster, (int)Echo::ParamIndices::NumParams, 2, 2, 'Echo', new Echo())
{
	setEditor(new EchoEditor(this));
}

void EchoVst::getParameterName(VstInt32 index, char *text)
{
	switch ((Echo::ParamIndices)index)
	{
	case Echo::ParamIndices::LeftDelayCoarse: vst_strncpy(text, "LDly Crs", kVstMaxParamStrLen); break;
	case Echo::ParamIndices::LeftDelayFine: vst_strncpy(text, "LDly Fin", kVstMaxParamStrLen); break;
	case Echo::ParamIndices::RightDelayCoarse: vst_strncpy(text, "RDly Crs", kVstMaxParamStrLen); break;
	case Echo::ParamIndices::RightDelayFine: vst_strncpy(text, "RDly Fin", kVstMaxParamStrLen); break;
	case Echo::ParamIndices::LowCutFreq: vst_strncpy(text, "LC Freq", kVstMaxParamStrLen); break;
	case Echo::ParamIndices::HighCutFreq: vst_strncpy(text, "HC Freq", kVstMaxParamStrLen); break;
	case Echo::ParamIndices::Feedback: vst_strncpy(text, "Feedback", kVstMaxParamStrLen); break;
	case Echo::ParamIndices::Cross: vst_strncpy(text, "Cross", kVstMaxParamStrLen); break;
	case Echo::ParamIndices::DryWet: vst_strncpy(text, "Dry/Wet", kVstMaxParamStrLen); break;
	}
}

bool EchoVst::getEffectName(char *name)
{
	vst_strncpy(name, "WaveSabre - Echo", kVstMaxEffectNameLen);
	return true;
}

bool EchoVst::getProductString(char *text)
{
	vst_strncpy(text, "WaveSabre - Echo", kVstMaxProductStrLen);
	return true;
}
