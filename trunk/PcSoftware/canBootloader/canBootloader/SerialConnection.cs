using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using System.Net;
using System.Net.Sockets;

namespace canBootloader {
	public class SerialConnection {
		public const byte UART_START_BYTE = 253;
		public const byte UART_END_BYTE = 250;
		public const int PACKET_LENGTH = 17;
		private const int readBufferSize = 8192;
		
		private Socket udpserver = null;
		private IPEndPoint iep = null;
		private string remoteIP = null;
		private int remotePort = 1100;
		
		private SerialPort serial_conn = null;
		private byte[] iBuff = new byte[8192];
		private int iBuffPointer = 0;

		public SerialConnection() {  }
		
		public bool open() { 
			if (serial_conn != null) {
				if (!serial_conn.IsOpen) serial_conn.Open();
				return true; 
			} else if (udpserver != null) {
				return true;
			} else {
				return false;
			}
		}
		
		public bool close() {
			if (serial_conn != null) {
				if (serial_conn.IsOpen) serial_conn.Close();
				return true; 
			} else if (udpserver != null) {
				udpserver.Close();
				return true;
			} else {
				return false;
			}
				
		}
		
		public bool isOpen() { 
			if (serial_conn != null) {
				return serial_conn.IsOpen; 
			} else if (udpserver != null) {
				return true;
			} else {
				return false;
			}
		}
		
		public void setConfiguration(int baudRate, Parity parity, string portName, StopBits stopbits, int databits, bool rtsEnable) {
			if (!portName.Contains("/udp")) {
				try {
					serial_conn = new SerialPort();
					serial_conn.ReadBufferSize = readBufferSize;
					serial_conn.BaudRate = baudRate;
					serial_conn.Parity = parity;
					serial_conn.PortName = portName;
					serial_conn.StopBits = stopbits;
					serial_conn.DataBits = databits;
					serial_conn.RtsEnable = rtsEnable;
				}
				catch (Exception e) {
					throw e;
				}
			} else {
				try {
					string[] splitted = portName.Split('/');
					if (splitted.Length == 4) {
						if (splitted[0] == "" && splitted[1] == "udp") {
							remoteIP = splitted[2];
							remotePort = Int32.Parse(splitted[3]);
							iep = new IPEndPoint(IPAddress.Any, remotePort);
							udpserver = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
							udpserver.Bind(iep);
						}
					}
					
				} catch (Exception e) {
					throw e;
				}
			}
		}
	   
		public bool writePacket(CanPacket cp) {
			if (serial_conn != null) {
				if (serial_conn.IsOpen) {
					byte[] bytes = new byte[PACKET_LENGTH];
					Array.Copy(cp.getBytes(), 0, bytes, 1, PACKET_LENGTH - 2);
					bytes[0] = UART_START_BYTE;
					bytes[PACKET_LENGTH - 1] = UART_END_BYTE;
					serial_conn.Write(bytes, 0, PACKET_LENGTH);
					return true;
				}
				return false;
			} else if (udpserver != null) {
				byte[] bytes = new byte[PACKET_LENGTH];
				Array.Copy(cp.getBytes(), 0, bytes, 1, PACKET_LENGTH - 2);
				bytes[0] = UART_START_BYTE;
				bytes[PACKET_LENGTH - 1] = UART_END_BYTE;
				
				udpserver.SendTo(bytes, bytes.Length, SocketFlags.None, new IPEndPoint(IPAddress.Parse(remoteIP), remotePort));
				
				return true;
			} else {
				return false;
			}
		}

		public bool getPacket(out CanPacket cp) {
			cp = null;
			if ((serial_conn != null) || (udpserver != null)) {
				
				if (serial_conn != null) {
				   	int bytesToRead = serial_conn.BytesToRead;
					if (bytesToRead > 0) {
						byte[] data = new byte[bytesToRead];
						
						try {
							serial_conn.Read(data, 0, data.Length);
						} catch (Exception e) {
							Console.WriteLine("Exception " + e + " occured.\n");
						}
							
						Array.Copy(data, 0, iBuff, iBuffPointer, data.Length);
						iBuffPointer += bytesToRead;
					}
				} else if (udpserver != null) {
				   	int bytesToRead = udpserver.Available;
					if (udpserver.Available > 0) {
						byte[] data = new byte[bytesToRead];
						try {
							EndPoint tmpRemote = (EndPoint)iep;
							udpserver.ReceiveFrom(data, ref tmpRemote);
						} catch (Exception e) {
							Console.WriteLine("Exception " + e + " occured.\n");
						}
						
						int bytestomove = data.Length;
						if (bytestomove ==18) {
							bytestomove = data.Length -1;
						}
						Array.Copy(data, 0, iBuff, iBuffPointer, bytestomove);
						iBuffPointer += bytestomove;
					}
					
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
				return false;
				
			} else {
				return false;
			}
		}

	}
}
