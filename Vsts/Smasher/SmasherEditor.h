#ifndef __SMASHEREDITOR_H__
#define __SMASHEREDITOR_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class SmasherEditor : public VstEditor
{
public:
	SmasherEditor(AudioEffect *audioEffect);
	virtual ~SmasherEditor();

	virtual void Open();
};

#endif
