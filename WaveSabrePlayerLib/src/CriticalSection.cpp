#include <WaveSabrePlayerLib/CriticalSection.h>

namespace WaveSabrePlayerLib
{
	CriticalSection::CriticalSectionGuard::CriticalSectionGuard(CriticalSection *criticalSection)
		: criticalSection(criticalSection)
	{
#if defined(WIN32) || defined(_WIN32)
		EnterCriticalSection(&criticalSection->criticalSection);
#elif HAVE_PTHREAD
		pthread_mutex_lock(&criticalSection->pmutex);
#endif
	}

	CriticalSection::CriticalSectionGuard::~CriticalSectionGuard()
	{
#if defined(WIN32) || defined(_WIN32)
		LeaveCriticalSection(&criticalSection->criticalSection);
#elif HAVE_PTHREAD
		pthread_mutex_unlock(&criticalSection->pmutex);
#endif
	}


	CriticalSection::CriticalSection()
#if HAVE_PTHREAD
		: pmutex(PTHREAD_MUTEX_INITIALIZER)
#endif
	{
#if defined(WIN32) || defined(_WIN32)
		InitializeCriticalSection(&criticalSection);
#elif HAVE_PTHREAD
		pthread_mutex_init(&pmutex, NULL);
#endif
	}

	CriticalSection::~CriticalSection()
	{
#if defined(WIN32) || defined(_WIN32)
		DeleteCriticalSection(&criticalSection);
#elif HAVE_PTHREAD
		pthread_mutex_destroy(&pmutex);
#endif
	}

	CriticalSection::CriticalSectionGuard CriticalSection::Enter()
	{
		return CriticalSectionGuard(this);
	}
}
