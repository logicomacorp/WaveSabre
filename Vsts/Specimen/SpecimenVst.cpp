#include "SpecimenVst.h"
#include "SpecimenEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
	Helpers::Init();
	return new SpecimenVst(audioMaster);
}

SpecimenVst::SpecimenVst(audioMasterCallback audioMaster)
	: VstPlug(audioMaster, (int)Specimen::ParamIndices::NumParams, 0, 2, 'Spcm', new Specimen(), true)
{
	setEditor(new SpecimenEditor(this));
}

void SpecimenVst::getParameterName(VstInt32 index, char *text)
{
	switch ((Specimen::ParamIndices)index)
	{
	case Specimen::ParamIndices::AmpAttack: vst_strncpy(text, "Amp Atk", kVstMaxParamStrLen); break;
	case Specimen::ParamIndices::AmpDecay: vst_strncpy(text, "Amp Dcy", kVstMaxParamStrLen); break;
	case Specimen::ParamIndices::AmpSustain: vst_strncpy(text, "Amp Sus", kVstMaxParamStrLen); break;
	case Specimen::ParamIndices::AmpRelease: vst_strncpy(text, "Amp Rls", kVstMaxParamStrLen); break;

	case Specimen::ParamIndices::SampleStart: vst_strncpy(text, "Smp Str", kVstMaxParamStrLen); break;
	case Specimen::ParamIndices::Reverse: vst_strncpy(text, "Reverse", kVstMaxParamStrLen); break;
	case Specimen::ParamIndices::LoopMode: vst_strncpy(text, "Lop Mod", kVstMaxParamStrLen); break;
	case Specimen::ParamIndices::LoopStart: vst_strncpy(text, "Lop Str", kVstMaxParamStrLen); break;
	case Specimen::ParamIndices::LoopLength: vst_strncpy(text, "Lop Len", kVstMaxParamStrLen); break;
	case Specimen::ParamIndices::InterpolationMode: vst_strncpy(text, "Int Mod", kVstMaxParamStrLen); break;

	case Specimen::ParamIndices::CoarseTune: vst_strncpy(text, "Cor Tun", kVstMaxParamStrLen); break;
	case Specimen::ParamIndices::FineTune: vst_strncpy(text, "Fin Tun", kVstMaxParamStrLen); break;

	case Specimen::ParamIndices::FilterType: vst_strncpy(text, "Flt Type", kVstMaxParamStrLen); break;
	case Specimen::ParamIndices::FilterFreq: vst_strncpy(text, "Flt Freq", kVstMaxParamStrLen); break;
	case Specimen::ParamIndices::FilterResonance: vst_strncpy(text, "Flt Res", kVstMaxParamStrLen); break;
	case Specimen::ParamIndices::FilterModAmt: vst_strncpy(text, "Flt Mod", kVstMaxParamStrLen); break;

	case Specimen::ParamIndices::ModAttack: vst_strncpy(text, "Mod Atk", kVstMaxParamStrLen); break;
	case Specimen::ParamIndices::ModDecay: vst_strncpy(text, "Mod Dcy", kVstMaxParamStrLen); break;
	case Specimen::ParamIndices::ModSustain: vst_strncpy(text, "Mod Sus", kVstMaxParamStrLen); break;
	case Specimen::ParamIndices::ModRelease: vst_strncpy(text, "Mod Rls", kVstMaxParamStrLen); break;

	case Specimen::ParamIndices::VoicesUnisono: vst_strncpy(text, "Vcs Uni", kVstMaxParamStrLen); break;
	case Specimen::ParamIndices::VoicesDetune: vst_strncpy(text, "Vcs Det", kVstMaxParamStrLen); break;
	case Specimen::ParamIndices::VoicesPan: vst_strncpy(text, "Vcs Pan", kVstMaxParamStrLen); break;

	case Specimen::ParamIndices::Master: vst_strncpy(text, "Master", kVstMaxParamStrLen); break;
	case Specimen::ParamIndices::VoiceMode: vst_strncpy(text, "Mode", kVstMaxParamStrLen); break;
	case Specimen::ParamIndices::SlideTime: vst_strncpy(text, "Slide", kVstMaxParamStrLen); break;
	}
}

bool SpecimenVst::getEffectName(char *name)
{
	vst_strncpy(name, "WaveSabre - Specimen", kVstMaxEffectNameLen);
	return true;
}

bool SpecimenVst::getProductString(char *text)
{
	vst_strncpy(text, "WaveSabre - Specimen", kVstMaxProductStrLen);
	return true;
}

Specimen *SpecimenVst::GetSpecimen() const
{
	return (Specimen *)getDevice();
}
