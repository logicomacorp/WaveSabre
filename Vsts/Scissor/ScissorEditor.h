#ifndef __SCISSOREDITOR_H__
#define __SCISSOREDITOR_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class ScissorEditor : public VstEditor
{
public:
	ScissorEditor(AudioEffect *audioEffect);
	virtual ~ScissorEditor();

	virtual void Open();
};

#endif
