#ifndef __WAVESABRECORE_THUNDER_H__
#define __WAVESABRECORE_THUNDER_H__

#include "SynthDevice.h"

#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#include <mmreg.h>

#ifdef UNICODE
#define _UNICODE
#endif
#include <MSAcm.h>
#endif /* WIN32 */

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

#if defined(WIN32) || defined(_WIN32)
		void LoadSample(char *data, int compressedSize, int uncompressedSize, WAVEFORMATEX *waveFormat);
#endif

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

#if defined(WIN32) || defined(_WIN32)
		static BOOL __stdcall driverEnumCallback(HACMDRIVERID driverId, DWORD_PTR dwInstance, DWORD fdwSupport);
		static BOOL __stdcall formatEnumCallback(HACMDRIVERID driverId, LPACMFORMATDETAILS formatDetails, DWORD_PTR dwInstance, DWORD fdwSupport);

		static HACMDRIVERID driverId;
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
