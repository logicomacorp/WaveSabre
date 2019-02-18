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
        static void Main(string[] args)
        {
            var testsDir = new DirectoryInfo("Tests");
            var refsDir = new DirectoryInfo("Refs");

            foreach (var testFileInfo in testsDir.EnumerateFiles())
            {
                var testFileName = testFileInfo.Name;
                var refFileName = testFileName + ".ref";

                new Test().Run(testsDir.FullName + "\\" + testFileName, refsDir.FullName + "\\" + refFileName);
            }
        }
    }
}
