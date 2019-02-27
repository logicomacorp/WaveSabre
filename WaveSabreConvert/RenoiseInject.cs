using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Renoise;
using System.Xml.Serialization;
using System.IO;

namespace WaveSabreConvert
{
    public class RenoiseInject
    {
        ILog logger;

        private string instrumentTemplate;
        private string trackTemplate;
        private string fxTemplate;

        public RenoiseSong InjectPatches(Song song, ILog logger)
        {
            this.logger = logger;

            var parser = new RenoiseParser();
            var rnsSong = parser.Process(WaveSabreConvert.Properties.Resources.RenoiseTemplate);
            
            var insTemplate = rnsSong.Instruments.Instrument[0];

            instrumentTemplate = Utils.Serializer(rnsSong.Instruments.Instrument[0]);
            var trackTemp = (SequencerTrack)rnsSong.Tracks.Items[0];
            trackTemplate = Utils.Serializer(trackTemp);
            var fxTemp = (AudioPluginDevice)trackTemp.FilterDevices.Devices.Items[1];
            fxTemplate = Utils.Serializer(fxTemp);

            var insList = new List<RenoiseInstrument>();
            var trkList = new List<object>();
            var trkIndex = 0;

            foreach (var track in song.Tracks)
            {
                if (track.Events.Count > 0)
                {
                    logger.WriteLine(string.Format("Converting track: {0}", track.Name));
                    insList.Add(CreateInstrument(track, trkIndex));
                    trkList.Add(CreateTrack(track));
                    trkIndex++;
                }
            }

            foreach (var t in rnsSong.Tracks.Items)
            {
                if (t.GetType() == typeof(SequencerMasterTrack))
                {
                    trkList.Add(t);
                    break;
                }
            }

            rnsSong.Instruments.Instrument = insList.ToArray();
            rnsSong.Tracks.Items = trkList.ToArray();

            var trk = rnsSong.PatternPool.Patterns.Pattern[0].Tracks.Items[0];
            var mst = rnsSong.PatternPool.Patterns.Pattern[0].Tracks.Items[1];

            var patList = new List<object>();

            for (var i = 0; i < trkList.Count; i++)
            {
                if (i == trkList.Count - 1)
                    patList.Add(mst);
                else
                    patList.Add(trk);
            }
            rnsSong.PatternPool.Patterns.Pattern[0].Tracks.Items = patList.ToArray();
            return rnsSong;
        }

        private SequencerTrack CreateTrack(Song.Track track)
        {
            var rnsTrack = (SequencerTrack)Utils.Deserializer(trackTemplate, typeof(SequencerTrack));
            rnsTrack.Name = track.Name;

            var fxList = new List<object>();
            fxList.Add(rnsTrack.FilterDevices.Devices.Items[0]);

            for (var i = 1; i < track.Devices.Count; i++)
            {
                fxList.Add(CreateDevice(track.Devices[i]));
            }

            rnsTrack.FilterDevices.Devices.Items = fxList.ToArray();

            return rnsTrack;
        }

        private AudioPluginDevice CreateDevice(Song.Device device)
        {
            var rnsDevice = (AudioPluginDevice)Utils.Deserializer(fxTemplate, typeof(AudioPluginDevice));
            
            var deviceName = device.Id.ToString();
            rnsDevice.Parameters.Parameter = null;
            rnsDevice.ParameterChunk = Convert.ToBase64String(device.Chunk);
            rnsDevice.PluginIdentifier = deviceName;
            rnsDevice.PluginDisplayName = string.Format("VST: Logicoma: {0}", deviceName);
            rnsDevice.PluginShortDisplayName = deviceName;

            return rnsDevice;
        }

        private RenoiseInstrument CreateInstrument(Song.Track track, int index)
        {
            var ins = (RenoiseInstrument)Utils.Deserializer(instrumentTemplate, typeof(RenoiseInstrument));

            var deviceName = track.Devices[0].Id.ToString();

            ins.Name = track.Name;
            var plug = ins.PluginGenerator.PluginDevice;
            plug.Parameters.Parameter = null;
            plug.ParameterChunk = Convert.ToBase64String(track.Devices[0].Chunk);
            plug.PluginIdentifier = deviceName;
            plug.PluginDisplayName = string.Format("VST: Logicoma: {0}", deviceName);
            plug.PluginShortDisplayName = deviceName;

            ins.PluginGenerator.OutputRoutings.OutputRouting[0].AssignedTrack = index;
            ins.PluginGenerator.OutputRoutings.OutputRouting[0].AutoAssign = false;
            return ins;
        }
    }
}
