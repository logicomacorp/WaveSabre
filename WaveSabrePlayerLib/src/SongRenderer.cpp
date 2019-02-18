#include <WaveSabrePlayerLib/SongRenderer.h>

using namespace WaveSabreCore;

namespace WaveSabrePlayerLib
{
	SongRenderer::SongRenderer(const SongRenderer::Song *song)
	{
		Helpers::Init();

		songBlobPtr = song->blob;

		bpm = readInt();
		sampleRate = readInt();
		length = readDouble();

		numDevices = readInt();
		devices = new Device *[numDevices];
		for (int i = 0; i < numDevices; i++)
		{
			devices[i] = song->factory((DeviceId)readByte());
			devices[i]->SetSampleRate((float)sampleRate);
			devices[i]->SetTempo(bpm);
			int chunkSize = readInt();
			devices[i]->SetChunk((void *)songBlobPtr, chunkSize);
			songBlobPtr += chunkSize;
		}

		numMidiLanes = readInt();
		midiLanes = new MidiLane *[numMidiLanes];
		for (int i = 0; i < numMidiLanes; i++)
		{
			midiLanes[i] = new MidiLane;
			int numEvents = readInt();
			midiLanes[i]->numEvents = numEvents;
			midiLanes[i]->events = new Event[numEvents];
			for (int m = 0; m < numEvents; m++)
			{
				midiLanes[i]->events[m].TimeStamp = readInt();
				byte note = readByte();
				if ((note & 0x80) == 0x00)
				{
					midiLanes[i]->events[m].Type = (EventType)0;
					midiLanes[i]->events[m].Note = (note & 0x7F);
					midiLanes[i]->events[m].Velocity = readByte();
				}
				else
				{
					midiLanes[i]->events[m].Type = (EventType)1;
					midiLanes[i]->events[m].Note = (note & 0x7F);
					midiLanes[i]->events[m].Velocity = 0;
				}
			}
		}

		numTracks = readInt();
		tracks = new Track *[numTracks];
		for (int i = 0; i < numTracks; i++) tracks[i] = new Track(this, song->factory);
	}

	SongRenderer::~SongRenderer()
	{
		for (int i = 0; i < numDevices; i++) delete devices[i];
		delete [] devices;

		for (int i = 0; i < numMidiLanes; i++) delete midiLanes[i];
		delete[] midiLanes;

		for (int i = 0; i < numTracks; i++) delete tracks[i];
		delete [] tracks;
	}

	void SongRenderer::RenderSamples(Sample *buffer, int numSamples)
	{
		MxcsrFlagGuard mxcsrFlagGuard;

		int numFloatSamples = numSamples / 2;

		for (int i = 0; i < numTracks; i++) tracks[i]->Run(numFloatSamples);

		float **masterTrackBuffers = tracks[numTracks - 1]->Buffers;
		for (int i = 0; i < numSamples; i++)
		{
			int sample = (int)(masterTrackBuffers[i & 1][i / 2] * 32767.0f);
			if (sample < -32768) sample = -32768;
			if (sample > 32767) sample = 32767;
			buffer[i] = (Sample)sample;
		}
	}

	int SongRenderer::GetTempo() const
	{
		return bpm;
	}

	int SongRenderer::GetSampleRate() const
	{
		return sampleRate;
	}

	double SongRenderer::GetLength() const
	{
		return length;
	}

	unsigned char SongRenderer::readByte()
	{
		unsigned char ret = *songBlobPtr;
		songBlobPtr++;
		return ret;
	}

	int SongRenderer::readInt()
	{
		int ret = *(int *)songBlobPtr;
		songBlobPtr += sizeof(int);
		return ret;
	}

	float SongRenderer::readFloat()
	{
		float ret = *(float *)songBlobPtr;
		songBlobPtr += sizeof(float);
		return ret;
	}

	double SongRenderer::readDouble()
	{
		double ret = *(double *)songBlobPtr;
		songBlobPtr += sizeof(double);
		return ret;
	}
}
