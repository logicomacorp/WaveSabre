using System;
using System.Collections.Generic;
using System.Linq;
using Monad.FLParser;

namespace WaveSabreConvert
{
    public class FLConverter
    {
        ILog logger;

        private Project project;
        private Dictionary<object, List<FLReceive>> trackReceives;
        private List<object> visitedTracks, orderedTracks;
        private int sampleRate;
        private int tempo;

        class FLReceive
        {
            public object SendingTrack;
            public int ReceivingChannelIndex;
            public double Volume;
            public FLReceive(object sendingTrack, int receivingChannelIndex, double volume)
            {
                SendingTrack = sendingTrack;
                ReceivingChannelIndex = receivingChannelIndex;
                Volume = volume;
            }
        }

        class FLDevice
        {
            public int DeviceIndex { get; set; }
            public object Device { get; set; }
        }

        public Song Process(Project project, ILog logger)
        {
            this.logger = logger;
            this.project = project;
            
            var song = new Song();

            song.Tempo = (int) project.Tempo;
            song.SampleRate = 44100;
            sampleRate = song.SampleRate;
            tempo = song.Tempo;

            song.Length = GetSongLength();

            // remove unwanted insert channels
            for (var i = 1; i < project.Inserts.Length; i++)
            {
                var insert = project.Inserts[i];
                if (!IsInsertUsed(insert))
                {
                    project.Inserts[i] = null;
                }
            }

            // deal with sends and routing

            // master track is insert channel 0
            var master = project.Inserts[0];

            // add both insert and channel tracks
            trackReceives = new Dictionary<object, List<FLReceive>>();
            foreach (var projectTrack in project.Inserts)
            {
                if (projectTrack != null) trackReceives.Add(projectTrack, new List<FLReceive>());
            }
            foreach (var channel in project.Channels)
            {
                if (channel.Data is GeneratorData) trackReceives.Add(channel, new List<FLReceive>());
            }

            // link insert track routes
            foreach (var projectTrack in this.project.Inserts)
            {
                if (projectTrack == null) continue;

                for (int i = 0; i < projectTrack.Routes.Length; i++)
                {
                    if (projectTrack.Routes[i])
                    {
                        // route enabled
                        var sendTrack = project.Inserts[i];
                        var channelIndex = projectTrack.RouteVolumes[i] == 0 ? 2 : 0;  // very odd but sidechains are volume 0 which means 1 volume
                        double volume = 1;
                        if (projectTrack.RouteVolumes[i] > 0)
                        {
                            volume = (double) projectTrack.RouteVolumes[i]/12800;
                        }

                        trackReceives[sendTrack].Add(new FLReceive(projectTrack, channelIndex, volume));
                    }
                }
            }

            // link channel track routes
            foreach (var channel in project.Channels)
            {
                if (channel.Data is GeneratorData)
                {
                    var genData = (GeneratorData)channel.Data;
                    var insert = project.Inserts[genData.Insert];
                    trackReceives[insert].Add(new FLReceive(channel, 0, genData.Volume / 10000));  // no idea why FL decided on two different volume types!
                }
            }

            visitedTracks = new List<object>();
            orderedTracks = new List<object>();

            // visit master to order tracks using DFS
            VisitTrack(master);

            var projectTracksToSongTracks = new Dictionary<object, Song.Track>();

            foreach (var projectTrack in orderedTracks)
            {
                var track = ConvertTrack(projectTrack);
                projectTracksToSongTracks.Add(projectTrack, track);
                song.Tracks.Add(track);
            }

            // convert each tracks data
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

        private Song.Track ConvertTrack(object projectTrack)
        {
            if (projectTrack is Insert) return ConvertInsert((Insert) projectTrack);
            if (projectTrack is Channel) return ConvertGenerator((Channel) projectTrack);
            return null;
        }

        // convert insert track to wavesabre song track
        private Song.Track ConvertInsert(Insert insert)
        {
            var track = new Song.Track();
            track.Devices = new List<Song.Device>();

            track.Volume = (float)insert.Volume / 12800;
            string trackName = insert.Name != "" ? insert.Name : project.Inserts.ToList().IndexOf(insert).ToString();
            track.Name = trackName;
            var sabreDevices = new List<FLDevice>();

            // add slot devices with Id
            for (var i = 0; i < insert.Slots.Length; i++)
            {
                var slot = insert.Slots[i];
                if (slot.Plugin != null && slot.Plugin.Name != null)
                {
                    var sabreDevice = PlugToDevice(slot.Plugin);
                    if (sabreDevice == null)
                    {
                        logger.WriteLine("WARNING: {0} slot {1} has unkown plugin {2}",
                            trackName, 
                            i,
                            slot.Plugin.Name);
                    }

                    if (slot.State == 0)
                    {
                        logger.WriteLine("WARNING: {0} slot {1} has plugin {2} disabled",
                            trackName,
                            i,
                            slot.Plugin.Name);
                    }
                    else if (slot.Volume != 12800)
                    {
                        logger.WriteLine("WARNING: {0} slot {1} has plugin {2} dry / wet level unsupported, skipping",
                                trackName,
                                i,
                                slot.Plugin.Name);
                    }
                    else
                    {
                        sabreDevices.Add(new FLDevice() {DeviceIndex = i, Device = sabreDevice});
                    }
                }
            }

            foreach (var device in sabreDevices)
            {
                track.Devices.Add((Song.Device)device.Device);
            }

            // automations
            foreach (var flTrack in project.Tracks)
            {
                foreach (var pl in flTrack.Items)
                {
                    if (pl is ChannelPlaylistItem)
                    {
                        var pl2 = (ChannelPlaylistItem) pl;
                        int position = pl2.Position;
                        if (pl2.Channel.Data is AutomationData)
                        {
                            var auto = (AutomationData) pl2.Channel.Data;
                            if (insert.Id == auto.InsertId)
                            {
                                var deviceIndex = sabreDevices.IndexOf(sabreDevices.FirstOrDefault(p => p.DeviceIndex == auto.SlotId));
                                if (deviceIndex < 0)
                                {
                                    logger.WriteLine("WARNING: Insert {0} automation linked to missing slot device", trackName);
                                    continue;
                                }

                                var newAuto = track.Automations.FirstOrDefault(a => a.ParamId == auto.Parameter && a.DeviceIndex == deviceIndex);
                                if (newAuto == null)
                                {
                                    newAuto = new Song.Automation()
                                    {
                                        DeviceIndex = deviceIndex,
                                        ParamId = auto.Parameter
                                    };
                                    track.Automations.Add(newAuto);
                                }

                                foreach (var key in auto.Keyframes)
                                {
                                    position += key.Position;
                                    newAuto.Points.Add(new Song.Point()
                                    {
                                        TimeStamp = PositionToSamples(position),
                                        Value = (float) key.Value
                                    });
                                }
                            }
                        }
                    }
                }
            }

            foreach (var auto in track.Automations) SortPoints(auto);

            return track;
        }

        // convert channel track to wavesabre song track
        private Song.Track ConvertGenerator(Channel chan)
        {
            var track = new Song.Track();
            track.Devices = new List<Song.Device>();

            var generator = (GeneratorData) chan.Data;
            track.Volume = (float)generator.Volume / 10000;
            string trackName = chan.Name != "" ? chan.Name : project.Channels.IndexOf(chan).ToString();
            track.Name = trackName;
            var sabreDevice = PlugToDevice(generator.Plugin);
            if (sabreDevice == null)
            {
                logger.WriteLine("WARNING: Channel {0} has unknown plugin {1}", trackName, generator.Plugin.Name);
            }
            else
            {
                if (sabreDevice != null) track.Devices.Add(sabreDevice);
            }

            // notes
            foreach (var flTrack in project.Tracks)
            {
                foreach (var pl in flTrack.Items)
                {
                    if (pl is PatternPlaylistItem)
                    {
                        var pl2 = (PatternPlaylistItem) pl;
                        if (pl2.Muted) continue;    // muted, skip it

                        int position = pl.Position;
                        foreach (var notes in pl2.Pattern.Notes)
                        {
                            if (notes.Key == chan)      //  generator matches
                            {
                                foreach (var note in notes.Value)
                                {
                                    var startOffset = pl2.StartOffset < 0 ? 0 : pl2.StartOffset;
                                    var endOffset = pl2.EndOffset < 0 ? pl2.Length : pl2.EndOffset;
                                    var notePosition = note.Position - startOffset;
                                    var noteEnd = notePosition + note.Length;

                                    if (project.PlayTruncatedNotes)     // handling for play truncated notes
                                    {
                                        if (noteEnd <= 0)                // notes ends before block, skip it
                                            continue;

                                        if (notePosition >= pl2.Length)   // note start is after block, skip it
                                            continue;

                                        if (notePosition < 0)
                                            notePosition = 0;           // note ends inside block, start position inside block

                                        if (noteEnd > endOffset)        // note ends out after block, truncate
                                            noteEnd = endOffset;
                                    }
                                    else
                                    {
                                        if (notePosition < 0)
                                            continue;                   // note starts before block, skip it
                                    }

                                    // note position is good, so add it
                                    track.Events.Add(new Song.Event()
                                    {
                                        Type = Song.EventType.NoteOn,
                                        Note = note.Key,
                                        Velocity = note.Velocity,
                                        TimeStamp = PositionToSamples(notePosition + position)
                                    });

                                    track.Events.Add(new Song.Event()
                                    {
                                        Type = Song.EventType.NoteOff,
                                        Note = note.Key,
                                        Velocity = 0,
                                        TimeStamp = PositionToSamples(noteEnd + position)
                                    });
                                }
                            }
                        }
                    }
                }
            }

            // sort note events
            track.Events.Sort((a, b) =>
            {
                if (a.TimeStamp > b.TimeStamp) return 1;
                if (a.TimeStamp < b.TimeStamp) return -1;
                if (a.Type == Song.EventType.NoteOn && b.Type == Song.EventType.NoteOff) return 1;
                if (a.Type == Song.EventType.NoteOff && b.Type == Song.EventType.NoteOn) return -1;
                return 0;
            });

            // automations
            foreach (var flTrack in project.Tracks)
            {
                foreach (var pl in flTrack.Items)
                {
                    if (pl is ChannelPlaylistItem)
                    {
                        var pl2 = (ChannelPlaylistItem) pl;
                        if (pl2.Muted) continue;

                        var startOffset = pl2.StartOffset < 0 ? 0 : pl2.StartOffset;
                        var endOffset = pl2.EndOffset < 0 ? pl2.Length : pl2.EndOffset;

                        int position = pl2.Position;
                        if (pl2.Channel.Data is AutomationData)
                        {
                            var auto = (AutomationData) pl2.Channel.Data;
                            if (auto.Channel == null) continue;

                            if (chan.Id == auto.Channel.Id)
                            {
                                if (!auto.VstParameter)
                                {
                                    logger.WriteLine("WARNING: Track {0} contains channel param automation which is not supported", flTrack.Name);
                                    continue;
                                }

                                var newAuto = track.Automations.FirstOrDefault(a => a.ParamId == auto.Parameter);
                                if (newAuto == null)
                                {
                                    newAuto = new Song.Automation()
                                    {
                                        DeviceIndex = 0,
                                        ParamId = auto.Parameter
                                    };
                                    track.Automations.Add(newAuto);
                                }

                                var keyCount = 0;
                                var currentPosition = 0;
                                var previousPosition = 0;
                                double previousValue = 0;
                                double newValue = 0;

                                position = position - startOffset;
                                foreach (var key in auto.Keyframes)
                                {
                                    if (key.Tension != 0)
                                        logger.WriteLine(string.Format("Tension value not supported on track {0}", trackName));

                                    currentPosition += key.Position;
                                    var autoPosition = currentPosition;

                                    // ensure previous value persists between blocks
                                    if (keyCount == 0)                          
                                    {
                                        if (newAuto.Points.Count > 0)           
                                        {
                                            var lastAuto = newAuto.Points.LastOrDefault();
                                            newAuto.Points.Add(new Song.Point()
                                            {
                                                TimeStamp = PositionToSamples(position),
                                                Value = lastAuto.Value
                                            });
                                        }
                                    }

                                    // this key is before the clip block, skip it
                                    if (autoPosition < startOffset)               
                                    {
                                        previousPosition = autoPosition;
                                        previousValue = key.Value;
                                        continue;
                                    }

                                    // this key is after the clip block, must be last one, process and stop
                                    if (autoPosition > endOffset)               
                                    {
                                        newValue = previousValue - key.Value;
                                        newValue = newValue / (double)key.Position;
                                        newValue = newValue * (double)(autoPosition - endOffset);
                                        newValue = key.Value + newValue;
                                        newAuto.Points.Add(new Song.Point()
                                        {
                                            TimeStamp = PositionToSamples(position + endOffset),
                                            Value = (float)newValue
                                        });
                                        break;
                                    }

                                    // key is inside the clip block
                                                 
                                    if (keyCount == 0 && autoPosition > startOffset)                   // position not 0 in clip, calculat start point
                                    {
                                        newValue = previousValue - key.Value;
                                        newValue = newValue / (double)key.Position;
                                        newValue = newValue * (double)(autoPosition - startOffset);
                                        newValue = key.Value + newValue;

                                        newAuto.Points.Add(new Song.Point()
                                        {
                                            TimeStamp = PositionToSamples(position),
                                            Value = (float)newValue
                                        });
                                    }

                                    keyCount++;
                                    previousPosition = autoPosition;
                                    previousValue = key.Value;
                                    newAuto.Points.Add(new Song.Point()
                                    {
                                        TimeStamp = PositionToSamples(autoPosition + position),
                                        Value = (float) key.Value
                                    });
                                }
                            }
                        }
                    }
                }
            }

            foreach (var auto in track.Automations) SortPoints(auto);

            return track;
        }

        private void SortPoints(Song.Automation auto)
        {
            auto.Points.Sort((a, b) =>
            {
                if (a.TimeStamp > b.TimeStamp) return 1;
                if (a.TimeStamp < b.TimeStamp) return -1;
                return 0;
            });

        }

        private int PositionToSamples(int position)
        {
            var value = (long)position * (long)sampleRate * (long)60 / ((long)tempo * (long)project.Ppq);
            return (int)value;
        }

        public bool IsInsertUsed(Insert insert)
        {
            var insertIndex = insert.Id;
            foreach (var curInsert in project.Inserts)
            {
                if (curInsert != null)
                {
                    if (curInsert.Routes[insertIndex])
                    {
                        return true;
                    }
                }
            }

            foreach (var channel in project.Channels)
            {
                if (channel.Data is GeneratorData)
                {
                    var genData = (GeneratorData) channel.Data;
                    if (insertIndex == genData.Insert)
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        void VisitTrack(object projectTrack)
        {
            if (visitedTracks.Contains(projectTrack)) return;

            visitedTracks.Add(projectTrack);
            foreach (var projectReceive in trackReceives[projectTrack])
            {
                if (projectReceive.Volume > 0.0) VisitTrack(projectReceive.SendingTrack);
            }
            orderedTracks.Add(projectTrack);
        }

        private Song.Device PlugToDevice(Plugin plug)
        {
            if (plug.Name == null) return null;
            var name = plug.Name.Replace("WaveSabre - ", "");
            Song.Device device = null;

            Song.DeviceId deviceId;
            if (Enum.TryParse<Song.DeviceId>(name, out deviceId))
            {
                device = new Song.Device();
                device.Id = deviceId;
                device.Chunk = plug.State.Skip(21).ToArray();
            }

            return (device);
        }

        private double GetSongLength()
        {
            int totalPos = 0;

            foreach (var flTrack in project.Tracks)
            {
                foreach (var pl in flTrack.Items)
                {
                    if (pl is PatternPlaylistItem)
                    {
                        if (pl.Position + pl.Length > totalPos)
                        {
                            totalPos = pl.Position + pl.Length;
                        }
                    }
                }
            }
            int samples = PositionToSamples(totalPos);
            return (double)samples / (double)sampleRate;
        }
    }
}
