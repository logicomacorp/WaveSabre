using System;
using System.Collections.Generic;
using System.Linq;

namespace WaveSabreConvert
{
    public class SongDiff
    {
        class Printer
        {
            int indent;

            public Printer()
            {
                Reset();
            }

            public void Reset()
            {
                indent = 0;
            }

            public void PushIndent()
            {
                indent++;
            }

            public void PopIndent()
            {
                indent--;
            }

            public void PrintLine(string line)
            {
                for (int i = 0; i < indent * 4; i++) line = " " + line;
                Console.WriteLine(line);
            }

            public void PrintValues(string valueName, params object[] values)
            {
                if (values.Length < 1) throw new Exception("Must specify at least one value");
                var line = valueName + ": ";
                for (int i = 0; i < values.Length; i++)
                {
                    var value = values[i];
                    line += value;
                    if (i != values.Length - 1) line += ", ";
                }
                PrintLine(line);
            }
        }

        class TrackDiff
        {
            class ReceiveDiff
            {
                readonly Song.Receive a, b;
                readonly int index;

                readonly bool sendingTrackIndex, receivingChannelIndex;
                readonly bool volume;

                public bool IsEmpty { get { return !sendingTrackIndex && !receivingChannelIndex && !volume; } }

                public ReceiveDiff(Song.Receive a, Song.Receive b, int index)
                {
                    this.a = a;
                    this.b = b;
                    this.index = index;

                    sendingTrackIndex = a.SendingTrackIndex != b.SendingTrackIndex;
                    receivingChannelIndex = a.ReceivingChannelIndex != b.ReceivingChannelIndex;
                    volume = a.Volume != b.Volume;
                }

                public void Print(Printer p)
                {
                    p.PrintLine("Receive " + index + ":");
                    p.PushIndent();

                    if (sendingTrackIndex) p.PrintValues("Sending track index", a.SendingTrackIndex, b.SendingTrackIndex);
                    if (receivingChannelIndex) p.PrintValues("Receiving channel index", a.ReceivingChannelIndex, b.ReceivingChannelIndex);
                    if (volume) p.PrintValues("Volume", a.Volume, b.Volume);

                    p.PopIndent();
                }
            }

            class DeviceDiff
            {
                readonly Song.Device a, b;
                readonly int index;

                readonly bool id;
                readonly bool chunk;

                public bool IsEmpty { get { return !id && !chunk; } }

                public DeviceDiff(Song.Device a, Song.Device b, int index)
                {
                    this.a = a;
                    this.b = b;
                    this.index = index;

                    id = a.Id != b.Id;
                    chunk = !a.Chunk.SequenceEqual(b.Chunk);
                }

                public void Print(Printer p)
                {
                    p.PrintLine("Device " + index + ": [ " + a.Id + " ]");
                    p.PushIndent();

                    if (id) p.PrintValues("ID", a.Id, b.Id);
                    if (chunk) p.PrintLine("Chunks don't match");

                    p.PopIndent();
                }
            }

            class EventDiff
            {
                readonly Song.DeltaCodedEvent a, b;
                readonly int index;

                readonly bool timeFromLastEvent;
                readonly bool type;
                readonly bool note, velocity;

                public bool IsEmpty { get { return !timeFromLastEvent && !type && !note && !velocity; } }

                public EventDiff(Song.DeltaCodedEvent a, Song.DeltaCodedEvent b, int index)
                {
                    this.a = a;
                    this.b = b;
                    this.index = index;

                    timeFromLastEvent = a.TimeFromLastEvent != b.TimeFromLastEvent;
                    type = a.Type != b.Type;
                    note = a.Note != b.Note;
                    velocity = a.Velocity != b.Velocity;
                }

                public void Print(Printer p)
                {
                    p.PrintLine("Event " + index + ":");
                    p.PushIndent();

                    if (timeFromLastEvent) p.PrintValues("Time from last event", a.TimeFromLastEvent, b.TimeFromLastEvent);
                    if (type) p.PrintValues("Type", a.Type, b.Type);
                    if (note) p.PrintValues("Note", a.Note, b.Note);
                    if (velocity) p.PrintValues("Velocity", a.Velocity, b.Velocity);

                    p.PopIndent();
                }
            }

            class AutomationDiff
            {
                class PointDiff
                {
                    readonly Song.DeltaCodedPoint a, b;
                    readonly int index;

                    readonly bool timeFromLastPoint;
                    readonly bool value;

                    public bool IsEmpty { get { return !timeFromLastPoint && !value; } }

                    public PointDiff(Song.DeltaCodedPoint a, Song.DeltaCodedPoint b, int index)
                    {
                        this.a = a;
                        this.b = b;
                        this.index = index;

                        timeFromLastPoint = a.TimeFromLastPoint != b.TimeFromLastPoint;
                        value = a.Value != b.Value;
                    }

                    public void Print(Printer p)
                    {
                        p.PrintLine("Point " + index + ":");
                        p.PushIndent();

                        if (timeFromLastPoint) p.PrintValues("TimeFromLastPoint", a.TimeFromLastPoint, b.TimeFromLastPoint);
                        if (value) p.PrintValues("Value", a.Value, b.Value);

                        p.PopIndent();
                    }
                }

                readonly Song.Automation a, b;
                readonly int index;

                readonly bool deviceIndex, paramId;
                readonly bool pointCounts;
                readonly List<PointDiff> pointDiffs = new List<PointDiff>();

                public bool IsEmpty { get { return !deviceIndex && !paramId && !pointCounts && pointDiffs.Count == 0; } }

                public AutomationDiff(Song.Automation a, Song.Automation b, int index)
                {
                    this.a = a;
                    this.b = b;
                    this.index = index;

                    deviceIndex = a.DeviceIndex != b.DeviceIndex;
                    paramId = a.ParamId != b.ParamId;

                    if (a.DeltaCodedPoints.Count != b.DeltaCodedPoints.Count)
                    {
                        pointCounts = true;
                    }
                    else
                    {
                        for (int i = 0; i < a.Points.Count; i++)
                        {
                            var aPoint = a.DeltaCodedPoints[i];
                            var bPoint = b.DeltaCodedPoints[i];
                            var pointDiff = new PointDiff(aPoint, bPoint, i);
                            if (!pointDiff.IsEmpty) pointDiffs.Add(pointDiff);
                        }
                    }
                }

                public void Print(Printer p)
                {
                    p.PrintLine("Automation " + index + ":");
                    p.PushIndent();

                    if (deviceIndex) p.PrintValues("Device index", a.DeviceIndex, b.DeviceIndex);
                    if (paramId) p.PrintValues("Param ID", a.ParamId, b.ParamId);

                    if (pointCounts)
                    {
                        p.PrintValues("Point counts don't match", a.DeltaCodedPoints.Count, b.DeltaCodedPoints.Count);
                    }
                    else if (pointDiffs.Count > 0)
                    {
                        for (int i = 0; i < pointDiffs.Count; i++)
                        {
                            if (i >= 10)
                            {
                                p.PrintLine("Diff count exceeds 10, aborting");
                                break;
                            }
                            pointDiffs[i].Print(p);
                        }
                    }

                    p.PopIndent();
                }
            }

            readonly Song.Track a, b;
            readonly int index;

            readonly bool volume;
            readonly bool trackName;
            readonly bool receiveCounts;
            readonly List<ReceiveDiff> receiveDiffs = new List<ReceiveDiff>();
            readonly bool deviceCounts;
            readonly List<DeviceDiff> deviceDiffs = new List<DeviceDiff>();
            readonly bool eventCounts;
            readonly List<EventDiff> eventDiffs = new List<EventDiff>();
            readonly bool automationCounts;
            readonly List<AutomationDiff> automationDiffs = new List<AutomationDiff>();

            public TrackDiff(Song.Track a, Song.Track b, int index)
            {
                this.a = a;
                this.b = b;
                this.index = index;

                volume = a.Volume != b.Volume;

                trackName = a.Name != b.Name;

                if (a.Receives.Count != b.Receives.Count)
                {
                    receiveCounts = true;
                }
                else
                {
                    for (int i = 0; i < a.Receives.Count; i++)
                    {
                        var aReceive = a.Receives[i];
                        var bReceive = b.Receives[i];
                        var receiveDiff = new ReceiveDiff(aReceive, bReceive, i);
                        if (!receiveDiff.IsEmpty) receiveDiffs.Add(receiveDiff);
                    }
                }

                if (a.Devices.Count != b.Devices.Count)
                {
                    deviceCounts = true;
                }
                else
                {
                    for (int i = 0; i < a.Devices.Count; i++)
                    {
                        var aDevice = a.Devices[i];
                        var bDevice = b.Devices[i];
                        var deviceDiff = new DeviceDiff(aDevice, bDevice, i);
                        if (!deviceDiff.IsEmpty) deviceDiffs.Add(deviceDiff);
                    }
                }

                if (a.DeltaCodedEvents.Count != b.DeltaCodedEvents.Count)
                {
                    eventCounts = true;
                }
                else
                {
                    for (int i = 0; i < a.DeltaCodedEvents.Count; i++)
                    {
                        var aEvent = a.DeltaCodedEvents[i];
                        var bEvent = b.DeltaCodedEvents[i];
                        var eventDiff = new EventDiff(aEvent, bEvent, i);
                        if (!eventDiff.IsEmpty) eventDiffs.Add(eventDiff);
                    }
                }

                if (a.Automations.Count != b.Automations.Count)
                {
                    automationCounts = true;
                }
                else
                {
                    for (int i = 0; i < a.Automations.Count; i++)
                    {
                        var aAutomation = a.Automations[i];
                        var bAutomation = b.Automations[i];
                        var automationDiff = new AutomationDiff(aAutomation, bAutomation, i);
                        if (!automationDiff.IsEmpty) automationDiffs.Add(automationDiff);
                    }
                }
            }

            public bool IsEmpty
            {
                get
                {
                    if (a == null || b == null) return false;
                    if (volume) return false;
                    if (receiveCounts || receiveDiffs.Count != 0) return false;
                    if (deviceCounts || deviceDiffs.Count != 0) return false;
                    if (eventCounts || eventDiffs.Count != 0) return false;
                    if (automationCounts || automationDiffs.Count != 0) return false;
                    return true;
                }
            }

            public void Print(Printer p)
            {
                p.PrintLine("Track " + index + ": [ " + a.Name + " ]");
                p.PushIndent();

                if (volume) p.PrintValues("Volume", a.Volume, b.Volume);

                if (trackName) p.PrintValues("Track Name", a.Name, b.Name);

                if (receiveCounts)
                {
                    p.PrintValues("Receive counts don't match", a.Receives.Count, b.Receives.Count);
                }
                else if (receiveDiffs.Count > 0)
                {
                    for (int i = 0; i < receiveDiffs.Count; i++)
                    {
                        if (i >= 10)
                        {
                            p.PrintLine("Diff count exceeds 10, aborting");
                            break;
                        }
                        receiveDiffs[i].Print(p);
                    }
                }

                if (deviceCounts)
                {
                    p.PrintValues("Device counts don't match", a.Devices.Count, b.Devices.Count);
                }
                else if (deviceDiffs.Count > 0)
                {
                    for (int i = 0; i < deviceDiffs.Count; i++)
                    {
                        if (i >= 10)
                        {
                            p.PrintLine("Diff count exceeds 10, aborting");
                            break;
                        }
                        deviceDiffs[i].Print(p);
                    }
                }

                if (eventCounts)
                {
                    p.PrintValues("Event counts don't match", a.DeltaCodedEvents.Count, b.DeltaCodedEvents.Count);
                }
                else if (eventDiffs.Count > 0)
                {
                    for (int i = 0; i < eventDiffs.Count; i++)
                    {
                        if (i >= 10)
                        {
                            p.PrintLine("Diff count exceeds 10, aborting");
                            break;
                        }
                        eventDiffs[i].Print(p);
                    }
                }

                if (automationCounts)
                {
                    p.PrintValues("Automation counts don't match", a.Automations.Count, b.Automations.Count);
                }
                else if (automationDiffs.Count > 0)
                {
                    for (int i = 0; i < automationDiffs.Count; i++)
                    {
                        if (i >= 10)
                        {
                            p.PrintLine("Diff count exceeds 10, aborting");
                            break;
                        }
                        automationDiffs[i].Print(p);
                    }
                }

                p.PopIndent();
            }
        }

        readonly Song a, b;

        readonly bool tempo, sampleRate, length;
        readonly bool trackCounts;
        readonly List<TrackDiff> trackDiffs = new List<TrackDiff>();

        public bool IsEmpty { get { return !tempo && !sampleRate && !trackCounts && trackDiffs.Count == 0; } }

        public SongDiff(Song a, Song b)
        {
            this.a = a;
            this.b = b;

            tempo = a.Tempo != b.Tempo;
            sampleRate = a.SampleRate != b.SampleRate;
            length = a.Length != b.Length;
            if (a.Tracks.Count != b.Tracks.Count)
            {
                trackCounts = true;
            }
            else
            {
                for (int i = 0; i < a.Tracks.Count; i++)
                {
                    var aTrack = a.Tracks[i];
                    var bTrack = b.Tracks[i];
                    var trackDiff = new TrackDiff(aTrack, bTrack, i);
                    if (!trackDiff.IsEmpty) trackDiffs.Add(trackDiff);
                }
            }
        }

        public void Print()
        {
            if (IsEmpty) return;

            var p = new Printer();
            p.PushIndent();

            if (tempo) p.PrintValues("Tempo", a.Tempo, b.Tempo);
            if (sampleRate) p.PrintValues("Sample Rate", a.SampleRate, b.SampleRate);
            if (length) p.PrintValues("Length", a.Length, b.Length);

            if (trackCounts)
            {
                p.PrintValues("Track counts don't match", a.Tracks.Count, b.Tracks.Count);
            }
            else if (trackDiffs.Count > 0)
            {
                for (int i = 0; i < trackDiffs.Count; i++)
                {
                    if (i >= 10)
                    {
                        p.PrintLine("Diff count exceeds 10, aborting");
                        break;
                    }
                    trackDiffs[i].Print(p);
                }
            }

            p.PopIndent();
        }
    }
}
