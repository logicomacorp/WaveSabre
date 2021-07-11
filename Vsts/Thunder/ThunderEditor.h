#ifndef __THUNDEREDITOR_H__
#define __THUNDEREDITOR_H__

#include <WaveSabreVstLib.h>
using namespace WaveSabreVstLib;

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

#include "vstgui.sf/vstgui/cfileselector.h"

class ThunderEditor : public VstEditor, public CBaseObject
{
public:
	ThunderEditor(AudioEffect *audioEffect);
	virtual ~ThunderEditor();

	virtual void Open();

	virtual void setParameter(VstInt32 index, float value);

    virtual CMessageResult notify(CBaseObject* sender, const char* message);

private:
#ifdef _WIN32
	static BOOL __stdcall driverEnumCallback(HACMDRIVERID driverId, DWORD_PTR dwInstance, DWORD fdwSupport);
	static BOOL __stdcall formatEnumCallback(HACMDRIVERID driverId, LPACMFORMATDETAILS formatDetails, DWORD_PTR dwInstance, DWORD fdwSupport);

	static HACMDRIVERID driverId;
	static WAVEFORMATEX *foundWaveFormat;
#endif
	bool pressedTheFuck;

	Thunder *thunder;
};

#endif
