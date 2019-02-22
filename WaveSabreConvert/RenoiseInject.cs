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

            instrumentTemplate = Serializer(rnsSong.Instruments.Instrument[0]);
            var trackTemp = (SequencerTrack)rnsSong.Tracks.Items[0];
            trackTemplate = Serializer(trackTemp);
            var fxTemp = (AudioPluginDevice)trackTemp.FilterDevices.Devices.Items[1];
            fxTemplate = Serializer(fxTemp);

            var insList = new List<RenoiseInstrument>();
            var trkList = new List<object>();

            foreach (var track in song.Tracks)
            {
                if (track.Events.Count > 0)
                {
                    insList.Add(CreateInstrument(track));
                    trkList.Add(CreateTrack(track));
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

            return rnsSong;
        }

        private SequencerTrack CreateTrack(Song.Track track)
        {
            var rnsTrack = (SequencerTrack)Deserializer(trackTemplate, typeof(SequencerTrack));

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
            var rnsDevice = (AudioPluginDevice)Deserializer(fxTemplate, typeof(AudioPluginDevice));
            
            var deviceName = device.Id.ToString();
            rnsDevice.Parameters.Parameter = null;
            rnsDevice.ParameterChunk = Convert.ToBase64String(device.Chunk);
            rnsDevice.PluginIdentifier = deviceName;
            rnsDevice.PluginDisplayName = string.Format("VST: Logicoma: {0}", deviceName);
            rnsDevice.PluginShortDisplayName = deviceName;

            return rnsDevice;
        }

        private RenoiseInstrument CreateInstrument(Song.Track track)
        {
            var ins = (RenoiseInstrument)Deserializer(instrumentTemplate, typeof(RenoiseInstrument));

            var deviceName = track.Devices[0].Id.ToString();

            ins.Name = track.Name;
            var plug = ins.PluginGenerator.PluginDevice;
            plug.Parameters.Parameter = null;
            plug.ParameterChunk = Convert.ToBase64String(track.Devices[0].Chunk);
            plug.PluginIdentifier = deviceName;
            plug.PluginDisplayName = string.Format("VST: Logicoma: {0}", deviceName);
            plug.PluginShortDisplayName = deviceName;

            return ins;
        }

        private string Serializer(object o)
        {
            var data = "";

            try
            {
                var x = new System.Xml.Serialization.XmlSerializer(o.GetType());
                using (StringWriter writer = new StringWriter())
                {
                    x.Serialize(writer, o);
                    data = writer.ToString();
                }
            }
            catch
            {
            }

            return data;
        }

        private object Deserializer(string data, Type type)
        {
            object output;

            try
            {
                var x = new System.Xml.Serialization.XmlSerializer(type);
                using (StringReader reader = new StringReader(data))
                {
                    output = x.Deserialize(reader);
                }
            }
            catch
            {
                output = null;
            }

            return output;
        }
    }
}
