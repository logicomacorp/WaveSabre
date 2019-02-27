using System;
using System.Collections.Generic;
using System.Linq;
using Renoise;
using System.Text;
using System.IO;

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
        private Dictionary<Song.Track, object> instrumentTracks;
        private List<List<RnsPatternLineNode>> noteTracks;
        private List<RnsAutoMap> automationMaps;

        class RnsPatternLineNode : PatternLineNode 
        {
            public int OrigianlIndex { get; set; }
        }

        class RnsAutoMap
        {
            public object AutoSource { get; set; }
            public object AutoDevice { get; set; }
            public Song.Automation Auotmation { get; set;}
        }

        class RnsIns
        {
            public int InstrumentId { get; set; }
            public string Name { get; set; }
            public Song.Device Device { get; set; }
            public int AssignedTrack { get; set; }
            public object InstrumentSource { get; set; }
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
            public object Device { get; set; }
            public object DeviceSource { get; set; }
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
            
            // put instruments into a nice list
            GetInstruments();

            // build all track notes into full picture of song
            BuildSongPicture();

            instrumentTracks = new Dictionary<Song.Track, object>();

            // create tracks (with notes and automation) for each instrument
            CreateInstrumentTracks();

            var master = this.project.Tracks.Items.Where(track => track is SequencerMasterTrack).First();

            trackReceives = new Dictionary<object, List<RnsReceive>>();

            foreach (var track in instrumentTracks)
            {
                trackReceives.Add(track.Key, new List<RnsReceive>());
            }

            foreach (var projectTrack in this.project.Tracks.Items)
            {
                trackReceives.Add(projectTrack, new List<RnsReceive>());
            }

            // find which instruments we are supposed to receive
            foreach (var ins in instrumentTracks)
            {
                trackReceives[ins.Value].Add(new RnsReceive(ins.Key, 0, 1.0));
            }

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
                        logger.WriteLine("WARNING: Track [{0}] has device [{1}] after send", trackName, device);
                    }
                }

                foreach (var send in sends)
                {
                    if (send.MuteSource && sends.IndexOf(send) != sends.Count-1)
                    {
                        logger.WriteLine("WARNING: Track [{0}] has muted send which is not the last send in the chain", trackName);
                    }

                    if (send.IsActive.Value == 1 && send.SendAmount.Value > 0)
                    {
                        var sendTrack = GetSendTrack((int) send.DestSendTrack.Value);
                        trackReceives[sendTrack].Add(new RnsReceive(projectTrack, 0, send.SendAmount.Value));
                    }
                    else
                    {
                        logger.WriteLine("WARNING: Track [{0}] has disabled send", trackName);
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
                        logger.WriteLine(string.Format("WARNING: Track [{0}] routing to soundcard?? WTF!", trackName));
                    }
                }
            }

            SideChainRouting();

            // clean up, i.e. remove any sequencer tracks with no receiving signal
            var deleteMe = new List<object>();

            foreach (var track in trackReceives)
            {
                if (!(track.Key is Song.Track))
                {
                    var trackName = GetProp("Name", track.Key);
                    if (track.Value.Count == 0)
                    {
                        logger.WriteLine("WARNING: Track [{0}] has no inputs, skipping", trackName);
                        deleteMe.Add(track.Key);
                    }
                }
            }

            foreach (var track in deleteMe)
            {
                trackReceives.Remove(track);
            }

            visitedTracks = new List<object>();
            orderedTracks = new List<object>();

            // time to head down to DFS for a new sofa (and yes I appreciate only UK people will get this reference)
            VisitTrack(master);

            var projectTracksToSongTracks = new Dictionary<object, Song.Track>();

            foreach (var projectTrack in orderedTracks)
            {
                Song.Track track;

                if (projectTrack is Song.Track)
                {
                    track = (Song.Track)projectTrack;
                }
                else
                {
                    track = ConvertTrack(this.project.Tracks.Items.ToList().IndexOf(projectTrack));
                }

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

        private void SideChainRouting()
        {
            // side chain routing
            foreach (var receive in trackReceives)
            {
                if (!(receive.Key is Song.Track))
                {
                    var trackName = GetProp("Name", receive.Key).ToString();
                    // not an instrument track so check side chain routine
                    var devices = (TrackFilterDeviceChain)GetProp("FilterDevices", receive.Key);
                    foreach (var device in devices.Devices.Items)
                    {
                        if (device is AudioPluginDevice)
                        {
                            var plug = (AudioPluginDevice)device;
                            if (plug.PluginIdentifier == "Metaplugin")
                            {
                                var meta = RipMetaSendIt(plug);
                                if (meta != null)
                                {
                                    if (meta.Mode != 1)
                                    {
                                        logger.WriteLine("WARNING: MetaPlug on track [{0}] has incorrect config", trackName);
                                        continue;
                                    }

                                    var source = GetSideChainSourceTracks(meta.Channel);
                                    foreach (var sourceTrack in source)
                                    {
                                        receive.Value.Add(new RnsReceive(sourceTrack, 2, 1.0));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        private List<object> GetSideChainSourceTracks(int channel)
        {
            var tracks = new List<object>();

            // side chain searching
            foreach (var receive in trackReceives)
            {
                if (!(receive.Key is Song.Track))
                {
                    var trackName = GetProp("Name", receive.Key);
                    // not an instrument track so check side chain routine
                    var devices = (TrackFilterDeviceChain)GetProp("FilterDevices", receive.Key);
                    foreach (var device in devices.Devices.Items)
                    {
                        if (device is AudioPluginDevice)
                        {
                            var plug = (AudioPluginDevice)device;
                            if (plug.PluginIdentifier == "SendIt")
                            {
                                if (devices.Devices.Items.ToList().IndexOf(plug) != devices.Devices.Items.Count() - 1)
                                {
                                    logger.WriteLine("WARNING: SendIt device on [{0}] not last in chain, results will be incorrect", trackName);
                                }

                                var sendItChunk = Convert.FromBase64String(FixBase64(plug.ParameterChunk));
                                var sendIt = (MYPLUGINSETTINGS)MetaPlugConvert(sendItChunk, typeof(MYPLUGINSETTINGS));

                                if (sendIt.Channel == channel && sendIt.Mode == 0)
                                {
                                    tracks.Add(receive.Key);
                                }


                            }
                        }
                    }
                }
            }

            return tracks;
        }

        private void CreateInstrumentTracks()
        {
            foreach (var ins in instruments)
            {
                if (ins.Device != null)
                {
                    var track = new Song.Track();
                    track.Name = ins.Name;
                    track.Devices = new List<Song.Device>();
                    track.Devices.Add(ins.Device);
                    track.Volume = ((RenoiseInstrument)ins.InstrumentSource).PluginGenerator.Volume * project.GlobalSongData.GlobalTrackHeadroom;
                    PopulateInstrumentTrack(track, ins);

                    if (track.Events.Count > 0)
                    {
                        instrumentTracks.Add(track, project.Tracks.Items[ins.AssignedTrack]);
                    }
                    else
                    {
                        logger.WriteLine("WARNING: Instrument {0} has no note data, skipping", ins.Name);
                    }
                }
            }
        }

        private void PopulateInstrumentTrack(Song.Track track, RnsIns instrument)
        {
            // specific track assign, midi can come from any track but audio must go to track
            if (instrument.AssignedTrack >= 0)
            {
                List<Song.Event> events = new List<Song.Event>();

                // loop each track for notes and collect any for this instrument
                foreach (var notes in noteTracks)
                {
                    track.Events.AddRange(NotesToEvents(notes, instrument.InstrumentId));
                }
            }
            else
            {
                // no specific track, so find first track with instrument id and warn that the rest are ignored
                var trackId = -1;

                foreach (var notes in noteTracks)
                {
                    var events = NotesToEvents(notes, instrument.InstrumentId);
                    if (events.Count > 0 && trackId == -1)
                    {
                        trackId = noteTracks.IndexOf(notes);
                        instrument.AssignedTrack = trackId;
                        track.Events.AddRange(events);
                    }
                    else if (events.Count > 0)
                    {
                        string trackA = GetProp("Name", project.Tracks.Items[trackId]).ToString();
                        string trackB = GetProp("Name", project.Tracks.Items[noteTracks.IndexOf(notes)]).ToString();
                        
                        // warning you pesky renoise users!
                        logger.WriteLine(
                            "WARNING: Instrument [{0}] dynamically using track [{1}] but also present on track [{2}], notes will be ignored", 
                            instrument.Name,
                            trackA,
                            trackB);
                    }
                }
            }

            foreach (var auto in automationMaps)
            {
                if (auto.AutoSource is RenoiseInstrument)
                {
                    if (auto.AutoSource == instrument.InstrumentSource)
                    {
                        auto.Auotmation.DeviceIndex = 0;
                        track.Automations.Add(auto.Auotmation);
                    }
                }
            }

            // all collected, now sort the events as they could come from multiple tracks
            track.Events.Sort((a, b) =>
            {
                if (a.TimeStamp > b.TimeStamp) return 1;
                if (a.TimeStamp < b.TimeStamp) return -1;
                if (a.Type == Song.EventType.NoteOn && b.Type == Song.EventType.NoteOff) return 1;
                if (a.Type == Song.EventType.NoteOff && b.Type == Song.EventType.NoteOn) return -1;
                return 0;
            });
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
                
                if (device is AudioPluginDevice)
                {
                    var plug = (AudioPluginDevice)device;
                    if (plug.PluginIdentifier == "SendIt")
                    {
                        var sendItChunk = Convert.FromBase64String(FixBase64(plug.ParameterChunk));
                        var sendIt = (MYPLUGINSETTINGS)MetaPlugConvert(sendItChunk, typeof(MYPLUGINSETTINGS));
                        if (sendIt.PassThru == 0)
                        {
                            muted = true;
                            break;
                        }
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

        private void BuildSongPicture()
        {
            noteTracks = new List<List<RnsPatternLineNode>>();

            // copy all patterns out to full song
            var songPatterns = new List<Pattern>();
            foreach (var seq in project.PatternSequence.SequenceEntries.SequenceEntry)
            {
                songPatterns.Add(project.PatternPool.Patterns.Pattern[seq.Pattern]);
            }

            // loop each track and build full song picture of notes and automations
            // because tracking in 2019 is stoopid!
            for (var trackId = 0; trackId < project.Tracks.Items.Count(); trackId++)
            {
                // loop each pattern to create complete picture of notes and automations...
                var allLines = new List<RnsPatternLineNode>();
                int lineIndex = 0;
                foreach (var pattern in songPatterns)
                {
                    var track = pattern.Tracks.Items[trackId];

                    // grab current lines on pattern
                    var currentLines = (PatternLineNode[])GetProp("Lines", track);

                    if (currentLines != null)
                    {
                        foreach (PatternLineNode line in currentLines)
                        {
                            var newLine = new RnsPatternLineNode();
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

                noteTracks.Add(allLines);
            }

            // now create map of all automations
            automationMaps = new List<RnsAutoMap>();
            for (var trackId = 0; trackId < project.Tracks.Items.Count(); trackId++)
            {
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
                                // this pattern has automations..  so process
                                foreach (var a in myAutos)
                                {
                                    var autoTemp = new List<RnsAuto>();
                                    foreach (string point in a.Envelope.Points) // add all current points
                                    {
                                        var timePoint = Convert.ToInt32(point.Split(',')[0]);
                                        var value = (float)Convert.ToDouble(point.Split(',')[1]);

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
                    finalAuto.ParamId = paramId - 1;  // renoise param index is out by one due to automation on active flag

                    foreach (var p in thisAutoList)
                    {
                        var point = new Song.Point();
                        point.Value = p.Value;
                        point.TimeStamp = SecondsToSamples(p.TimePoint / 256.00 * (double)secondsPerIndex, sampleRate);
                        finalAuto.Points.Add(point);
                    }

                    allAuto.Add(finalAuto);
                }

                var tempTrack = project.Tracks.Items[trackId];
                var trackName = GetProp("Name", tempTrack).ToString();

                var devices = (TrackFilterDeviceChain)GetProp("FilterDevices", tempTrack);

                foreach (var auto in allAuto)
                {
                    if (devices.Devices.Items[auto.DeviceIndex] is InstrumentAutomationDevice)
                    {
                        // TODO: Add it to the instrument auto bank
                        var autoMap = new RnsAutoMap();
                        autoMap.Auotmation = auto;
                        autoMap.AutoSource = project.Instruments.Instrument[auto.DeviceIndex];
                        automationMaps.Add(autoMap);
                    }
                    else if (devices.Devices.Items[auto.DeviceIndex] is AudioPluginDevice)
                    {
                        // TODO: add it to the track dsp auto bank
                        var autoMap = new RnsAutoMap();
                        autoMap.Auotmation = auto;
                        autoMap.AutoSource = tempTrack;
                        autoMap.AutoDevice = devices.Devices.Items[auto.DeviceIndex];
                        automationMaps.Add(autoMap);
                    }
                    else
                    {
                        // TODO: not sure how this could even happen!
                        logger.WriteLine("WARNING: unknown automation device on track [{0}]", trackName);
                    }
                }
            }
        }

        private Song.Track ConvertTrack(int trackId)
        {
            Song.Track songTrack = null;

            var trackObject = project.Tracks.Items[trackId];

            string trackName = GetProp("Name", trackObject).ToString();
            object[] trackDevices = ((TrackFilterDeviceChain)GetProp("FilterDevices", trackObject)).Devices.Items;

            songTrack = new Song.Track();
            songTrack.Name = trackName;
            songTrack.Volume = GetTrackVolume(trackDevices[0]); 
            var devices = new List<RnsDevice>();

            devices.AddRange(GetDevices(trackDevices, trackName));  // add track devices

            foreach (var device in devices)
            {
                if (device.Device is Song.Device)
                {
                    songTrack.Devices.Add((Song.Device)device.Device);
                }
            }
            
            foreach (var auto in automationMaps)
            {
                if (auto.AutoSource == trackObject)
                {
                    // found the track, now map the automations
                    auto.Auotmation.DeviceIndex = devices.IndexOf(devices.Single(d => d.DeviceSource == auto.AutoDevice));
                    if (auto.Auotmation.DeviceIndex < 0)
                    {
                        logger.WriteLine("WARNING: Track {0} has a bad automation link", trackName);
                    }
                    songTrack.Automations.Add(auto.Auotmation);
                }
            }

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

        private List<RnsDevice> GetDevices(object[] devices, string trackName)
        {
            var sabreDevices = new List<RnsDevice>();

            int deviceIndex = 0;

            foreach (var device in devices)
            {
                if (device is AudioPluginDevice)
                {
                    var plug = (AudioPluginDevice)device;
                    var sabreDevice = PlugToDevice(plug);

                    // check if sabre device is inside a Metaplugin for side chaining
                    if (plug.PluginIdentifier == "Metaplugin")
                    {
                        sabreDevice = RipMetaPlug(plug);
                    }

                    if (plug.PluginIdentifier != "SendIt")
                    {
                        if (sabreDevice == null)
                        {
                            logger.WriteLine(string.Format("WARNING: Track {0} has unkown plugin {1}",
                                trackName, plug.PluginIdentifier));
                        }
                    }

                    if (plug.IsActive.Value == 0)
                    {
                        logger.WriteLine(string.Format("WARNING: Track {0} has device {1} disabled", 
                            trackName, plug.PluginIdentifier));
                    }
                    else
                    {
                        sabreDevices.Add(new RnsDevice() { Device = sabreDevice, DeviceSource = plug });
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

        private MYPLUGINSETTINGS RipMetaSendIt(AudioPluginDevice plug)
        {
            var plugData = Convert.FromBase64String(FixBase64(plug.ParameterChunk));
            var meta = (AllData)MetaPlugConvert(plugData, typeof(AllData));
            if (meta == null)
                return null;

            if (!CheckMetaRouting(meta))
                return null;

            var sendIt = meta.FILTERGRAPH.FILTER.Where(f => f.PLUGIN.name == "SendIt").First();
            var metaChunk = Utils.Dejucer(sendIt.STATE);

            using (var reader = new BinaryReader(new MemoryStream(metaChunk)))
            {
                reader.BaseStream.Position = 0xA0;
                return (MYPLUGINSETTINGS)MetaPlugConvert(reader, typeof(MYPLUGINSETTINGS));
            }
        }

        private Song.Device RipMetaPlug(AudioPluginDevice plug)
        {
            var plugData = Convert.FromBase64String(FixBase64(plug.ParameterChunk));
            var meta = (AllData)MetaPlugConvert(plugData, typeof(AllData));
            if (meta == null)
                return null;

            if (!CheckMetaRouting(meta))
                return null;
            
            var sabre = meta.FILTERGRAPH.FILTER.Where(f => f.PLUGIN.manufacturer == "Logicoma").First();
            var metaChunk = Utils.Dejucer(sabre.STATE);
            byte[] sabreChunk;
            using (var reader = new BinaryReader(new MemoryStream(metaChunk)))
            {
                reader.BaseStream.Position = 0xA0-4;
                var size = BigEndianInt(reader);
                sabreChunk = reader.ReadBytes(size);
            }

            Song.Device device = null;
            Song.DeviceId deviceId;
            if (Enum.TryParse<Song.DeviceId>(sabre.PLUGIN.name, out deviceId))
            {
                device = new Song.Device();
                device.Id = deviceId;
                device.Chunk = sabreChunk;
            }

            return device;
        }

        private bool CheckMetaRouting(AllData meta)
        {
            var input = meta.FILTERGRAPH.FILTER.Where(f => f.PLUGIN.name == "Audio Input").First();
            var output = meta.FILTERGRAPH.FILTER.Where(f => f.PLUGIN.name == "Audio Output").First();
            var sabre = meta.FILTERGRAPH.FILTER.Where(f => f.PLUGIN.manufacturer == "Logicoma").First();
            var sendIt = meta.FILTERGRAPH.FILTER.Where(f => f.PLUGIN.name == "SendIt").First();

            if (input == null || output == null || sabre == null || sendIt == null)
                return false;

            // check input
            if (meta.FILTERGRAPH.CONNECTION.Where(c =>
                c.srcFilter == input.uid
                && c.dstFilter == sabre.uid
                && c.dstChannel <= 1).Count() != 2)
                return false;

            // check output
            if (meta.FILTERGRAPH.CONNECTION.Where(c =>
                c.srcFilter == sabre.uid
                && c.dstFilter == output.uid
                && c.dstChannel <= 1).Count() != 2)
                return false;

            // check sendit
            if (meta.FILTERGRAPH.CONNECTION.Where(c =>
                c.srcFilter == sendIt.uid
                && c.dstFilter == sabre.uid
                && c.dstChannel >= 2).Count() != 2)
                return false;

            return true;
        }

        private int BigEndianInt(BinaryReader reader)
        {
            int result = 0;

            result += (int)(reader.ReadByte() << (8 * 3));
            result += (int)(reader.ReadByte() << (8 * 2));
            result += (int)(reader.ReadByte() << (8 * 1));
            result += (int)(reader.ReadByte());

            return result;
        }

        private object MetaPlugConvert(byte[] chunk, Type type)
        {
            using (var reader = new BinaryReader(new MemoryStream(chunk)))
            {
                return MetaPlugConvert(reader, type);
            }
        }

        private object MetaPlugConvert(BinaryReader reader, Type type)
        {
            var tag = new string(reader.ReadChars(4));
            if (tag != "VC2!")
                return null;

            var size = reader.ReadInt32();
            var xmlChunk = new string(reader.ReadChars(size));
            return Utils.Deserializer(xmlChunk, type);
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

        private List<Song.Event> NotesToEvents(List<RnsPatternLineNode> lines, int instrumentId)
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

                // shift event time based on global groove
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
                    if (Convert.ToInt32(line.NoteColumns.NoteColumn[i].Instrument) == instrumentId)
                    {
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
            }
            return events;
        }

        // converts tracker notes to midi
        private List<Song.Event> NotesToEvents(List<RnsPatternLineNode> lines)
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

                // I "think" we look in two places here as we dual support old and new file formats
                AudioPluginDevice plug = null;
                if (instrument.PluginGenerator != null)
                {
                    plug = instrument.PluginGenerator.PluginDevice;
                    rnsins.AssignedTrack = instrument.PluginGenerator.OutputRoutings.OutputRouting[0].AssignedTrack;
                }
                else if (instrument.PluginProperties != null)
                {
                    plug = instrument.PluginProperties.PluginDevice;
                    rnsins.AssignedTrack = instrument.PluginProperties.OutputRoutings.OutputRouting[0].AssignedTrack;
                }

                var instrumentName = string.IsNullOrEmpty(instrument.Name) ? insId.ToString("X2") : instrument.Name;

                if (plug != null)
                {
                    Song.Device device = null;

                    Song.DeviceId deviceId;
                    if (Enum.TryParse<Song.DeviceId>(plug.PluginIdentifier, out deviceId))
                    {
                        device = new Song.Device();
                        device.Id = deviceId;
                        device.Chunk = Convert.FromBase64String(FixBase64(plug.ParameterChunk));
                        rnsins.Device = device;
                        rnsins.InstrumentSource = instrument;
                    }
                    if (device == null)
                    {
                        logger.WriteLine(string.Format("WARNING: Instrument [{0}] skipped (unsupported plugin): {1}",
                        instrumentName, plug.PluginIdentifier));
                    }
                }
                else
                {
                    logger.WriteLine(string.Format("WARNING: Instrument [{0}] skipped (not a vst plugin)", instrumentName));
                }

                instruments.Add(rnsins);
                insId++;
            }
        }

        void VisitTrack(object projectTrack)
        {
            if (!trackReceives.ContainsKey(projectTrack))
            {
                return;
            }

            var state = "Active";

            if (!(projectTrack is Song.Track))
            {
                state = GetProp("State", projectTrack).ToString();
            }

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
