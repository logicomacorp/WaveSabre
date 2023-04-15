using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Xml;

namespace WaveSabreConvert
{
    public class LiveParser
    {
        class ReturnSendInfo
        {
            public double MinVolume;
            public double Volume;
            public bool IsActive;
        }

        LiveProject project;
        XmlReader reader;

        Dictionary<LiveProject.Track, string> outputRoutingStrings;
        Dictionary<LiveProject.Track, List<ReturnSendInfo>> returnSendInfos;

        string getAttrib(string attribName)
        {
            return reader.GetAttribute(attribName);
        }

        double getDoubleAttrib(string attribName)
        {
            return double.Parse(getAttrib(attribName), System.Globalization.CultureInfo.InvariantCulture);
        }

        int getIntAttrib(string attribName)
        {
            return int.Parse(getAttrib(attribName), System.Globalization.CultureInfo.InvariantCulture);
        }

        bool getBoolAttrib(string attribName)
        {
            return bool.Parse(getAttrib(attribName));
        }

        string getValueAttrib()
        {
            return getAttrib("Value");
        }

        double getDoubleValueAttrib()
        {
            return getDoubleAttrib("Value");
        }

        int getIntValueAttrib()
        {
            return getIntAttrib("Value");
        }

        bool getBoolValueAttrib()
        {
            return getBoolAttrib("Value");
        }

        public LiveProject Process(string fileName)
        {
            project = new LiveProject();

            outputRoutingStrings = new Dictionary<LiveProject.Track, string>();
            returnSendInfos = new Dictionary<LiveProject.Track, List<ReturnSendInfo>>();

            using (var originalStream = new FileInfo(fileName).OpenRead())
            {
                using (var decompressionStream = new GZipStream(originalStream, CompressionMode.Decompress))
                {
                    using (reader = XmlReader.Create(decompressionStream))
                    {
                        while (reader.Read())
                        {
                            if (reader.NodeType == XmlNodeType.Element)
                            {
                                switch (reader.Name)
                                {
                                    case "Tracks":
                                        while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "Tracks"))
                                        {
                                            if (reader.NodeType == XmlNodeType.Element)
                                            {
                                                switch (reader.Name)
                                                {
                                                    case "MidiTrack":
                                                    case "AudioTrack":
                                                    case "GroupTrack":
                                                        parseTrack();
                                                        break;

                                                    case "ReturnTrack":
                                                        parseTrack(false, true);
                                                        break;
                                                }
                                            }
                                        }
                                        break;

                                    case "MasterTrack":
                                        parseTrack(true);
                                        break;

                                    case "Transport":
                                        parseTransport();
                                        break;
                                }
                            }
                        }
                    }
                }
            }

            foreach (var kvp in outputRoutingStrings)
            {
                try
                {
                    var routingString = kvp.Value;
                    switch (routingString)
                    {
                        case "AudioOut/None": break;
                        case "AudioOut/Master": kvp.Key.Sends.Add(new LiveProject.Send(project.MasterTrack, 1, 1.0, kvp.Key.IsSpeakerOn)); break;

                        default:
                            if (!routingString.StartsWith("AudioOut/")) throw new Exception("routing string must begin with \"AudioOut/\"");
                            routingString = routingString.Replace("AudioOut/", "");
                            if (!routingString.StartsWith("Track.") && routingString != "GroupTrack") throw new Exception("unrecognized routing string format");
                            string trackId = "";
                            string trackInputId = "";
                            if (routingString == "GroupTrack")
                            {
                                trackId = kvp.Key.TrackGroupId;
                                trackInputId = "";
                            }
                            else
                            {
                                routingString = routingString.Replace("Track.", "");
                                var parts = routingString.Split('/');
                                if (routingString != "GroupTrack" && parts.Length != 2) throw new Exception("routing string has too many parts");
                                trackId = parts[0];
                                trackInputId = parts[1];
                            }
                            LiveProject.Track sendTarget = null;
                            int sendTargetChannelIndex = 1;
                            foreach (var track in project.Tracks)
                            {
                                if (track != kvp.Key && track.Id == trackId)
                                {
                                    sendTarget = track;
                                    break;
                                }
                            }
                            if (sendTarget == null) throw new Exception("couldn't find target track");

                            if (routingString != "GroupTrack" && trackInputId != "TrackIn")
                            {
                                if (!trackInputId.StartsWith("DeviceIn.")) throw new Exception("unrecognized track input string");
                                trackInputId = trackInputId.Replace("DeviceIn.", "");
                                var parts = trackInputId.Split('.');
                                if (parts.Length != 2) throw new Exception("unrecognized track device input string");
                                if (parts[1] != "S1") throw new Exception("unrecognized track device input channel");
                                var deviceId = parts[0];
                                bool found = false;
                                foreach (var device in sendTarget.Devices)
                                {
                                    if (device.Id == deviceId)
                                    {
                                        found = true;
                                        break;
                                    }
                                }
                                if (!found) throw new Exception("couldn't find target track device");
                                sendTargetChannelIndex = 3;
                            }
                            kvp.Key.Sends.Add(new LiveProject.Send(sendTarget, sendTargetChannelIndex, 1.0, kvp.Key.IsSpeakerOn));
                            break;
                    }
                }
                catch (Exception e)
                {
                    throw new Exception("Unrecognized AudioOutputRouting: " + kvp.Value + " (" + e.Message + ")");
                }
            }
            foreach (var kvp in returnSendInfos)
            {
                foreach (var returnSendInfo in kvp.Value)
                {
                    // Cull sends whose value is -inf
                    if (returnSendInfo.Volume > returnSendInfo.MinVolume)
                        kvp.Key.Sends.Add(new LiveProject.Send(project.ReturnTracks[kvp.Value.IndexOf(returnSendInfo)], 1, returnSendInfo.Volume, returnSendInfo.IsActive));
                }
            }

            return project;
        }

        void parseTrack(bool isMasterTrack = false, bool isReturnTrack = false)
        {
            var currentNode = reader.Name;
            var track = new LiveProject.Track();
            returnSendInfos.Add(track, new List<ReturnSendInfo>());
            if (!isMasterTrack) track.Id = getAttrib("Id");
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == currentNode))
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    switch (reader.Name)
                    {
                        case "Name":
                            if (track.Name == null)
                            {
                                reader.ReadToFollowing("UserName");
                                track.Name = getValueAttrib();
                            }
                            break;

                        case "DeviceChain":
                        case "MasterChain":
                            if (!reader.IsEmptyElement) parseDeviceChain(track, isMasterTrack);
                            break;
                        case "TrackGroupId":
                            track.TrackGroupId = getValueAttrib();
                            break;
                        case "AutomationEnvelopes":
                            parseAutomationEnvelopes(track);
                            break;
                    }
                }
            }
            project.Tracks.Add(track);
            if (isReturnTrack) project.ReturnTracks.Add(track);
            if (isMasterTrack) project.MasterTrack = track;
        }

        void parseTransport()
        {
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "Transport"))
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    switch (reader.Name)
                    {
                        case "LoopOn": project.IsLoopOn = getBoolValueAttrib(); break;
                        case "LoopStart": project.LoopStart = getDoubleValueAttrib(); break;
                        case "LoopLength": project.LoopLength = getDoubleValueAttrib(); break;
                    }
                }
            }
        }

        void parseDeviceChain(LiveProject.Track track, bool isMasterTrack)
        {
            var currentNode = reader.Name;
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == currentNode))
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    switch (reader.Name)
                    {
                        case "DeviceChain":
                            if (!reader.IsEmptyElement) parseInnerDeviceChain(track);
                            break;

                        case "AudioOutputRouting":
                            if (!isMasterTrack)
                            {
                                while (reader.Read())
                                {
                                    if (reader.NodeType == XmlNodeType.Element && reader.Name == "Target")
                                    {
                                        outputRoutingStrings.Add(track, getValueAttrib());
                                        break;
                                    }
                                }
                            }
                            break;

                        case "Mixer":
                            if (!reader.IsEmptyElement) parseMixer(track, isMasterTrack);
                            break;

                        case "MainSequencer":
                            if (!reader.IsEmptyElement) parseMainSequencer(track);
                            break;
                    }
                }
            }
        }

        void parseInnerDeviceChain(LiveProject.Track track)
        {
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "DeviceChain"))
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    switch (reader.Name)
                    {
                        case "PluginDevice":
                            var device = new LiveProject.Device();
                            if (!reader.IsEmptyElement) parsePluginDevice(device, track);
                            track.Devices.Add(device);
                            break;
                    }
                }
            }
        }

        void parseMixer(LiveProject.Track track, bool isMasterTrack)
        {
            var currentNode = reader.Name;
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == currentNode))
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    switch (reader.Name)
                    {
                        case "Sends":
                            if (!reader.IsEmptyElement) parseSends(track);
                            break;

                        case "Speaker":
                            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "Speaker"))
                            {
                                if (reader.NodeType == XmlNodeType.Element && (reader.Name == "BoolEvent" || reader.Name == "Manual"))
                                {
                                    track.IsSpeakerOn = getBoolValueAttrib();
                                    break;
                                }
                            }
                            break;

                        case "Volume":
                            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "Volume"))
                            {
                                if (reader.NodeType == XmlNodeType.Element && (reader.Name == "FloatEvent" || reader.Name == "Manual"))
                                {
                                    track.Volume = getDoubleValueAttrib();
                                    break;
                                }
                            }
                            break;

                        case "Tempo":
                            if (isMasterTrack)
                            {
                                while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "Tempo"))
                                {
                                    if (reader.NodeType == XmlNodeType.Element && (reader.Name == "FloatEvent" || reader.Name == "Manual"))
                                    {
                                        project.Tempo = getDoubleValueAttrib();
                                        break;
                                    }
                                }
                            }
                            break;
                    }
                }
            }
        }

        void parseSends(LiveProject.Track track)
        {
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "Sends"))
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    switch (reader.Name)
                    {
                        case "TrackSendHolder":
                            var returnSendInfo = new ReturnSendInfo();
                            if (!reader.IsEmptyElement) parseTrackSendHolder(returnSendInfo);
                            returnSendInfos[track].Add(returnSendInfo);
                            break;
                    }
                }
            }
        }

        void parseTrackSendHolder(ReturnSendInfo returnSendInfo)
        {
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "TrackSendHolder"))
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    switch (reader.Name)
                    {
                        case "Manual":
                            returnSendInfo.Volume = getDoubleValueAttrib();
                            break;

                        case "Min":
                            returnSendInfo.MinVolume = getDoubleValueAttrib();
                            break;

                        case "Active":
                            returnSendInfo.IsActive = getBoolValueAttrib();
                            break;
                    }
                }
            }
        }

        void parseMainSequencer(LiveProject.Track track)
        {
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "MainSequencer"))
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    switch (reader.Name)
                    {
                        case "ClipTimeable":
                            if (!reader.IsEmptyElement) parseClipTimeable(track);
                            break;
                    }
                }
            }
        }

        void parseClipTimeable(LiveProject.Track track)
        {
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "ClipTimeable"))
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    switch (reader.Name)
                    {
                        case "MidiClip":
                            if (!reader.IsEmptyElement) parseMidiClip(track);
                            break;
                    }
                }
            }
        }

        void parseMidiClip(LiveProject.Track track)
        {
            var midiClip = new LiveProject.MidiClip();
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "MidiClip"))
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    switch (reader.Name)
                    {
                        case "CurrentStart":
                            midiClip.CurrentStart = getDoubleValueAttrib();
                            break;

                        case "CurrentEnd":
                            midiClip.CurrentEnd = getDoubleValueAttrib();
                            break;

                        case "Loop":
                            if (!reader.IsEmptyElement) parseMidiClipLoop(midiClip);
                            break;

                        case "Disabled":
                            midiClip.IsDisabled = getBoolValueAttrib();
                            break;

                        case "KeyTracks":
                            if (!reader.IsEmptyElement) parseKeyTracks(midiClip);
                            break;
                    }
                }
            }
            track.MidiClips.Add(midiClip);
        }

        void parseMidiClipLoop(LiveProject.MidiClip midiClip)
        {
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "Loop"))
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    switch (reader.Name)
                    {
                        case "LoopStart":
                            midiClip.LoopStart = getDoubleValueAttrib();
                            break;

                        case "LoopEnd":
                            midiClip.LoopEnd = getDoubleValueAttrib();
                            break;

                        case "StartRelative":
                            midiClip.LoopStartRelative = getDoubleValueAttrib();
                            break;
                    }
                }
            }
        }

        void parseKeyTracks(LiveProject.MidiClip midiClip)
        {
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "KeyTracks"))
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    switch (reader.Name)
                    {
                        case "KeyTrack":
                            if (!reader.IsEmptyElement) parseKeyTrack(midiClip);
                            break;
                    }
                }
            }
        }

        void parseKeyTrack(LiveProject.MidiClip midiClip)
        {
            var keyTrack = new LiveProject.KeyTrack();
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "KeyTrack"))
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    switch (reader.Name)
                    {
                        case "MidiNoteEvent":
                            var note = new LiveProject.Note();
                            note.Time = getDoubleAttrib("Time");
                            note.Duration = getDoubleAttrib("Duration");
                            note.Velocity = (int)getDoubleAttrib("Velocity");
                            note.IsEnabled = getBoolAttrib("IsEnabled");
                            keyTrack.Notes.Add(note);
                            break;

                        case "MidiKey":
                            keyTrack.MidiKey = getIntValueAttrib();
                            break;
                    }
                }
            }
            midiClip.KeyTracks.Add(keyTrack);
        }

        void parsePluginDevice(LiveProject.Device device, LiveProject.Track track)
        {
            device.Id = getAttrib("Id");
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "PluginDevice"))
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    switch (reader.Name)
                    {
                        case "On":
                            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "On"))
                            {
                                if (reader.NodeType == XmlNodeType.Element && (reader.Name == "BoolEvent" || reader.Name == "Manual"))
                                {
                                    device.Bypass = !getBoolValueAttrib();
                                    break;
                                }
                            }
                            break;

                        case "VstPluginInfo":
                            if (!reader.IsEmptyElement) parseDeviceVstInfo(device);
                            break;

                        case "ParameterList":
                            if (!reader.IsEmptyElement) parseParameterList(device, track);
                            break;
                    }
                }
            }
        }

        void parseDeviceVstInfo(LiveProject.Device device)
        {
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "VstPluginInfo"))
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    switch (reader.Name)
                    {
                        case "FileName":
                            device.PluginDll = getValueAttrib();
                            break;

                        case "Path": // Added for Ableton 11 compatibility
                            device.PluginDll = Path.GetFileName(getValueAttrib());
                            break;

                        case "Preset": // Added for Ableton 11 compatibility
                        case "VstPreset":
                            if (!reader.IsEmptyElement) parseVstPreset(device);
                            break;
                    }
                }
            }
        }

        void parseVstPreset(LiveProject.Device device)
        {
            var currentNode = reader.Name;
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == currentNode))
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    switch (reader.Name)
                    {
                        case "Buffer":
                            if (!reader.IsEmptyElement) parseVstPresetBuffer(device);
                            break;
                    }
                }
            }
        }

        void parseVstPresetBuffer(LiveProject.Device device)
        {
            var currentNode = reader.Name;
            var rawData = new List<byte>();
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == currentNode))
            {
                var line = reader.Value.Replace("\t", "").Replace("\n", "").TrimStart().TrimEnd();
                if (line.Length > 0)
                {
                    for (int i = 0; i < line.Length; i += 2)
                    {
                        rawData.Add(Convert.ToByte(line.Substring(i, 2), 16));
                    }
                    device.RawData = rawData.ToArray();
                }
            }
        }

        void parseParameterList(LiveProject.Device device, LiveProject.Track track)
        {
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "ParameterList"))
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    switch (reader.Name)
                    {
                        case "PluginFloatParameter":
                            if (!reader.IsEmptyElement) parseFloatParameter(device, track);
                            break;
                    }
                }
            }
        }

        void parseFloatParameter(LiveProject.Device device, LiveProject.Track track)
        {
            var floatParameter = new LiveProject.FloatParameter();
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "PluginFloatParameter"))
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    switch (reader.Name)
                    {
                        case "ParameterId":
                            floatParameter.Id = getIntValueAttrib();
                            break;
                        case "AutomationTarget":
                            floatParameter.AutomationTarget = getIntAttrib("Id");
                            break;
                        case "Events":
                            if (!reader.IsEmptyElement)
                            {
                                while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "Events"))
                                {
                                    if (reader.NodeType == XmlNodeType.Element && reader.Name == "FloatEvent")
                                    {
                                        var e = new LiveProject.Event();
                                        e.Time = getDoubleAttrib("Time");
                                        e.Value = (float)getDoubleValueAttrib();
                                        if (e.Time >= 0)
                                        {
                                            floatParameter.Events.Add(e);
                                        }
                                    }
                                }
                            }
                            break;
                    }
                }
            }

            // new special handling of automation events for Live 10
            // they appear higher up in the XML so are now stored in a separate object on the track
            // and linked together here.
            if (floatParameter.Events.Count == 0)
            {
                foreach (var auto in track.AutomationEnvelopes)
                {
                    if (auto.PointeeId == floatParameter.AutomationTarget)
                    {
                        floatParameter.Events = auto.Events;
                        break;
                    }
                }
            }

            if (floatParameter.Events.Count > 0)
            {
                device.FloatParameters.Add(floatParameter);
            }
        }

        // new Live 10 automation storage
        void parseAutomationEnvelopes(LiveProject.Track track)
        {
            var currentNode = reader.Name;
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == currentNode))
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    switch (reader.Name)
                    {
                        case "AutomationEnvelope":
                            parseAutomationEnvelope(track);
                            break;
                    }
                }
            }
        }

        void parseAutomationEnvelope(LiveProject.Track track)
        {
            var currentNode = reader.Name;
            var automation = new LiveProject.AutomationEnvelope();
            while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == currentNode))
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    switch (reader.Name)
                    {
                        case "PointeeId":
                            automation.PointeeId = getIntValueAttrib();
                            break;
                        case "Events":
                            if (!reader.IsEmptyElement)
                            {
                                while (reader.Read() && !(reader.NodeType == XmlNodeType.EndElement && reader.Name == "Events"))
                                {
                                    if (reader.NodeType == XmlNodeType.Element && reader.Name == "FloatEvent")
                                    {
                                        var e = new LiveProject.Event();
                                        e.Time = getDoubleAttrib("Time");
                                        e.Value = (float)getDoubleValueAttrib();
                                        if (e.Time >= 0)
                                        {
                                            automation.Events.Add(e);
                                        }
                                    }
                                }
                            }
                            break;
                    }
                }
            }
            track.AutomationEnvelopes.Add(automation);
        }
    }
}
