#ifndef __LEVELLEREDITOR_H__
#define __LEVELLEREDITOR_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class LevellerEditor : public VstEditor
{
public:
	LevellerEditor(AudioEffect *audioEffect);
	virtual ~LevellerEditor();

	virtual void Open();
};

#endif
