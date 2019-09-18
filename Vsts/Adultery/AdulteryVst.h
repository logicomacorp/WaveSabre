#ifndef __ADULTERYVST_H__
#define __ADULTERYVST_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

#include <WaveSabreCore.h>
using namespace WaveSabreCore;


class AdulteryVst : public VstPlug
{
public:
	AdulteryVst(audioMasterCallback audioMaster);

	virtual void getParameterName(VstInt32 index, char *text);

	virtual bool getEffectName(char *name);
	virtual bool getProductString(char *text);

	Adultery *GetAdultery() const;
};

#endif
