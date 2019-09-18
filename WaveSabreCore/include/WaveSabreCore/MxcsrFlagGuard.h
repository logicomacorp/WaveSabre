#ifndef __WAVESABRECORE_MXCSR_FLAG_GUARD_H__
#define __WAVESABRECORE_MXCSR_FLAG_GUARD_H__

namespace WaveSabreCore
{
	class MxcsrFlagGuard
	{
	public:
		MxcsrFlagGuard();
		~MxcsrFlagGuard();

	private:
		unsigned int mxcsrRestore;
	};
}

#endif
