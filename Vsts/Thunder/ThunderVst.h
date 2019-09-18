#ifndef __THUNDERVST_H__
#define __THUNDERVST_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

class ThunderVst : public VstPlug
{
public:
	ThunderVst(audioMasterCallback audioMaster);

	virtual bool getEffectName(char *name);
	virtual bool getProductString(char *text);

	Thunder *GetThunder() const;
};

#endif
