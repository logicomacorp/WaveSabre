#ifndef __CRUSHEREDITOR_H__
#define __CRUSHEREDITOR_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class CrusherEditor : public VstEditor
{
public:
	CrusherEditor(AudioEffect *audioEffect);
	virtual ~CrusherEditor();

	virtual void Open();
};

#endif
