using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ReaperParser.ReaperElements
{
    [ReaperTag("REAPER_PROJECT")]
    public class ReaperProject : ReaperElement
    {
        public float SomeVersion { get; set; }
        public string Version { get; set; }
        public int ProjectId { get; set; }

        [ReaperTag("RIPPLE")]
        public int Ripple { get; set; }
        [ReaperTag("AUTOXFADE")]
        public int AuroCrossFade { get; set; }
        [ReaperTag("PANLAW")]
        public int PanLaw { get; set; }
        [ReaperTag("SAMPLERATE")]
        public int SampleRate { get; set; }
        [ReaperTag("LOOP")]
        public bool Loop { get; set; }

        public ReaperMasterVolumePan MasterVolume { get; set; }
        public ReaperTempo Tempo { get; set; }
        public ReaperSelection Selection { get; set; }
        public ReaperZoom Zoom { get; set; }
        public List<ReaperTrack> Tracks { get; set; }
        public List<ReaperMasterFxChain> MasterEffectsChain { get; set; }

        public ReaperProject()
        {
            Tracks = new List<ReaperTrack>();
            MasterEffectsChain = new List<ReaperMasterFxChain>();
        }
    }

    [ReaperTag("SELECTION")]
    public class ReaperSelection : ReaperElement
    {
        public double Start { get; set; }
        public double End { get; set; }
    }

    [ReaperTag("TEMPO")]
    public class ReaperTempo : ReaperElement
    {
        public float BPM { get; set; }
        public int Beats { get; set; }
        public int Bars { get; set; }
    }

    [ReaperTag("ZOOM")]
    public class ReaperZoom : ReaperElement
    {
        public float ZoomSize { get; set; }
        public float ZoomA { get; set; }
        public float ZoomB { get; set; }
    }

    [ReaperTag("MASTER_VOLUME")]
    public class ReaperMasterVolumePan : ReaperElement
    {
        public float Volume { get; set; }
        public float Pan { get; set; }
        public float VolumeX { get; set; }
        public float VolumeY { get; set; }
    }

    [ReaperTag("MASTERFXLIST")]
    public class ReaperMasterFxChain : ReaperElement
    {
        [ReaperTag("FXID")]
        public string Fxid { get; set; }
        [ReaperTag("WET")]
        public float Wet { get; set; }
        public ReaperVst Vst { get; set; }
        public List<ReaperAutomation> Automations { get; set; }

        public ReaperMasterFxChain()
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
                var project = (ReaperProject)this.ParentElement;
                project.MasterEffectsChain.Remove(this);
            }
        }
    }
}
