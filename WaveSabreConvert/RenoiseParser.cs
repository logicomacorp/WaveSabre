using System.IO;
using System.IO.Compression;
using System.Xml.Serialization;
using Renoise;

namespace WaveSabreConvert
{
    public class RenoiseParser
    {
        public RenoiseSong Process(string fileName)
        {
            var song = new RenoiseSong();

            using (FileStream zipFile = new FileStream(fileName, FileMode.Open))
            {
                using (ZipArchive archive = new ZipArchive(zipFile, ZipArchiveMode.Read))
                {
                    foreach (var zipEntry in archive.Entries)
                    {
                        if (zipEntry.Name == "Song.xml")
                        {
                            var serializer = new XmlSerializer(typeof(RenoiseSong));
                            song = (RenoiseSong)serializer.Deserialize(zipEntry.Open());
                            break;
                        }
                    }
                }
            }

            return song;
        }
    }
}
