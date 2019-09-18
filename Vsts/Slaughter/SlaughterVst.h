#ifndef __SLAUGHTERVST_H__
#define __SLAUGHTERVST_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class SlaughterVst : public VstPlug
{
public:
	SlaughterVst(audioMasterCallback audioMaster);

	virtual void getParameterName(VstInt32 index, char *text);

	virtual bool getEffectName(char *name);
	virtual bool getProductString(char *text);
};

#endif
