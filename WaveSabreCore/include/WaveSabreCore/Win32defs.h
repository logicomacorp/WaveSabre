#ifndef __WIN32DEFS_H__
#define __WIN32DEFS_H__

#define WAVE_FORMAT_PCM 1

typedef struct __attribute__((packed)) {
    uint16_t wFormatTag;
    uint16_t nChannels;
    uint32_t nSamplesPerSec;
    uint32_t nAvgBytesPerSec;
    uint16_t nBlockAlign;
    uint16_t wBitsPerSample;
    uint16_t cbSize;
} WAVEFORMATEX, *LPWAVEFORMATEX;

#define WAVE_FORMAT_GSM610 49

typedef struct __attribute__((packed)) {
	WAVEFORMATEX wf;
	uint16_t wSamplesPerPacket;
} GSMWAVEFORMAT;

#endif
