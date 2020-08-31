#include <WaveSabreCore/GmDls.h>

#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
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
#else
		return nullptr;
#endif
	}
}
