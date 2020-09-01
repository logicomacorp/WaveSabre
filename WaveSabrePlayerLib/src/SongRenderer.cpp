#include <WaveSabrePlayerLib/SongRenderer.h>

#include <stddef.h>
#include <stdint.h>

#include <stdio.h>

#if HAVE_PTHREAD
#include <signal.h>
#endif

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
#elif HAVE_PTHREAD
		renderThreadStartEvents = new sem_t[numRenderThreads];
		for (int i = 0; i < numRenderThreads; i++)
			sem_init(&renderThreadStartEvents[i], 0, 0);
		sem_init(&renderDoneEvent, 0, 0);
#endif

		if (numRenderThreads > 1)
		{
#if defined(WIN32) || defined(_WIN32)
			additionalRenderThreads = new HANDLE[numRenderThreads - 1];
#elif HAVE_PTHREAD
			additionalRenderThreads = new pthread_t[numRenderThreads - 1];
#endif

			for (int i = 0; i < numRenderThreads - 1; i++)
			{
				auto renderThreadData = new RenderThreadData();
				renderThreadData->songRenderer = this;
				renderThreadData->renderThreadIndex = i + 1;

#if defined(WIN32) || defined(_WIN32)
				additionalRenderThreads[i] = CreateThread(0, 0, renderThreadProc, (LPVOID)renderThreadData, 0, 0);
				SetThreadPriority(additionalRenderThreads[i], THREAD_PRIORITY_HIGHEST);
#elif HAVE_PTHREAD
				pthread_create(&additionalRenderThreads[i], NULL, renderThreadProc,
						renderThreadData);
				pthread_setschedprio(additionalRenderThreads[i], -15);
#endif
			}
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
#elif HAVE_PTHREAD
			for (int i = 0; i < numRenderThreads; i++)
				sem_post(&renderThreadStartEvents[i]);

			// properly stop threads
			{
				struct timespec to;
				to.tv_sec = 0;
				//          .1sec  milli  micro
				to.tv_nsec = 100 * 1000 * 1000;
				for (int i = 0; i < numRenderThreads - 1; i++) {
					if (pthread_timedjoin_np(additionalRenderThreads[i], NULL, &to)) {
						pthread_cancel(additionalRenderThreads[i]);
						pthread_join(additionalRenderThreads[i], NULL);
					}
				}
			}

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
		delete [] renderThreadStartEvents;

		CloseHandle(renderDoneEvent);
#elif HAVE_PTHREAD
		for (int i = 0; i < numRenderThreads; i++)
			sem_destroy(&renderThreadStartEvents[i]);
		delete [] renderThreadStartEvents;

		sem_destroy(&renderDoneEvent);
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
#if defined(WIN32) || defined(_WIN32)
		renderThreadsRunning = numRenderThreads;
		for (int i = 0; i < numRenderThreads; i++)
			SetEvent(renderThreadStartEvents[i]);
#elif HAVE_PTHREAD
		renderThreadsRunning = numRenderThreads;
		for (int i = 0; i < numRenderThreads; i++)
			sem_post(&renderThreadStartEvents[i]);
#endif

		renderThreadWork(0);

#if defined(WIN32) || defined(_WIN32)
		// Wait for render threads to complete their work
		WaitForSingleObject(renderDoneEvent, INFINITE);
#elif HAVE_PTHREAD
		sem_wait(&renderDoneEvent);
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

#if defined(WIN32) || defined(_WIN32) || HAVE_PTHREAD
	#if defined(WIN32) || defined(_WIN32)
	DWORD WINAPI SongRenderer::renderThreadProc(LPVOID lpParameter)
	#elif HAVE_PTHREAD
	void* SongRenderer::renderThreadProc(void* lpParameter)
	#endif
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
#elif HAVE_PTHREAD
		sem_wait(&renderThreadStartEvents[renderThreadIndex]);
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
				if ((TrackRenderState)InterlockedCompareExchange(
							(unsigned int *)&trackRenderStates[i],
							(unsigned int)TrackRenderState::Rendering,
							(unsigned int)TrackRenderState::Idle)
						== TrackRenderState::Idle)
#elif HAVE_PTHREAD
				int xv = (int)TrackRenderState::Idle;
				if (std::atomic_compare_exchange_strong(
							(std::atomic_int *)&trackRenderStates[i],
							&xv, (int)TrackRenderState::Rendering)
						)
#else
				if (trackRenderStates[i] == TrackRenderState::Idle)
#endif
				{
#if !(defined(WIN32) || defined(_WIN32)) && !(defined(HAVE_PTHREAD) && HAVE_PTHREAD)
					trackRenderStates[i] = TrackRenderState::Rendering;
#endif

					// We marked it successfully, so now we'll do the work
					tracks[i]->Run(renderThreadNumFloatSamples);
					// And mark it as finished :)
					trackRenderStates[i] = TrackRenderState::Finished;
					break;
				}
			}

			// might take a wihle before more work becomes available
#if HAVE_PTHREAD
			pthread_yield();
#endif
		}

		//asm volatile("int3\n");
#if defined(WIN32) || defined(_WIN32)
		if (!InterlockedDecrement(&renderThreadsRunning))
			SetEvent(renderDoneEvent);
#elif HAVE_PTHREAD
		// returns the value *before* the call
		if (std::atomic_fetch_sub(&renderThreadsRunning, 1) == 1)
			sem_post(&renderDoneEvent);
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
