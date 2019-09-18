#ifndef __WAVESABREVSTLIB_VSTPLUG_H__
#define __WAVESABREVSTLIB_VSTPLUG_H__

#include "audioeffectx.h"
#include "VstEditor.h"

#include <WaveSabreCore.h>

namespace WaveSabreVstLib
{
	class VstPlug : public AudioEffectX
	{
	public:
		VstPlug(audioMasterCallback audioMaster, int numParams, int numInputs, int numOutputs, VstInt32 id, WaveSabreCore::Device *device, bool synth = false);
		virtual ~VstPlug();

		virtual void processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames);
		virtual VstInt32 processEvents(VstEvents *ev);

		virtual void setProgramName(char *name);
		virtual void getProgramName(char *name);

		virtual void setSampleRate(float sampleRate);
		virtual void setTempo(int tempo);

		virtual void setParameter(VstInt32 index, float value);
		virtual float getParameter(VstInt32 index);
		virtual void getParameterLabel(VstInt32 index, char *label);
		virtual void getParameterDisplay(VstInt32 index, char *text);
		virtual void getParameterName(VstInt32 index, char *text);

		virtual VstInt32 getChunk(void **data, bool isPreset);
		virtual VstInt32 setChunk(void *data, VstInt32 byteSize, bool isPreset);

		virtual bool getEffectName(char *name);
		virtual bool getVendorString(char *text);
		virtual bool getProductString(char *text);
		virtual VstInt32 getVendorVersion();

		virtual VstInt32 canDo(char *text);
		virtual VstInt32 getNumMidiInputChannels();

	protected:
		void setEditor(VstEditor *editor);
		WaveSabreCore::Device *getDevice() const;

	private:
		int numParams, numInputs, numOutputs;
		bool synth;

		char programName[kVstMaxProgNameLen + 1];

		WaveSabreCore::Device *device;
	};
}

#endif
