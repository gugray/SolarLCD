using System;
using System.IO;
using System.IO.Ports;
using System.Threading;
using System.Globalization;

namespace VPC;

class Program
{
    const int periodMsec = 1000 * 60;
    const string portName = "/dev/tty.usbserial-10";

    static SerialPort port;
    static Exception timerEx;

    static void work()
    {
        port = new(portName, 9600);
        port.Open();
        Timer timer = new(period, null, 0, periodMsec);
        while (port.IsOpen)
        {
            Thread.Sleep(100);
            if (timerEx != null)
                throw timerEx;
        }
    }

    static void period(object o)
    {
        try
        {
            float val;
            while (true)
            {
                port.Write("\n");
                string ln = port.ReadLine();
                if (float.TryParse(ln, out val))
                    break;
            }
            var now = DateTime.Now;
            Console.WriteLine(now.ToString("s", CultureInfo.InvariantCulture) + '\t' + val);
        }
        catch (Exception ex)
        {
            timerEx = ex;
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

