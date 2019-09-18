#ifndef __FALCONVST_H__
#define __FALCONVST_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class FalconVst : public VstPlug
{
public:
	FalconVst(audioMasterCallback audioMaster);

	virtual void getParameterName(VstInt32 index, char *text);

	virtual bool getEffectName(char *name);
	virtual bool getProductString(char *text);
};

#endif
