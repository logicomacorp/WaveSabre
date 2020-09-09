#ifndef __WAVESABREPLAYERLIB_CRITICALSECTION__
#define __WAVESABREPLAYERLIB_CRITICALSECTION__

#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#elif HAVE_PTHREAD
#include <pthread.h>
#include <semaphore.h>

#include <atomic>
#endif

namespace WaveSabrePlayerLib
{
	class CriticalSection
	{
	public:
		class CriticalSectionGuard
		{
		public:
			CriticalSectionGuard(CriticalSection *criticalSection);
			~CriticalSectionGuard();

		private:
			CriticalSection *criticalSection;
		};

		CriticalSection();
		~CriticalSection();

		CriticalSectionGuard Enter();

	private:
#if defined(WIN32) || defined(_WIN32)
		CRITICAL_SECTION criticalSection;
#elif HAVE_PTHREAD
		pthread_mutex_t pmutex;
#endif
	};
}

#endif
