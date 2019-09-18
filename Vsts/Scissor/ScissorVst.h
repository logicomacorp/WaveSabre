#ifndef __SCISSORVST_H__
#define __SCISSORVST_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class ScissorVst : public VstPlug
{
public:
	ScissorVst(audioMasterCallback audioMaster);

	virtual void getParameterName(VstInt32 index, char *text);

	virtual bool getEffectName(char *name);
	virtual bool getProductString(char *text);
};

#endif
