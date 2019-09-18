#include "SlaughterVst.h"
#include "SlaughterEditor.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
	Helpers::Init();
	return new SlaughterVst(audioMaster);
}

SlaughterVst::SlaughterVst(audioMasterCallback audioMaster)
	: VstPlug(audioMaster, (int)Slaughter::ParamIndices::NumParams, 0, 2, 'Sltr', new Slaughter(), true)
{
	setEditor(new SlaughterEditor(this));
}

void SlaughterVst::getParameterName(VstInt32 index, char *text)
{
	switch ((Slaughter::ParamIndices)index)
	{
	case Slaughter::ParamIndices::Osc1Waveform: vst_strncpy(text, "Osc1 Wf", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::Osc1PulseWidth: vst_strncpy(text, "Osc1 Pw", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::Osc1Volume: vst_strncpy(text, "Osc1 Vol", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::Osc1DetuneCoarse: vst_strncpy(text, "Osc1 Cs", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::Osc1DetuneFine: vst_strncpy(text, "Osc1 Fn", kVstMaxParamStrLen); break;

	case Slaughter::ParamIndices::Osc2Waveform: vst_strncpy(text, "Osc2 Wf", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::Osc2PulseWidth: vst_strncpy(text, "Osc2 Pw", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::Osc2Volume: vst_strncpy(text, "Osc2 Vol", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::Osc2DetuneCoarse: vst_strncpy(text, "Osc2 Cs", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::Osc2DetuneFine: vst_strncpy(text, "Osc2 Fn", kVstMaxParamStrLen); break;

	case Slaughter::ParamIndices::Osc3Waveform: vst_strncpy(text, "Osc3 Wf", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::Osc3PulseWidth: vst_strncpy(text, "Osc3 Pw", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::Osc3Volume: vst_strncpy(text, "Osc3 Vol", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::Osc3DetuneCoarse: vst_strncpy(text, "Osc3 Cs", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::Osc3DetuneFine: vst_strncpy(text, "Osc3 Fn", kVstMaxParamStrLen); break;

	case Slaughter::ParamIndices::NoiseVolume: vst_strncpy(text, "Noiz Vol", kVstMaxParamStrLen); break;

	case Slaughter::ParamIndices::FilterType: vst_strncpy(text, "Flt Type", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::FilterFreq: vst_strncpy(text, "Flt Freq", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::FilterResonance: vst_strncpy(text, "Flt Res", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::FilterModAmt: vst_strncpy(text, "Flt Mod", kVstMaxParamStrLen); break;

	case Slaughter::ParamIndices::AmpAttack: vst_strncpy(text, "Amp Atk", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::AmpDecay: vst_strncpy(text, "Amp Dcy", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::AmpSustain: vst_strncpy(text, "Amp Sus", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::AmpRelease: vst_strncpy(text, "Amp Rls", kVstMaxParamStrLen); break;

	case Slaughter::ParamIndices::ModAttack: vst_strncpy(text, "Mod Atk", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::ModDecay: vst_strncpy(text, "Mod Dcy", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::ModSustain: vst_strncpy(text, "Mod Sus", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::ModRelease: vst_strncpy(text, "Mod Rls", kVstMaxParamStrLen); break;

	case Slaughter::ParamIndices::PitchAttack: vst_strncpy(text, "Ptch Atk", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::PitchDecay: vst_strncpy(text, "Ptch Dcy", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::PitchSustain: vst_strncpy(text, "Ptch Sus", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::PitchRelease: vst_strncpy(text, "Ptch Rls", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::PitchEnvAmt: vst_strncpy(text, "Ptch Env", kVstMaxParamStrLen); break;

	case Slaughter::ParamIndices::MasterLevel: vst_strncpy(text, "Mstr Lvl", kVstMaxParamStrLen); break;

	case Slaughter::ParamIndices::VoicesUnisono: vst_strncpy(text, "Vcs Uni", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::VoicesDetune: vst_strncpy(text, "Vcs Det", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::VoicesPan: vst_strncpy(text, "Vcs Pan", kVstMaxParamStrLen); break;

	case Slaughter::ParamIndices::VibratoFreq: vst_strncpy(text, "Vib Frq", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::VibratoAmount: vst_strncpy(text, "Vib Amt", kVstMaxParamStrLen); break;

	case Slaughter::ParamIndices::Rise: vst_strncpy(text, "Rise", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::VoiceMode: vst_strncpy(text, "Mode", kVstMaxParamStrLen); break;
	case Slaughter::ParamIndices::SlideTime: vst_strncpy(text, "Slide", kVstMaxParamStrLen); break;
	}
}

bool SlaughterVst::getEffectName(char *name)
{
	vst_strncpy(name, "WaveSabre - Slaughter", kVstMaxEffectNameLen);
	return true;
}

bool SlaughterVst::getProductString(char *text)
{
	vst_strncpy(text, "WaveSabre - Slaughter", kVstMaxProductStrLen);
	return true;
}
