#include "SpecimenEditor.h"
#include "SpecimenVst.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

#include <iostream>
#include <fstream>
#include <exception>
using namespace std;

#include <Windows.h>

HACMDRIVERID SpecimenEditor::driverId = NULL;
WAVEFORMATEX *SpecimenEditor::foundWaveFormat = nullptr;

SpecimenEditor::SpecimenEditor(AudioEffect *audioEffect)
	: VstEditor(audioEffect, 580, 400, "SPECIMEN")
{
	pressedTheFuck = false;

	fileSelector = nullptr;

	specimen = ((SpecimenVst *)audioEffect)->GetSpecimen();
}

SpecimenEditor::~SpecimenEditor()
{
}

void SpecimenEditor::Open()
{
	if (!fileSelector) fileSelector = new CFileSelector(nullptr);

	addSpacer();
	addButton(1000, "LOAD SAMPLE");
	addSpacer();

	startNextRow();

	addKnob((VstInt32)Specimen::ParamIndices::SampleStart, "SAMPLE START");
	addSpacer();
	addKnob((VstInt32)Specimen::ParamIndices::Reverse, "REVERSE");
	addSpacer();
	addKnob((VstInt32)Specimen::ParamIndices::LoopMode, "LOOP MODE");
	addSpacer();
	addKnob((VstInt32)Specimen::ParamIndices::LoopStart, "LOOP START");
	addSpacer();
	addKnob((VstInt32)Specimen::ParamIndices::LoopLength, "LOOP LENGTH");
	addSpacer();
	addKnob((VstInt32)Specimen::ParamIndices::InterpolationMode, "INTERPOLATION");

	startNextRow();

	addKnob((VstInt32)Specimen::ParamIndices::AmpAttack, "ATTACK");
	addSpacer();
	addKnob((VstInt32)Specimen::ParamIndices::AmpDecay, "DECAY");
	addSpacer();
	addKnob((VstInt32)Specimen::ParamIndices::AmpSustain, "SUSTAIN");
	addSpacer();
	addKnob((VstInt32)Specimen::ParamIndices::AmpRelease, "RELEASE");

	startNextRow();

	addKnob((VstInt32)Specimen::ParamIndices::FilterType, "FLT TYPE");
	addSpacer();
	addKnob((VstInt32)Specimen::ParamIndices::FilterFreq, "FLT FREQ");
	addSpacer();
	addKnob((VstInt32)Specimen::ParamIndices::FilterResonance, "FLT RES");
	addSpacer();
	addKnob((VstInt32)Specimen::ParamIndices::FilterModAmt, "FLT MOD");

	startNextRow();

	addKnob((VstInt32)Specimen::ParamIndices::ModAttack, "M ATTACK");
	addSpacer();
	addKnob((VstInt32)Specimen::ParamIndices::ModDecay, "M DECAY");
	addSpacer();
	addKnob((VstInt32)Specimen::ParamIndices::ModSustain, "M SUSTAIN");
	addSpacer();
	addKnob((VstInt32)Specimen::ParamIndices::ModRelease, "M RELEASE");

	startNextRow();

	addKnob((VstInt32)Specimen::ParamIndices::VoicesUnisono, "UNISONO");
	addKnob((VstInt32)Specimen::ParamIndices::VoicesDetune, "DETUNE");
	addKnob((VstInt32)Specimen::ParamIndices::VoicesPan, "PAN");
	addSpacer();
	addKnob((VstInt32)Specimen::ParamIndices::CoarseTune, "COARSE TUNE");
	addSpacer();
	addKnob((VstInt32)Specimen::ParamIndices::FineTune, "FINE TUNE");
	addSpacer();
	addKnob((VstInt32)Slaughter::ParamIndices::VoiceMode, "MODE");
	addKnob((VstInt32)Slaughter::ParamIndices::SlideTime, "SLIDE");
	addSpacer();
	addKnob((VstInt32)Specimen::ParamIndices::Master, "MASTER");

	VstEditor::Open();
}

void SpecimenEditor::setParameter(VstInt32 index, float value)
{
	if (!frame) return;
	if (index == 1000)
	{
		bool oldValue = pressedTheFuck;
		pressedTheFuck = value != 0.0f;
		if (pressedTheFuck != oldValue && oldValue)
		{
			VstFileSelect vfs;
			memset(&vfs, 0, sizeof(vfs));
			vfs.command = kVstFileLoad;
			vfs.type = kVstFileType;
			if (fileSelector->run(&vfs))
			{
				try
				{
					ifstream input(vfs.returnPath, ios::in | ios::binary | ios::ate);
					if (!input.is_open()) throw exception("Could not open file.");
					auto inputSize = input.tellg();
					auto inputBuf = new unsigned char[(unsigned int)inputSize];
					input.seekg(0, ios::beg);
					input.read((char *)inputBuf, inputSize);
					input.close();

					if (*((unsigned int *)inputBuf) != 0x46464952) throw exception("Input file missing RIFF header.");
					if (*((unsigned int *)(inputBuf + 4)) != (unsigned int)inputSize - 8) throw exception("Input file contains invalid RIFF header.");
					if (*((unsigned int *)(inputBuf + 8)) != 0x45564157) throw exception("Input file missing WAVE chunk.");

					if (*((unsigned int *)(inputBuf + 12)) != 0x20746d66) throw exception("Input file missing format sub-chunk.");
					if (*((unsigned int *)(inputBuf + 16)) != 16) throw exception("Input file is not a PCM waveform.");
					auto inputFormat = (LPWAVEFORMATEX)(inputBuf + 20);
					if (inputFormat->wFormatTag != WAVE_FORMAT_PCM) throw exception("Input file is not a PCM waveform.");
					if (inputFormat->nChannels != 1) throw exception("Input file is not mono.");
					if (inputFormat->nSamplesPerSec != Specimen::SampleRate) throw exception(("Input file is not " + to_string(Specimen::SampleRate) + "hz.").c_str());
					if (inputFormat->wBitsPerSample != sizeof(short) * 8) throw exception("Input file is not 16-bit.");

					int chunkPos = 36;
					int chunkSizeBytes;
					while (true)
					{
						if (chunkPos >= (int)inputSize) throw exception("Input file missing data sub-chunk.");
						chunkSizeBytes = *((unsigned int *)(inputBuf + chunkPos + 4));
						if (*((unsigned int *)(inputBuf + chunkPos)) == 0x61746164) break;
						else chunkPos += 8 + chunkSizeBytes;
					}
					int rawDataLength = chunkSizeBytes / 2;
					auto rawData = new short[rawDataLength];
					memcpy(rawData, inputBuf + chunkPos + 8, chunkSizeBytes);

					auto compressedData = new char[chunkSizeBytes];

					int waveFormatSize = 0;
					acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &waveFormatSize);
					auto waveFormat = (WAVEFORMATEX *)(new char[waveFormatSize]);
					memset(waveFormat, 0, waveFormatSize);
					waveFormat->wFormatTag = WAVE_FORMAT_GSM610;
					waveFormat->nSamplesPerSec = Specimen::SampleRate;

					ACMFORMATCHOOSE formatChoose;
					memset(&formatChoose, 0, sizeof(formatChoose));
					formatChoose.cbStruct = sizeof(formatChoose);
					formatChoose.pwfx = waveFormat;
					formatChoose.cbwfx = waveFormatSize;
					formatChoose.pwfxEnum = waveFormat;
					formatChoose.fdwEnum = ACM_FORMATENUMF_WFORMATTAG | ACM_FORMATENUMF_NSAMPLESPERSEC;

					if (acmFormatChoose(&formatChoose)) throw exception("acmFormatChoose failed");

					acmDriverEnum(driverEnumCallback, (DWORD_PTR)waveFormat, NULL);
					HACMDRIVER driver = NULL;
					if (acmDriverOpen(&driver, driverId, 0)) throw exception("acmDriverOpen failed");

					HACMSTREAM stream = NULL;
					if (acmStreamOpen(&stream, driver, inputFormat, waveFormat, NULL, NULL, NULL, ACM_STREAMOPENF_NONREALTIME)) throw exception("acmStreamOpen failed");

					ACMSTREAMHEADER streamHeader;
					memset(&streamHeader, 0, sizeof(streamHeader));
					streamHeader.cbStruct = sizeof(streamHeader);
					streamHeader.pbSrc = (LPBYTE)rawData;
					streamHeader.cbSrcLength = chunkSizeBytes;
					streamHeader.pbDst = (LPBYTE)compressedData;
					streamHeader.cbDstLength = chunkSizeBytes;
					if (acmStreamPrepareHeader(stream, &streamHeader, 0)) throw exception("acmStreamPrepareHeader failed");
					if (acmStreamConvert(stream, &streamHeader, 0)) throw exception("acmStreamConvert failed");

					delete [] rawData;

					acmStreamClose(stream, 0);
					acmDriverClose(driver, 0);

					specimen->LoadSample(compressedData, streamHeader.cbDstLengthUsed, chunkSizeBytes, waveFormat);

					delete [] (char *)waveFormat;

					delete [] compressedData;

					delete [] inputBuf;
				}
				catch (const exception& e)
				{
					MessageBoxA(0, e.what(), "FUCK THAT SHIT", MB_OK | MB_ICONEXCLAMATION);
				}
			}
		}
	}	else
	{
		VstEditor::setParameter(index, value);
	}

}

BOOL __stdcall SpecimenEditor::driverEnumCallback(HACMDRIVERID driverId, DWORD_PTR dwInstance, DWORD fdwSupport)
{
	ACMDRIVERDETAILS driverDetails;
	driverDetails.cbStruct = sizeof(driverDetails);
	acmDriverDetails(driverId, &driverDetails, 0);

	HACMDRIVER driver = NULL;
	acmDriverOpen(&driver, driverId, 0);

	int waveFormatSize = 0;
	acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &waveFormatSize);
	auto waveFormat = (WAVEFORMATEX *)(new char[waveFormatSize]);
	memset(waveFormat, 0, waveFormatSize);
	ACMFORMATDETAILS formatDetails;
	memset(&formatDetails, 0, sizeof(formatDetails));
	formatDetails.cbStruct = sizeof(formatDetails);
	formatDetails.pwfx = waveFormat;
	formatDetails.cbwfx = waveFormatSize;
	formatDetails.dwFormatTag = WAVE_FORMAT_UNKNOWN;
	acmFormatEnum(driver, &formatDetails, formatEnumCallback, dwInstance, NULL);

	delete [] (char *)waveFormat;

	return 1;
}

BOOL __stdcall SpecimenEditor::formatEnumCallback(HACMDRIVERID driverId, LPACMFORMATDETAILS formatDetails, DWORD_PTR dwInstance, DWORD fdwSupport)
{
	auto waveFormat = (WAVEFORMATEX *)dwInstance;

	if (!memcmp(waveFormat, formatDetails->pwfx, sizeof(WAVEFORMATEX)))
	{
		SpecimenEditor::driverId = driverId;
		foundWaveFormat = formatDetails->pwfx;
	}

	return 1;
}
