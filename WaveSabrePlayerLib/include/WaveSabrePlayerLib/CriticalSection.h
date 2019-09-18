#ifndef __WAVESABREPLAYERLIB_CRITICALSECTION__
#define __WAVESABREPLAYERLIB_CRITICALSECTION__

#include <Windows.h>

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
		CRITICAL_SECTION criticalSection;
	};
}

#endif
