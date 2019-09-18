#include "CathedralVst.h"
#include "CathedralEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
	Helpers::Init();
	return new CathedralVst(audioMaster);
}

CathedralVst::CathedralVst(audioMasterCallback audioMaster)
	: VstPlug(audioMaster, (int)Cathedral::ParamIndices::NumParams, 2, 2, 'Cath', new Cathedral())
{
	setEditor(new CathedralEditor(this));
}

void CathedralVst::getParameterName(VstInt32 index, char *text)
{
	switch ((Cathedral::ParamIndices)index)
	{
	case Cathedral::ParamIndices::Freeze: vst_strncpy(text, "Freeze", kVstMaxParamStrLen); break;
	case Cathedral::ParamIndices::RoomSize: vst_strncpy(text, "Roomsize", kVstMaxParamStrLen); break;
	case Cathedral::ParamIndices::Damp: vst_strncpy(text, "Damp", kVstMaxParamStrLen); break;
	case Cathedral::ParamIndices::Width: vst_strncpy(text, "Width", kVstMaxParamStrLen); break; 
	case Cathedral::ParamIndices::LowCutFreq: vst_strncpy(text, "LC Freq", kVstMaxParamStrLen); break;
	case Cathedral::ParamIndices::HighCutFreq: vst_strncpy(text, "HC Freq", kVstMaxParamStrLen); break;
	case Cathedral::ParamIndices::DryWet: vst_strncpy(text, "Dry/Wet", kVstMaxParamStrLen); break;
	case Cathedral::ParamIndices::PreDelay: vst_strncpy(text, "Pre Dly", kVstMaxParamStrLen); break;
	}
}

bool CathedralVst::getEffectName(char *name)
{
	vst_strncpy(name, "WaveSabre - Cathedral", kVstMaxEffectNameLen);
	return true;
}

bool CathedralVst::getProductString(char *text)
{
	vst_strncpy(text, "WaveSabre - Cathedral", kVstMaxProductStrLen);
	return true;
}
