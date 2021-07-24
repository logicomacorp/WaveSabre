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
        public bool Run(string testFileName, string refFileName, bool interactive)
        {
            var testFileInfo = new FileInfo(testFileName);
            var refFileInfo = new FileInfo(refFileName);

            Console.Write(testFileInfo.Name + ": ");
            var originalColor = Console.ForegroundColor;

            var logger = new NullLogger();

            var testSong = new ProjectConverter().Convert(testFileName, logger);

            bool success = false;

            if (refFileInfo.Exists)
            {
                var refSong = JsonConvert.DeserializeObject<Song>(File.ReadAllText(refFileName));
                var diff = new Diff(testSong, refSong);
                if (diff.IsEmpty)
                {
                    Console.ForegroundColor = ConsoleColor.Green;
                    Console.Write("PASSED");
                    success = true;
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
                    if (interactive)
                    {
                        Response response;
                        do
                        {
                            response = YesNoPrompt("Update ref?");
                        } while (response == Response.Invalid);
                        if (response == Response.Yes)
                        {
                            writeRef(refFileName, testSong);
                            Console.ForegroundColor = ConsoleColor.Cyan;
                            Console.Write("UPDATED REF");
                            success = true;
                        }
                        else
                        {
                            Console.ForegroundColor = ConsoleColor.Red;
                            Console.Write("FAILED");
                        }
                    }
                    else
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.Write("FAILED");
                    }
                }
            }
            else
            {
                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.WriteLine("NO REF FOUND");
                Console.ForegroundColor = originalColor;
                if (interactive)
                {
                    Response response;
                    do
                    {
                        response = YesNoPrompt("Create ref?");
                    } while (response == Response.Invalid);
                    if (response == Response.Yes)
                    {
                        writeRef(refFileName, testSong);
                        Console.ForegroundColor = ConsoleColor.Cyan;
                        Console.Write("CREATED REF");
                        success = true;
                    }
                    else
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.Write("FAILED");
                    }
                }
                else
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.Write("FAILED");
                }
            }

            Console.ForegroundColor = originalColor;
            Console.WriteLine();

            return success;
        }

        enum Response
        {
            Yes,
            No,
            Invalid,
        }

        Response YesNoPrompt(string prompt)
        {
            Console.WriteLine(prompt + " (y/n)");
            var line = Console.ReadLine().ToLower();
            if (line.Length == 1)
            {
                var c = line[0];
                if (c == 'y')
                {
                    return Response.Yes;
                }
                else if (c == 'n')
                {
                    return Response.No;
                }
            }
            return Response.Invalid;
        }

        void writeRef(string refFileName, Song song)
        {
            File.WriteAllText(refFileName, JsonConvert.SerializeObject(song));
        }
    }
}
