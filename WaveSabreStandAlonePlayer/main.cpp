#include <WaveSabreCore.h>
#include <WaveSabreCore/Helpers.h>
#include <WaveSabrePlayerLib.h>
using namespace WaveSabrePlayerLib;

#include <stdlib.h>
#include <string.h>
//#include <math.h>
#if !defined(WIN32) && !defined(_WIN32)
#include <unistd.h>
#endif

#if defined(_MSC_VER)
// MSVC WHYYYYYY
extern "C" FILE* __cdecl __iob_func(void)
{
	static FILE _iob[] = {*stdin, *stdout, *stderr};
	return _iob;
}
#endif

WaveSabreCore::Device *SongFactory(SongRenderer::DeviceId id)
{
	switch (id)
	{
	case SongRenderer::DeviceId::Falcon: return new WaveSabreCore::Falcon();
	case SongRenderer::DeviceId::Slaughter: return new WaveSabreCore::Slaughter();
#if defined(WIN32) || defined(_WIN32)
	case SongRenderer::DeviceId::Thunder: return new WaveSabreCore::Thunder();
#endif
	case SongRenderer::DeviceId::Scissor: return new WaveSabreCore::Scissor();
	case SongRenderer::DeviceId::Leveller: return new WaveSabreCore::Leveller();
	case SongRenderer::DeviceId::Crusher: return new WaveSabreCore::Crusher();
	case SongRenderer::DeviceId::Echo: return new WaveSabreCore::Echo();
	case SongRenderer::DeviceId::Smasher: return new WaveSabreCore::Smasher();
	case SongRenderer::DeviceId::Chamber: return new WaveSabreCore::Chamber();
	case SongRenderer::DeviceId::Twister: return new WaveSabreCore::Twister();
	case SongRenderer::DeviceId::Cathedral: return new WaveSabreCore::Cathedral();
#if defined(WIN32) || defined(_WIN32)
	case SongRenderer::DeviceId::Adultery: return new WaveSabreCore::Adultery();
	case SongRenderer::DeviceId::Specimen: return new WaveSabreCore::Specimen();
#endif
	}
	fprintf(stderr, "ack, unknown device %d!\n", id);
	return nullptr;
}

void progressCallback(double progress, void *data)
{
	const int barLength = 32;
	int filledChars = (int)(progress * (double)(barLength - 1));
	printf("\r[");
	for (int j = 0; j < barLength; j++) putchar(filledChars >= j ? '*' : '-');
	printf("]");
}

int main(int argc, char **argv)
{
	/*double minerr = 1.0/0, maxerr = -1.0/0;
	double stddev = 0, avg = 0;

	const int N = 1000;
	const double scaler = 0.001;

	//for (int y = 1; y <= N; ++y)
	int y = 1.0/12;
		for (int x = 1; x <= N; ++x) {
			double xd = x * scaler,
				   yd = y * scaler;

			double pf = WaveSabreCore::Helpers::Pow(xd, yd),
				   pm = pow(xd, yd); // from libm

			double err = abs(pf - pm);
			//err = err / pm; // fuck it, relative error

			if (err < minerr) minerr = err;
			if (err > maxerr) maxerr = err;

			avg += err;
			stddev += err*err;
			printf("%f\n",err);
		}

	avg /= N*N;
	stddev /= N*N - 1;
	stddev = sqrt(stddev);

	printf("#min=%f avg=%f max=%f stddev=%f\n", minerr, avg, maxerr, stddev);

	return 0;*/

	bool writeWav = argc >= 3 && !strcmp(argv[2], "-w");
	bool preRender = argc == 3 && !strcmp(argv[2], "-p");
#if !defined(WIN32) && !defined(_WIN32)
	/*if (!writeWav) {
		printf("W: playback not yet supported on non-Windows platforms."
		       "Writing WAV instead...\n");
		writeWav = true;
	}*/
	writeWav = true;
	preRender = true;
#endif

#if defined(WIN32) || defined(_WIN32)
	const int numRenderThreads = 3;
#else
	const int numRenderThreads = 3; // TODO
#endif

	FILE * pFile;
	long lSize;
	unsigned char * buffer;
	size_t result;

	pFile = fopen(argv[1], "rb");
	if (pFile == NULL) { printf("File error\n"); exit(1); }

	// obtain file size:
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	// allocate memory to contain the whole file:
	buffer = (unsigned char*)malloc(sizeof(unsigned char)*lSize);

	// copy the file into the buffer:
	result = fread(buffer, 1, lSize, pFile);
	if (result != lSize) { printf("Reading error\n"); exit(3); }

	// terminate
	fclose(pFile);

	SongRenderer::Song song;
	song.blob = buffer;
	song.factory = SongFactory;

	if (writeWav)
	{
		WavWriter wavWriter(&song, numRenderThreads);

		printf("WAV writer activated.\n");

		auto fileName = argc >= 4 ? argv[3] : "out.wav";
		printf("Rendering...\n");
		//asm volatile("int3\n");
		wavWriter.Write(fileName, progressCallback, nullptr);

		printf("\n\nWAV file written to \"%s\". Enjoy.\n", fileName);
	}
	else
	{
		IPlayer *player;

		if (preRender)
		{
			printf("Prerender activated.\n");
			printf("Rendering...\n");

			player = new PreRenderPlayer(&song, numRenderThreads, progressCallback, nullptr);

			printf("\n\n");
		}
		else
		{
			player = new RealtimePlayer(&song, numRenderThreads);
		}

#if defined(WIN32) || defined(_WIN32)
		printf("Realtime player activated. Press ESC to quit.\n");
#else
		printf("Realtime player activated. Press ^C to quit.\n");
#endif

		player->Play();
#if defined(WIN32) || defined(_WIN32)
		while (!GetAsyncKeyState(VK_ESCAPE))
#else
		while (true)
#endif
		{
			auto songPos = player->GetSongPos();
			if (songPos >= player->GetLength()) break;
			int minutes = (int)songPos / 60;
			int seconds = (int)songPos % 60;
			int hundredths = (int)(songPos * 100.0) % 100;
			printf("\r %.1i:%.2i.%.2i", minutes, seconds, hundredths);

			player->DoForegroundWork();
#if defined(WIN32) || defined(_WIN32)
			Sleep(10);
#else
			sleep( 1);
#endif
		}
		printf("\n");

		delete player;
	}

	free(buffer);
	return 0;
}
