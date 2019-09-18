#include "TwisterVst.h"
#include "TwisterEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
	Helpers::Init();
	return new TwisterVst(audioMaster);
}

TwisterVst::TwisterVst(audioMasterCallback audioMaster)
	: VstPlug(audioMaster, (int)Twister::ParamIndices::NumParams, 2, 2, 'Twst', new Twister())
{
	setEditor(new TwisterEditor(this));
}

void TwisterVst::getParameterName(VstInt32 index, char *text)
{
	switch ((Twister::ParamIndices)index)
	{
	case Twister::ParamIndices::Type: vst_strncpy(text, "Type", kVstMaxParamStrLen); break;
	case Twister::ParamIndices::Amount: vst_strncpy(text, "Amount", kVstMaxParamStrLen); break;
	case Twister::ParamIndices::Feedback: vst_strncpy(text, "Feedback", kVstMaxParamStrLen); break;
	case Twister::ParamIndices::Spread: vst_strncpy(text, "Spread", kVstMaxParamStrLen); break;
	case Twister::ParamIndices::VibratoFreq: vst_strncpy(text, "Vib Frq", kVstMaxParamStrLen); break;
	case Twister::ParamIndices::VibratoAmount: vst_strncpy(text, "Vib Amt", kVstMaxParamStrLen); break;
	case Twister::ParamIndices::LowCutFreq: vst_strncpy(text, "LC Freq", kVstMaxParamStrLen); break;
	case Twister::ParamIndices::HighCutFreq: vst_strncpy(text, "HC Freq", kVstMaxParamStrLen); break;
	case Twister::ParamIndices::DryWet: vst_strncpy(text, "Dry/Wet", kVstMaxParamStrLen); break;
	}
}

bool TwisterVst::getEffectName(char *name)
{
	vst_strncpy(name, "WaveSabre - Twister", kVstMaxEffectNameLen);
	return true;
}

bool TwisterVst::getProductString(char *text)
{
	vst_strncpy(text, "WaveSabre - Twister", kVstMaxProductStrLen);
	return true;
}
