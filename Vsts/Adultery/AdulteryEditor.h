#ifndef __ADULTERYEDITOR_H__
#define __ADULTERYEDITOR_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

class AdulteryEditor : public VstEditor
{
public:
	AdulteryEditor(AudioEffect *audioEffect, Adultery *adultery);
	virtual ~AdulteryEditor();

	virtual void Open();

	virtual void setParameter(VstInt32 index, float value);

private:
	void updateCaption();
	void uncheckMenuItems();

	Adultery *adultery;

	COptionMenu *menu;
	CTextLabel *caption;

	std::map<int, std::string> optionNames;
};

#endif
