using System;
using System.Threading;
using System.Collections.Generic;
using System.Text;

///TODO:
// argument -v # (debuglevel)
// argument till konstruktor på alla objekt: debuglevel

class Program {
	const byte DEBUG_LEVEL = 3;	//0, 1, 2, 3
	static bool HWID_INUSE = false;
	
	//connects to candaemon on tcp to send and receive packets
	static DaemonConnection dc;
	//a commandline arguments parser
	static Arguments CommandLine;
	//functions for downloading hex to target
	static Downloader dl;
	//maintains a hexfile
	private static HexFile hf;
	
	static bool aBios=false;		//if commandline says it is a bios update
	static bool aReset=false;		//if commandline says a reset should be sent before downloading to target
	static bool aStart=false;		//if commandline says a start should be sent after downloading to target
	static bool aTerminal=false;	//if commandline says we should start in interactive mode
		
	static bool sNodeid=false;		//true when a nodeid has been parsed
	static bool sHWid=false;		//true when a hardware id has been parsed
	static bool sHexfile=false;		//true when a hexfile has been specified
		
	static string hexfile=null;		//path to a hexfile
	static byte nodeid=0;			//id of current node
	static uint HWid=0;   			//id of current node
	static string host=null;		//address of candaemon-host
	static int port=1200;			//port of candaemon-host
	
	static void Main(string[] args) {
		bool error = false;
		if (args.Length < 4) {
			//if to few arguments then print info and quit
			printSyntax();
			error = true;
		} 

		if (!error) {
			//parse commandline arguments
			CommandLine=new Arguments(args);
			
			//check single arguments
			if (CommandLine["b"] == "true") { aBios = true; }
			if (CommandLine["r"] == "true") { aReset = true; }
			if (CommandLine["s"] == "true") { aStart = true; }
			if (CommandLine["t"] == "true") { aTerminal = true; }
			
			//check hexfile argument
			hexfile = CommandLine["f"];
			if (hexfile != null) {
				sHexfile = true;
			}
			if (!aTerminal && !sHexfile && !aReset && !aStart) {
				if (DEBUG_LEVEL>0) { Console.WriteLine("When not in terminal mode a hexfile, a reset or a start parameter must be supplied, I quit"); }
				error = true;
			} 
			
			//check nodeid argument
			string node = CommandLine["n"];
			if (node != null) {
				if (!parseNodeId(node)) {
					error = true;
				}
			}
			//check hwid argument
			string hwid = CommandLine["i"];
			if (hwid != null) {
				if (!parseHWId(hwid)) {
					error = true;
				}
				HWID_INUSE = true;
			} 
			if (!aTerminal && (!sNodeid && !sHWid)) {
				if (DEBUG_LEVEL>0) { Console.WriteLine("When not in terminal mode a nodeid or a HWid must be supplied, I quit"); }
				error = true;
			} 
			
			//check host and port arguments
			host = CommandLine["h"];
			string sPort = CommandLine["p"];
			if ((host == null) || (sPort.Length == 0)) {
				if (DEBUG_LEVEL>0) { printSyntax(); }
				error = true;
			} else {
				try {
					port = int.Parse(sPort);
				} catch {
					if (DEBUG_LEVEL>0) { Console.WriteLine("Was not able to parse port, I quit"); }
					error = true;
				}
			}
		}
		
		if  (!error) {
			//commandline feedback output
			if (DEBUG_LEVEL>2) { 
				Console.WriteLine("canDude settings:");
				if (sNodeid) { Console.WriteLine("Nodeaddress: 0x" + String.Format("{0:x2}", nodeid)); }
				if (sHWid) { Console.WriteLine("HW id: 0x" + String.Format("{0:x8}", HWid)); }
				Console.WriteLine("Host: {0}:{1}", host, port);
				if (hexfile != null) { Console.WriteLine("Hexfile: {0}", hexfile); }
				Console.Write("Parameters: ");
				if (aBios) { Console.Write("Bios "); }
				if (aReset) { Console.Write("Reset "); }
				if (aStart) { Console.Write("Start "); }
				if (aTerminal) { Console.Write("Terminal"); }
				Console.WriteLine("");
			}
		}
		
		if (!error) {
			//connect to candaemon
			dc = new DaemonConnection();
			if (dc.init(host, port)) {
				if (DEBUG_LEVEL>1) { Console.WriteLine("Connected to candaemon"); }
			} else { 
				if (DEBUG_LEVEL>0) { Console.WriteLine("Connection to candaemon could not be established, I quit"); }
				error = true; 
			}
			Thread.Sleep(100);
		}
			
		if (!error && aTerminal) {
			//terminal mode (==interactive mode)
			bool success = true;
			//load hexfile if one has been specified as commandline argument
			hf = new HexFile();
			if (sHexfile && success) {
				if (hf.loadHex(hexfile)) {
					if (DEBUG_LEVEL>1) { Console.WriteLine("Hexfile loaded"); }
				} else { success = false; }
			}
			
			if (success) {
				if (DEBUG_LEVEL>1) {
					Console.WriteLine("");
					printHelp();		//print available commands
					Console.WriteLine("");
				}
				
				string instr;
				do {
					Console.Write("> ");			//a command has been executed, print a new console char
					instr = Console.ReadLine();		//read std in
				} while (parseInput(instr));		//parse a line from std in
				
				//exit command
			}
		}

		if (!error && !aTerminal) {
			//not terminal mode
			bool success = true;
			
			if (sHexfile) {
				//load hexfile (a hexfile must be supplied as commandline argument)
				hf = new HexFile();
				if (hf.loadHex(hexfile)) {
					if (DEBUG_LEVEL>1) { Console.WriteLine("Hexfile loaded"); }
				} else { 
					success = false;
					if (DEBUG_LEVEL>0) { Console.WriteLine("Hexfile could not be loaded, I quit"); }
				}
			}
			
			//if a hexfile is loaded then start autodownloading sequence
			if (success) {
				bool autosuccess = true;
				
				CanNMT cpn = new CanNMT(HWID_INUSE);
				if (aReset) {
					//send a reset command (and wait for feedback)
				   autosuccess = cpn.doReset(dc, nodeid, HWid);

					if (!autosuccess) {
						if (DEBUG_LEVEL>0) { Console.WriteLine("Target node did not respond to reset, I quit"); }
					}
				}
				
				if (sHexfile && autosuccess) {
					//send application
					dl = new Downloader(hf, dc, nodeid, aBios, HWID_INUSE, HWid);
					if (!dl.go()) { 
						if (DEBUG_LEVEL>0) { Console.WriteLine("Error occured during download"); }
						autosuccess = false;
					}
				}
				
				if (autosuccess && aStart) {
					//start application
					cpn.doStart(dc, nodeid, HWid);
				}
				
			}
		}
		if (dc != null) {
			//stop tcp client thread
			dc.stop();
		}
	}
	
	static private void printSyntax() {
		Console.WriteLine("Syntax: program.exe [options] -h <host> -p <port>");
		Console.WriteLine("Host and port are host and port of canDaemon, most likely localhost and 1200.");
		Console.WriteLine("Options:");
		Console.WriteLine("  -f <hexfile>   Specify a file to be loaded to target.");
		Console.WriteLine("  -n <nodeid>    Id of target node, hex or decimal.");
		Console.WriteLine("  -t             Enter terminal mode.");
		Console.WriteLine("  -r             Reset node before loading target.");
		Console.WriteLine("  -s             Start node after loading target.");
		Console.WriteLine("  -b             Program hex as bios, use with caution.");
	}
	
	static private void printHelp() {
		Console.WriteLine("canDude commands:");
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
		// the goal here is to find a byte from 0-255 in a string, format in decimal (0-255) or in hex (0x0-0xff)
		// this is probably not done very neat
		sNodeid = false;
		if (node.Length > 0) {
			try {
				nodeid = byte.Parse(node);
				sNodeid = true;
			} catch {}
		}
		if (node.Length > 2) {
			if (node.Substring(0, 2).Equals("0x")) {
				try {
					nodeid = byte.Parse(node.Substring(2, node.Length-2), System.Globalization.NumberStyles.HexNumber);
					sNodeid = true;
				} catch {
					if (DEBUG_LEVEL>0) { Console.WriteLine("nodeadress parse error 1"); }
					return false;
				}
			}
		}
		
		return true;
	}
	
	static private bool parseHWId(string node) {
		// the goal here is to find a uint in a string, format in decimal (0-4294967296) or in hex (0x0-0xffffffff)
		// this is probably not done very neat
		sHWid = false;
		if (node.Length > 0) {
			try {
				HWid = uint.Parse(node);
				sHWid = true;
			} catch {}
		}
		if (node.Length > 2) {
			if (node.Substring(0, 2).Equals("0x")) {
				try {
					HWid = uint.Parse(node.Substring(2, node.Length-2), System.Globalization.NumberStyles.HexNumber);
					sHWid = true;
				} catch {
					if (DEBUG_LEVEL>0) { Console.WriteLine("HW id parse error 1"); }
					return false;
				}
			}
		}
		
		return true;
	}
	
	static private bool parseInput(string instr) {
		//parse commands entered on std in
		if (instr.Equals("reset")) {		//reset command, send a reset to current node
			if (sNodeid) {
				dc.flushData();
				CanNMT cpn = new CanNMT(HWID_INUSE);
                cpn.doReset(dc, nodeid, HWid);
			} else {
				if (DEBUG_LEVEL>0) { Console.WriteLine("No nodeid has been specified"); }
			}
		}
		else if (instr.Equals("start")) {	//start command, send a start to current node
			if (sNodeid) {
				CanNMT cpn = new CanNMT(HWID_INUSE);
				cpn.doStart(dc, nodeid, HWid);
			} else {
				if (DEBUG_LEVEL>0) { Console.WriteLine("No nodeid has been specified"); }
			}
		}
		else if (instr.StartsWith("go")) {	//downloading command
			bool dlBios = false;
			bool error = true;
			if (instr.Equals("go bios")) {	//if bios should be downloaded
				dlBios = true;
				error = false;
			} else if (instr.Equals("go")) {	//is an application should be downloaded
				error = false;
			}
			
			if (!sNodeid) {
				if (DEBUG_LEVEL>0) { Console.WriteLine("No nodeid has been specified"); }
				error = true;
			}
			if (!sHexfile) {
				if (DEBUG_LEVEL>0) { Console.WriteLine("No hexfile has been specified"); }
				error = true;
			}
			
			CanNMT cpn = new CanNMT(HWID_INUSE);
			if (!error && aReset) {				//commandline arguments specified that a reset should be done before download
				//send a reset command (and wait for feedback)
				if (!cpn.doReset(dc, nodeid, HWid)) {
					if (DEBUG_LEVEL>0) { Console.WriteLine("Target node did not respond to reset"); }
					error = true;
				}
			}
				
			if (!error) {
				//send application
				dc.flushData();
				dl = new Downloader(hf, dc, nodeid, dlBios, HWID_INUSE, HWid);
				if (!dl.go()) { 
					if (DEBUG_LEVEL>0) { Console.WriteLine("Error occured during download"); }
					error = true;
				}
			}
				
			if (!error && aStart) {				//commandline arguments specified that a start should be done after download
				//start applikation
				cpn.doStart(dc, nodeid, HWid);
			}
			
		}
		else if (instr.StartsWith("load")) {	//load hexfile or reload current hexfile
			if (instr.Length > 5) {
				//second argument is a hexfile, this file should be loaded
				hexfile = instr.Substring(5);
				if (hf.loadHex(hexfile)) {
					sHexfile = true;
					if (DEBUG_LEVEL>1) { Console.WriteLine("Hexfile loaded"); }
				} else { 
					if (DEBUG_LEVEL>0) { Console.WriteLine("Hexfile could not be loaded"); }
				}
			} else {
				//reload current hexfile. if a hexfile already has been specified
				if (sHexfile) {
					if (hf.loadHex(hexfile)) {
						if (DEBUG_LEVEL>1) { Console.WriteLine("Hexfile reloaded"); }
					} else { 
						if (DEBUG_LEVEL>0) { Console.WriteLine("Hexfile could not be reloaded"); }
					}
				} else {
					if (DEBUG_LEVEL>0) { Console.WriteLine("No hexfile has been specified"); }
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
			if (DEBUG_LEVEL>1) { Console.WriteLine("NodeId is 0x"+ String.Format("{0:x2}", nodeid)); }
		}
		else if (instr.Equals("help")) {
			printHelp();
		}
		else if (instr.Equals("exit") || instr.Equals("quit") || instr.Equals("q")) {
			return false;
		}
		else {
			Console.WriteLine("Unknown command.");
		}
		
		return true;
	}
	
}
