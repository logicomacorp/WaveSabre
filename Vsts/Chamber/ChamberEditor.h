#ifndef __CHAMBEREDITOR_H__
#define __CHAMBEREDITOR_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class ChamberEditor : public VstEditor
{
public:
	ChamberEditor(AudioEffect *audioEffect);
	virtual ~ChamberEditor();

	virtual void Open();
};

#endif
