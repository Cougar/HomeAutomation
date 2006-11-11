using System;
using System.Collections.Generic;
using System.Text;

namespace canBootloader
{
    class Program
    {
        static HexFile hf;
        static SerialConnection sc;
        static Downloader dl;

        static void Main(string[] args)
        {
            Console.WriteLine("canBootloader");
            Console.WriteLine("Commands:");
            Console.WriteLine("load - Load predefined hexfile.");
            Console.WriteLine("go - download hexfile.");
            Console.WriteLine("exit - exit program.");

            string instr;
            do
            {
                Console.Write("> ");
                instr = Console.ReadLine().ToLower();
            } while (parseInput(instr));

        }

        static private bool parseInput(string instr)
        {
            if (instr.Equals("load"))
            {
                Console.WriteLine("loading...");
                string filepath = "F:/MCP/projects/CAN/canNode/_output/canNode.hex";
                hf = new HexFile();
                if (hf.loadHex(filepath))
                {
                    Console.WriteLine("done!");
                    Console.WriteLine("File "+filepath+" loaded.");
                    Console.WriteLine("Size: " + hf.getLength().ToString() + " bytes, end adress " + hf.getAddrUpper().ToString() + "(0x" + hf.getAddrUpper().ToString("X") + ").");
                }
                else Console.WriteLine("Error loading " + filepath + "!");
                
            }
            else if (instr.Equals("go"))
            {
                Console.WriteLine("Connecting..");
                sc = new SerialConnection();
                try
                {
                    sc.setConfiguration(115200, System.IO.Ports.Parity.None, "COM2", System.IO.Ports.StopBits.One, 8, false);
                }
                catch (Exception e) { Console.WriteLine("Error: " + e.Message); return true; }
                if (!sc.open()) { Console.WriteLine("Error opening port."); return true; }
                dl = new Downloader(hf,sc);
                if (!dl.go()) { Console.WriteLine("Error gooing..."); return true; }

            }
            else if (instr.Equals("abort"))
            {
                if (dl != null) dl.abort();
            }
            else if (instr.Equals("exit"))
            {
                return false;
            }
            else
            {
                Console.WriteLine("Unknown command.");
            }

            return true;
        }

    }
}
