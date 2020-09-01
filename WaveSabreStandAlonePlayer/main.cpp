#include <WaveSabreCore.h>
#include <WaveSabreCore/Helpers.h>
#include <WaveSabrePlayerLib.h>
using namespace WaveSabrePlayerLib;

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if !defined(WIN32) && !defined(_WIN32)
#include <unistd.h>
#include <time.h>
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
	printf("ack, unknown device %d!\n", id);
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
#if defined(WIN32) || defined(_WIN32)
		"\t%s [/p|/prerender] [/w|/wav [out.wav]] [/t|/threads 3] <input.bin>\n"
#else
		"\t%s [-p|--prerender] [-w|--wav [out.wav]] [-t|--threads 3] <input.bin>\n"
#endif
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
	exit(0);
}
static void parse_args(struct player_args* args, int argc, char** argv)
{
	if (argc < 2) {
		print_usage(argv[0]);
		// unreachable
	}

	args->infile = NULL;
	args->outfile = NULL;
	args->nthreads = 3;
	args->prerender = false;
	args->wavwriter = false;

	char argpfix;
#if defined(WIN32) || defined(_WIN32)
	argpfix = '/';
#else
	argpfix = '-';
#endif

	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == argpfix) {
			const char* as = &argv[i][1];

			if (!strcmp(as, "h") || !strcmp(as, "-help") || !strcmp(as, "help")) {
				print_usage(argv[0]);
				// unreachable
			}
			// TODO: option for printing version info
			if (!strcmp(as, "p") || !strcmp(as, "-prerender") || !strcmp(as, "prerender")) {
				args->prerender = true;
				continue;
			}
			if (!strcmp(as, "w") || !strcmp(as, "-wav") || !strcmp(as, "wav")) {
				args->wavwriter = true;

				if (i+1 < argc && argv[i+1][0] != argpfix) {
					args->outfile = argv[i+1];
					++i;
				}
				continue;
			}
			if (!strcmp(as, "t") || !strcmp(as, "-threads") || !strcmp(as, "threads")) {
				if (i+1 == argc) {
					printf("Expecting thread amount\n");
					exit(2);
				}

				++i;
				int res = sscanf(argv[i], "%d", &args->nthreads);
				if (res != 1 || args->nthreads < 0) {
					printf("Can't parse thread amount '%s'\n", argv[i]);
					exit(2);
				}

				continue;
			}

			printf("Unrecognised option '%s', ignoring...\n", argv[i]);
		} else {
			if (args->infile != NULL) {
				printf("Unexpected argument '%s'\n", argv[i]);
				exit(2);
			}

			args->infile = argv[i];
		}
	}

	if (args->infile == NULL) {
		if (args->outfile != NULL) {
			// oops, was probably meant as input file path
			args->infile = args->outfile;
			args->outfile = NULL;
		}
	}

	if (args->outfile == NULL && args->wavwriter) {
		args->outfile = "out.wav";
	}
}

/*#if HAVE_SDL2
int SDLmain(int argc, char **argv)
#else*/
int main(int argc, char **argv)
//#endif
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
		if (!outf) {
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
			fflush(stdout);
	#if HAVE_PTHREAD
			sleep( 1);
	#else
			struct timespec to;
			to.tv_sec = 0;
			to.tv_nsec = 50*1000*1000; // 50 ms
			nanosleep(&to, NULL);
	#endif
#endif
		}
		printf("\n");

		delete player;
	}

	free(buffer);
	return 0;
}
