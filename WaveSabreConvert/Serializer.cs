using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace WaveSabreConvert
{
    public class Serializer
    {
        public string Serialize(Song song)
        {
            var sb = new StringBuilder();

            sb.AppendLine("#include <WaveSabreCore.h>");
            sb.AppendLine("#include <WaveSabrePlayerLib.h>");
            sb.AppendLine();

            SerializeFactory(sb, song);
            sb.AppendLine();

            SerializeBlob(sb, song);
            sb.AppendLine();

            sb.AppendLine("SongRenderer::Song Song = {");
            sb.AppendLine("\tSongFactory,");
            sb.AppendLine("\tSongBlob");
            sb.AppendLine("};");

            return sb.ToString();
        }

        public byte[] SerializeBinary(Song song)
        {
            return CreateBinary(song);
        }

        void SerializeFactory(StringBuilder sb, Song song)
        {
            sb.AppendLine("WaveSabreCore::Device *SongFactory(SongRenderer::DeviceId id)");
            sb.AppendLine("{");

            var devicesUsed = new List<Song.DeviceId>();

            // We want to output these in the same order as they're listed
            //  in the DeviceId enum for debugging purposes
            foreach (var id in (Song.DeviceId[])Enum.GetValues(typeof(Song.DeviceId)))
            {
                if (song.Tracks.Any(track => track.Devices.Any(device => device.Id == id)))
                    devicesUsed.Add(id);
            }

            if (devicesUsed.Count > 0)
            {
                sb.AppendLine("\tswitch (id)");
                sb.AppendLine("\t{");

                foreach (var id in devicesUsed)
                {
                    var name = id.ToString();
                    sb.AppendLine("\tcase SongRenderer::DeviceId::" + name + ": return new WaveSabreCore::" + name + "();");
                }

                sb.AppendLine("\t}");
            }

            sb.AppendLine("\treturn nullptr;");

            sb.AppendLine("}");
        }

        byte[] CreateBinary(Song song)
        {
            var stream = new MemoryStream();
            var writer = new BinaryWriter(stream);

            // song settings
            writer.Write(song.Tempo);
            writer.Write(song.SampleRate);
            writer.Write(song.Length);

            // serialize all devices
            writer.Write(song.Devices.Count);
            foreach (var device in song.Devices)
            {
                writer.Write((byte)device.Id);
                writer.Write(device.Chunk.Length);
                writer.Write(device.Chunk);
            }

            // serialize all midi lanes
            writer.Write(song.MidiLanes.Count);
            foreach (var midiLane in song.MidiLanes)
            {
                writer.Write(midiLane.MidiEvents.Count);
                foreach (var e in midiLane.MidiEvents)
                {
                    writer.Write(e.TimeFromLastEvent);
                    byte note = e.Note;
                    switch (e.Type)
                    {
                        case Song.EventType.NoteOn:
                            writer.Write(note);
                            writer.Write(e.Velocity);
                            break;
                        case Song.EventType.NoteOff:
                            note = (byte)((int)note | 0x80);
                            writer.Write(note);
                            break;
                        default:
                            throw new Exception("Unsupported event type");
                    }
                }
            }

            // serialize each track
            writer.Write(song.Tracks.Count);
            foreach (var track in song.Tracks)
            {
                var trackSize = stream.Position;

                writer.Write(track.Volume);

                writer.Write(track.Receives.Count);
                foreach (var receive in track.Receives)
                {
                    writer.Write(receive.SendingTrackIndex);
                    writer.Write(receive.ReceivingChannelIndex);
                    writer.Write(receive.Volume);
                }

                writer.Write(track.Devices.Count);
                foreach (var deviceId in track.DeviceIndices)
                {
                    writer.Write(deviceId);
                }

                writer.Write(track.MidiLaneId);

                writer.Write(track.Automations.Count);
                foreach (var automation in track.Automations)
                {
                    writer.Write(automation.DeviceIndex);
                    writer.Write(automation.ParamId);
                    writer.Write(automation.DeltaCodedPoints.Count);
                    foreach (var point in automation.DeltaCodedPoints)
                    {
                        writer.Write(point.TimeFromLastPoint);
                        writer.Write(point.Value);
                    }
                }
            }

            return stream.ToArray();
        }

        void SerializeBlob(StringBuilder sb, Song song)
        {
            var blob = CreateBinary(song);

            sb.AppendLine("const unsigned char SongBlob[] =");
            sb.Append("{");
            int numsPerLine = 10;
            for (int i = 0; i < blob.Length; i++)
            {
                if (i < blob.Length - 1 && (i % numsPerLine) == 0)
                {
                    sb.AppendLine();
                    sb.Append("\t");
                }
                sb.Append("0x" + blob[i].ToString("x2") + ",");
                if (i < blob.Length - 1 && (i % numsPerLine) != numsPerLine - 1) sb.Append(" ");
            }
            sb.AppendLine();
            sb.AppendLine("};");
        }
    }
}
