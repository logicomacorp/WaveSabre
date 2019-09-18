#ifndef __CHAMBERVST_H__
#define __CHAMBERVST_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class ChamberVst : public VstPlug
{
public:
	ChamberVst(audioMasterCallback audioMaster);

	virtual void getParameterName(VstInt32 index, char *text);

	virtual bool getEffectName(char *name);
	virtual bool getProductString(char *text);
};

#endif