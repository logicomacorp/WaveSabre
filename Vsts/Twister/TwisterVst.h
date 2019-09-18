#ifndef __TWISTERVST_H__
#define __TWISTERVST_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class TwisterVst : public VstPlug
{
public:
	TwisterVst(audioMasterCallback audioMaster);

	virtual void getParameterName(VstInt32 index, char *text);

	virtual bool getEffectName(char *name);
	virtual bool getProductString(char *text);
};

#endif