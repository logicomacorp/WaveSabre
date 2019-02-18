using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace ReaperParser.ReaperElements
{
    [ReaperTag("TRACK")]
    public class ReaperTrack : ReaperElement
    {
        public string Guid { get; set; }

        public List<ReaperFxChain> EffectsChain { get; set; }
        public ReaperVolumePan VolumePanning { get; set; }
        public List<ReaperMediaItem> MediaItems { get; set; }
        public List<ReaperTrackReceive> Receives { get; set; }
        public ReaperBusConfig BusConfig { get; set; }

        [ReaperTag("SEL")]
        public bool Selected { get; set; }

        [ReaperTag("NAME")]
        public string TrackName { get; set; }

        public ReaperTrack()
        {
            EffectsChain = new List<ReaperFxChain>();
            MediaItems = new List<ReaperMediaItem>();
            Receives = new List<ReaperTrackReceive>();
        }

        public override void CompleteParse()
        {
            if (string.IsNullOrEmpty(TrackName) || TrackName == "\"\"")
            {
                var trackId = ((ReaperProject)this.ParentElement).Tracks.IndexOf(this);
                TrackName = string.Format("[Track {0}]", trackId);
            }
        }

        public override string ToString()
        {
            return TrackName;
        }
    }

    [ReaperTag("ISBUS")]
    public class ReaperBusConfig : ReaperElement
    {
        public ReaperBusMode BusMode { get; set; }
        public int BusIcrement { get; set; }
    }

    public enum ReaperBusMode
    {
        Current = 0,
        OpenBus = 1,
        CloseBus = 2
    }

    [ReaperTag("AUXRECV")]
    public class ReaperTrackReceive : ReaperElement
    {
        public int ReceiveTrackId { get; set; }
        public ReaperFader ReceiveFader { get; set; }
        public float Volume { get; set; }
        public float Pan { get; set; }
        public bool Mute { get; set; }
        public bool InvertPhase { get; set; }
        public float Speaker { get; set; }
        public int ReceiveChannelIndex { get; set; }
        public int DestinationChannelIndex { get; set; }
        public string Thing { get; set; }
        public int MidiRouting { get; set; }
        public float ValueG { get; set; }
    }

    public enum ReaperFader
    {
        PostFader = 0,
        PreFader = 3,
        PreFx = 1
    }

    [ReaperTag("ITEM")]
    public class ReaperMediaItem : ReaperElement
    {
        [ReaperTag("POSITION")]
        public double Position { get; set; }
        [ReaperTag("LENGTH")]
        public double Length { get; set; }
        [ReaperTag("LOOP")]
        public bool Loop { get; set; }
        [ReaperTag("MUTE")]
        public bool Mute { get; set; }
        [ReaperTag("GROUP")]
        public int Group { get; set; }
        [ReaperTag("NAME")]
        public string ItemName { get; set; }
        [ReaperTag("CHANMODE")]
        public int ChannelMode { get; set; }
        [ReaperTag("IID")]
        public int ItemId { get; set; }
        [ReaperTag("SOFFS")]
        public float StartOffest { get; set; }
        public ReaperFadeIn FadeIn { get; set; }
        public ReaperFadeOut FadeOut { get; set; }
        public List<ReaperMediaSource> MediaSource { get; set; }
        
        public ReaperMediaItem()
        {
            MediaSource = new List<ReaperMediaSource>();
        }
    }

    [ReaperTag("FADEIN")]
    public class ReaperFadeIn : ReaperElement
    {
        public float Volume { get; set; }
        public float Duration { get; set; }
    }

    [ReaperTag("FADEOUT")]
    public class ReaperFadeOut : ReaperElement
    {
        public float Volume { get; set; }
        public float Duration { get; set; }
    }

    [ReaperTag("VOLPAN")]
    public class ReaperVolumePan : ReaperElement
    {
        public float Volume { get; set; }
        public float Pan { get; set; }
        public float VolumeX { get; set; }
        public float VolumeY { get; set; }
    }

    [ReaperTag("SOURCE")]
    public class ReaperMediaSource : ReaperElement
    {
        public string MediaType { get; set; }
        public List<ReaperMidiEvent> MidiEvents { get; set; }
        public ReaperMidiEventConfig MidiConfig { get; set; }
        public ReaperMediaSource()
        {
            MidiEvents = new List<ReaperMidiEvent>();
        }

        public override void CompleteParse()
        {
            if (MediaType == "MIDI")
            {
                foreach (var c in this.ChildElements)
                {
                    if (c.ElementName.ToUpper() == "E")
                    {
                        var m = new ReaperMidiEvent();
                        m.Selected = c.Data[0] == "e" ? true : false;
                        m.PositionDelta = Convert.ToInt32(c.Data[0]);
                        m.MidiEvent = (ReaperNoteEvent)Convert.ToInt32(c.Data[1], 16);
                        m.Note = Convert.ToInt32(c.Data[2], 16);
                        m.Velocity = Convert.ToInt32(c.Data[3], 16);

                        MidiEvents.Add(m);
                    }
                }
            }
        }
    }

    [ReaperTag("HASDATA")]
    public class ReaperMidiEventConfig : ReaperElement
    {
        public bool ItemHasData { get; set; }
        public int NoteSize { get; set; }
        public string NoteSizeType { get; set; }
    }

    public class ReaperMidiEvent : ReaperElement
    {
        public bool Selected { get; set; }
        public int PositionDelta { get; set; }
        public ReaperNoteEvent MidiEvent { get; set; }
        public int Note { get; set; }
        public int Velocity { get; set; }
    }

    public enum ReaperNoteEvent
    {
        NoteOn = 144,
        NoteOff = 128,
        MidiEnd = 176
    };

    [ReaperTag("FXCHAIN")]
    public class ReaperFxChain : ReaperElement
    {
        [ReaperTag("FXID")]
        public string Fxid { get; set; }
        [ReaperTag("WET")]
        public float Wet { get; set; }
        public ReaperVst Vst { get; set; }
        public List<ReaperAutomation> Automations { get; set; }

        public ReaperFxChain()
        {
            Automations = new List<ReaperAutomation>();
        }

        // override for customer parse of data
        public override void CompleteParse()
        {
            // clean up empty reaper effect chains
            // wanted to do this a better way, but the file format is lame
            if (this.Fxid == null)
            {
                var track = (ReaperTrack)this.ParentElement;
                track.EffectsChain.Remove(this);
            }
        }
    }

    [ReaperTag("PARMENV")]
    public class ReaperAutomation : ReaperElement
    {
        public int ParameterId { get; set; }
        public float ParaA { get; set; }
        public float ParaB { get; set; }
        public float ParaC { get; set; }
        public List<ReaperAutomationPoint> Points { get; set; }

        public ReaperAutomation()
        {
            Points = new List<ReaperAutomationPoint>();
        }
    }

    [ReaperTag("PT")]
    public class ReaperAutomationPoint : ReaperElement
    {
        public float Position { get; set; }
        public float Value { get; set; }
        public ReaperPointShape Shape { get; set; }
        public float Unknown { get; set; }
        public bool Selected { get; set; }
    }

    public enum ReaperPointShape
    {
        Linear = 0,
        Square = 1,
        SlowStartEnd = 2,
        FastStart = 3,
        FastEnd = 4,
        Bezier = 5
    }

    [ReaperTag("VST")]
    public class ReaperVst : ReaperElement
    {
        public string VstName { get; set; }
        public string VstFile { get; set; }
        public int VstFlag { get; set; }
        public string VstComment { get; set; }
        public int VstId { get; set; }

        [ReaperTag("FXID")]
        public string FxId { get; set; }

        public byte[] VstChunkData { get; set; }

        public ReaperVst()
        {
        }

        // hack to build vst chunk data as they aren't labelled
        public override void CompleteParse()
        {
            var chunkList = new List<byte>();

            // compile all base64 strings to one bin chunk
            foreach (var element in this.ChildElements)
            {
                chunkList.AddRange(Convert.FromBase64String(element.ElementName).ToList());
            }

            // vst chunk
            using (var reader = new BinaryReader(new MemoryStream(chunkList.ToArray())))
            {
                var tag = reader.ReadChars(4);
                var something = reader.ReadInt32();
                var inputChannels = reader.ReadInt32();
                reader.ReadBytes(inputChannels * 8);
                var outputChannels = reader.ReadInt32();
                reader.ReadBytes(outputChannels * 8);
                var chunkLength = reader.ReadInt32();
                reader.ReadInt32();     // no idea what this is
                reader.ReadInt32();     // or this for that matter
                
                VstChunkData = reader.ReadBytes(chunkLength);
            }
        }
    }
}
