#include "ChamberVst.h"
#include "ChamberEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
	Helpers::Init();
	return new ChamberVst(audioMaster);
}

ChamberVst::ChamberVst(audioMasterCallback audioMaster)
	: VstPlug(audioMaster, (int)Chamber::ParamIndices::NumParams, 2, 2, 'Chmb', new Chamber())
{
	setEditor(new ChamberEditor(this));
}

void ChamberVst::getParameterName(VstInt32 index, char *text)
{
	switch ((Chamber::ParamIndices)index)
	{
	case Chamber::ParamIndices::Mode: vst_strncpy(text, "Mode", kVstMaxParamStrLen); break;
	case Chamber::ParamIndices::Feedback: vst_strncpy(text, "Feedback", kVstMaxParamStrLen); break;
	case Chamber::ParamIndices::LowCutFreq: vst_strncpy(text, "LC Freq", kVstMaxParamStrLen); break;
	case Chamber::ParamIndices::HighCutFreq: vst_strncpy(text, "HC Freq", kVstMaxParamStrLen); break;
	case Chamber::ParamIndices::DryWet: vst_strncpy(text, "Dry/Wet", kVstMaxParamStrLen); break;
	case Chamber::ParamIndices::PreDelay: vst_strncpy(text, "Pre Dly", kVstMaxParamStrLen); break;
	}
}

bool ChamberVst::getEffectName(char *name)
{
	vst_strncpy(name, "WaveSabre - Chamber", kVstMaxEffectNameLen);
	return true;
}

bool ChamberVst::getProductString(char *text)
{
	vst_strncpy(text, "WaveSabre - Chamber", kVstMaxProductStrLen);
	return true;
}
