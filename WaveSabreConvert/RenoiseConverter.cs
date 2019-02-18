using System;
using System.Collections.Generic;
using System.Linq;
using Renoise;

namespace WaveSabreConvert
{
    public class RenoiseConverter
    {
        ILog logger;

        private RenoiseSong project;
        private List<RnsIns> instruments;
        private double secondsPerIndex;
        private int sampleRate;
        private Dictionary<string, byte> hexLookUp = new Dictionary<string, byte>();
        private Dictionary<object, List<RnsReceive>> trackReceives;
        private List<object> visitedTracks, orderedTracks;

        class RsnPatternLineNode : PatternLineNode 
        {
            public int OrigianlIndex { get; set; }
        }

        class RnsIns
        {
            public int InstrumentId { get; set; }
            public string Name { get; set; }
            public Song.Device Device { get; set; }
        }

        class RnsAuto
        {
            public int TimePoint { get; set; }
            public float Value { get; set; }
            public int Offset { get; set; }
            public int AutoLength { get; set; }
        }

        class RnsDevice
        {
            public int DeviceIndex { get; set; }
            public object Device { get; set; }
        }

        class RnsReceive
        {
            public object SendingTrack;
            public int ReceivingChannelIndex;
            public double Volume;
            public RnsReceive(object sendingTrack, int receivingChannelIndex, double volume)
            {
                SendingTrack = sendingTrack;
                ReceivingChannelIndex = receivingChannelIndex;
                Volume = volume;
            }
        }

        public Song Process(RenoiseSong project, ILog logger)
        {
            this.logger = logger;
            // hex lookup..  because reasons...
            for (int i = 0; i <= 255; i++) hexLookUp.Add(i.ToString("X2"), (byte)i);

            this.project = project;

            var song = new Song();

            song.Tempo = (int)this.project.GlobalSongData.BeatsPerMin;
            song.SampleRate = 44100;
            secondsPerIndex = (double)song.Tempo * (double)this.project.GlobalSongData.LinesPerBeat; 
            secondsPerIndex = 60 / secondsPerIndex;     

            sampleRate = song.SampleRate;

            song.Length = GetSongLength();

            var master = this.project.Tracks.Items.Where(track => track is SequencerMasterTrack).First();

            trackReceives = new Dictionary<object, List<RnsReceive>>();
            foreach (var projectTrack in this.project.Tracks.Items) trackReceives.Add(projectTrack, new List<RnsReceive>());

            // send mapping
            foreach (var projectTrack in this.project.Tracks.Items)
            {
                string trackName = GetProp("Name", projectTrack).ToString();
                object[] trackDevices = ((TrackFilterDeviceChain)GetProp("FilterDevices", projectTrack)).Devices.Items;
                var sends = new List<SendDevice>();

                // get all send devices
                bool found = false;
                foreach (var device in trackDevices)
                {
                    if (device is SendDevice)
                    {
                        found = true;
                        sends.Add((SendDevice) device);
                    }

                    if (found && !(device is SendDevice))
                    {
                        logger.WriteLine("WARNING: Track {0} has device {1} after send", trackName, device);
                    }
                }

                foreach (var send in sends)
                {
                    if (send.MuteSource && sends.IndexOf(send) != sends.Count-1)
                    {
                        logger.WriteLine("WARNING: Track {0} has muted send which is not the last send in the chain", trackName);
                    }

                    if (send.IsActive.Value == 1 && send.SendAmount.Value > 0)
                    {
                        var sendTrack = GetSendTrack((int) send.DestSendTrack.Value);
                        trackReceives[sendTrack].Add(new RnsReceive(projectTrack, 0, send.SendAmount.Value));
                    }
                    else
                    {
                        logger.WriteLine("WARNING: Track {0} has disabled send", trackName);
                    }
                }

                // track is not master, so find where it goes...
                if (projectTrack != master)
                {
                    int routing = (int)GetProp("TrackRouting", projectTrack);
                    if (routing == 0)
                    {
                        // routed to master
                        if (!MutedSend(projectTrack))  // muted send so no actual output
                        {
                            var trackVolume = GetTrackVolume(trackDevices[0]);
                            trackReceives[master].Add(new RnsReceive(projectTrack, 0, trackVolume));
                        }
                    }
                    else if (routing == -1)
                    {
                        // routed to group track
                        if (!MutedSend(projectTrack))  // muted send so no actual output
                        {
                            var group = GetGroupTrack(projectTrack);
                            var trackVolume = GetTrackVolume(trackDevices[0]);
                            trackReceives[group].Add(new RnsReceive(projectTrack, 0, trackVolume));
                        }
                    }
                    else
                    {
                        // routed to specific soundcard output
                        logger.WriteLine(string.Format("WARNING: Track {0} routing to soundcard?? WTF!", trackName));
                    }
                }
            }

            visitedTracks = new List<object>();
            orderedTracks = new List<object>();

            VisitTrack(master);

            GetInstruments();

            var projectTracksToSongTracks = new Dictionary<object, Song.Track>();

            foreach (var projectTrack in orderedTracks)
            {
                var track = ConvertTrack(this.project.Tracks.Items.ToList().IndexOf(projectTrack));
                projectTracksToSongTracks.Add(projectTrack, track);
                song.Tracks.Add(track);
            }

            foreach (var kvp in projectTracksToSongTracks)
            {
                foreach (var projectReceive in trackReceives[kvp.Key])
                {
                    if (projectTracksToSongTracks.ContainsKey(projectReceive.SendingTrack))
                    {
                        var receive = new Song.Receive();
                        receive.SendingTrackIndex = song.Tracks.IndexOf(projectTracksToSongTracks[projectReceive.SendingTrack]);
                        receive.ReceivingChannelIndex = projectReceive.ReceivingChannelIndex;
                        receive.Volume = (float)projectReceive.Volume;
                        kvp.Value.Receives.Add(receive);
                    }
                }
            }

            return song;
        }

        private float GetTrackVolume(object trackDevice)
        {
            float volume = (float)GetProp("Value", GetProp("Volume", trackDevice));
            return volume;
        }

        private bool MutedSend(object projectTrack)
        {
            object[] trackDevices = ((TrackFilterDeviceChain)GetProp("FilterDevices", projectTrack)).Devices.Items;
            bool muted = false;

            foreach (var device in trackDevices)
            {
                if (device is SendDevice)
                {
                    var send = (SendDevice) device;
                    if (send.MuteSource)
                    {
                        muted = true;
                        break;
                    }
                }
            }

            return muted;
        }

        private object GetGroupTrack(object track)
        {
            int index = project.Tracks.Items.ToList().IndexOf(track)+1;
            int max = project.Tracks.Items.Count();

            for (; index <= max; index++)
            {
                if (project.Tracks.Items[index] is SequencerGroupTrack)
                {
                    return project.Tracks.Items[index];
                }
            }

            return null;
        }

        private object GetSendTrack(int id)
        {
            var sendTracks = project.Tracks.Items.Where(track => track is SequencerSendTrack).ToList();
            return sendTracks[id];
        }

        private Song.Track ConvertTrack(int trackId)
        {
            Song.Track songTrack = null;

            var trackObject = project.Tracks.Items[trackId];

            string trackName = GetProp("Name", trackObject).ToString();
            object[] trackDevices = ((TrackFilterDeviceChain)GetProp("FilterDevices", trackObject)).Devices.Items;

            var songPatterns = new List<Pattern>();

            var allLines = new List<RsnPatternLineNode>();

            // copy all patterns out to full song
            foreach (var seq in project.PatternSequence.SequenceEntries.SequenceEntry)
            {
                songPatterns.Add(project.PatternPool.Patterns.Pattern[seq.Pattern]);
            }

            // loop each pattern to create complete picture of notes and automations...
            int lineIndex = 0;
            foreach (var pattern in songPatterns)
            {
                var track = pattern.Tracks.Items[trackId];

                // grab current lines on pattern
                var currentLines = (PatternLineNode[]) GetProp("Lines", track);

                if (currentLines != null)
                {
                    foreach (PatternLineNode line in currentLines)
                    {
                        var newLine = new RsnPatternLineNode();
                        newLine.NoteColumns = line.NoteColumns;
                        newLine.EffectColumns = line.EffectColumns;
                        newLine.OrigianlIndex = line.index;     // keep original index for groooooove
                        newLine.index = line.index += lineIndex;
                        newLine.type = line.type;
                        allLines.Add(newLine);
                    }
                }

                lineIndex += pattern.NumberOfLines;
            }

            // convert tracker notes to midi events
            var allNotes = allLines.SelectMany(n => n.NoteColumns.NoteColumn);
            var allIns = allNotes.Select(n => n.Instrument).Where(i => i != null && i != "..").Distinct().ToList();
            
            if (allIns.Count() > 1)
            {
                logger.WriteLine(string.Format(
                    "WARNING: Track {0} has {1} instruments used, defaulting to instrument {2}",
                    trackName, allIns.Count(), allIns.First()));
            }
            
            // automations....
            int autoIndex = 0;
            var allAuto = new List<Song.Automation>();
            var autoGroup = new List<PatternTrackAutomation>();

            foreach (var pattern in songPatterns)
            {
                var track = pattern.Tracks.Items[trackId];
                var automations = (PatternTrackAutomation)GetProp("Automations", track);
                if (automations != null) autoGroup.Add(automations);
            }

            // generate distinct list of device id's and params used on this track
            var pita = new List<PatternTrackEnvelope>();
            if (autoGroup.Count > 0)
            {
                var allEnv = autoGroup.Select(g => g.Envelopes);
                if (allEnv != null)
                {
                    foreach (var temp in allEnv)
                    {
                        pita.AddRange(temp.Envelope);
                    }
                }
            }
            var distinct = pita.Select(a => new { DeviceIndex = a.DeviceIndex, ParamId = a.ParameterIndex }).Distinct().ToList();

            //  now populate each distinct device and param
            foreach (var thisAuto in distinct)
            {
                var thisAutoList = new List<RnsAuto>();
                int deviceIndex = thisAuto.DeviceIndex;
                int paramId = thisAuto.ParamId;

                autoIndex = 0;
                foreach (var pattern in songPatterns)
                {
                    var track = pattern.Tracks.Items[trackId];
                    var automations = (PatternTrackAutomation)GetProp("Automations", track);
                    if (automations == null)
                    {
                        // pattern has no autos, add start / end from last auto value
                        if (thisAutoList.Count > 0)
                        {
                            var newAuto = new RnsAuto();
                            newAuto.TimePoint = 0; // <--- start of pattern
                            newAuto.Value = thisAutoList.Last().Value;
                            newAuto.AutoLength = pattern.NumberOfLines * 256;
                            newAuto.Offset = autoIndex;
                            thisAutoList.Add(newAuto);

                            newAuto = new RnsAuto();
                            newAuto.TimePoint = (pattern.NumberOfLines * 256) - 1; // <--- end of pattern
                            newAuto.Value = thisAutoList.Last().Value;
                            newAuto.AutoLength = pattern.NumberOfLines * 256;
                            newAuto.Offset = autoIndex;
                            thisAutoList.Add(newAuto);
                        }
                    }
                    else
                    {
                        var myAutos = automations.Envelopes.Envelope.Where(a => a.DeviceIndex == deviceIndex && a.ParameterIndex == paramId);

                        if (myAutos.ToList().Count == 0)
                        {
                            // this pattern has no autos, add start / end from last auto value
                            if (thisAutoList.Count > 0)
                            {
                                var newAuto = new RnsAuto();
                                newAuto.TimePoint = 0; // <--- start of pattern
                                newAuto.Value = thisAutoList.Last().Value;
                                newAuto.AutoLength = pattern.NumberOfLines*256;
                                newAuto.Offset = autoIndex;
                                thisAutoList.Add(newAuto);

                                newAuto = new RnsAuto();
                                newAuto.TimePoint = (pattern.NumberOfLines * 256) - 1; // <--- end of pattern
                                newAuto.Value = thisAutoList.Last().Value;
                                newAuto.AutoLength = pattern.NumberOfLines * 256;
                                newAuto.Offset = autoIndex;
                                thisAutoList.Add(newAuto);
                            }
                        }
                        else
                        {
                            // this pattern has automations..  so process
                            foreach (var a in myAutos)
                            {
                                var autoTemp = new List<RnsAuto>();
                                foreach (string point in a.Envelope.Points) // add all current points
                                {
                                    var timePoint = Convert.ToInt32(point.Split(',')[0]);
                                    var value = (float) Convert.ToDouble(point.Split(',')[1]);

                                    var newAuto = new RnsAuto();
                                    newAuto.TimePoint = timePoint;
                                    newAuto.Value = value;
                                    newAuto.AutoLength = a.Envelope.Length;
                                    newAuto.Offset = autoIndex;
                                    autoTemp.Add(newAuto);
                                }

                                // create auto point for the start of this pattern
                                if (autoTemp.First().TimePoint != 0)
                                {
                                    var newAuto = new RnsAuto();
                                    newAuto.TimePoint = 0; // <--- start of pattern
                                    newAuto.Value = autoTemp.First().Value;
                                    newAuto.AutoLength = autoTemp.First().AutoLength;
                                    newAuto.Offset = autoIndex;
                                    autoTemp.Insert(0, newAuto);
                                }

                                // create auto point for the end of this pattern
                                if (autoTemp.Last().TimePoint != autoTemp.Last().AutoLength - 1)
                                {
                                    var newAuto = new RnsAuto();
                                    newAuto.TimePoint = autoTemp.Last().AutoLength - 1; // <--- end of pattern
                                    newAuto.Value = autoTemp.Last().Value;
                                    newAuto.AutoLength = autoTemp.Last().AutoLength;
                                    newAuto.Offset = autoIndex;
                                    autoTemp.Add(newAuto);
                                }

                                thisAutoList.AddRange(autoTemp);
                            }
                        }
                    }

                    // next position
                    autoIndex += pattern.NumberOfLines * 256;
                }

                // double check we have starting point for this auto in case of a blank pattern
                if (thisAutoList.First().TimePoint != 0)
                {
                    var newAuto = new RnsAuto();
                    newAuto.TimePoint = 0;                      // <--- start of pattern
                    newAuto.Value = thisAutoList.First().Value;
                    newAuto.AutoLength = thisAutoList.First().AutoLength;
                    newAuto.Offset = autoIndex;
                    thisAutoList.ToList().Insert(0, newAuto);
                }

                // all automation points for this device / param collated, now convert to our automations
                thisAutoList.ForEach(a => a.TimePoint += a.Offset);

                var finalAuto = new Song.Automation();
                finalAuto.DeviceIndex = deviceIndex;
                finalAuto.ParamId = paramId-1;  // renoise param index is out by one due to automation on active flag
                
                foreach (var p in thisAutoList)
                {
                    var point = new Song.Point();
                    point.Value = p.Value;
                    point.TimeStamp = SecondsToSamples(p.TimePoint / 256.00 * (double)secondsPerIndex, sampleRate);
                    finalAuto.Points.Add(point);
                }

                allAuto.Add(finalAuto);
            }

            RnsIns instrument = null;
            int instrumentId = -1;

            if (allIns.Count > 0)
            {
                instrumentId = Convert.ToInt32(allIns.First());
                instrument = instruments.Where(i => i.InstrumentId == instrumentId).First();
            }

            songTrack = new Song.Track();
            songTrack.Name = trackName;
            songTrack.Volume = GetTrackVolume(trackDevices[0]); 
            var devices = new List<RnsDevice>();

            if (instrument != null) devices.Add(new RnsDevice() {DeviceIndex = 0, Device = instrument.Device});
            devices.AddRange(GetDevices(trackDevices, trackName, instrumentId));  // add track devices

            foreach (var device in devices)
            {
                if (device.Device is Song.Device)
                {
                    songTrack.Devices.Add((Song.Device)device.Device);
                }
            }
            
            // remap automations to correct devices
            foreach (var auto in allAuto)
            {
                if (devices.Find(a => a.DeviceIndex == auto.DeviceIndex).Device is InstrumentAutomationDevice)
                {
                    auto.DeviceIndex = 0;   // 0 is always instrument
                }
                else
                {
                    auto.DeviceIndex = devices.IndexOf(
                        devices.Find(a => a.DeviceIndex == auto.DeviceIndex && a.Device is Song.Device));
                }
            }

            if (allAuto.RemoveAll(auto => auto.DeviceIndex == -1) > 0)
            {
                logger.WriteLine(string.Format("WARNING: Some automations skipped on track {0}", trackName));
            }

            songTrack.Automations.AddRange(allAuto);
            songTrack.Events = NotesToEvents(allLines);
            return songTrack;
        }
        
        private object GetProp(string name, object source)
        {
            return source.GetType().GetProperty(name).GetValue(source);
        }

        private double GetSongLength()
        {
            int lineCount = 0;

            foreach (var seq in project.PatternSequence.SequenceEntries.SequenceEntry)
            {
                var pattern = project.PatternPool.Patterns.Pattern[seq.Pattern];
                lineCount += pattern.NumberOfLines;
            }
            return (lineCount + 1) * secondsPerIndex;
        }

        private List<RnsDevice> GetDevices(object[] devices, string trackName, int instrumentId)
        {
            var sabreDevices = new List<RnsDevice>();

            int deviceIndex = 0;

            foreach (var device in devices)
            {
                if (device is AudioPluginDevice)
                {
                    var plug = (AudioPluginDevice)device;
                    var sabreDevice = PlugToDevice(plug);
                    if (sabreDevice == null)
                    {
                        logger.WriteLine(string.Format("WARNING: Track {0} has unkown plugin {1}", 
                            trackName, plug.PluginIdentifier));
                    }

                    if (plug.IsActive.Value == 0)
                    {
                        logger.WriteLine(string.Format("WARNING: Track {0} has device {1} disabled", 
                            trackName, plug.PluginIdentifier));
                    }
                    else
                    {
                        sabreDevices.Add(new RnsDevice() { DeviceIndex = deviceIndex, Device = sabreDevice });
                    }
                }
                else if (device is InstrumentAutomationDevice)
                {
                    var plug = (InstrumentAutomationDevice)device;
                    if (plug.LinkedInstrument != instrumentId)
                    {
                        logger.WriteLine(string.Format("WARNING: Track {0} instrument automation mismatch, pointing to {1}, should be {2}", 
                            trackName,
                            instrumentId,
                            plug.LinkedInstrument));
                    }
                    else
                    {
                        sabreDevices.Add(new RnsDevice() { DeviceIndex = deviceIndex, Device = plug });
                    }
                }
                else
                {
                    if (deviceIndex > 0)
                    {
                        logger.WriteLine(string.Format("WARNING: Track {0} has device {1} which is not supported", 
                            trackName,
                            device.GetType()));
                    }
                }

                deviceIndex++;
            }

            return sabreDevices;
        }

        private Song.Device PlugToDevice(AudioPluginDevice plug)
        {
            Song.Device device = null;

            Song.DeviceId deviceId;
            if (Enum.TryParse<Song.DeviceId>(plug.PluginIdentifier, out deviceId))
            {
                device = new Song.Device();
                device.Id = deviceId;
                device.Chunk = Convert.FromBase64String(FixBase64(plug.ParameterChunk));
            }

            return (device);
        }

        private string FixBase64(string value)
        {
            while (value.Length % 4 != 0) value += "=";
            return value;
        }

        // could have done this better
        private int NoteValue(string note)
        {
            switch (note)
            {
                case "C-":
                    return 0;
                case "C#":
                    return 1;
                case "D-":
                    return 2;
                case "D#":
                    return 3;
                case "E-":
                    return 4;
                case "F-":
                    return 5;
                case "F#":
                    return 6;
                case "G-":
                    return 7;
                case "G#":
                    return 8;
                case "A-":
                    return 9;
                case "A#":
                    return 10;
                case "B-":
                    return 11;
                default:
                    return -1;
            }
        }

        // converts note string to midi note
        private byte NoteToByte(string note)
        {
            int noteVal = NoteValue(note.Substring(0, 2));
            if (noteVal < 0)
            {
                throw new Exception(string.Format("note spaz out: [{0}]", note));
            }
            int octave = Convert.ToInt32(note.Substring(2, 1));
            return (byte)((octave * 12) + noteVal);
        }


        // converts tracker notes to midi
        private List<Song.Event> NotesToEvents(List<RsnPatternLineNode> lines)
        {
            var events = new List<Song.Event>();

            // quit if no notes..
            if (lines.Count == 0) return events;

            var lanes = lines.Select(l => l.NoteColumns.NoteColumn).ToList().Max(x => x.Count());

            bool[] active = new bool[lanes];
            string[] lastNote = new string[lanes];
            for (int i = 0; i < lanes; i++)
            {
                lastNote[i] = "";
                active[i] = false;
            }

            foreach (var line in lines)
            {
                double eventTime = (double)line.index * secondsPerIndex;

                if (line.OrigianlIndex % 2 == 1)        // odd line, add groove
                {
                    int shuffle = (line.OrigianlIndex % 8) / 2;
                    double indexFraction = (double)secondsPerIndex / 3 * 2;
                    indexFraction = indexFraction / 100 * project.GlobalSongData.ShuffleAmounts[shuffle];
                    eventTime += indexFraction;
                }

                for (int i = 0; i < line.NoteColumns.NoteColumn.ToList().Count; i++)
                {
                    string note = line.NoteColumns.NoteColumn[i].Note;
                    byte velocity = VolumeToByte(line.NoteColumns.NoteColumn[i].Volume);
                    // active and new note or off command
                    if (active[i] && note != "")
                    {
                        active[i] = false;
                        events.Add(new Song.Event()
                        {
                            TimeStamp = SecondsToSamples(eventTime, (double)sampleRate),
                            Type = Song.EventType.NoteOff,
                            Note = NoteToByte(lastNote[i])
                        });
                    }

                    // new note
                    if (note != "OFF" && note != "")
                    {
                        active[i] = true;
                        lastNote[i] = note;
                        events.Add(new Song.Event()
                        {
                            TimeStamp = SecondsToSamples(eventTime, (double)sampleRate),
                            Type = Song.EventType.NoteOn,
                            Note = NoteToByte(note),
                            Velocity = velocity
                        });
                    }
                }
            }
            return events;
        }


        private byte VolumeToByte(string volume)
        {
            byte result = 0x7f;
            if (volume != null && hexLookUp.ContainsKey(volume)) result = hexLookUp[volume];
            if (result > 127) result = 127;
            return result;
        }

        private void GetInstruments()
        {
            instruments = new List<RnsIns>();
            int insId = 0;

            foreach (var instrument in project.Instruments.Instrument)
            {
                var rnsins = new RnsIns();
                rnsins.Name = instrument.Name;
                rnsins.InstrumentId = insId;

                AudioPluginDevice plug = null;
                if (instrument.PluginGenerator != null)
                {
                    plug = instrument.PluginGenerator.PluginDevice;
                }
                else if (instrument.PluginProperties != null)
                {
                    plug = instrument.PluginProperties.PluginDevice;
                }

                if (plug != null)
                {
                    //var plug = instrument.PluginGenerator.PluginDevice;

                    Song.Device device = null;

                    Song.DeviceId deviceId;
                    if (Enum.TryParse<Song.DeviceId>(plug.PluginIdentifier, out deviceId))
                    {
                        device = new Song.Device();
                        device.Id = deviceId;
                        device.Chunk = Convert.FromBase64String(FixBase64(plug.ParameterChunk));
                        rnsins.Device = device;
                    }
                    if (device == null)
                    {
                        logger.WriteLine(string.Format("WARNING: Instrument {0}[{2}] skipped (unsupported plugin): {1}",
                        instrument.Name, plug.PluginIdentifier, insId));
                    }
                }
                else
                {
                    logger.WriteLine(string.Format("WARNING: Instrument {0}[{1}] skipped (not a vst plugin)",
                    instrument.Name, insId));
                }

                instruments.Add(rnsins);
                insId++;
            }
        }

        void VisitTrack(object projectTrack)
        {
            string state = GetProp("State", projectTrack).ToString();  

            if (visitedTracks.Contains(projectTrack) || state != "Active") return;

            visitedTracks.Add(projectTrack);
            foreach (var projectReceive in trackReceives[projectTrack])
            {
                if (projectReceive.Volume > 0.0) VisitTrack(projectReceive.SendingTrack);
            }
            orderedTracks.Add(projectTrack);
        }

        static int SecondsToSamples(double time, double sampleRate)
        {
            return (int)(time * sampleRate);
        }
    }
}
