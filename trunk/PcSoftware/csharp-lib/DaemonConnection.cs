using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Collections;
using System.IO;

public class DaemonConnection {
	private TcpClient tc;
	private bool running;
//	private string host;
//	private int port;
	private NetworkStream ns;
	
	const byte DEBUG_LEVEL = 2;	//0, 1, 2, 3
	
	private Queue canPackets = Queue.Synchronized(new Queue());
	
	public DaemonConnection() {
	}
	
	public void flushData() {
		canPackets.Clear();
	}
	
	public bool init(string host, int port) {
		//connect
		bool success = false;
		try {
			tc = new TcpClient();
			tc.Connect(host, port);
			ns = tc.GetStream();
			Console.WriteLine("Connected to host " + host + " on port " + port);
			success = true;
			running = true;
			
			Thread t = new Thread(thread);
			t.Start();
			
		} catch {
			Console.WriteLine("Unknown error when connecting to host " + host + " on port " + port);
		}
		return success;
	}

	public bool getData(out CanPacket cp) {
		cp = null;
		if (canPackets.Count>0) {
			cp = (CanPacket)canPackets.Dequeue();
			return true;
		}
		return false;
	}
	
	public void thread() {
		byte[] data = new byte[1024];
		int recv;
		while (running) {
			Thread.Sleep(1);
			//Console.Write(".");
			if (ns.DataAvailable) {
				recv = ns.Read(data, 0, data.Length);
				if (recv == 0)
					break;
				
				string tcpdata = Encoding.ASCII.GetString(data, 0, recv);
				string [] split = null;
				split = tcpdata.Split( '\n' );
				for (int i = 0; i < split.Length; i++)
				{
					if (split[i].Trim().Length>3) {
						CanPacket cp = new CanPacket(split[i].Trim().Substring(4));
						canPackets.Enqueue(cp);
						//Console.WriteLine(cp.toRawString());
					}
				}
			}
		}
		
		ns.Close();
		tc.Close();
	}
	
	public void stop() {
		running = false;
	}
	
	public void sendCanPacket(CanPacket cp) {
		if (running) {
			try {
				byte[] data = new byte[1024];
				
				data = Encoding.ASCII.GetBytes(cp.toRawString()+"\n");
				ns.Write(data, 0, data.Length);
			}
			catch (Exception) {
				Console.WriteLine("Unknown error occured when trying to send data to host");
				running = false;
			}
		}
	}

}
