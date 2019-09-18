#ifndef __WAVESABREPLAYERLIB_IPLAYER_H__
#define __WAVESABREPLAYERLIB_IPLAYER_H__

namespace WaveSabrePlayerLib
{
	class IPlayer
	{
	public:
		virtual ~IPlayer();

		virtual void Play() = 0;

		virtual int GetTempo() const = 0;
		virtual int GetSampleRate() const = 0;
		virtual double GetLength() const = 0;
		virtual double GetSongPos() const = 0;
	};
}

#endif
