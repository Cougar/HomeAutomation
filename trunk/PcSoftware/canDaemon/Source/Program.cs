using System;
using System.Threading;
using System.Collections.Generic;
using System.Text;

class Program {
	//static HexFile hf;
	const byte DEBUG_LEVEL = 3;	//0, 1, 2, 3
	static SerialConnection sc;
	static Daemon d;
	static TcpServer tcps;
	//static Downloader dl;
	
	//private const byte CAN_NMT = 0x00;
	
	//private const byte CAN_NMT_RESET = 0x04;
	//private const byte CAN_NMT_START_APP 	= 0x24;
	//private const byte CAN_NMT_APP_START 	= 0x28;
	private const byte MY_ID = 0x00;
	private const byte MY_NID = 0x0;
	
	static private string argPort;
	static private int argBaud;
	
	static void Main(string[] args) {
		bool error = false;
		if (args.Length != 2) {
			Console.WriteLine("Syntax: program.exe <port> <baudrate>");
			Console.WriteLine("port        - can be in form of /dev/ttyS0 or com2 or /udp/ipaddress/port");
			Console.WriteLine("baudrate    - any baudrate like 19200 (for udp baudrate could be any number)");
			Console.WriteLine("");
			Console.WriteLine("example: program.exe /dev/ttyUSB0 19200");
			Console.WriteLine("example: program.exe /udp/192.168.0.10/1100 0");
			error = true;
            return;
		} 
		
		if (!error) {
			//Console.WriteLine("arg0: " + args[0] + "\n");
			argPort = args[0];
			argBaud = Int32.Parse(args[1]);
		}
		
		if (!error) {
			if (DEBUG_LEVEL>0) {
				Console.WriteLine("canDaemon");
				Console.WriteLine("Commands:");
				//Console.WriteLine("reset - reset communication.");
				Console.WriteLine("exit  - exit program");
				Console.WriteLine("");
				Thread.Sleep(1000);
			}
			
			sc = new SerialConnection();
			try {
				sc.setConfiguration(argBaud, System.IO.Ports.Parity.None, argPort, System.IO.Ports.StopBits.One, 8, false);
			}
			catch (Exception e) { Console.WriteLine("Error: " + e.Message); }
			if (!sc.open()) { 
				Console.WriteLine("Error opening port to target"); 
				//... här ska man testa om igen...
				
			} else {
				
				d = new Daemon(sc);
				tcps = new TcpServer(1200);
				d.addServer(tcps);
				
				Thread t = new Thread(d.thread);
				t.Start();
				
				string instr;
				do {
					//Console.Write("> ");
					instr = Console.ReadLine().ToLower();
				} while (parseInput(instr));
				d.stop();
			}
		}
	}
	
	static private bool parseInput(string instr) {
		if (instr.Equals("reset")) {
			//Console.WriteLine("Resetting..");
			//sc = new SerialConnection();
			//try {
			//	sc.setConfiguration(argBaud, System.IO.Ports.Parity.None, argPort, System.IO.Ports.StopBits.One, 8, false);
			//}
			//catch (Exception e) { Console.WriteLine("Error: " + e.Message); return true; }
			//if (!sc.open()) { Console.WriteLine("Error opening port."); return true; }
			
			//byte[] data = new byte[8];
			//CanPacket cp = new CanPacket(CAN_NMT, CAN_NMT_RESET, MY_ID, argReceiverID, 0, data);
			//sc.writePacket(cp);
            //sc.close();
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
