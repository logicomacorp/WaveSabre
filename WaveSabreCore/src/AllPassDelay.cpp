#include <WaveSabreCore/AllPassDelay.h>
#include <WaveSabreCore/Helpers.h>

#include <math.h>

namespace WaveSabreCore
{
	AllPassDelay::AllPassDelay()
	{
		a1 = 0.0f;
		zm1 = 0.0f;
	}

	void AllPassDelay::Delay(float delay)
	{
		a1 = (1.f - delay) / (1.f + delay);
	}

	float AllPassDelay::Update(float inSamp)
	{
		float y = inSamp * -a1 + zm1;
		zm1 = y * a1 + inSamp;

		return y;
	}
}
