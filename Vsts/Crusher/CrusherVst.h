#ifndef __CRUSHERVST_H__
#define __CRUSHERVST_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class CrusherVst : public VstPlug
{
public:
	CrusherVst(audioMasterCallback audioMaster);

	virtual void getParameterName(VstInt32 index, char *text);

	virtual bool getEffectName(char *name);
	virtual bool getProductString(char *text);
};

#endif