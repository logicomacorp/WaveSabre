#ifndef __FALCONEDITOR_H__
#define __FALCONEDITOR_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class FalconEditor : public VstEditor
{
public:
	FalconEditor(AudioEffect *audioEffect);
	virtual ~FalconEditor();

	virtual void Open();
};

#endif
