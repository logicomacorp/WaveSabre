#ifndef __WAVESABRECORE_COMB_H__
#define __WAVESABRECORE_COMB_H__

namespace WaveSabreCore
{
	class Comb
	{
	public:
		Comb();
		virtual ~Comb();

		void SetBufferSize(int size);
		float Process(float inp);
		void SetDamp(float val);
		float GetDamp();
		void SetFeedback(float val);
		float GetFeedback();

	private:
		float feedback, filterStore;
		float damp1, damp2;
		float *buffer;
		int bufferSize, bufferIndex;
	};
}

#endif
