using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using WaveSabreConvert;
using Newtonsoft.Json;
using Newtonsoft.Json.Serialization;

namespace WaveSabreConvertTests
{
    class Test
    {
        public void Run(string testFileName, string refFileName)
        {
            var testFileInfo = new FileInfo(testFileName);
            var refFileInfo = new FileInfo(refFileName);

            Console.Write(testFileInfo.Name + ": ");
            var originalColor = Console.ForegroundColor;

            var logger = new NullLogger();

            var testSong = new ProjectConverter().Convert(testFileName, logger);

            if (refFileInfo.Exists)
            {
                var refSong = JsonConvert.DeserializeObject<Song>(File.ReadAllText(refFileName));
                var diff = new Diff(testSong, refSong);
                if (diff.IsEmpty)
                {
                    Console.ForegroundColor = ConsoleColor.Green;
                    Console.Write("PASSED");
                }
                else
                {
                    Console.ForegroundColor = ConsoleColor.Yellow;
                    Console.WriteLine("MISMATCH");
                    Console.ForegroundColor = originalColor;
                    Console.WriteLine("Converted song does not match stored ref.");
                    Console.WriteLine("Diff (test, ref):");
                    Console.ForegroundColor = ConsoleColor.White;
                    diff.Print();
                    Console.ForegroundColor = originalColor;
                    while (true)
                    {
                        Console.WriteLine("Update ref? (y/n)");
                        var line = Console.ReadLine().ToLower();
                        if (line.Length == 1)
                        {
                            var c = line[0];
                            if (c == 'y')
                            {
                                writeRef(refFileName, testSong);
                                Console.ForegroundColor = ConsoleColor.Cyan;
                                Console.Write("UPDATED REF");
                                break;
                            }
                            else if (c == 'n')
                            {
                                Console.ForegroundColor = ConsoleColor.Red;
                                Console.Write("FAILED");
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                writeRef(refFileName, testSong);
                Console.ForegroundColor = ConsoleColor.Cyan;
                Console.Write("CREATED REF");
            }

            Console.ForegroundColor = originalColor;
            Console.WriteLine();
        }

        void writeRef(string refFileName, Song song)
        {
            File.WriteAllText(refFileName, JsonConvert.SerializeObject(song));
        }
    }
}
