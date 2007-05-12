using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Collections;
using System.IO;

public class TcpServer {
	private TcpListener listener;
	private bool running;
	private int port;
	private ArrayList clients;
	const byte DEBUG_LEVEL = 2;	//0, 1, 2, 3
	
	private Queue tcpPackets = Queue.Synchronized(new Queue());
	
	public TcpServer(int port) {
		running = true;
		listener = new TcpListener(port);
		listener.Start();
		this.port = port;
		clients = new ArrayList();
	}

	public bool getData(out string data) {
		data = null;
		if (tcpPackets.Count>0) {
			data = (string)tcpPackets.Dequeue();
			return true;
		}
		return false;
	}
	
	public void thread() {
		if (DEBUG_LEVEL>1) { Console.WriteLine("Waiting for clients on port {0}", port); }
		while (running) {
			Thread.Sleep(10);
			foreach (Connection client in clients) {
				//... hämta data från client...
				//om ny data så skicka till alla i clients utom client
				string tcpdata;
				bool hasData = client.getData(out tcpdata);
				if (hasData) {
					
					//lägg data på kö så demonen kan hämta för vidare process
					tcpPackets.Enqueue(tcpdata);
					
					//skicka data till övriga klienter, senare ska datat parsas innan så man bara skickar ut can-paket
					foreach (Connection sendclient in clients) {
						if (sendclient != client) {
							sendclient.sendDataDummy(tcpdata);
						}
					}
				}
			}
			
			foreach (Connection client in clients) {
				if (!client.isConnected()) {
					clients.Remove(client);
					if (DEBUG_LEVEL>1) { Console.WriteLine("Client dropped"); }
					break;
				}
			}
			
         	if (running && listener.Pending()) {
         		if (DEBUG_LEVEL>1) { Console.WriteLine("Client connected on port {0}", port); }
				TcpClient handler = listener.AcceptTcpClient();
				Connection newconn = new Connection(handler);
				clients.Add(newconn);
				Thread t = new Thread(newconn.thread);
				t.Start();
			}
		}
	}
	
	public void stop() {
		running = false;
		foreach (Connection client in clients) {
			client.stop();
		}
	}
	
	public void sendCanPacket(CanPacket cp) {
		foreach (Connection client in clients) {
			client.sendData(cp);
		}
	}

}

class Connection {
	private TcpClient client;
	private NetworkStream ns;
	private bool connected;
	
	private Queue tcpPackets = Queue.Synchronized(new Queue());
	
	public Connection(TcpClient client) {
		this.client = client;
		ns = client.GetStream();
		//this.id = id;
	}
	
	public bool getData(out string data) {
		data = null;
		if (tcpPackets.Count>0) {
			data = (string)tcpPackets.Dequeue();
			return true;
		}
		return false;
	}
	
	public void thread() {
		//läs och lagra så att en funktion kan läsa från mig
		byte[] data = new byte[1024];
		int recv;
		connected = true;
		while (connected) {
			Thread.Sleep(20);
			if (ns.DataAvailable) {
				recv = ns.Read(data, 0, data.Length);
				if (recv == 0)
					break;
				
				string tcpdata = Encoding.ASCII.GetString(data, 0, recv);
				//string [] split = null;
				//split = tcpdata.Split(" ");

				//if (split.Length ) {
					tcpPackets.Enqueue(tcpdata);
				//}
				//Console.WriteLine(System.Text.Encoding.ASCII.GetString(data));
			}
		}
		connected = false;
		ns.Close();
		client.Close();
	}
	
	public bool isConnected() {
		return connected;
	}
	
	public void stop() {
		connected = false;
	}
	
	public void sendData(CanPacket cp) {
		if (connected) {
			try {
				byte[] data = new byte[1024];
				
				data = Encoding.ASCII.GetBytes(cp.toRawString()+"\n");
				ns.Write(data, 0, data.Length);
			}
			catch (Exception) {
				connected = false;
			}
		}
	}
	
	public void sendDataDummy(string senddata) {
		if (connected) {
			try {
				byte[] data = new byte[1024];
				
				data = Encoding.ASCII.GetBytes(senddata+"\n");
				ns.Write(data, 0, data.Length);
			}
			catch (Exception) {
				connected = false;
			}
		}
	}
		
}