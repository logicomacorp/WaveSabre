#include <WaveSabrePlayerLib/SongRenderer.h>

#include <stddef.h>
#include <stdint.h>

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
				uint8_t note = readByte();
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

		this->numRenderThreads = numRenderThreads;

		renderThreadShutdown = false;
#if defined(WIN32) || defined(_WIN32)
		renderThreadStartEvents = new HANDLE[numRenderThreads];
		for (int i = 0; i < numRenderThreads; i++)
			renderThreadStartEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		renderDoneEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
#endif

		if (numRenderThreads > 1)
		{
#if defined(WIN32) || defined(_WIN32)
			additionalRenderThreads = new HANDLE[numRenderThreads - 1];
			for (int i = 0; i < numRenderThreads - 1; i++)
			{
				auto renderThreadData = new RenderThreadData();
				renderThreadData->songRenderer = this;
				renderThreadData->renderThreadIndex = i + 1;
				additionalRenderThreads[i] = CreateThread(0, 0, renderThreadProc, (LPVOID)renderThreadData, 0, 0);
				SetThreadPriority(additionalRenderThreads[i], THREAD_PRIORITY_HIGHEST);
			}
#endif
		}
	}

	SongRenderer::~SongRenderer()
	{
		// Dispatch shutdown
		renderThreadShutdown = true;

		if (numRenderThreads > 1)
		{
#if defined(WIN32) || defined(_WIN32)
			for (int i = 0; i < numRenderThreads; i++)
				SetEvent(renderThreadStartEvents[i]);
			WaitForMultipleObjects(numRenderThreads - 1, additionalRenderThreads, TRUE, INFINITE);
			for (int i = 0; i < numRenderThreads - 1; i++)
				CloseHandle(additionalRenderThreads[i]);
			delete [] additionalRenderThreads;
#endif
		}

		for (int i = 0; i < numDevices; i++) delete devices[i];
		delete [] devices;

		for (int i = 0; i < numMidiLanes; i++) delete midiLanes[i];
		delete [] midiLanes;

		for (int i = 0; i < numTracks; i++) delete tracks[i];
		delete [] tracks;
		delete [] trackRenderStates;

#if defined(WIN32) || defined(_WIN32)
		for (int i = 0; i < numRenderThreads; i++)
			CloseHandle(renderThreadStartEvents[i]);
		CloseHandle(renderDoneEvent);

		delete [] renderThreadStartEvents;
#endif
	}

	void SongRenderer::RenderSamples(Sample *buffer, int numSamples)
	{
		MxcsrFlagGuard mxcsrFlagGuard;

		// Set up work
		for (int i = 0; i < numTracks; i++)
			trackRenderStates[i] = TrackRenderState::Idle;
		renderThreadNumFloatSamples = numSamples / 2;

		// Dispatch work
		renderThreadsRunning = numRenderThreads;
#if defined(WIN32) || defined(_WIN32)
		for (int i = 0; i < numRenderThreads; i++)
			SetEvent(renderThreadStartEvents[i]);

		renderThreadWork(0);

		// Wait for render threads to complete their work
		WaitForSingleObject(renderDoneEvent, INFINITE);
#endif

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

#if defined(WIN32) || defined(_WIN32)
	DWORD WINAPI SongRenderer::renderThreadProc(LPVOID lpParameter)
	{
		auto renderThreadData = (RenderThreadData *)lpParameter;

		auto songRenderer = renderThreadData->songRenderer;
		int renderThreadIndex = renderThreadData->renderThreadIndex;

		delete renderThreadData;

		while (songRenderer->renderThreadWork(renderThreadIndex))
			;

		return 0;
	}
#endif

	bool SongRenderer::renderThreadWork(int renderThreadIndex)
	{
#if defined(WIN32) || defined(_WIN32)
		WaitForSingleObject(renderThreadStartEvents[renderThreadIndex], INFINITE);
#endif

		if (renderThreadShutdown)
			return false;

		MxcsrFlagGuard mxcsrFlagGuard;

		// Check that there's _any_ potential work to be done
		while (trackRenderStates[numTracks - 1] != TrackRenderState::Finished)
		{
			// Try to find some work to do
			for (int i = 0; i < numTracks; i++)
			{
				// If track isn't idle, skip it
				if (trackRenderStates[i] != TrackRenderState::Idle)
					continue;

				// If any of the track's dependencies aren't finished, skip it
				bool allDependenciesFinished = true;
				for (int j = 0; j < tracks[i]->NumReceives; j++)
				{
					if (trackRenderStates[tracks[i]->Receives[j].SendingTrackIndex] != TrackRenderState::Finished)
					{
						allDependenciesFinished = false;
						break;
					}
				}
				if (!allDependenciesFinished)
					continue;

				// We have a free track that we can work on, yay!
				//  Let's try to mark it so that no other thread takes it
#if defined(WIN32) || defined(_WIN32)
				if ((TrackRenderState)InterlockedCompareExchange((unsigned int *)&trackRenderStates[i], (unsigned int)TrackRenderState::Rendering, (unsigned int)TrackRenderState::Idle) == TrackRenderState::Idle)
				{
					// We marked it successfully, so now we'll do the work
					tracks[i]->Run(renderThreadNumFloatSamples);
					// And mark it as finished :)
					trackRenderStates[i] = TrackRenderState::Finished;
					break;
				}
#endif
			}
		}

#if defined(WIN32) || defined(_WIN32)
		if (!InterlockedDecrement(&renderThreadsRunning))
			SetEvent(renderDoneEvent);
#endif

		return true;
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
