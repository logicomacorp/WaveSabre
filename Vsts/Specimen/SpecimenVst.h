#ifndef __SPECIMENVST_H__
#define __SPECIMENVST_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

#include <WaveSabreCore.h>
using namespace WaveSabreCore;


class SpecimenVst : public VstPlug
{
public:
	SpecimenVst(audioMasterCallback audioMaster);
	
	virtual void getParameterName(VstInt32 index, char *text);

	virtual bool getEffectName(char *name);
	virtual bool getProductString(char *text);

	Specimen *GetSpecimen() const;
};

#endif
