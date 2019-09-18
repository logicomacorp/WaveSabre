#include "AdulteryVst.h"
#include "AdulteryEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
	Helpers::Init();
	return new AdulteryVst(audioMaster);
}

AdulteryVst::AdulteryVst(audioMasterCallback audioMaster)
	: VstPlug(audioMaster, (int)Adultery::ParamIndices::NumParams, 0, 2, 'Adlt', new Adultery(), true)
{
	setEditor(new AdulteryEditor(this, GetAdultery()));
}

void AdulteryVst::getParameterName(VstInt32 index, char *text)
{
	switch ((Adultery::ParamIndices)index)
	{
	case Adultery::ParamIndices::AmpAttack: vst_strncpy(text, "Amp Atk", kVstMaxParamStrLen); break;
	case Adultery::ParamIndices::AmpDecay: vst_strncpy(text, "Amp Dcy", kVstMaxParamStrLen); break;
	case Adultery::ParamIndices::AmpSustain: vst_strncpy(text, "Amp Sus", kVstMaxParamStrLen); break;
	case Adultery::ParamIndices::AmpRelease: vst_strncpy(text, "Amp Rls", kVstMaxParamStrLen); break;

	case Adultery::ParamIndices::SampleStart: vst_strncpy(text, "Smp Str", kVstMaxParamStrLen); break;
	case Adultery::ParamIndices::Reverse: vst_strncpy(text, "Reverse", kVstMaxParamStrLen); break;
	case Adultery::ParamIndices::LoopMode: vst_strncpy(text, "Lop Mod", kVstMaxParamStrLen); break;
	case Adultery::ParamIndices::LoopBoundaryMode: vst_strncpy(text, "Lop Bnd", kVstMaxParamStrLen); break;
	case Adultery::ParamIndices::LoopStart: vst_strncpy(text, "Lop Str", kVstMaxParamStrLen); break;
	case Adultery::ParamIndices::LoopLength: vst_strncpy(text, "Lop Len", kVstMaxParamStrLen); break;
	case Adultery::ParamIndices::InterpolationMode: vst_strncpy(text, "Int Mod", kVstMaxParamStrLen); break;

	case Adultery::ParamIndices::CoarseTune: vst_strncpy(text, "Cor Tun", kVstMaxParamStrLen); break;
	case Adultery::ParamIndices::FineTune: vst_strncpy(text, "Fin Tun", kVstMaxParamStrLen); break;

	case Adultery::ParamIndices::FilterType: vst_strncpy(text, "Flt Type", kVstMaxParamStrLen); break;
	case Adultery::ParamIndices::FilterFreq: vst_strncpy(text, "Flt Freq", kVstMaxParamStrLen); break;
	case Adultery::ParamIndices::FilterResonance: vst_strncpy(text, "Flt Res", kVstMaxParamStrLen); break;
	case Adultery::ParamIndices::FilterModAmt: vst_strncpy(text, "Flt Mod", kVstMaxParamStrLen); break;

	case Adultery::ParamIndices::ModAttack: vst_strncpy(text, "Mod Atk", kVstMaxParamStrLen); break;
	case Adultery::ParamIndices::ModDecay: vst_strncpy(text, "Mod Dcy", kVstMaxParamStrLen); break;
	case Adultery::ParamIndices::ModSustain: vst_strncpy(text, "Mod Sus", kVstMaxParamStrLen); break;
	case Adultery::ParamIndices::ModRelease: vst_strncpy(text, "Mod Rls", kVstMaxParamStrLen); break;

	case Adultery::ParamIndices::VoicesUnisono: vst_strncpy(text, "Vcs Uni", kVstMaxParamStrLen); break;
	case Adultery::ParamIndices::VoicesDetune: vst_strncpy(text, "Vcs Det", kVstMaxParamStrLen); break;
	case Adultery::ParamIndices::VoicesPan: vst_strncpy(text, "Vcs Pan", kVstMaxParamStrLen); break;

	case Adultery::ParamIndices::Master: vst_strncpy(text, "Master", kVstMaxParamStrLen); break;
	case Adultery::ParamIndices::VoiceMode: vst_strncpy(text, "Mode", kVstMaxParamStrLen); break;
	case Adultery::ParamIndices::SlideTime: vst_strncpy(text, "Slide", kVstMaxParamStrLen); break;
	}
}

bool AdulteryVst::getEffectName(char *name)
{
	vst_strncpy(name, "WaveSabre - Adultery", kVstMaxEffectNameLen);
	return true;
}

bool AdulteryVst::getProductString(char *text)
{
	vst_strncpy(text, "WaveSabre - Adultery", kVstMaxProductStrLen);
	return true;
}

Adultery *AdulteryVst::GetAdultery() const
{
	return (Adultery *)getDevice();
}
