using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace WaveSabreConvert
{
    public class ReaperInject
    {        
        ILog logger;

        //private string _track;
        
        private Dictionary<string, string> _devices;

        public string InjectPatches(Song song, ILog logger)
        {
            this.logger = logger;

            CreateDevices();

            var project = Encoding.UTF8.GetString(WaveSabreConvert.Properties.Resources.ReaperProject);
            //_track = Encoding.UTF8.GetString(WaveSabreConvert.Properties.Resources.ReaperTrack);

            var sb = new StringBuilder();

            foreach (var track in song.Tracks)
            {
                if (track.Events.Count > 0)
                {
                    logger.WriteLine(string.Format("Converting track: {0}", track.Name));
                    sb.Append(CreateTrack(track));
                }
            }

            project = project.Replace("~Tracks~", sb.ToString());

            return project;
        }

        private void CreateDevices()
        {
            _devices = new Dictionary<string, string>();
            var devices = Encoding.UTF8.GetString(WaveSabreConvert.Properties.Resources.ReaperDevices);
            using (StringReader reader = new StringReader(devices))
            {
                var line = "";
                while (line != null)
                {
                    line = reader.ReadLine();
                    if (line != null && line.Contains(".dll"))
                    {
                        var sb = new StringBuilder();
                        sb.AppendLine(line);
                        var items = SplitData(line);
                        line = reader.ReadLine();
                        while (!line.Contains(">"))
                        {
                            sb.AppendLine(line);
                            line = reader.ReadLine();
                        }
                        sb.AppendLine(line);
                        _devices.Add(items[2], sb.ToString());
                    }
                }
            }
        }

        private List<string> SplitData(string data)
        {
            return Regex.Matches(data, @"[\""].+?[\""]|[^ ]+")
                .Cast<Match>()
                .Select(m => m.Value)
                .ToList();
        }

        private string CreateTrack(Song.Track track)
        {
            var reaperTrack = Encoding.UTF8.GetString(WaveSabreConvert.Properties.Resources.ReaperTrack).Replace("~TrackName~", track.Name);

            var sb = new StringBuilder();
            foreach (var device in track.Devices)
            {
                sb.Append(CreateDevice(device));
            }
            reaperTrack = reaperTrack.Replace("~Devices~", sb.ToString());

            return reaperTrack;
        }

        private string CreateDevice(Song.Device device)
        {
            var reaperDevice = "";
            var injected = "";

            var deviceId = device.Id.ToString() + ".dll";

            _devices.TryGetValue(deviceId, out reaperDevice);

            if (string.IsNullOrEmpty(reaperDevice))
            {
                // Whoops
                logger.WriteLine(string.Format("Device {0} not found", deviceId));
            }
            else
            {
                injected = InjectDevice(reaperDevice, device.Chunk);
            }

            return injected;
        }

        private string InjectDevice(string reaperDevice, byte[] deviceChunk)
        {
            var chunkList = new List<byte>();

            var header = "";
            var footer = "";

            // compile all base64 strings to one bin chunk
            using (var reader = new StringReader(reaperDevice))
            {
                var line = reader.ReadLine();
                while (line != null)
                {
                    line = line.TrimStart();
                    if (line.StartsWith("<"))
                    {
                        header = line;
                    }
                    else if (line.StartsWith(">"))
                    {
                        footer = line;
                    }
                    else
                    {
                        chunkList.AddRange(Convert.FromBase64String(line).ToList());
                    }

                    line = reader.ReadLine();
                }
            }

            var writer = new BinaryWriter(new MemoryStream());

            // vst chunk
            using (var reader = new BinaryReader(new MemoryStream(chunkList.ToArray())))
            {
                writer.Write(reader.ReadChars(4));
                //var tag = reader.ReadChars(4);
                writer.Write(reader.ReadInt32());
                //var something = reader.ReadInt32();
                var inputChannels = reader.ReadInt32();
                writer.Write(inputChannels);
                writer.Write(reader.ReadBytes(inputChannels * 8));
                //reader.ReadBytes(inputChannels * 8);
                
                var outputChannels = reader.ReadInt32();
                writer.Write(outputChannels);
                writer.Write(reader.ReadBytes(outputChannels * 8));
                //reader.ReadBytes(outputChannels * 8);
                
                var chunkLength = reader.ReadInt32();
                writer.Write(deviceChunk.Length);
                writer.Write(reader.ReadInt32());
                writer.Write(reader.ReadInt32());
                //reader.ReadInt32();     // no idea what this is
                //reader.ReadInt32();     // or this for that matter
                
                writer.Write(deviceChunk);
                reader.ReadBytes(chunkLength);
                int leftover = (int)reader.BaseStream.Length - (int)reader.BaseStream.Position;
                writer.Write(reader.ReadBytes(leftover));
            }

            var sb = new StringBuilder();
            var ms = (MemoryStream)writer.BaseStream;

            sb.AppendLine(header);
            sb.AppendLine(Convert.ToBase64String(ms.ToArray()));
            sb.AppendLine(footer);

            return sb.ToString();

        }


    }
}
