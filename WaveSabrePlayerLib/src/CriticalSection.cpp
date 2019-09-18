#include <WaveSabrePlayerLib/CriticalSection.h>

namespace WaveSabrePlayerLib
{
	CriticalSection::CriticalSectionGuard::CriticalSectionGuard(CriticalSection *criticalSection)
		: criticalSection(criticalSection)
	{
		EnterCriticalSection(&criticalSection->criticalSection);
	}

	CriticalSection::CriticalSectionGuard::~CriticalSectionGuard()
	{
		LeaveCriticalSection(&criticalSection->criticalSection);
	}

	CriticalSection::CriticalSection()
	{
		InitializeCriticalSection(&criticalSection);
	}

	CriticalSection::~CriticalSection()
	{
		DeleteCriticalSection(&criticalSection);
	}

	CriticalSection::CriticalSectionGuard CriticalSection::Enter()
	{
		return CriticalSectionGuard(this);
	}
}
