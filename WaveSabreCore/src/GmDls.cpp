#include <WaveSabreCore/GmDls.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef __APPLE__
#include <sys/stat.h>
#include <CoreFoundation/CoreFoundation.h>
#include <glob.h>
#endif

#ifdef _WIN32
static char *gmDlsPaths[2] =
{
	"drivers/gm.dls",
	"drivers/etc/gm.dls"
};
#endif

namespace WaveSabreCore
{
	unsigned char *GmDls::Load()
	{
#ifdef _WIN32
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
#endif
#ifdef __APPLE__
        glob_t g;
        glob("~/Library/Audio/Sounds/Banks/gm.dls", GLOB_TILDE, NULL, &g);
        FILE *file = fopen(g.gl_pathv[0], "rb");
	    struct stat st;
	    stat(g.gl_pathv[0], &st);
	    unsigned char *gmDls = new unsigned char[st.st_size];
	    fread(gmDls, 1, st.st_size, file);
	    fclose(file);
        globfree(&g);
#endif
		return gmDls;
	}
}
