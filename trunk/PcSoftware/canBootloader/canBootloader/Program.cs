using System;
using System.Collections.Generic;
using System.Text;

namespace canBootloader {
	class Program {
		static HexFile hf;
		static SerialConnection sc;
		static Downloader dl;

		private const byte CAN_NMT = 0x00;
		
		private const byte CAN_NMT_RESET = 0x00;
		private const byte CAN_NMT_START_APP 	= 0x07;
		private const byte CAN_NMT_APP_START 	= 0x08;
		private const byte MY_ID = 0x00;
		private const byte MY_NID = 0x0;
		
		static private string argPort;
		static private int argBaud;
		static private string argHexfile;
		static private byte argReceiverID;
				
		static void Main(string[] args) {
			bool error = false;
			if (args.Length != 4) {
				Console.WriteLine("Syntax: program.exe <port> <baudrate> <hexfile> <nodeaddress>");
				Console.WriteLine("port        - can be in form of /dev/ttyS0 or com2 or /udp/ipaddress/port");
				Console.WriteLine("baudrate    - any baudrate like 19200 (for udp baudrate could be any number)");
				Console.WriteLine("hexfile     - path to the hexfile to program");
				Console.WriteLine("nodeaddress - address of the node to program, in hex or decimal");
				Console.WriteLine("");
				Console.WriteLine("example: program.exe /dev/ttyUSB0 19200 /tmp/program.hex 24");
				Console.WriteLine("example: program.exe /udp/192.168.0.10/1100 0 /tmp/program.hex 24");
				error = true;
                return;
			} 
				
			if (!error) {
				//Console.WriteLine("arg0: " + args[0] + "\n");
				argPort = args[0];
				argBaud = Int32.Parse(args[1]);
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
			}

			if (!error) {
				Console.WriteLine("canBootloader");
				Console.WriteLine("Nodeaddress: "+argReceiverID);
				Console.WriteLine("Baudrate: "+argBaud);
				Console.WriteLine("Commands:");
				Console.WriteLine("load  - Load predefined hexfile.");
				Console.WriteLine("go    - download hexfile.");
				Console.WriteLine("start - start application in node.");
				Console.WriteLine("reset - reset node.");
				Console.WriteLine("exit  - exit program.");
	
				string instr;
				do {
					Console.Write("> ");
					instr = Console.ReadLine().ToLower();
				} while (parseInput(instr));
			}
		}

		static private bool parseInput(string instr) {
			if (instr.Equals("load")) {
				Console.WriteLine("loading...");
				//string filepath = "/home/arune/eclipse/c/AVR-App/main.hex";
				string filepath = argHexfile;
				hf = new HexFile();
				if (hf.loadHex(filepath)) {
					Console.WriteLine("done!");
					Console.WriteLine("File "+filepath+" loaded.");
					Console.WriteLine("Size: " + hf.getLength().ToString() + " bytes, end adress " + hf.getAddrUpper().ToString() + "(0x" + hf.getAddrUpper().ToString("X") + ").");
				}
				else Console.WriteLine("Error loading " + filepath + "!");
				
			}
			else if (instr.Equals("go")) {
				Console.WriteLine("Connecting..");
				sc = new SerialConnection();
				try {
					sc.setConfiguration(argBaud, System.IO.Ports.Parity.None, argPort, System.IO.Ports.StopBits.One, 8, false);
				}
				catch (Exception e) { Console.WriteLine("Error: " + e.Message); return true; }
				if (!sc.open()) { Console.WriteLine("Error opening port."); return true; }
				dl = new Downloader(hf, sc, argReceiverID);
				if (!dl.go()) { Console.WriteLine("Error gooing..."); return true; }
                sc.close();
			}
			else if (instr.Equals("start")) {
				Console.WriteLine("Starting..");
				sc = new SerialConnection();
				try {
					sc.setConfiguration(argBaud, System.IO.Ports.Parity.None, argPort, System.IO.Ports.StopBits.One, 8, false);
				}
				catch (Exception e) { Console.WriteLine("Error: " + e.Message); return true; }
				if (!sc.open()) { Console.WriteLine("Error opening port."); return true; }
				byte[] data = new byte[8];
				CanPacket cp = new CanPacket(CAN_NMT, CAN_NMT_START_APP, MY_NID, MY_ID, argReceiverID, 0, data);
				sc.writePacket(cp);
                sc.close();
			}
			else if (instr.Equals("reset")) {
				Console.WriteLine("Resetting..");
				sc = new SerialConnection();
				try {
					sc.setConfiguration(argBaud, System.IO.Ports.Parity.None, argPort, System.IO.Ports.StopBits.One, 8, false);
				}
				catch (Exception e) { Console.WriteLine("Error: " + e.Message); return true; }
				if (!sc.open()) { Console.WriteLine("Error opening port."); return true; }
				
				byte[] data = new byte[8];
				CanPacket cp = new CanPacket(CAN_NMT, CAN_NMT_RESET, MY_NID, MY_ID, argReceiverID, 0, data);
				sc.writePacket(cp);
                sc.close();
			}
			else if (instr.Equals("abort")) {
				if (dl != null) dl.abort();
			}
			else if (instr.Equals("exit")) {
				return false;
			}
			else {
				Console.WriteLine("Unknown command.");
			}

			return true;
		}

	}
}
