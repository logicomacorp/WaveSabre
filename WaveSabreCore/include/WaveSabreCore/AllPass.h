#ifndef __WAVESABRECORE_ALLPASS_H__
#define __WAVESABRECORE_ALLPASS_H__

namespace WaveSabreCore
{
	class AllPass
	{
	public:
		AllPass();
		virtual ~AllPass();

		void SetBufferSize(int size); 
		float Process(float inp);
		void SetFeedback(float val);
		float GetFeedback();

	private:
		float feedback;
		float *buffer;
		int bufferSize;
		int bufferIndex;
	};
}

#endif
