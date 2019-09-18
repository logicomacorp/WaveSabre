#ifndef __CATHEDRALEDITOR_H__
#define __CATHEDRALEDITOR_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

class CathedralEditor : public VstEditor
{
public:
	CathedralEditor(AudioEffect *audioEffect);
	virtual ~CathedralEditor();

	virtual void Open();
};

#endif
