#ifndef __WAVESABRECORE_THUNDER_H__
#define __WAVESABRECORE_THUNDER_H__

#include "SynthDevice.h"

#ifdef _WIN32
#include <Windows.h>
#include <mmreg.h>

#ifdef UNICODE
#define _UNICODE
#endif
#include <MSAcm.h>
#endif

#ifdef __APPLE__
#include <AudioToolbox/AudioToolbox.h>
#include "Win32defs.h"
#endif

namespace WaveSabreCore
{
	class Thunder : public SynthDevice
	{
	public:
		static const int SampleRate = 44100;

		Thunder();
		virtual ~Thunder();

		virtual void SetChunk(void *data, int size);
		virtual int GetChunk(void **data);

		void LoadSample(char *data, int compressedSize, int uncompressedSize, WAVEFORMATEX *waveFormat);

	private:
		class ThunderVoice : public Voice
		{
		public:
			ThunderVoice(Thunder *thunder);
			virtual WaveSabreCore::SynthDevice *GetSynthDevice() const;

			virtual void Run(double songPosition, float **outputs, int numSamples);

			virtual void NoteOn(int note, int velocity, float detune, float pan);

		private:
			Thunder *thunder;

			int samplePos;
		};

#ifdef _WIN32
		static BOOL __stdcall driverEnumCallback(HACMDRIVERID driverId, DWORD_PTR dwInstance, DWORD fdwSupport);
		static BOOL __stdcall formatEnumCallback(HACMDRIVERID driverId, LPACMFORMATDETAILS formatDetails, DWORD_PTR dwInstance, DWORD fdwSupport);

		static HACMDRIVERID driverId;
#endif
#ifdef __APPLE__
		static OSStatus callback(AudioConverterRef inAudioConverter, UInt32 *ioNumberDataPackets,
		                         AudioBufferList *ioData, AudioStreamPacketDescription **outDataPacketDescription,
		                         void *inUserData);
#endif
		char *chunkData;

		char *waveFormatData;
		int compressedSize, uncompressedSize;

		char *compressedData;
		float *sampleData;

		int sampleLength;
	};
}

#endif
