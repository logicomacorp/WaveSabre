#include <WaveSabreCore.h>
#include <WaveSabrePlayerLib.h>
using namespace WaveSabrePlayerLib;

#include <string.h>

WaveSabreCore::Device *SongFactory(SongRenderer::DeviceId id)
{
	switch (id)
	{
	case SongRenderer::DeviceId::Falcon: return new WaveSabreCore::Falcon();
	case SongRenderer::DeviceId::Slaughter: return new WaveSabreCore::Slaughter();
	case SongRenderer::DeviceId::Thunder: return new WaveSabreCore::Thunder();
	case SongRenderer::DeviceId::Scissor: return new WaveSabreCore::Scissor();
	case SongRenderer::DeviceId::Leveller: return new WaveSabreCore::Leveller();
	case SongRenderer::DeviceId::Crusher: return new WaveSabreCore::Crusher();
	case SongRenderer::DeviceId::Echo: return new WaveSabreCore::Echo();
	case SongRenderer::DeviceId::Smasher: return new WaveSabreCore::Smasher();
	case SongRenderer::DeviceId::Chamber: return new WaveSabreCore::Chamber();
	case SongRenderer::DeviceId::Twister: return new WaveSabreCore::Twister();
	case SongRenderer::DeviceId::Cathedral: return new WaveSabreCore::Cathedral();
	case SongRenderer::DeviceId::Adultery: return new WaveSabreCore::Adultery();
	case SongRenderer::DeviceId::Specimen: return new WaveSabreCore::Specimen();
	}
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
	bool writeWav = argc >= 3 && !strcmp(argv[2], "-w");
	bool preRender = argc == 3 && !strcmp(argv[2], "-p");

	const int numRenderThreads = 3;

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

		printf("Realtime player activated. Press ESC to quit.\n");
		player->Play();
		while (!GetAsyncKeyState(VK_ESCAPE))
		{
			auto songPos = player->GetSongPos();
			if (songPos >= player->GetLength()) break;
			int minutes = (int)songPos / 60;
			int seconds = (int)songPos % 60;
			int hundredths = (int)(songPos * 100.0) % 100;
			printf("\r %.1i:%.2i.%.2i", minutes, seconds, hundredths);

			Sleep(10);
		}
		printf("\n");

		delete player;
	}

	free(buffer);
	return 0;
}