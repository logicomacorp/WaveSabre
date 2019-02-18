using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WaveSabreConvert
{
    public interface ILog
    {
        void Write(string value);
        void WriteLine(string value);
        void WriteLine(string format, object arg0);
        void WriteLine(string format, params object[] arg);
    }

    public class ConsoleLogger : ILog
    {
        public void Write(string value)
        {
            Console.Write(value);
        }
        public void WriteLine(string value)
        {
            Console.WriteLine(value);
        }
        public void WriteLine(string format, object arg0)
        {
            Console.WriteLine(format, arg0);
        }
        public void WriteLine(string format, params object[] arg)
        {
            Console.WriteLine(format, arg);
        }
    }

    public class LogEvent : EventArgs
    {
        public string Output { get; set; }
    }

    public class EventLogger : ILog
    {
        public event EventHandler OnLog;

        protected virtual void LogOutput(EventArgs e)
        {
            EventHandler handler = OnLog;
            if (handler != null)
            {
                handler(this, e);
            }
        }

        public void Write(string value)
        {
            var e = new LogEvent();
            e.Output = value;
            LogOutput(e);
        }
        public void WriteLine(string value)
        {
            var e = new LogEvent();
            e.Output = value + Environment.NewLine;
            LogOutput(e);
        }
        public void WriteLine(string format, object arg0)
        {
            var e = new LogEvent();
            e.Output = string.Format(format, arg0) + Environment.NewLine;
            LogOutput(e);
        }
        public void WriteLine(string format, params object[] arg)
        {
            var e = new LogEvent();
            e.Output = string.Format(format, arg) + Environment.NewLine;
            LogOutput(e);
        }
    }

    public class NullLogger : ILog
    {
        public void Write(string value) { }
        public void WriteLine(string value) { }
        public void WriteLine(string format, object arg0) { }
        public void WriteLine(string format, params object[] arg) { }
    }
}
