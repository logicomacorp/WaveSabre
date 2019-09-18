#ifndef __ECHOVST_H__
#define __ECHOVST_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class EchoVst : public VstPlug
{
public:
	EchoVst(audioMasterCallback audioMaster);

	virtual void getParameterName(VstInt32 index, char *text);

	virtual bool getEffectName(char *name);
	virtual bool getProductString(char *text);
};

#endif