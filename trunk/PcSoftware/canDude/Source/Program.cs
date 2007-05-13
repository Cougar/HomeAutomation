using System;
using System.Threading;
using System.Collections.Generic;
using System.Text;

class Program {
	const byte DEBUG_LEVEL = 3;	//0, 1, 2, 3
	//static SerialConnection sc;
	static DaemonConnection dc;
	static Arguments CommandLine;
	//static TcpServer tcps;
	static Downloader dl;
	
	//private const byte CAN_NMT = 0x00;
	
	//private const byte CAN_NMT_RESET = 0x04;
	//private const byte CAN_NMT_START_APP 	= 0x24;
	//private const byte CAN_NMT_APP_START 	= 0x28;
	private const byte MY_ID = 0x00;
	private const byte MY_NID = 0x0;

	private static HexFile hf;
	static bool aBios=false;
	static bool aReset=false;
	static bool aStart=false;
	static bool aTerminal=false;
		
	static bool sNodeid=false;
	static bool sHexfile=false;
		
	static string hexfile=null;
	static byte nodeid=0;
	static string host=null;
	static int port=1200;
	
	static void Main(string[] args) {

		bool error = false;
		if (args.Length < 4) {
			Console.WriteLine("Syntax: program.exe [options] -h <host> -p <port>");
			Console.WriteLine("Host and port are host and port of canDaemon, most likely localhost and 1200.");
			Console.WriteLine("Options:");
			Console.WriteLine("  -f <hexfile>   Specify a file to be loaded to target.");
			Console.WriteLine("  -n <nodeid>    Id of target node, hex or decimal.");
			Console.WriteLine("  -t             Enter terminal mode.");
			Console.WriteLine("  -r             Reset node before loading target.");
			Console.WriteLine("  -s             Start node after loading target.");
			Console.WriteLine("  -b             Program hex as bios, use with caution.");
			error = true;
            return;
		} 

		if (!error) {
			//Console.WriteLine("arg0: " + args[0] + "\n");
			//argPort = args[0];
			//argBaud = Int32.Parse(args[1]);
			CommandLine=new Arguments(args);
			
			if (CommandLine["b"] == "true") { aBios = true; }
			if (CommandLine["r"] == "true") { aReset = true; }
			if (CommandLine["s"] == "true") { aStart = true; }
			if (CommandLine["t"] == "true") { aTerminal = true; }

			hexfile = CommandLine["f"];
			
			if (hexfile != null) {
				sHexfile = true;
			}
			if (!aTerminal && !sHexfile) {
				Console.WriteLine("When not in terminal mode a hexfile must be supplied");
				error = true;
			} 
			
			string node = CommandLine["n"];
			
			if (node != null) {
				if (!parseNodeId(node)) {
					error = true;
				}
			}
			
			if (!aTerminal && !sNodeid) {
				Console.WriteLine("When not in terminal mode a nodeid must be supplied");
				error = true;
			} 
			
			host = CommandLine["h"];
			string sPort = CommandLine["p"];
			if ((host == null) || (sPort.Length == 0)) {
				error = true;
			} else {
				try {
					port = int.Parse(sPort);
				} catch {
					error = true;
				}
			}
		}
		
		if  (!error) {
			//commandline feedback output (use debuglevel for hiding these)
			Console.WriteLine("canDude settings:");
			if (sNodeid) { Console.WriteLine("Nodeaddress: 0x" + String.Format("{0:x2}", nodeid)); }
			Console.WriteLine("Host: {0}:{1}", host, port);
			if (hexfile != null) { Console.WriteLine("Hexfile: {0}", hexfile); }
			Console.Write("Parameters: ");
			if (aBios) { Console.Write("Bios "); }
			if (aReset) { Console.Write("Reset "); }
			if (aStart) { Console.Write("Start "); }
			if (aTerminal) { Console.Write("Terminal"); }
			Console.WriteLine("");
		}
		
		if (!error && aTerminal) {
			bool success = true;
			
			dc = new DaemonConnection();
			if (dc.init(host, port)) {
				Console.WriteLine("Connected to candaemon");
			} else { 
				Console.WriteLine("Connection to candaemon could not be established, I quit");
				success = false; 
			}
			
			hf = new HexFile();
			if (sHexfile && success) {
				if (hf.loadHex(hexfile)) {
					Console.WriteLine("Hexfile loaded");
				} else { success = false; }
			}
			
			if (success) {
				if (DEBUG_LEVEL>0) {
					Console.WriteLine("");
					printHelp();
					Console.WriteLine("");
					//Thread.Sleep(1000);
				}
				
				string instr;
				do {
					Console.Write("> ");
					instr = Console.ReadLine();
				} while (parseInput(instr));
				
				//exit command
				dc.stop();
			}
		}

		if (!error && !aTerminal) {
			//not terminal mode
			bool success = true;
			
			//load hexfile (a hexfile must be supplied as commandline argument)
			hf = new HexFile();
			if (hf.loadHex(hexfile)) {
				Console.WriteLine("Hexfile loaded");
			} else { 
				success = false;
				Console.WriteLine("Hexfile could not be loaded, I quit");
			}
			
			//connect to candaemon
			dc = new DaemonConnection();
			if (success) {
				if (dc.init(host, port)) {
					Console.WriteLine("Connected to candaemon");
				} else { 
					Console.WriteLine("Connection to candaemon could not be established, I quit");
					success = false; 
				}
			}
			
			//if a hexfile is loaded and we are connected to candaemon then start autodownloading sequence
			if (success) {
				bool autosuccess = true;
				
				CanNMT cpn = new CanNMT();
				if (aReset) {
					//send a reset command (and wait for feedback)
					if (!cpn.doReset(dc, nodeid)) {
						Console.WriteLine("Target node did not respond to reset, I quit");
						autosuccess = false;
					}
				}
				
				if (autosuccess) {
					//send application
					Console.WriteLine("Programming not implemented yet, I quit");
					autosuccess = false;
				}
				
				if (autosuccess && aStart) {
					//start applikation
					cpn.doStart(dc, nodeid);
				}
				
			}
		}
	}
	
	static private void printHelp() {
		Console.WriteLine("canDude commands:");
		//Console.WriteLine("reset - reset communication.");
		Console.WriteLine("load [<hexfile>] - reload hexfile or load new hexfile");
		Console.WriteLine("node <nodeid>    - specify id of target node, hex or decimal");
		Console.WriteLine("reset            - reset node");
		Console.WriteLine("start            - start application in node (no feedback yet)");
		Console.WriteLine("go               - start download application to target");
		Console.WriteLine("go bios          - start download bios to target, use with caution");
		Console.WriteLine("help             - prints this help");
		Console.WriteLine("exit or quit     - exit program");
	}	
	
	static private bool parseNodeId(string node) {
		sNodeid = false;
		if (node.Length > 0) {
			try {
				nodeid = byte.Parse(node);
				sNodeid = true;
			} catch {}
		}
		if (node.Length > 2) {
			if (node.Substring(0, 2).Equals("0x")) {
				if (node.Length == 3) {
					try {
						nodeid = byte.Parse(node.Substring(2, 1), System.Globalization.NumberStyles.HexNumber);
						sNodeid = true;
					} catch {
						Console.WriteLine("nodeadress error!");
						return false;
					}
				} else if (node.Length == 4) {
					try {
						nodeid = byte.Parse(node.Substring(2, 2), System.Globalization.NumberStyles.HexNumber);
						sNodeid = true;
					} catch {
						Console.WriteLine("nodeadress error!");
						return false;
					}
				} else {
					Console.WriteLine("nodeadress error!");
					return false;
				}
			}
		}
		
		return true;
	}
	
	static private bool parseInput(string instr) {
		if (instr.Equals("reset")) {
			if (sNodeid) {
				CanNMT cpn = new CanNMT();
				cpn.doReset(dc, nodeid);
			} else {
				Console.WriteLine("No nodeid has been specified");
			}
		}
		else if (instr.Equals("start")) {
			if (sNodeid) {
				CanNMT cpn = new CanNMT();
				cpn.doStart(dc, nodeid);
			} else {
				Console.WriteLine("No nodeid has been specified");
			}
		}
		else if (instr.StartsWith("go")) {
			bool dlBios = false;
			bool error = true;
			if (instr.Equals("go bios")) {
				dlBios = true;
				error = false;
			} else if (instr.Equals("go")) {
				error = false;
			}
			
			if (!sNodeid) {
				Console.WriteLine("No nodeid has been specified");
				error = true;
			}
			if (!sHexfile) {
				Console.WriteLine("No hexfile has been specified");
				error = true;
			}
			
			CanNMT cpn = new CanNMT();
			if (!error && aReset) {
				//send a reset command (and wait for feedback)
				if (!cpn.doReset(dc, nodeid)) {
					Console.WriteLine("Target node did not respond to reset");
					error = true;
				}
			}
				
			if (!error) {
				//send application
				dl = new Downloader(hf, dc, nodeid, dlBios);
				if (!dl.go()) { 
					Console.WriteLine("Error occured during download"); 
					error = true;
				}
			}
				
			if (!error && aStart) {
				//start applikation
				cpn.doStart(dc, nodeid);
			}
			
		}
		else if (instr.StartsWith("load")) {
			if (instr.Length > 5) {
				//second argument is a hexfile, this file should be loaded
				hexfile = instr.Substring(5);
				if (hf.loadHex(hexfile)) {
					sHexfile = true;
					Console.WriteLine("Hexfile loaded");
				} else { 
					Console.WriteLine("Hexfile could not be loaded");
				}
			} else {
				//reload current hexfile. if a hexfile already has been specified
				if (sHexfile) {
					if (hf.loadHex(hexfile)) {
						Console.WriteLine("Hexfile reloaded");
					} else { 
						Console.WriteLine("Hexfile could not be reloaded");
					}
				} else {
					Console.WriteLine("No hexfile has been specified");
				}
				
			}
		}
		else if (instr.StartsWith("node")) {
			//change nodeid
			if (instr.Length > 5) {
				//second argument is nodeid
				string node = instr.Substring(5);
				parseNodeId(node);
			}
			Console.WriteLine("NodeId is 0x"+ String.Format("{0:x2}", nodeid));
		}
		else if (instr.Equals("help")) {
			printHelp();
		}
		else if (instr.Equals("exit") || instr.Equals("quit")) {
			return false;
		}
		else {
			Console.WriteLine("Unknown command.");
		}
		
		return true;
	}
	
}
