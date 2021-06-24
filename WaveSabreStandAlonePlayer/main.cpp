#include <WaveSabreCore.h>
#include <WaveSabrePlayerLib.h>
using namespace WaveSabrePlayerLib;

#include <stdlib.h>
#include <stdio.h>
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

struct player_args
{
	const char* infile;
	const char* outfile;
	int nthreads;
	bool prerender;
	bool wavwriter;
};

static void print_usage(const char* prgm)
{
	printf("%s: WaveSabre standalone song player/renderer\n"
		"Usage:\n"
		"\t%s [-p|--prerender] [-w|--wav [out.wav]] [-t|--threads n] <input.bin>\n"
		"\n"
		"Arguments:\n"
		"\tprerender prerender the song instead of rendering while playing.\n"
		"\twav       instead of playing back the song, write a WAV file. By default,\n"
		"\t          the output file is called `out.wav', but another may be supplied\n"
		"\t          immediately after this argument.\n"
		"\tthreads   the amount of threads to use in parallel for rendering. By\n"
		"\t          default, this number is 3.\n"
		"\tinput.bin the input song file, exported by the WaveSabre exporter.\n"
		, prgm, prgm);
}
static void parse_args(struct player_args* args, int argc, char** argv)
{
	if (argc < 2)
	{
		print_usage(argv[0]);
		exit(1);
	}

	args->infile = NULL;
	args->outfile = NULL;
	args->nthreads = 3;
	args->prerender = false;
	args->wavwriter = false;

	for (int i = 1; i < argc; ++i)
	{
		if (argv[i][0] == '-')
		{
			const char* as = argv[i] + 1; // skip arg prefix char ('-')

			if (!strcmp(as, "h") || !strcmp(as, "-help"))
			{
				print_usage(argv[0]);
				exit(0);
			}
			// TODO: option for printing version info
			if (!strcmp(as, "p") || !strcmp(as, "-prerender"))
			{
				args->prerender = true;
				continue;
			}
			if (!strcmp(as, "w") || !strcmp(as, "-wav"))
			{
				args->wavwriter = true;

				if (i+1 < argc && argv[i+1][0] != '-')
				{
					// last argument, and no input file set yet, so we really
					// need to prioritise the input file here
					if (argc-1 == i+1 && args->infile == NULL)
					{
						args->outfile = "out.wav";
					}
					else
					{
						args->outfile = argv[i+1];
						++i;
					}
				}
				else
				{
					args->outfile = "out.wav";
				}
				continue;
			}
			if (!strcmp(as, "t") || !strcmp(as, "-threads"))
			{
				if (i+1 == argc)
				{
					printf("Expecting thread amount\n");
					exit(2);
				}

				++i;
				int res = sscanf(argv[i], "%d", &args->nthreads);
				if (res != 1 || args->nthreads < 0)
				{
					printf("Can't parse thread amount '%s'\n", argv[i]);
					exit(2);
				}

				continue;
			}

			printf("Unrecognised option '%s', ignoring...\n", argv[i]);
		}
		else
		{
			if (args->infile != NULL)
			{
				printf("Unexpected argument '%s'\n", argv[i]);
				exit(2);
			}

			args->infile = argv[i];
		}
	}
}

int main(int argc, char **argv)
{
	struct player_args args;
	parse_args(&args, argc, argv);

	FILE * pFile;
	long lSize;
	unsigned char * buffer;
	size_t result;

	pFile = fopen(args.infile, "rb");
	if (pFile == NULL) {
		printf("Can't open input file '%s'\n", args.infile);
		exit(1);
	}

	// obtain file size:
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	// allocate memory to contain the whole file:
	buffer = (unsigned char*)malloc(sizeof(unsigned char)*lSize);

	// copy the file into the buffer:
	result = fread(buffer, 1, lSize, pFile);
	if (result != lSize) {
		printf("Can't read from input file '%s'\n", args.infile);
		exit(3);
	}

	// terminate
	fclose(pFile);

	SongRenderer::Song song;
	song.blob = buffer;
	song.factory = SongFactory;

	if (args.wavwriter)
	{
		FILE* outf = fopen(args.outfile, "wb");
		if (!outf)
		{
			printf("Can't open output file '%s'\n", args.outfile);
			exit(4);
		}
		fclose(outf); // close again because WavWriter wants a file path

		WavWriter wavWriter(&song, args.nthreads);

		printf("WAV writer activated.\n");

		printf("Rendering...\n");
		wavWriter.Write(args.outfile, progressCallback, nullptr);

		printf("\n\nWAV file written to \"%s\". Enjoy.\n", args.outfile);
	}
	else
	{
		IPlayer *player;

		if (args.prerender)
		{
			printf("Prerender activated.\n");
			printf("Rendering...\n");

			player = new PreRenderPlayer(&song, args.nthreads, progressCallback, nullptr);

			printf("\n\n");
		}
		else
		{
			player = new RealtimePlayer(&song, args.nthreads);
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
