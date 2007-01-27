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
		
		static private string argPort;
		static private string argBaud;
		static private string argHexfile;
		static private byte argReceiverID;
				
        static void Main(string[] args)
        {
        	bool error = false;
        	if (args.Length != 4) {
        		Console.WriteLine("Syntax: program.exe <port> <baudrate> <hexfile> <nodeaddress>");
        		Console.WriteLine("port        - can be inform of /dev/ttyS0 or com2");
        		Console.WriteLine("baudrate    - any baudrate like 19200");
        		Console.WriteLine("hexfile     - path to the hexfile to program");
        		Console.WriteLine("nodeaddress - address of the node to program, in hex or decimal");
        		Console.WriteLine("");
        		Console.WriteLine("example: program.exe /dev/ttyUSB0 19200 /tmp/program.hex 24");
        		error = true;
        	} 
	        	
        	//Console.WriteLine("arg0: " + args[0] + "\n");
        	argPort = args[0];
        	argBaud = args[1];
        	argHexfile = args[2];
        	if (args[3].Length > 1) {
	        	if (args[3].Substring(0, 2).Equals("0x")) {
	        		if (args[3].Length == 3) {
			        	argReceiverID = byte.Parse(args[3].Substring(2, 1), System.Globalization.NumberStyles.HexNumber);
			        } else if (args[3].Length == 4) {
			        	argReceiverID = byte.Parse(args[3].Substring(2, 2), System.Globalization.NumberStyles.HexNumber);
			        } else {
			        	Console.WriteLine("nodeadress error!");
			        	error = true;
			        }
	    	    } else {
	        		argReceiverID = byte.Parse(args[3]);
		        }
		    } else {
		    	argReceiverID = byte.Parse(args[3]);
		    }
		    if (!error) {
	            Console.WriteLine("canBootloader");
	        	Console.WriteLine("Nodeaddress: "+argReceiverID);
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
        }

        static private bool parseInput(string instr)
        {
            if (instr.Equals("load"))
            {
                Console.WriteLine("loading...");
                //string filepath = "/home/arune/eclipse/c/AVR-App/main.hex";
                string filepath = argHexfile;
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
                    sc.setConfiguration(Int32.Parse(argBaud), System.IO.Ports.Parity.None, argPort, System.IO.Ports.StopBits.One, 8, false);
                }
                catch (Exception e) { Console.WriteLine("Error: " + e.Message); return true; }
                if (!sc.open()) { Console.WriteLine("Error opening port."); return true; }
                dl = new Downloader(hf, sc, argReceiverID);
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
