#include "FalconVst.h"
#include "FalconEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
	Helpers::Init();
	return new FalconVst(audioMaster);
}

FalconVst::FalconVst(audioMasterCallback audioMaster)
	: VstPlug(audioMaster, (int)Falcon::ParamIndices::NumParams, 0, 2, 'Flcn', new Falcon(), true)
{
	setEditor(new FalconEditor(this));
}

void FalconVst::getParameterName(VstInt32 index, char *text)
{
	switch ((Falcon::ParamIndices)index)
	{
	case Falcon::ParamIndices::Osc1Waveform: vst_strncpy(text, "Osc1 Wf", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::Osc1RatioCoarse: vst_strncpy(text, "Osc1 Rc", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::Osc1RatioFine: vst_strncpy(text, "Osc1 Rf", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::Osc1Feedback: vst_strncpy(text, "Osc1 Fb", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::Osc1FeedForward: vst_strncpy(text, "Osc1 Ff", kVstMaxParamStrLen); break;

	case Falcon::ParamIndices::Osc1Attack: vst_strncpy(text, "Osc1 Atk", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::Osc1Decay: vst_strncpy(text, "Osc1 Dcy", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::Osc1Sustain: vst_strncpy(text, "Osc1 Sus", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::Osc1Release: vst_strncpy(text, "Osc1 Rls", kVstMaxParamStrLen); break;

	case Falcon::ParamIndices::Osc2Waveform: vst_strncpy(text, "Osc2 Wf", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::Osc2RatioCoarse: vst_strncpy(text, "Osc2 Rc", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::Osc2RatioFine: vst_strncpy(text, "Osc2 Rf", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::Osc2Feedback: vst_strncpy(text, "Osc2 Fb", kVstMaxParamStrLen); break;

	case Falcon::ParamIndices::Osc2Attack: vst_strncpy(text, "Osc2 Atk", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::Osc2Decay: vst_strncpy(text, "Osc2 Dcy", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::Osc2Sustain: vst_strncpy(text, "Osc2 Sus", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::Osc2Release: vst_strncpy(text, "Osc2 Rls", kVstMaxParamStrLen); break;

	case Falcon::ParamIndices::MasterLevel: vst_strncpy(text, "Mstr Lvl", kVstMaxParamStrLen); break;

	case Falcon::ParamIndices::VoicesUnisono: vst_strncpy(text, "Vcs Uni", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::VoicesDetune: vst_strncpy(text, "Vcs Det", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::VoicesPan: vst_strncpy(text, "Vcs Pan", kVstMaxParamStrLen); break;

	case Falcon::ParamIndices::VibratoFreq: vst_strncpy(text, "Vib Frq", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::VibratoAmount: vst_strncpy(text, "Vib Amt", kVstMaxParamStrLen); break;

	case Falcon::ParamIndices::Rise: vst_strncpy(text, "Rise", kVstMaxParamStrLen); break;

	case Falcon::ParamIndices::PitchAttack: vst_strncpy(text, "Ptch Atk", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::PitchDecay: vst_strncpy(text, "Ptch Dcy", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::PitchSustain: vst_strncpy(text, "Ptch Sus", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::PitchRelease: vst_strncpy(text, "Ptch Rls", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::PitchEnvAmt1: vst_strncpy(text, "Ptc Env1", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::PitchEnvAmt2: vst_strncpy(text, "Ptc Env2", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::VoiceMode: vst_strncpy(text, "Mode", kVstMaxParamStrLen); break;
	case Falcon::ParamIndices::SlideTime: vst_strncpy(text, "Slide", kVstMaxParamStrLen); break;
	}
}

bool FalconVst::getEffectName(char *name)
{
	vst_strncpy(name, "WaveSabre - Falcon", kVstMaxEffectNameLen);
	return true;
}

bool FalconVst::getProductString(char *text)
{
	vst_strncpy(text, "WaveSabre - Falcon", kVstMaxProductStrLen);
	return true;
}
