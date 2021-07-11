#include "ThunderEditor.h"
#include "ThunderVst.h"

#include <WaveSabreCore.h>
using namespace WaveSabreCore;

#include <iostream>
#include <fstream>
#include <exception>
using namespace std;

#ifdef _WIN32
#include <Windows.h>

HACMDRIVERID ThunderEditor::driverId = NULL;
WAVEFORMATEX *ThunderEditor::foundWaveFormat = nullptr;
#endif

#ifdef HAVE_LIBGSM
#include <WaveSabreCore/Win32defs.h>
#include <gsm.h>

#define GSM_PACKET_SIZE 160
#define GSM_MS_PACKET_SIZE (GSM_PACKET_SIZE * 2)
#define GSM_MS_BLOCK_SIZE 65
#endif

ThunderEditor::ThunderEditor(AudioEffect *audioEffect)
	: VstEditor(audioEffect, 140, 80, "THUNDER")
{
	pressedTheFuck = false;

	thunder = ((ThunderVst *)audioEffect)->GetThunder();
}

ThunderEditor::~ThunderEditor()
{
}

void ThunderEditor::Open()
{
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
            CNewFileSelector* fileSelector =
                CNewFileSelector::create(getFrame(), CNewFileSelector::kSelectFile);
            if (fileSelector)
            {
                fileSelector->setDefaultExtension(CFileExtension("WAVE", "wav"));
                fileSelector->setTitle("Choose An Audio File");
                fileSelector->run(this);
                fileSelector->forget();
            }
        }
    }
}

CMessageResult ThunderEditor::notify(CBaseObject* sender, const char* message)
{
    if (message == CNewFileSelector::kSelectEndMessage) {
        CNewFileSelector* sel = dynamic_cast<CNewFileSelector*>(sender);
        if (sel && (sel->getNumSelectedFiles() > 0))
        {
            try
            {
                const char *selectedFile = sel->getSelectedFile(0);
                ifstream input(selectedFile, ios::in | ios::binary | ios::ate);
                if (!input.is_open()) throw runtime_error("Could not open file.");
                auto inputSize = input.tellg();
                auto inputBuf = new unsigned char[(unsigned int)inputSize];
                input.seekg(0, ios::beg);
                input.read((char *)inputBuf, inputSize);
                input.close();

                if (*((unsigned int *)inputBuf) != 0x46464952) throw runtime_error("Input file missing RIFF header.");
                if (*((unsigned int *)(inputBuf + 4)) != (unsigned int)inputSize - 8) throw runtime_error("Input file contains invalid RIFF header.");
                if (*((unsigned int *)(inputBuf + 8)) != 0x45564157) throw runtime_error("Input file missing WAVE chunk.");

                if (*((unsigned int *)(inputBuf + 12)) != 0x20746d66) throw runtime_error("Input file missing format sub-chunk.");
                if (*((unsigned int *)(inputBuf + 16)) != 16) throw runtime_error("Input file is not a PCM waveform.");
                auto inputFormat = (LPWAVEFORMATEX)(inputBuf + 20);
                if (inputFormat->wFormatTag != WAVE_FORMAT_PCM) throw runtime_error("Input file is not a PCM waveform.");
                if (inputFormat->nChannels != 1) throw runtime_error("Input file is not mono.");
                if (inputFormat->nSamplesPerSec != Thunder::SampleRate) throw runtime_error(("Input file is not " + to_string(Thunder::SampleRate) + "hz.").c_str());
                if (inputFormat->wBitsPerSample != sizeof(short) * 8) throw runtime_error("Input file is not 16-bit.");

                int chunkPos = 36;
                int chunkSizeBytes;
                while (true)
                {
                    if (chunkPos >= (int)inputSize) throw runtime_error("Input file missing data sub-chunk.");
                    chunkSizeBytes = *((unsigned int *)(inputBuf + chunkPos + 4));
                    if (*((unsigned int *)(inputBuf + chunkPos)) == 0x61746164) break;
                    else chunkPos += 8 + chunkSizeBytes;
                }
#ifdef _WIN32
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

                if (acmFormatChoose(&formatChoose)) throw runtime_error("acmFormatChoose failed");

                acmDriverEnum(driverEnumCallback, (DWORD_PTR)waveFormat, NULL);
                HACMDRIVER driver = NULL;
                if (acmDriverOpen(&driver, driverId, 0)) throw runtime_error("acmDriverOpen failed");

                HACMSTREAM stream = NULL;
                if (acmStreamOpen(&stream, driver, inputFormat, waveFormat, NULL, NULL, NULL, ACM_STREAMOPENF_NONREALTIME)) throw runtime_error("acmStreamOpen failed");

                ACMSTREAMHEADER streamHeader;
                memset(&streamHeader, 0, sizeof(streamHeader));
                streamHeader.cbStruct = sizeof(streamHeader);
                streamHeader.pbSrc = (LPBYTE)rawData;
                streamHeader.cbSrcLength = chunkSizeBytes;
                streamHeader.pbDst = (LPBYTE)compressedData;
                streamHeader.cbDstLength = chunkSizeBytes;
                if (acmStreamPrepareHeader(stream, &streamHeader, 0)) throw runtime_error("acmStreamPrepareHeader failed");
                if (acmStreamConvert(stream, &streamHeader, 0)) throw runtime_error("acmStreamConvert failed");

                delete [] rawData;

                acmStreamClose(stream, 0);
                acmDriverClose(driver, 0);

                thunder->LoadSample(compressedData, streamHeader.cbDstLengthUsed, chunkSizeBytes, waveFormat);
#elif HAVE_LIBGSM
                int numberOfSamples = chunkSizeBytes / sizeof(gsm_signal);
                int numberOfPackets = ((numberOfSamples + GSM_MS_PACKET_SIZE - 1) / GSM_MS_PACKET_SIZE);
                int rawDataLength = GSM_MS_PACKET_SIZE * numberOfPackets;
                auto rawData = new gsm_signal[rawDataLength];
                memcpy(rawData, inputBuf + chunkPos + 8, chunkSizeBytes);
                bzero(rawData + numberOfSamples, sizeof(gsm_signal) * (rawDataLength - numberOfSamples));

                int compressedSize = GSM_MS_BLOCK_SIZE * numberOfPackets;
                auto compressedData = new gsm_byte[compressedSize];
                gsm context = gsm_create();

                int one = 1;
                gsm_option(context, GSM_OPT_WAV49, &one);

                gsm_signal *samples = rawData;
                gsm_byte *output = compressedData;
                for (int currentPacket = 0; currentPacket < numberOfPackets; currentPacket++) {
                    gsm_encode(context, samples, output);
                    gsm_encode(context, samples + GSM_PACKET_SIZE, output + 32);
                    samples += GSM_MS_PACKET_SIZE;
                    output += GSM_MS_BLOCK_SIZE;
                }
                gsm_destroy(context);

                int waveFormatSize = sizeof(GSMWAVEFORMAT);
                auto waveFormat = (GSMWAVEFORMAT *)(new char[waveFormatSize]);
                memset(waveFormat, 0, waveFormatSize);
                waveFormat->wf.wFormatTag = WAVE_FORMAT_GSM610;
                waveFormat->wf.nSamplesPerSec = Specimen::SampleRate;
                waveFormat->wf.nChannels = 1;
                waveFormat->wf.nAvgBytesPerSec = 8957;
                waveFormat->wf.nBlockAlign = GSM_MS_BLOCK_SIZE;
                waveFormat->wf.cbSize = sizeof(GSMWAVEFORMAT) - sizeof(WAVEFORMATEX);
                waveFormat->wSamplesPerPacket = GSM_MS_PACKET_SIZE;

                delete [] rawData;

                thunder->LoadSample((char *) compressedData, compressedSize, chunkSizeBytes, (WAVEFORMATEX *) waveFormat);
#else
#error "Install libgsm so we can GSM encode in Thunder"
#endif
                delete [] (char *)waveFormat;

                delete [] compressedData;

                delete [] inputBuf;
            }
            catch (const exception& e)
            {
#ifdef _WIN32
                MessageBoxA(0, e.what(), "FUCK THAT SHIT", MB_OK | MB_ICONEXCLAMATION);
#endif
#ifdef __APPLE__
                SInt32 nRes = 0;
                const void* keys[] = {
                    kCFUserNotificationAlertHeaderKey,
                    kCFUserNotificationAlertMessageKey
                };
                const void* vals[] = {
                    CFSTR("FUCK THAT SHIT"),
                    CFStringCreateWithCString(kCFAllocatorDefault, e.what(), kCFStringEncodingUTF8)
                };
                CFDictionaryRef dict = CFDictionaryCreate(kCFAllocatorDefault, keys, vals, 2,
                               &kCFTypeDictionaryKeyCallBacks,
                               &kCFTypeDictionaryValueCallBacks);

                CFUserNotificationRef notificationRef = CFUserNotificationCreate(kCFAllocatorDefault, 0, kCFUserNotificationPlainAlertLevel, &nRes, dict);
                CFRelease(notificationRef);
                CFRelease(dict);
                CFRelease(vals[1]);
#endif
            }
        }
        return kMessageNotified;
	}
}

#ifdef _WIN32
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
#endif
