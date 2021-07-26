#include "ThunderEditor.h"
#include "ThunderVst.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

#include <iostream>
#include <fstream>
#include <exception>
using namespace std;

#include <Windows.h>

HACMDRIVERID ThunderEditor::driverId = NULL;
WAVEFORMATEX *ThunderEditor::foundWaveFormat = nullptr;

ThunderEditor::ThunderEditor(AudioEffect *audioEffect)
	: VstEditor(audioEffect, 280, 80, "THUNDER (DEPRECATED)")
{
	pressedTheFuck = false;

	fileSelector = nullptr;

	thunder = ((ThunderVst *)audioEffect)->GetThunder();
}

ThunderEditor::~ThunderEditor()
{
}

void ThunderEditor::Open()
{
	if (!fileSelector) fileSelector = new CFileSelector(nullptr);

	addSpacer();
	addButton(1000, "LOAD SAMPLE");
	addSpacer();

	VstEditor::Open();
}

void ThunderEditor::setParameter(VstInt32 index, float value)
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
					if (inputFormat->nSamplesPerSec != Thunder::SampleRate) throw exception(("Input file is not " + to_string(Thunder::SampleRate) + "hz.").c_str());
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
					waveFormat->nSamplesPerSec = Thunder::SampleRate;

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

					thunder->LoadSample(compressedData, streamHeader.cbDstLengthUsed, chunkSizeBytes, waveFormat);

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
	}
}

BOOL __stdcall ThunderEditor::driverEnumCallback(HACMDRIVERID driverId, DWORD_PTR dwInstance, DWORD fdwSupport)
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

BOOL __stdcall ThunderEditor::formatEnumCallback(HACMDRIVERID driverId, LPACMFORMATDETAILS formatDetails, DWORD_PTR dwInstance, DWORD fdwSupport)
{
	auto waveFormat = (WAVEFORMATEX *)dwInstance;

	if (!memcmp(waveFormat, formatDetails->pwfx, sizeof(WAVEFORMATEX)))
	{
		ThunderEditor::driverId = driverId;
		foundWaveFormat = formatDetails->pwfx;
	}

	return 1;
}
