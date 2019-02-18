using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using WaveSabreConvert;

namespace ConvertTheFuck
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                if (args.Length != 3)
                {
                    Console.WriteLine(
                        "usage: ConvertTheFuck [input file] [option] [output file]\n\toption:\t-h export cpp song header file\n\t\t-b export binary song file\n");
                }
                else
                {
                    var logger = new ConsoleLogger();
                    var song = new ProjectConverter().Convert(args[0], logger);
                    var option = args[1];
                    var outFile = args[2];

                    switch (option)
                    {
                        case "-h":
                            File.WriteAllText(outFile, new Serializer().Serialize(song));
                            break;
                        case "-b":
                            File.WriteAllBytes(outFile, new Serializer().SerializeBinary(song));
                            break;
                        default:
                            Console.WriteLine(string.Format("ERROR: unknown option {0}", args[1]));
                            break;
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine("ERROR: " + e.Message);
            }
        }
    }
}
