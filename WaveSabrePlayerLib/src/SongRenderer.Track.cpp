#include <WaveSabrePlayerLib/SongRenderer.h>

using namespace WaveSabreCore;

namespace WaveSabrePlayerLib
{
	SongRenderer::Track::Track(SongRenderer *songRenderer, SongRenderer::DeviceFactory factory)
	{
		for (int i = 0; i < numBuffers; i++) Buffers[i] = new float[songRenderer->sampleRate];

		this->songRenderer = songRenderer;

		volume = songRenderer->readFloat();

		numReceives = songRenderer->readInt();
		if (numReceives)
		{
			receives = new Receive[numReceives];
			for (int i = 0; i < numReceives; i++)
			{
				receives[i].SendingTrackIndex = songRenderer->readInt();
				receives[i].ReceivingChannelIndex = songRenderer->readInt();
				receives[i].Volume = songRenderer->readFloat();
			}
		}

		numDevices = songRenderer->readInt();
		if (numDevices)
		{
			devicesIndicies = new int[numDevices];
			for (int i = 0; i < numDevices; i++)
			{
				devicesIndicies[i] = songRenderer->readInt();
			}
		}

		midiLaneId = songRenderer->readInt();

		numAutomations = songRenderer->readInt();
		if (numAutomations)
		{
			automations = new Automation *[numAutomations];
			for (int i = 0; i < numAutomations; i++)
			{
				int deviceIndex = songRenderer->readInt();
				automations[i] = new Automation(songRenderer, songRenderer->devices[devicesIndicies[deviceIndex]]);
			}
		}

		lastSamplePos = 0;
		accumEventTimestamp = 0;
		eventIndex = 0;
	}

	SongRenderer::Track::~Track()
	{
		for (int i = 0; i < numBuffers; i++) delete [] Buffers[i];
		if (numReceives) delete [] receives;
		
		if (numDevices)
		{
			delete[] devicesIndicies;
		}

		if (numAutomations)
		{
			for (int i = 0; i < numAutomations; i++) delete automations[i];
			delete [] automations;
		}
	}

	void SongRenderer::Track::Run(int numSamples)
	{
		MidiLane* lane = songRenderer->midiLanes[midiLaneId];
		for ( ; eventIndex < lane->numEvents; eventIndex++)
		{
			Event *e = &lane->events[eventIndex];
			int samplesToEvent = accumEventTimestamp + e->TimeStamp - lastSamplePos;
			if (samplesToEvent >= numSamples) break;
			switch (e->Type)
			{
			case EventType::NoteOn:
				for (int i = 0; i < numDevices; i++) songRenderer->devices[devicesIndicies[i]]->NoteOn(e->Note, e->Velocity, samplesToEvent);
				break;

			case EventType::NoteOff:
				for (int i = 0; i < numDevices; i++) songRenderer->devices[devicesIndicies[i]]->NoteOff(e->Note, samplesToEvent);
				break;
			}
			accumEventTimestamp += e->TimeStamp;
		}

		for (int i = 0; i < numAutomations; i++) automations[i]->Run(numSamples);

		for (int i = 0; i < numBuffers; i++) memset(Buffers[i], 0, numSamples * sizeof(float));
		for (int i = 0; i < numReceives; i++)
		{
			Receive *r = &receives[i];
			float **receiveBuffers = songRenderer->tracks[r->SendingTrackIndex]->Buffers;
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < numSamples; k++) Buffers[j + r->ReceivingChannelIndex][k] += receiveBuffers[j][k] * r->Volume;
			}
		}

		for (int i = 0; i < numDevices; i++) songRenderer->devices[devicesIndicies[i]]->Run((double)lastSamplePos / Helpers::CurrentSampleRate, Buffers, Buffers, numSamples);

		if (volume != 1.0f)
		{
			for (int i = 0; i < numBuffers; i++)
			{
				for (int j = 0; j < numSamples; j++) Buffers[i][j] *= volume;
			}
		}

		lastSamplePos += numSamples;
	}

	SongRenderer::Track::Automation::Automation(SongRenderer *songRenderer, WaveSabreCore::Device *device)
	{
		this->device = device;
		paramId = songRenderer->readInt();
		numPoints = songRenderer->readInt();
		points = new Point[numPoints];
		int lastPointTime = 0;
		for (int i = 0; i < numPoints; i++)
		{
			int absTime = lastPointTime + songRenderer->readInt();
			points[i].TimeStamp = absTime;
			lastPointTime = absTime;
			points[i].Value = (float)((double)songRenderer->readByte() / 255.0);
		}
		samplePos = 0;
		pointIndex = 0;
	}

	SongRenderer::Track::Automation::~Automation()
	{
		delete [] points;
	}

	void SongRenderer::Track::Automation::Run(int numSamples)
	{
		for ( ; pointIndex < numPoints; pointIndex++)
		{
			if (points[pointIndex].TimeStamp > samplePos) break;
		}
		if (pointIndex >= numPoints)
		{
			device->SetParam(paramId, points[numPoints - 1].Value);
		}
		else if (pointIndex <= 0)
		{
			device->SetParam(paramId, points[0].Value);
		}
		else
		{
			int timestampDelta = points[pointIndex].TimeStamp - points[pointIndex - 1].TimeStamp;
			float mixAmount = timestampDelta > 0 ?
				(float)(samplePos - points[pointIndex - 1].TimeStamp) / (float)timestampDelta :
				0.0f;
			device->SetParam(paramId, Helpers::Mix(points[pointIndex - 1].Value, points[pointIndex].Value, mixAmount));
		}
		samplePos += numSamples;
	}
}
