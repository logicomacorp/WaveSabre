using System;

namespace WaveSabreConvertTests
{
    class Printer
    {
        internal int Indent;

        public Printer()
        {
            Reset();
        }

        public void Reset()
        {
            Indent = 0;
        }

        public PrinterScope Scope()
        {
            return new PrinterScope(this);
        }

        public void PrintLine(string line)
        {
            for (int i = 0; i < Indent; i++)
                line = "    " + line;
            Console.WriteLine(line);
        }
    }

    class PrinterScope : IDisposable
    {
        readonly Printer _p;

        public PrinterScope(Printer p)
        {
            _p = p;

            _p.Indent++;
        }

        public void Dispose()
        {
            _p.Indent--;
        }
    }
}
