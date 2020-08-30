#ifndef __WAVESABREPLAYERLIB_SONGRENDERER_H__
#define __WAVESABREPLAYERLIB_SONGRENDERER_H__

#if defined(WIN32) || defined(_WIN32)
#include "CriticalSection.h"
#endif

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
		static const int BitsPerSample = 16;
		static const int BlockAlign = NumChannels * BitsPerSample / 8;

		SongRenderer(const SongRenderer::Song *song, int numRenderThreads);
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
			typedef struct
			{
				int SendingTrackIndex;
				int ReceivingChannelIndex;
				float Volume;
			} Receive;

			Track(SongRenderer *songRenderer, DeviceFactory factory);
			~Track();

			void Run(int numSamples);

		private:
			static const int numBuffers = 4;
		public:
			float *Buffers[numBuffers];

			int NumReceives;
			Receive *Receives;

		private:
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

			int numDevices;
			int *devicesIndicies;

			int midiLaneId;

			int numAutomations;
			Automation **automations;

			int lastSamplePos;
			int accumEventTimestamp;
			int eventIndex;
		};

		enum class TrackRenderState : unsigned int
		{
			Idle,
			Rendering,
			Finished,
		};

		typedef struct
		{
			SongRenderer *songRenderer;
			int renderThreadIndex;
		} RenderThreadData;

#if defined(WIN32) || defined(_WIN32)
		static DWORD WINAPI renderThreadProc(LPVOID lpParameter);
#endif

		bool renderThreadWork(int renderThreadIndex);

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
		TrackRenderState *trackRenderStates;

		int numRenderThreads;
#if defined(WIN32) || defined(_WIN32)
		HANDLE *additionalRenderThreads;
#endif

		bool renderThreadShutdown;
		int renderThreadNumFloatSamples;
		unsigned int renderThreadsRunning;
#if defined(WIN32) || defined(_WIN32)
		HANDLE *renderThreadStartEvents;
		HANDLE renderDoneEvent;
#endif
	};
}

#endif
