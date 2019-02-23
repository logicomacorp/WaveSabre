#include <WaveSabrePlayerLib/SongRenderer.h>

using namespace WaveSabreCore;

namespace WaveSabrePlayerLib
{
	SongRenderer::SongRenderer(const SongRenderer::Song *song, int numRenderThreads)
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
		trackRenderStates = new TrackRenderState[numTracks];
		for (int i = 0; i < numTracks; i++)
		{
			tracks[i] = new Track(this, song->factory);
			trackRenderStates[i] = TrackRenderState::Finished;
		}

		InitializeCriticalSection(&criticalSection);
		this->numRenderThreads = numRenderThreads;
		renderThreads = new HANDLE[numRenderThreads];
		renderThreadShutdown = false;

		for (int i = 0; i < numRenderThreads; i++)
		{
			renderThreads[i] = CreateThread(0, 0, renderThreadProc, (LPVOID)this, 0, 0);
			SetThreadPriority(renderThreads[i], THREAD_PRIORITY_HIGHEST);
		}
	}

	SongRenderer::~SongRenderer()
	{
		// We don't need to enter/leave a critical section here since we're the only writer at this point.
		renderThreadShutdown = true;

		WaitForMultipleObjects(numRenderThreads, renderThreads, TRUE, INFINITE);
		DeleteCriticalSection(&criticalSection);

		delete [] renderThreads;

		for (int i = 0; i < numDevices; i++) delete devices[i];
		delete [] devices;

		for (int i = 0; i < numMidiLanes; i++) delete midiLanes[i];
		delete [] midiLanes;

		for (int i = 0; i < numTracks; i++) delete tracks[i];
		delete [] tracks;
		delete [] trackRenderStates;
	}

	void SongRenderer::RenderSamples(Sample *buffer, int numSamples)
	{
		MxcsrFlagGuard mxcsrFlagGuard;

		// Dispatch work for render threads
		EnterCriticalSection(&criticalSection);

		for (int i = 0; i < numTracks; i++)
			trackRenderStates[i] = TrackRenderState::Idle;

		renderThreadNumFloatSamples = numSamples / 2;

		LeaveCriticalSection(&criticalSection);

		// Wait for render threads to complete their work
		//  Note that we don't need to enter/leave a critical section here since we're the only reader at this point.
		//  However, if we don't want to sleep, entering/leaving a critical section here also works to not starve the
		//  worker threads it seems. Sleeping feels like the better approach to reduce contention though.
		while (trackRenderStates[numTracks - 1] != TrackRenderState::Finished)
			Sleep(0);

		// Copy final output
		float **masterTrackBuffers = tracks[numTracks - 1]->Buffers;
		for (int i = 0; i < numSamples; i++)
		{
			int sample = (int)(masterTrackBuffers[i & 1][i / 2] * 32767.0f);
			if (sample < -32768) sample = -32768;
			if (sample > 32767) sample = 32767;
			buffer[i] = (Sample)sample;
		}
	}

	DWORD WINAPI SongRenderer::renderThreadProc(LPVOID lpParameter)
	{
		auto songRenderer = (SongRenderer *)lpParameter;

		MxcsrFlagGuard mxcsrFlagGuard;

		int nextTrackIndex = songRenderer->numTracks;

		// We don't need to enter/leave a critical section here since there's only one writer for this value.
		while (!songRenderer->renderThreadShutdown)
		{
			EnterCriticalSection(&songRenderer->criticalSection);

			// If we just did some work, let's mark that as finished
			if (nextTrackIndex < songRenderer->numTracks)
				songRenderer->trackRenderStates[nextTrackIndex] = TrackRenderState::Finished;

			// Check if any new work is available
			nextTrackIndex = 0;
			for (; nextTrackIndex < songRenderer->numTracks; nextTrackIndex++)
			{
				// If track isn't idle, skip it
				if (songRenderer->trackRenderStates[nextTrackIndex] != TrackRenderState::Idle)
					continue;

				// If any of the track's dependencies aren't finished, skip it
				bool allDependenciesFinished = true;
				for (int i = 0; i < songRenderer->tracks[nextTrackIndex]->NumReceives; i++)
				{
					if (songRenderer->trackRenderStates[songRenderer->tracks[nextTrackIndex]->Receives[i].SendingTrackIndex] != TrackRenderState::Finished)
					{
						allDependenciesFinished = false;
						break;
					}
				}
				if (!allDependenciesFinished)
					continue;

				// We have a free track that we can work on, yay! Let's mark it so that no other thread takes it.
				songRenderer->trackRenderStates[nextTrackIndex] = TrackRenderState::Rendering;
				break;
			}

			LeaveCriticalSection(&songRenderer->criticalSection);

			// If we were able to find work, let's do that; otherwise, we'll yield to other threads
			if (nextTrackIndex < songRenderer->numTracks)
			{
				songRenderer->tracks[nextTrackIndex]->Run(songRenderer->renderThreadNumFloatSamples);
			}
			else
			{
				Sleep(0);
			}
		}

		return 0;
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
