#include <WaveSabreCore/Helpers.h>

#include <inttypes.h>
#define _USE_MATH_DEFINES
#include <math.h>

// TODO: make assembly equivalent for Windows x64 (use intrinsic ?)
//       ^--- you probably only need to change esp to rsp?  -poro

#if (defined(_MSC_VER) && defined(_M_IX86)) || defined(__GNUC__)
	#define ASM_MATH_AVAILABLE (1)
#else /* nor MSVC nor GCC/clang */
	#define ASM_MATH_AVAILABLE (0)
#endif

#if ASM_MATH_AVAILABLE == 1
#if defined(_MSC_VER) && defined(_M_IX86)
static __declspec(naked) double __vectorcall fpuPow(double x, double y)
{
	__asm
	{
		sub esp, 8

		xorpd xmm2, xmm2

		comisd xmm1, xmm2
		jne base_test

		fld1
		jmp done

base_test:
		comisd xmm0, xmm2
		jne calc_pow

		fldz
		jmp done

calc_pow:
		movsd mmword ptr [esp], xmm1
		fld qword ptr [esp]
		movsd mmword ptr [esp], xmm0
		fld qword ptr [esp]
		fyl2x
		fld st(0)
		frndint
		fsubr st(1), st(0)
		fxch st(1)
		fchs
		f2xm1
		fld1
		faddp st(1), st(0)
		fscale
		fstp st(1)

done:
		fstp qword ptr [esp]
		movsd xmm0, mmword ptr [esp]

		add esp, 8

		ret
	}
}
#elif defined(__GNUC__)
	#if defined(__x86_64__) || defined(__i386__)
__attribute__((__naked__,__noinline__)) static double fpuPow(double x, double y)
{
	// i386 Linux ABI: pass thru the stack, return in st(0)
	// x86_64 SysV ABI: pass/return thru xmm0/1
	asm volatile(
#ifdef __x86_64__
				 "subq $8, %%rsp\n"
#else
				 "movsd  4(%%esp), %%xmm0\n"
				 "movsd 12(%%esp), %%xmm1\n"
				 "subl $8, %%esp\n"
#endif
				 "xorpd %%xmm2, %%xmm2\n"
				 "comisd %%xmm2, %%xmm1\n"
				 "jne 1f\n"

				 "fld1\n"
				 "jmp 3f\n"

				 "1:\n"
				 "comisd %%xmm2, %%xmm0\n"
				 "jne 2f\n"

				 "fldz\n"
				 "jmp 3f\n"

				 "2:\n"
#ifdef __x86_64__
				 "movsd %%xmm1, (%%rsp)\n"
				 "fldl (%%rsp)\n"
				 "movsd %%xmm0, (%%rsp)\n"
				 "fldl (%%rsp)\n"
#else
				 "movsd %%xmm1, (%%esp)\n"
				 "fldl (%%esp)\n"
				 "movsd %%xmm0, (%%esp)\n"
				 "fldl (%%esp)\n"
#endif

				 "fyl2x\n"
				 "fld %%st(0)\n"
				 "frndint\n"
				 "fsub %%st(0), %%st(1)\n"
				 "fxch %%st(1)\n"
				 "fchs\n"
				 "f2xm1\n"
				 "fld1\n"
				 "faddp %%st(0), %%st(1)\n"
				 "fscale\n"
				 "fstp %%st(1)\n"

				 "3:\n"
#ifdef __x86_64__
				 "fstpl (%%rsp)\n"
				 "movsd (%%rsp), %%xmm0\n"
				 "addq $8, %%rsp\n"
#else
				 "addl $8, %%esp\n"
#endif
				 "ret\n"
				 :// no output
				 :// no input
				 :"xmm2" // clobbered
				 );
}
	#else
		// __builtin_pow only supports integer exponents... so if the exponent
		// is an integer, use __builtin_pow, using some preprocessor magic
		#define fpuPow(x, y) \
			((__builtin_constant_p(y) && ((y) == (int)(y))) \
				? __builtin_pow(x, y) \
				: pow(x, y)) \

	#endif
#else
#error "Unsupported compiler."
#endif /* compiler */

#if defined(_MSC_VER) && defined(_M_IX86)
static __declspec(naked) float __vectorcall fpuPowF(float x, float y)
{
	__asm
	{
		sub esp, 8

		xorps xmm2, xmm2

		comiss xmm1, xmm2
		jne base_test

		fld1
		jmp done

base_test:
		comiss xmm0, xmm2
		jne calc_pow

		fldz
		jmp done

calc_pow:
		movss mmword ptr [esp], xmm1
		fld dword ptr [esp]
		movss mmword ptr [esp], xmm0
		fld dword ptr [esp]
		fyl2x
		fld st(0)
		frndint
		fsubr st(1), st(0)
		fxch st(1)
		fchs
		f2xm1
		fld1
		faddp st(1), st(0)
		fscale
		fstp st(1)

done:
		fstp dword ptr [esp]
		movss xmm0, mmword ptr [esp]

		add esp, 8

		ret
	}
}
#elif defined(__GNUC__)
	#if defined(__x86_64__) || defined(__i386__)
__attribute__((__naked__,__noinline__)) static float fpuPowF(float x, float y)
{
	// i386 Linux ABI: pass thru the stack, return in st(0)
	// x86_64 SysV ABI: pass/return thru xmm0/1
	asm volatile(
#ifdef __x86_64__
				 "subq $8, %%rsp\n"
#else
				 "movss 4(%%esp), %%xmm0\n"
				 "movss 8(%%esp), %%xmm1\n"
				 "subl $8, %%esp\n"
#endif
				 "xorps %%xmm2, %%xmm2\n"
				 "comiss %%xmm2, %%xmm1\n"
				 "jne 1f\n"

				 "fld1\n"
				 "jmp 3f\n"

				 "1:\n"
				 "comiss %%xmm2, %%xmm0\n"
				 "jne 2f\n"

				 "fldz\n"
				 "jmp 3f\n"

				 "2:\n"
#ifdef __x86_64__
				 "movss %%xmm1, (%%rsp)\n"
				 "flds (%%rsp)\n"
				 "movss %%xmm0, (%%rsp)\n"
				 "flds (%%rsp)\n"
#else
				 "movss %%xmm1, (%%esp)\n"
				 "flds (%%esp)\n"
				 "movss %%xmm0, (%%esp)\n"
				 "flds (%%esp)\n"
#endif

				 "fyl2x\n"
				 "fld %%st(0)\n"
				 "frndint\n"
				 "fsub %%st(0), %%st(1)\n"
				 "fxch %%st(1)\n"
				 "fchs\n"
				 "f2xm1\n"
				 "fld1\n"
				 "faddp %%st(0), %%st(1)\n"
				 "fscale\n"
				 "fstp %%st(1)\n"

				 "3:\n"
#ifdef __x86_64__
				 "fstps (%%rsp)\n"
				 "movss (%%rsp), %%xmm0\n"
				 "addq $8, %%rsp\n"
#else
				 "addl $8, %%esp\n"
#endif
				 "ret\n"
				 :// no output
				 :// no input
				 :"xmm2" // clobbered
				 );
}
	#else
		// __builtin_powf only supports integer exponents... so if the exponent
		// is an integer, use __builtin_powf, using some preprocessor magic
		#define fpuPowF(x, y) \
			((__builtin_constant_p(y) && ((y) == (int)(y))) \
				? __builtin_powf(x, y) \
				: powf(x, y)) \

	#endif
#else
#error "Unsupported compiler."
#endif /* compiler */

#if defined(_MSC_VER) && defined(_M_IX86)
static __declspec(naked) double __vectorcall fpuCos(double x)
{
	__asm
	{
		sub esp, 8

		movsd mmword ptr [esp], xmm0
		fld qword ptr [esp]
		fcos
		fstp qword ptr [esp]
		movsd xmm0, mmword ptr [esp]

		add esp, 8

		ret
	}
}
#elif defined(__GNUC__)
	#if defined(__x86_64__) || defined(__i386__)
__attribute__((__always_inline__)) inline static double fpuCos(double x)
{
	// not writing the *entire* function body in assembly actually helps
	// gcc and clang with inlining and LTO
	// ... except trying this with fpuPow/F somehow got botched, so those I
	// wrote as pure assembly
	asm volatile("fcos\n":"+t"(x)::);
	return x;
}
	#else /* x86_64 */
		#define fpuCos(x) __builtin_cos(x)
	#endif /* GNUC, platform */
#else
#error "Unsupported compiler."
#endif /* compiler */
#endif // ASM_MATH_AVAILABLE == 1

namespace WaveSabreCore
{
	double Helpers::CurrentSampleRate = 44100.0;
	int Helpers::CurrentTempo = 120;
	int Helpers::RandomSeed = 1;

	static const int fastCosTabLog2Size = 10; // size = 1024
	static const int fastCosTabSize = (1 << fastCosTabLog2Size);
	double Helpers::fastCosTab[fastCosTabSize + 1];

	void Helpers::Init()
	{
		RandomSeed = 1;

		for (int i = 0; i < fastCosTabSize + 1; i++)
		{
			double phase = double(i) * ((M_PI * 2) / fastCosTabSize);
#if ASM_MATH_AVAILABLE == 1
			fastCosTab[i] = fpuCos(phase);
#else
			fastCosTab[i] = cos(phase);
#endif
		}
	}

	float Helpers::RandFloat()
	{
		return (float)((RandomSeed *= 0x15a4e35) % 255) / 255.0f;
	}

	double Helpers::Pow(double x, double y)
	{
#if ASM_MATH_AVAILABLE == 1
		return fpuPow(x, y);
#else
		return pow(x, y);
#endif
	}

	float Helpers::PowF(float x, float y)
	{
#if ASM_MATH_AVAILABLE == 1
		return fpuPowF(x, y);
#else
		return powf(x, y);
#endif
	}

	double Helpers::FastCos(double x)
	{
		x = fabs(x); // cosine is symmetrical around 0, let's get rid of negative values

		// normalize range from 0..2PI to 1..2
		const auto phaseScale = 1.0 / (M_PI * 2);
		auto phase = 1.0 + x * phaseScale;

		auto phaseAsInt = *reinterpret_cast<unsigned long long *>(&phase);
		int exponent = (phaseAsInt >> 52) - 1023;

		const auto fractBits = 32 - fastCosTabLog2Size;
		const auto fractScale = 1 << fractBits;
		const auto fractMask = fractScale - 1;

		auto significand = (unsigned int)((phaseAsInt << exponent) >> (52 - 32));
		auto index = significand >> fractBits;
		int fract = significand & fractMask;

		auto left = fastCosTab[index];
		auto right = fastCosTab[index + 1];

		auto fractMix = fract * (1.0 / fractScale);
		return left + (right - left) * fractMix;
	}

	double Helpers::FastSin(double x)
	{
		return FastCos(x - M_PI_2);
	}

	double Helpers::Square135(double phase)
	{
		return FastSin(phase) +
			FastSin(phase * 3.0) / 3.0 +
			FastSin(phase * 5.0) / 5.0;
	}

	double Helpers::Square35(double phase)
	{
		return FastSin(phase * 3.0) / 3.0 +
			FastSin(phase * 5.0) / 5.0;
	}

	float Helpers::Mix(float v1, float v2, float mix)
	{
		return v1 * (1.0f - mix) + v2 * mix;
	}

	float Helpers::Clamp(float f, float min, float max)
	{
		if (f < min) return min;
		if (f > max) return max;
		return f;
	}

	double Helpers::NoteToFreq(double note)
	{
		return 440.0 * Pow(2.0, (note - 69.0) / 12.0);
	}

	float Helpers::DbToScalar(float db)
	{
		return PowF(2.0f, db / 6.0f);
	}

	float Helpers::EnvValueToScalar(float value)
	{
		return sqrtf((value - 1.0f) / 5000.0f);
	}

	float Helpers::ScalarToEnvValue(float scalar)
	{
		return scalar * scalar * 5000.0f + 1.0f;
	}

	float Helpers::VolumeToScalar(float volume)
	{
		float v = volume * .4f;
		return v * v;
	}

	float Helpers::ScalarToVolume(float scalar)
	{
		return sqrtf(scalar) / .4f;
	}

	bool Helpers::ParamToBoolean(float value)
	{
		return value >= .5f;
	}

	float Helpers::BooleanToParam(bool b)
	{
		return b ? 1.0f : 0.0f;
	}

	float Helpers::ParamToFrequency(float param)
	{
		return 20.0f + (20000.0f - 20.0f) * param * param;
	}

	float Helpers::FrequencyToParam(float freq)
	{
		return sqrtf((freq - 20.0f) / (20000.0f - 20.0f));
	}

	float Helpers::ParamToQ(float param)
	{
		if (param < .5f)
		{
			return param / .5f * (1.0f - .33f) + .33f;
		}
		else
		{
			return (param - .5f) / .5f * 11.0f + 1.0f;
		}
	}

	float Helpers::QToParam(float q)
	{
		if (q < 1.0f)
		{
			return (q - .33f) / (1.0f - .33f) * .5f;
		}
		else
		{
			return (q - 1.0f) / 11.0f * .5f + .5f;
		}
	}

	float Helpers::ParamToDb(float param, float range)
	{
		return (param * 2.0f - 1.0f) * range;
	}

	float Helpers::DbToParam(float db, float range)
	{
		return (db / range + 1.0f) / 2.0f;
	}

	float Helpers::ParamToResonance(float param)
	{
		return param * .99f + .01f;
	}

	float Helpers::ResonanceToParam(float resonance)
	{
		return (resonance - .01f) / .99f;
	}

	StateVariableFilterType Helpers::ParamToStateVariableFilterType(float param)
	{
		return (StateVariableFilterType)(int)(param * 3.0f);
	}

	float Helpers::StateVariableFilterTypeToParam(StateVariableFilterType type)
	{
		return (float)type / 3.0f;
	}

	int Helpers::ParamToUnisono(float param)
	{
		return (int)(param * 15.0f) + 1;
	}

	float Helpers::UnisonoToParam(int unisono)
	{
		return (float)(unisono - 1) / 15.0f;
	}

	double Helpers::ParamToVibratoFreq(float param)
	{
		return (Pow((double)param, 2.0) + .1) * 70.0;
	}

	float Helpers::VibratoFreqToParam(double vf)
	{
		double d = vf / 70.0 - .1;
		return d >= 0.0 ? (float)sqrt(d) : 0.0f;
	}

	float Helpers::PanToScalarLeft(float pan)
	{
		return sqrtf(1.0f - pan);
	}

	float Helpers::PanToScalarRight(float pan)
	{
		return sqrtf(pan);
	}

	Spread Helpers::ParamToSpread(float param)
	{
		return (Spread)(int)(param * 2.0f);
	}

	float Helpers::SpreadToParam(Spread spread)
	{
		return (float)spread / 2.0f;
	}

	VoiceMode Helpers::ParamToVoiceMode(float param)
	{
		return (VoiceMode)(int)(param * 1.0f);
	}

	float Helpers::VoiceModeToParam(VoiceMode voiceMode)
	{
		return (float)voiceMode / 1.0f;
	}
}
