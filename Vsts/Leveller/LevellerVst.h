#ifndef __LEVELLERVST_H__
#define __LEVELLERVST_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class LevellerVst : public VstPlug
{
public:
	LevellerVst(audioMasterCallback audioMaster);

	virtual void getParameterName(VstInt32 index, char *text);

	virtual bool getEffectName(char *name);
	virtual bool getProductString(char *text);
};

#endif