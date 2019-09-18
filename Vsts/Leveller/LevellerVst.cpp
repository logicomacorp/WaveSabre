#include "LevellerVst.h"
#include "LevellerEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
	Helpers::Init();
	return new LevellerVst(audioMaster);
}

LevellerVst::LevellerVst(audioMasterCallback audioMaster)
	: VstPlug(audioMaster, (int)Leveller::ParamIndices::NumParams, 2, 2, 'Lvlr', new Leveller())
{
	setEditor(new LevellerEditor(this));
}

void LevellerVst::getParameterName(VstInt32 index, char *text)
{
	switch ((Leveller::ParamIndices)index)
	{
	case Leveller::ParamIndices::LowCutFreq: vst_strncpy(text, "LC Freq", kVstMaxParamStrLen); break;
	case Leveller::ParamIndices::LowCutQ: vst_strncpy(text, "LC Q", kVstMaxParamStrLen); break;

	case Leveller::ParamIndices::Peak1Freq: vst_strncpy(text, "Pk1 Freq", kVstMaxParamStrLen); break;
	case Leveller::ParamIndices::Peak1Gain: vst_strncpy(text, "Pk1 Gain", kVstMaxParamStrLen); break;
	case Leveller::ParamIndices::Peak1Q: vst_strncpy(text, "Pk1 Q", kVstMaxParamStrLen); break;

	case Leveller::ParamIndices::Peak2Freq: vst_strncpy(text, "Pk2 Freq", kVstMaxParamStrLen); break;
	case Leveller::ParamIndices::Peak2Gain: vst_strncpy(text, "Pk2 Gain", kVstMaxParamStrLen); break;
	case Leveller::ParamIndices::Peak2Q: vst_strncpy(text, "Pk2 Q", kVstMaxParamStrLen); break;

	case Leveller::ParamIndices::Peak3Freq: vst_strncpy(text, "Pk3 Freq", kVstMaxParamStrLen); break;
	case Leveller::ParamIndices::Peak3Gain: vst_strncpy(text, "Pk3 Gain", kVstMaxParamStrLen); break;
	case Leveller::ParamIndices::Peak3Q: vst_strncpy(text, "Pk3 Q", kVstMaxParamStrLen); break;

	case Leveller::ParamIndices::HighCutFreq: vst_strncpy(text, "HC Freq", kVstMaxParamStrLen); break;
	case Leveller::ParamIndices::HighCutQ: vst_strncpy(text, "HC Q", kVstMaxParamStrLen); break;

	case Leveller::ParamIndices::Master: vst_strncpy(text, "Master", kVstMaxParamStrLen); break;
	}
}

bool LevellerVst::getEffectName(char *name)
{
	vst_strncpy(name, "WaveSabre - Leveller", kVstMaxEffectNameLen);
	return true;
}

bool LevellerVst::getProductString(char *text)
{
	vst_strncpy(text, "WaveSabre - Leveller", kVstMaxProductStrLen);
	return true;
}
