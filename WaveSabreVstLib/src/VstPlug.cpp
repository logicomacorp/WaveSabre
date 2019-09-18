#include <WaveSabreVstLib/VstPlug.h>

using namespace std;
using namespace WaveSabreCore;

namespace WaveSabreVstLib
{
	VstPlug::VstPlug(audioMasterCallback audioMaster, int numParams, int numInputs, int numOutputs, VstInt32 id, Device *device, bool synth)
		: AudioEffectX(audioMaster, 1, numParams)
	{
		this->numParams = numParams;
		this->numInputs = numInputs;
		this->numOutputs = numOutputs;
		this->device = device;
		this->synth = synth;

		setNumInputs(numInputs);
		setNumOutputs(numOutputs);
		setUniqueID(id);
		canProcessReplacing();
		canDoubleReplacing(false);
		programsAreChunks();
		if (synth) isSynth();

		vst_strncpy(programName, "Default", kVstMaxProgNameLen);
	}

	VstPlug::~VstPlug()
	{
		if (device) delete device;
	}

	void VstPlug::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames)
	{
		if (device)
		{
			VstTimeInfo *ti = getTimeInfo(0);
			if (ti)
			{
				if ((ti->flags & kVstTempoValid) > 0)
					setTempo((int)ti->tempo);
			}

			MxcsrFlagGuard mxcsrFlagGuard;

			device->Run(ti ? ti->samplePos / ti->sampleRate : 0.0, inputs, outputs, sampleFrames);
		}
	}

	VstInt32 VstPlug::processEvents(VstEvents *ev)
	{
		if (device)
		{
			for (VstInt32 i = 0; i < ev->numEvents; i++)
			{
				if (ev->events[i]->type == kVstMidiType)
				{
					VstMidiEvent *midiEvent = (VstMidiEvent *)ev->events[i];
					char *midiData = midiEvent->midiData;
					int status = midiData[0] & 0xf0;
					if (status == 0xb0)
					{
						if (midiData[1] == 0x7e || midiData[1] == 0x7b) device->AllNotesOff();
					}
					else if (status == 0x90 || status == 0x80)
					{
						int note = midiData[1] & 0x7f;
						if (status == 0x80) device->NoteOff(note, midiEvent->deltaFrames);
						else device->NoteOn(note, midiData[2] & 0x7f, midiEvent->deltaFrames);
					}
				}
			}
		}
		return 1;
	}

	void VstPlug::setProgramName(char *name)
	{
		vst_strncpy(programName, name, kVstMaxProgNameLen);
	}

	void VstPlug::getProgramName(char *name)
	{
		vst_strncpy(name, programName, kVstMaxProgNameLen);
	}

	void VstPlug::setSampleRate(float sampleRate)
	{
		AudioEffect::setSampleRate(sampleRate);
		if (device) device->SetSampleRate(sampleRate);
	}

	void VstPlug::setTempo(int tempo)
	{
		if (device)	device->SetTempo(tempo);
	}

	void VstPlug::setParameter(VstInt32 index, float value)
	{
		if (device) device->SetParam(index, value);
		if (editor) ((AEffGUIEditor *)editor)->setParameter(index, value);
	}

	float VstPlug::getParameter(VstInt32 index)
	{
		return device ? device->GetParam(index) : 0.0f;
	}

	void VstPlug::getParameterLabel(VstInt32 index, char *label)
	{
		vst_strncpy(label, "%", kVstMaxParamStrLen);
	}

	void VstPlug::getParameterDisplay(VstInt32 index, char *text)
	{
		vst_strncpy(text, to_string(device->GetParam(index) * 100.0f).c_str(), kVstMaxParamStrLen);
	}

	void VstPlug::getParameterName(VstInt32 index, char *text)
	{
		vst_strncpy(text, "Name", kVstMaxParamStrLen);
	}

	VstInt32 VstPlug::getChunk(void **data, bool isPreset)
	{
		return device ? device->GetChunk(data) : 0;
	}

	VstInt32 VstPlug::setChunk(void *data, VstInt32 byteSize, bool isPreset)
	{
		if (device) device->SetChunk(data, byteSize);
		return byteSize;
	}

	bool VstPlug::getEffectName(char *name)
	{
		vst_strncpy(name, "I AM GOD, BITCH", kVstMaxEffectNameLen);
		return true;
	}

	bool VstPlug::getVendorString(char *text)
	{
		vst_strncpy(text, "Logicoma", kVstMaxVendorStrLen);
		return true;
	}

	bool VstPlug::getProductString(char *text)
	{
		vst_strncpy(text, "I AM GOD, BITCH", kVstMaxProductStrLen);
		return true;
	}

	VstInt32 VstPlug::getVendorVersion()
	{
		return 1337;
	}

	VstInt32 VstPlug::canDo(char *text)
	{
		if (synth && (!strcmp(text, "receiveVstEvents") || !strcmp(text, "receiveVstMidiEvents"))) return 1;
		return -1;
	}

	VstInt32 VstPlug::getNumMidiInputChannels()
	{
		return synth ? 1 : 0;
	}

	void VstPlug::setEditor(VstEditor *editor)
	{
		this->editor = editor;
	}

	Device *VstPlug::getDevice() const
	{
		return device;
	}
}
