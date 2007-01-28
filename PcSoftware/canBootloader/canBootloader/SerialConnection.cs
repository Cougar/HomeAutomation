using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;

namespace canBootloader
{
	public class SerialConnection
	{
		public const byte UART_START_BYTE = 253;
		public const byte UART_END_BYTE = 250;
		public const int PACKET_LENGTH = 17;
		private const int readBufferSize = 8192;

		//private Queue<CanPacket> cpq = new Queue<CanPacket>();
		//private System.Threading.Mutex queueMutex = new System.Threading.Mutex();
		private SerialPort serial_conn = null;
		private byte[] iBuff = new byte[8192];
		private int iBuffPointer = 0;

		public SerialConnection() {  }
		~SerialConnection() { serial_conn.Close();  }
		public bool open() 
		{ 
			if (serial_conn==null) return false;
			if (!serial_conn.IsOpen) serial_conn.Open();
			return true; 
		}
		public bool close() 
		{
			if (serial_conn == null) return false;
			if (serial_conn.IsOpen) serial_conn.Close();
			return true; 
		}
		public bool isOpen() { return serial_conn.IsOpen; }
		public void setConfiguration(int baudRate, Parity parity, string portName, StopBits stopbits, int databits, bool rtsEnable) 
		{
			try
			{
				serial_conn = new SerialPort();
				serial_conn.ReadBufferSize = readBufferSize;
				serial_conn.BaudRate = baudRate;
				serial_conn.Parity = parity;
				serial_conn.PortName = portName;
				serial_conn.StopBits = stopbits;
				serial_conn.DataBits = databits;
				serial_conn.RtsEnable = rtsEnable;
				//this.serial_conn.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serial_conn_DataReceived);
			}
			catch (Exception e)
			{
				throw e;
			}
		}
	   
		public bool writePacket(CanPacket cp) 
		{
			if (serial_conn.IsOpen)
			{
				byte[] bytes = new byte[PACKET_LENGTH];
				Array.Copy(cp.getBytes(), 0, bytes, 1, PACKET_LENGTH - 2);
				bytes[0] = UART_START_BYTE;
				bytes[PACKET_LENGTH - 1] = UART_END_BYTE;
				serial_conn.Write(bytes, 0, PACKET_LENGTH);
				return true;
			}
			return false;
		}

		public bool getPacket(out CanPacket cp) {
			cp = null;
			//bool dummy = false;
			if (serial_conn==null) return false;
		   	int bytesToRead = serial_conn.BytesToRead;
			if (bytesToRead > 0) {
				//Console.WriteLine("getPacket, data exist in buffer: " + bytesToRead + "bytes");
				//dummy = true;
				byte[] data = new byte[bytesToRead];
				
				try {
					serial_conn.Read(data, 0, data.Length);
				} catch (Exception e) {
					Console.WriteLine("Exception " + e + " occured.\n");
				}
					
				Array.Copy(data, 0, iBuff, iBuffPointer, data.Length);
				iBuffPointer += bytesToRead;
			}

			// N�r array st�rre eller lika med PACKET_LENGTH...
			while (iBuffPointer >= PACKET_LENGTH) {
				int startIndex = 0;
				
				// S�k igenom efter start byte fr�n b�rjan.
				for (int i = 0; i < iBuffPointer; i++) {
					// Poppa alla bytes som inte �r startbyte.
					if (iBuff[i] != UART_START_BYTE) startIndex++;
					else {
						// N�r startbyte hittas, kolla om �terst�ende l�ngd �r st�rre eller lika med PACKET_LENGTH (inkl startbyte)
						if ((iBuffPointer - startIndex) >= PACKET_LENGTH) {
							//om s�, kolla PACKET_LENGTH-1 byte fram.
							if (iBuff[startIndex + PACKET_LENGTH - 1] == UART_END_BYTE) {
								// Om byte PACKET_LENGTH-1 �r slutbyte s� extraktas startIndex till slutbyteindex.
								CanPacket cm = new CanPacket(iBuff, (uint)startIndex + 1);
								// S�tt ny startindex och avsluta loop.
								startIndex += PACKET_LENGTH;
								cp = cm;
								// och i slutet g�ra en array copy.
								// Flytta ner allt efter slutbyte till index 0 i array.
								Array.Copy(iBuff, startIndex, iBuff, 0, iBuffPointer - PACKET_LENGTH);
								iBuffPointer -= startIndex;
								
								return true;
							}
						}
					}
				}
				// och i slutet g�ra en array copy.
				// Flytta ner allt efter slutbyte till index 0 i array.
				Array.Copy(iBuff, startIndex, iBuff, 0, iBuffPointer - PACKET_LENGTH);
				iBuffPointer -= startIndex;
			}
			//if (dummy) {
			//	 Console.WriteLine("returned false even though we had data");
			//}
			return false;
		}

//		private void serial_conn_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
//		{
//			System.IO.Ports.SerialPort port = (System.IO.Ports.SerialPort)sender;
//			byte[] data = new byte[port.BytesToRead];

//			int bytesToRead = port.BytesToRead;

			// Fyll p� array hela tiden i slutet.
//			port.Read(data, 0, data.Length);
//			Array.Copy(data, 0, iBuff, iBuffPointer, data.Length);
//			iBuffPointer += bytesToRead;


			// N�r array st�rre eller lika med PACKET_LENGTH...
//			while (iBuffPointer >= PACKET_LENGTH)
//			{
//				int startIndex = 0;

				// S�k igenom efter start byte fr�n b�rjan.
//				for (int i = 0; i < iBuffPointer; i++)
//				{
					// Poppa alla bytes som inte �r startbyte.
//					if (iBuff[i] != UART_START_BYTE) startIndex++;
//					else
//					{
						// N�r startbyte hittas, kolla om �terst�ende l�ngd �r st�rre eller lika med PACKET_LENGTH (inkl startbyte)
//						if ((iBuffPointer - startIndex) >= PACKET_LENGTH)
//						{
							//om s�, kolla PACKET_LENGTH-1 byte fram.
//							if (iBuff[startIndex + PACKET_LENGTH - 1] == UART_END_BYTE)
//							{
								// Om byte PACKET_LENGTH-1 �r slutbyte s� extraktas startIndex till slutbyteindex.
//								CanPacket cm = new CanPacket(iBuff, (uint)startIndex + 1);
//								queueMutex.WaitOne();
//								cpq.Enqueue(cm);
//								queueMutex.ReleaseMutex();
								// S�tt ny startindex och avsluta loop.
//								startIndex += PACKET_LENGTH;
//								break;
//							}
//						}
//					}
//				}
				// och i slutet g�ra en array copy.
				// Flytta ner allt efter slutbyte till index 0 i array.
//				Array.Copy(iBuff, startIndex, iBuff, 0, iBuffPointer - PACKET_LENGTH);
//				iBuffPointer -= startIndex;
//			}


//		}

	}
}
