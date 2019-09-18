#ifndef __CATHEDRALVST_H__
#define __CATHEDRALVST_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class CathedralVst : public VstPlug
{
public:
	CathedralVst(audioMasterCallback audioMaster);

	virtual void getParameterName(VstInt32 index, char *text);

	virtual bool getEffectName(char *name);
	virtual bool getProductString(char *text);
};

#endif