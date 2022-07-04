using System;
using System.IO;
using System.IO.Ports;
using System.Threading;
using System.Globalization;

namespace VPC;

class Program
{
    const int periodMsec = 1000;
    const string fname = "voltages.txt";
    const string portName = "/dev/tty.usbserial-110";

    static SerialPort port;
    static StreamWriter sw;

    static void work()
    {
        port = new(portName, 9600);
        port.Open();
        port.ReadExisting();
        sw = new StreamWriter(fname);
        while (port.IsOpen)
        {
            float val;
            string ln = port.ReadLine();
            if (!float.TryParse(ln, out val))
                continue;
            var now = DateTime.Now;
            string msg = now.ToString("s", CultureInfo.InvariantCulture) + '\t' + val;
            sw.WriteLine(msg);
            sw.Flush();
            Console.WriteLine(msg);
        }
    }

    static void Main(string[] args)
    {
        try
        {
            work();
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex);
        }
    }
}

