#ifndef __TWISTEREDITOR_H__
#define __TWISTEREDITOR_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class TwisterEditor : public VstEditor
{
public:
	TwisterEditor(AudioEffect *audioEffect);
	virtual ~TwisterEditor();

	virtual void Open();
};

#endif
