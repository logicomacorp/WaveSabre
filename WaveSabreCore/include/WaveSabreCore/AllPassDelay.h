#ifndef __WAVESABRECORE_ALLPASSDELAY_H__
#define __WAVESABRECORE_ALLPASSDELAY_H__

namespace WaveSabreCore
{
	class AllPassDelay
	{
	public:
		AllPassDelay();

		void Delay(float delay);
		float Update(float inSamp);

	private:
		float a1, zm1;
	};
}

#endif
