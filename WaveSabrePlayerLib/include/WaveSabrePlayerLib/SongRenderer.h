#ifndef __WAVESABREPLAYERLIB_SONGRENDERER_H__
#define __WAVESABREPLAYERLIB_SONGRENDERER_H__

#include <WaveSabreCore.h>

namespace WaveSabrePlayerLib
{
	class SongRenderer
	{
	public:
		enum class DeviceId
		{
			Falcon,
			Slaughter,
			Thunder,
			Scissor,
			Leveller,
			Crusher,
			Echo,
			Smasher,
			Chamber,
			Twister,
			Cathedral,
			Adultery,
			Specimen
		};

		typedef WaveSabreCore::Device *(*DeviceFactory)(DeviceId);

		typedef struct {
			DeviceFactory factory;
			const unsigned char *blob;
		} Song;

		typedef short Sample;

		static const int NumChannels = 2;

		SongRenderer(const SongRenderer::Song *song);
		~SongRenderer();

		void RenderSamples(Sample *buffer, int numSamples);

		int GetTempo() const;
		int GetSampleRate() const;
		double GetLength() const;

	private:
		enum class EventType
		{
			NoteOn,
			NoteOff,
		};

		typedef struct
		{
			int TimeStamp;
			EventType Type;
			int Note, Velocity;
		} Event;

		class Devices
		{
		public:
			int numDevices;
			WaveSabreCore::Device **devices;
		};

		class MidiLane
		{
		public:
			int numEvents;
			Event *events;
		};

		class Track
		{
		public:
			Track(SongRenderer *songRenderer, DeviceFactory factory);
			~Track();
			
			void Run(int numSamples);

		private:
			static const int numBuffers = 4;
		public:
			float *Buffers[numBuffers];
		private:

			typedef struct
			{
				int SendingTrackIndex;
				int ReceivingChannelIndex;
				float Volume;
			} Receive;

			class Automation
			{
			public:
				Automation(SongRenderer *songRenderer, WaveSabreCore::Device *device);
				~Automation();

				void Run(int numSamples);

			private:
				typedef struct
				{
					int TimeStamp;
					float Value;
				} Point;

				WaveSabreCore::Device *device;
				int paramId;

				int numPoints;
				Point *points;

				int samplePos;
				int pointIndex;
			};

			SongRenderer *songRenderer;

			float volume;

			int numReceives;
			Receive *receives;

			int numDevices;
			int *devicesIndicies;

			int midiLaneId;

			int numAutomations;
			Automation **automations;

			int lastSamplePos;
			int accumEventTimestamp;
			int eventIndex;
		};

		// TODO: Templatize? Might actually be bigger..
		unsigned char readByte();
		int readInt();
		float readFloat();
		double readDouble();

		const unsigned char *songBlobPtr;
		int songDataIndex;

		int bpm;
		int sampleRate;
		double length;
	
		int numDevices;
		WaveSabreCore::Device **devices;

		int numMidiLanes;
		MidiLane **midiLanes;

		int numTracks;
		Track **tracks;

	};
}

#endif
