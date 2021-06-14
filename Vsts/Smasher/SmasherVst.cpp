#include "SmasherVst.h"
#include "SmasherEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
	Helpers::Init();
	return new SmasherVst(audioMaster);
}

SmasherVst::SmasherVst(audioMasterCallback audioMaster)
	: VstPlug(audioMaster, (int)Smasher::ParamIndices::NumParams, 4, 2, 'Smsh', new Smasher())
{
	setEditor(new SmasherEditor(this));
}

void SmasherVst::getParameterName(VstInt32 index, char *text)
{
	switch ((Smasher::ParamIndices)index)
	{
	case Smasher::ParamIndices::Sidechain: vst_strncpy(text, "Sidchain", kVstMaxParamStrLen); break;
	case Smasher::ParamIndices::InputGain: vst_strncpy(text, "In Gain", kVstMaxParamStrLen); break;
	case Smasher::ParamIndices::Threshold: vst_strncpy(text, "Thres", kVstMaxParamStrLen); break;
	case Smasher::ParamIndices::Ratio: vst_strncpy(text, "Ratio", kVstMaxParamStrLen); break;
	case Smasher::ParamIndices::Attack: vst_strncpy(text, "Attack", kVstMaxParamStrLen); break;
	case Smasher::ParamIndices::Release: vst_strncpy(text, "Release", kVstMaxParamStrLen); break;
	case Smasher::ParamIndices::OutputGain: vst_strncpy(text, "Out Gain", kVstMaxParamStrLen); break;
	}
}

bool SmasherVst::getEffectName(char *name)
{
	vst_strncpy(name, "WaveSabre - Smasher", kVstMaxEffectNameLen);
	return true;
}

bool SmasherVst::getProductString(char *text)
{
	vst_strncpy(text, "WaveSabre - Smasher", kVstMaxProductStrLen);
	return true;
}
