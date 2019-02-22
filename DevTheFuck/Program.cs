using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using WaveSabreConvert;

namespace DevTheFuck
{
    class Program
    {
        static void Main(string[] args)
        {
            var filename = @"C:\Users\Ian Ford\Google Drive\Colab\MusicDisk\Finals Project\Trashpanda.als";
            var logger = new ConsoleLogger();
            var song = new ProjectConverter().Convert(filename, logger);

            var inject = new RenoiseInject();
            var rnsSong = inject.InjectPatches(song, logger);

            var parser = new RenoiseParser();
            parser.Save(rnsSong, @"C:\Test\HelloWorld.xrns");


        }
    }
}
