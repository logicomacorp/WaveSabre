using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace WaveSabreConvertTests
{
    class Program
    {
        static int Main(string[] args)
        {
            var testsDir = new DirectoryInfo("Tests");
            var refsDir = new DirectoryInfo("Refs");

            var interactive = args.Length == 1 ? args[0] == "-i" : false;

            bool success = true;

            foreach (var testFileInfo in testsDir.EnumerateFiles())
            {
                var testFileName = testFileInfo.Name;
                var refFileName = testFileName + ".ref";

                success |= new Test().Run(testsDir.FullName + "\\" + testFileName, refsDir.FullName + "\\" + refFileName, interactive);
            }

            return success ? 0 : 1;
        }
    }
}
