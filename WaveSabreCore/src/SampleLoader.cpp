#include <WaveSabreCore/SampleLoader.h>

#include <string.h>

#if !defined(WIN32) && !defined(_WIN32) && HAVE_FFMPEG_GSM
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#endif

namespace WaveSabreCore
{
#if defined(WIN32) || defined(_WIN32)
	HACMDRIVERID SampleLoader::driverId = NULL;
#endif

	SampleLoader::LoadedSample SampleLoader::LoadSampleGSM(char *data, int compressedSize, int uncompressedSize, WAVEFORMATEX *waveFormat)
	{
		LoadedSample ret;

		ret.compressedSize = compressedSize;
		ret.uncompressedSize = uncompressedSize;

		//if (waveFormatData) delete [] waveFormatData;
		ret.waveFormatData = new char[sizeof(WAVEFORMATEX) + waveFormat->cbSize];
		memcpy(ret.waveFormatData, waveFormat, sizeof(WAVEFORMATEX) + waveFormat->cbSize);
		//if (compressedData) delete [] compressedData;
		ret.compressedData = new char[compressedSize];
		memcpy(ret.compressedData, data, compressedSize);

		//if (sampleData) delete [] sampleData;

#if defined(WIN32) || defined(_WIN32)
		acmDriverEnum(driverEnumCallback, NULL, NULL);
		HACMDRIVER driver = NULL;
		acmDriverOpen(&driver, driverId, 0);

		WAVEFORMATEX dstWaveFormat =
		{
			WAVE_FORMAT_PCM,
			1,
			waveFormat->nSamplesPerSec,
			waveFormat->nSamplesPerSec * 2,
			sizeof(short),
			sizeof(short) * 8,
			0
		};

		HACMSTREAM stream = NULL;
		acmStreamOpen(&stream, driver, waveFormat, &dstWaveFormat, NULL, NULL, NULL, ACM_STREAMOPENF_NONREALTIME);

		ACMSTREAMHEADER streamHeader;
		memset(&streamHeader, 0, sizeof(ACMSTREAMHEADER));
		streamHeader.cbStruct = sizeof(ACMSTREAMHEADER);
		streamHeader.pbSrc = (LPBYTE)ret.compressedData;
		streamHeader.cbSrcLength = compressedSize;
		auto uncompressedData = new short[uncompressedSize * 2];
		streamHeader.pbDst = (LPBYTE)uncompressedData;
		streamHeader.cbDstLength = uncompressedSize * 2;
		acmStreamPrepareHeader(stream, &streamHeader, 0);

		acmStreamConvert(stream, &streamHeader, 0);

		acmStreamClose(stream, 0);
		acmDriverClose(driver, 0);

		ret.sampleLength = streamHeader.cbDstLengthUsed / sizeof(short);
		ret.sampleData = new float[ret.sampleLength];
		for (int i = 0; i < ret.sampleLength; i++)
			ret.sampleData[i] = (float)((double)uncompressedData[i] / 32768.0);

		delete [] uncompressedData;
#elif HAVE_FFMPEG_GSM
		// you're going to hate me for this and I'll absolutely deserve it

		// cat test.gsm \
		//     | ffmpeg -i - -f f32le -acodec pcm_f32le - 2>/dev/null \
		//     | aplay -traw -c1 -r44100 -fFLOAT -

		int gsmpipe[2], wavpipe[2];
		int rv;
		rv = pipe(gsmpipe);
		assert(rv == 0 && "Can't set up input pipe for ffmpeg");
		rv = pipe(wavpipe);
		assert(rv == 0 && "Can't set up output pipe for ffmpeg");

		int gsmread = gsmpipe[0], gsmwrite = gsmpipe[1],
			wavread = wavpipe[0], wavwrite = wavpipe[1];

		pid_t child = fork(); // dun dun duuuuun
		assert(child >= 0 && "fork() failed.");

		if (child == 0) {
			// child
			close(gsmwrite);
			close(wavread);
			close(STDERR_FILENO);

			dup2(gsmread, STDIN_FILENO);
			dup2(wavwrite, STDOUT_FILENO);

			char *const args[] = {"/usr/bin/ffmpeg", "-i", "-", "-f", "f32le",
				"-acodec", "pcm_f32le", "-", NULL};
			rv = execve("/usr/bin/ffmpeg", args, environ);
			assert(rv >= 0 && "Failed to run FFmpeg!");
			// unreachable
		} else {
			// parent
			close(gsmread);
			close(wavwrite);

			// write fake(ish) WAV header
			size_t M = (waveFormat->nAvgBytesPerSec * waveFormat->nSamplesPerSec) / waveFormat->nChannels;
			size_t wvfmtsz = sizeof(WAVEFORMATEX) + waveFormat->cbSize;
#ifndef NDEBUG
			assert(waveFormat->cbSize <= 8 && "cbSize too high!");
#endif
			char fileheader[4+4/*RIFF*/ + 4/*WAVE*/ + 4+4+wvfmtsz/*fmt */ + 4+4+4/*fact*/ + 4+4/*data*/];

			memcpy(&fileheader[0], "RIFF", 4);
			*(uint32_t*)&fileheader[4] = 4 + 4+4+wvfmtsz + 4+4+4 + 4+4+compressedSize;

			memcpy(&fileheader[8], "WAVEfmt ", 8);
			*(uint32_t*)&fileheader[16] = wvfmtsz;
			memcpy(&fileheader[20], waveFormat, wvfmtsz);

			memcpy(&fileheader[20+wvfmtsz], "FACT\x04\0\0\0", 8);
			*(uint32_t*)&fileheader[28+wvfmtsz] = M;

			memcpy(&fileheader[32+wvfmtsz], "data", 4);
			*(uint32_t*)&fileheader[36+wvfmtsz] = compressedSize;

			bool wrote_hdr = false;
			uint8_t* uncompr = (uint8_t*)malloc(uncompressedSize);
			size_t uncomprOff = 0;

			int rev;
			while (true) {
				bool needDataOut = compressedSize > 0 || !wrote_hdr;
				bool needDataIn = uncomprOff < uncompressedSize;

				if (!needDataOut && !needDataIn)
					break;

				struct pollfd watched[2];
				watched[0].fd = wavread;
				watched[0].events = POLLIN;
				watched[0].revents = 0;
				watched[1].fd = gsmwrite;
				watched[1].events = needDataOut ? POLLOUT : 0;
				watched[1].revents = 0;

				rv = poll(watched, (compressedSize > 0) ? 2 : 1, -1);
				assert(rv >= 0 && "poll(2) failed?!");

				if (watched[0].revents & POLLNVAL)
					watched[0].revents |= POLLERR;
				if (watched[1].revents & POLLNVAL)
					watched[1].revents |= POLLERR;
				rev = (watched[0].revents | watched[1].revents);

				if ((rev & POLLOUT) && needDataOut && !(watched[1].revents & POLLERR)) {
					if (!wrote_hdr) {
						wrote_hdr = true;
						write(gsmwrite, fileheader, sizeof(fileheader));
					} else {
						size_t towr = compressedSize;
						if (towr > PIPE_BUF)
							towr = PIPE_BUF;
						rv = write(gsmwrite, data, towr);
						assert(rv >= 0 && "Couldn't write GSM data to FFmpeg");
						compressedSize -= rv;

						if (compressedSize <= 0) close(gsmwrite);
					}
				}
				if ((rev & POLLIN) && uncomprOff < uncompressedSize && !(watched[0].revents & POLLERR)) {
					// TODO: make nonblocking, seek for smallest possible data size left
					size_t toRead = 0x100;
					if (toRead > (uncompressedSize - uncomprOff))
						toRead = (uncompressedSize - uncomprOff);
					rv = read(wavread, uncompr + uncomprOff, toRead);
					assert(rv >= 0 && "Couldn't read converted GSM data from FFmpeg");
					uncomprOff += rv;

					if (uncomprOff >= uncompressedSize) close(wavread);
				}

				if (rev & (POLLHUP | POLLERR))
					break;
			}

			if (rev & POLLERR) {
				printf("I/O error to ffmpeg?\n");
			}

			waitpid(child, &rev, 0);
			if (rev != 0) {
				printf("FFmpeg exited with error %d!\n", rev);
			}

			ret.sampleLength = uncompressedSize / sizeof(float);
			ret.sampleData = new float[ret.sampleLength];
			memcpy(ret.sampleData, uncompr, uncompressedSize);
			free(uncompr);
		}
#else
		// sorry, not supported.
#ifndef NDEBUG
		printf("WARNING: trying to load a GSM sample, while this is unsupported. Output will be silence.\n");
#endif
		ret.sampleLength = 1;
		ret.sampleData = new float[1];
		ret.sampleData[0] = 0;
#endif

		return ret;
	}

#if defined(WIN32) || defined(_WIN32)
	BOOL __stdcall SampleLoader::driverEnumCallback(HACMDRIVERID driverId, DWORD_PTR dwInstance, DWORD fdwSupport)
	{
		if (SampleLoader::driverId) return 1;

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
		acmFormatEnum(driver, &formatDetails, formatEnumCallback, NULL, NULL);

		delete [] (char *)waveFormat;

		acmDriverClose(driver, 0);

		return 1;
	}

	BOOL __stdcall SampleLoader::formatEnumCallback(HACMDRIVERID driverId, LPACMFORMATDETAILS formatDetails, DWORD_PTR dwInstance, DWORD fdwSupport)
	{
		if (formatDetails->pwfx->wFormatTag == WAVE_FORMAT_GSM610 &&
			formatDetails->pwfx->nChannels == 1 &&
			formatDetails->pwfx->nSamplesPerSec == SampleRate)
		{
			SampleLoader::driverId = driverId;
		}
		return 1;
	}
#endif
}
