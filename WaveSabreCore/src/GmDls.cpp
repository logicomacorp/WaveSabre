#include <WaveSabreCore/GmDls.h>

#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#elif HAVE_EXTERNAL_GMDLS
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#endif

static char const *gmDlsPaths[2] =
{
	"drivers/gm.dls",
	"drivers/etc/gm.dls"
};

namespace WaveSabreCore
{
	unsigned char *GmDls::Load()
	{
#if defined(WIN32) || defined(_WIN32)
		HANDLE gmDlsFile = INVALID_HANDLE_VALUE;
		for (int i = 0; gmDlsFile == INVALID_HANDLE_VALUE; i++)
		{
			OFSTRUCT reOpenBuff;
			gmDlsFile = (HANDLE)(UINT_PTR)OpenFile(gmDlsPaths[i], &reOpenBuff, OF_READ);
		}

		auto gmDlsFileSize = GetFileSize(gmDlsFile, NULL);
		auto gmDls = new unsigned char[gmDlsFileSize];
		unsigned int bytesRead;
		ReadFile(gmDlsFile, gmDls, gmDlsFileSize, (LPDWORD)&bytesRead, NULL);
		CloseHandle(gmDlsFile);

		return gmDls;
#elif HAVE_EXTERNAL_GMDLS
		unsigned char *rv = NULL;

		bool gmDlsPathAlloc = false;
		bool dataHomeAlloc = false;
		char *gmDlsPath, *dataHome;
		char *home = getenv("HOME");
		FILE *filed;
		long filesz;

		// first, check if there's a user-specified one available
		gmDlsPath = getenv("WAVESABRE_GMDLS_PATH");

		if (gmDlsPath != NULL && access(gmDlsPath, R_OK) == 0)
			goto have_gmdls;

try_xdgdat:
		// try something XDG-conformant next
		dataHome = getenv("XDG_DATA_HOME");
		if (dataHome == NULL) {
			if (home == NULL)
				goto try_winepfx; // fuck it

			dataHomeAlloc = true;
			dataHome = (char*)malloc(0x100/*PATH_MAX*/);
			snprintf(dataHome, 0x100, "%s/.local/share", home);
		}

		gmDlsPathAlloc = true;
		gmDlsPath = (char*)malloc(0x100);
		snprintf(gmDlsPath, 0x100, "%s/WaveSabre/gm.dls", dataHome);
		if (dataHomeAlloc) free(dataHome);

		if (access(gmDlsPath, R_OK) == 0)
			goto have_gmdls;

		free(gmDlsPath);
		gmDlsPathAlloc = false;

		// ... try grabbing it from the user's wineprefix, because no trick too dirty for us
try_winepfx:
		if (home == NULL)
			goto try_cwd;

		for (int i = 0; i < sizeof(gmDlsPaths)/sizeof(*gmDlsPaths); ++i) {
			gmDlsPathAlloc = true;
			gmDlsPath = (char*)malloc(0x100);
			snprintf(gmDlsPath, 0x100, "%s/.wine/drive_c/windows/system32/%s", home, gmDlsPaths[i]);

			if (access(gmDlsPath, R_OK) == 0)
				goto have_gmdls;

			free(gmDlsPath);
			gmDlsPathAlloc = false;
		}

try_cwd: // fuck it, grab it from the current dir
		gmDlsPath = "./gm.dls";

		if (access(gmDlsPath, R_OK) == 0)
			goto have_gmdls;

		return rv;

have_gmdls: // actually read the file
		filed = fopen(gmDlsPath, "rb");
		if (filed == NULL)
			goto end;

		// get size of file
		fseek(filed, 0, SEEK_END);
		filesz = ftell(filed);
		fseek(filed, 0, SEEK_SET);

		rv = new unsigned char[filesz];
		fread(rv, 1, filesz, filed);
		fclose(filed);

end:
		if (gmDlsPathAlloc) free(gmDlsPath);

		return rv;
#else
		return NULL;
#endif
	}
}
