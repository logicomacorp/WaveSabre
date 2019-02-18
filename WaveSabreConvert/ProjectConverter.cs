using System;
using System.IO;

namespace WaveSabreConvert
{
    public class ProjectConverter
    {
        public Song Convert(string inputFileName, ILog logger)
        {
            Song song;

            if (!File.Exists(inputFileName))
            {
                throw new Exception("Project file not found.");
            }

            var extension = Path.GetExtension(inputFileName).ToLower();
            switch (extension)
            {
                case ".als":
                    var liveProject = new LiveParser().Process(inputFileName);
                    song = new LiveConverter().Process(liveProject, logger);
                    break;
                case ".xrns":
                    var renoiseProject = new RenoiseParser().Process(inputFileName);
                    song = new RenoiseConverter().Process(renoiseProject, logger);
                    break;
                case ".flp":
                    var flProject = Monad.FLParser.Project.Load(inputFileName, false);
                    song = new FLConverter().Process(flProject, logger);
                    break;
                case ".rpp":
                    var reaperProject = new ReaperParser.ReaperParser().ParseProject(inputFileName);
                    song = new ReaperConverter().Process(reaperProject, logger);
                    break;
                default:
                    throw new Exception("Invalid file format.");
            }

            song.DetectWarnings(logger);

            song.DeltaEncode();

            song.Restructure(logger);

            return song;
        }
    }
}
