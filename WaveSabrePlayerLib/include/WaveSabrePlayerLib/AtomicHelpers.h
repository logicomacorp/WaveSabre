#ifndef __WAVESABREPLAYERLIB_ATOMIC_HELPERS_H__
#define __WAVESABREPLAYERLIB_ATOMIC_HELPERS_H__

#ifdef _WIN32
#include <Windows.h>
#else
#include <atomic>
#endif

namespace WaveSabreCore
{
	namespace AtomicHelpers
	{
		template<typename T>
		inline bool CmpXchg(T* value, T newval, T expect)
		{
#ifdef _WIN32
			return InterlockedCompareExchange((unsigned int*)value,
					(unsigned int)newval, (unsigned int)expect)
				== (unsigned int)expect;
#else
			return std::atomic_compare_exchange_strong((std::atomic_int*)value,
					(int*)&expect, newval);
#endif
		}

		template<typename T>
		inline T XDec(T* value)
		{
#ifdef _WIN32
			return (T)InterlockedDecrement((unsigned int*)value);
#else
			// returns the value *before* the call
			return (T)(std::atomic_fetch_sub((std::atomic_int*)value, 1) - 1);
#endif
		}
	}
}

#endif
