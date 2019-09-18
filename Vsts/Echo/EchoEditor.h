#ifndef __ECHOEDITOR_H__
#define __ECHOEDITOR_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class EchoEditor : public VstEditor
{
public:
	EchoEditor(AudioEffect *audioEffect);
	virtual ~EchoEditor();

	virtual void Open();
};

#endif
