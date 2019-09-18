#ifndef __WAVESABRECORE_DELAYBUFFER_H__
#define __WAVESABRECORE_DELAYBUFFER_H__

namespace WaveSabreCore
{
	class DelayBuffer
	{
	public:
		DelayBuffer(float lengthMs = 0.0f);
		~DelayBuffer();

		void SetLength(float lengthMs);

		void WriteSample(float sample);
		float ReadSample() const;

	private:
		int length;
		float *buffer;
		int currentPosition;
	};
}

#endif