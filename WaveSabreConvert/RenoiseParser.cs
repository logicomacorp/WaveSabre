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

            return Process(File.ReadAllBytes(fileName));
        }

        public RenoiseSong Process(byte[] data)
        {
            var song = new RenoiseSong();
            using (var zipFile = new MemoryStream(data))
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

        public void Save(RenoiseSong song, string fileName)
        {
            var songData = Utils.Serializer(song);

            using (var fileStream = new FileStream(fileName, FileMode.Create))
            {
                using (ZipArchive archive = new ZipArchive(fileStream, ZipArchiveMode.Create))
                {
                    var entry = archive.CreateEntry("Song.xml");
                    using (StreamWriter writer = new StreamWriter(entry.Open()))
                    {
                        writer.Write(songData);
                    }
                }
            }
        }
    }
}
