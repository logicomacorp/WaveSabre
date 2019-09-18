#include "ThunderVst.h"
#include "ThunderEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
	Helpers::Init();
	return new ThunderVst(audioMaster);
}

ThunderVst::ThunderVst(audioMasterCallback audioMaster)
	: VstPlug(audioMaster, 0, 0, 2, 'Tndr', new Thunder(), true)
{
	setEditor(new ThunderEditor(this));
}

bool ThunderVst::getEffectName(char *name)
{
	vst_strncpy(name, "WaveSabre - Thunder", kVstMaxEffectNameLen);
	return true;
}

bool ThunderVst::getProductString(char *text)
{
	vst_strncpy(text, "WaveSabre - Thunder", kVstMaxProductStrLen);
	return true;
}

Thunder *ThunderVst::GetThunder() const
{
	return (Thunder *)getDevice();
}
