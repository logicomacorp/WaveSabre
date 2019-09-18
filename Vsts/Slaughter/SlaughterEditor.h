#ifndef __SLAUGHTEREDITOR_H__
#define __SLAUGHTEREDITOR_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class SlaughterEditor : public VstEditor
{
public:
	SlaughterEditor(AudioEffect *audioEffect);
	virtual ~SlaughterEditor();

	virtual void Open();
};

#endif
