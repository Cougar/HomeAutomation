using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace canBootloader {
	public class Downloader {
		private HexFile hf;
		private SerialConnection sc;
		private Thread down;
		private byte receiveID;

		private enum dState { SEND_START, WAIT_ACK_PRG, SEND_PGM_DATA, WAIT_ACK_DATA, RESEND_ADDR, WAIT_DONE, SEND_DONE, DONE, DEBUG_STATE1, DEBUG_STATE2 };
		
		private const byte CAN_NMT				= 0x00;
		
		private const byte CAN_NMT_BIOS_START 	= 0x01;
		private const byte CAN_NMT_PGM_START 	= 0x02;
		private const byte CAN_NMT_PGM_DATA 	= 0x03;
		private const byte CAN_NMT_PGM_END 		= 0x04;
		private const byte CAN_NMT_PGM_ACK 		= 0x05;
		private const byte CAN_NMT_PGM_NACK 	= 0x06;
		private const byte CAN_NMT_START_APP 	= 0x07;
		private const byte CAN_NMT_APP_START 	= 0x08;
		private const byte CAN_NMT_HEARTBEAT 	= 0x09;
		
		private const byte MY_ID = 0x00;
		//private const byte TARGET_ID = 0x78;
		private const byte MY_NID = 0x0;
		private const byte ADDR0_INDEX = 0;
		private const byte ADDR1_INDEX = 1;
		private const byte ADDR2_INDEX = 2;
		private const byte ADDR3_INDEX = 3;
		//private const byte RID_INDEX = 4;
		//private const byte ERR_INDEX = 4;
		//private const byte ERR_NO_ERROR = 0x00; //= inget fel
		//private const byte ERR_ERROR = 0x01; //= fel

		private const int TIMEOUT_MS = 2000;

		private long timeStart = 0;


		public Downloader(HexFile hf, SerialConnection sc, byte receiveID) {
			this.hf = hf;
			this.sc = sc;
			this.receiveID = receiveID;
		}

		public bool go() {
			if (sc==null || hf==null || hf.getLength()==0 || !sc.isOpen()) return false;
			down = new Thread(new ThreadStart(downloader));
			down.Start();
			return true;
		}

		public void abort() {
			if (down != null && down.IsAlive) down.Abort();
		}

		public void downloader() {
			dState pgs = dState.SEND_START;
			int t = 0;
			CanPacket outCm = null;
			CanPacket cm = null;
			byte[] data = new byte[8];
			uint byteSent = 0;
			ulong currentAddress = 0;


			try {
				while (true) {
					bool hasMessage = sc.getPacket(out cm);
					if (hasMessage) {
						Array.Copy(cm.getData(), data, 8);
					}
					
				   
					switch (pgs) {
						case dState.SEND_START:
							// Send boot start packet to target.
							// and wait for ack.
							currentAddress = hf.getAddrLower();
							//data[RID_INDEX] = TARGET_ID;
							data[ADDR0_INDEX] = (byte)(currentAddress & 0x0000FF);
							data[ADDR1_INDEX] = (byte)((currentAddress & 0x00FF00) >> 8);
							data[ADDR2_INDEX] = (byte)((currentAddress & 0xFF0000) >> 16);
							data[ADDR3_INDEX] = 0;
							outCm = new CanPacket(CAN_NMT, CAN_NMT_PGM_START, MY_NID, MY_ID, receiveID, 4, data);
							sc.writePacket(outCm);
							t = Environment.TickCount;
							pgs = dState.WAIT_ACK_PRG;
							timeStart = Environment.TickCount;
							break;


						case dState.WAIT_ACK_PRG:
							// Check for timeout, resend start packet in that case..
							if ((Environment.TickCount - t) > TIMEOUT_MS) {
								Console.WriteLine("Timeout while waiting for start prg ack.");
								pgs = dState.DONE;
							}

							// If received ack.
							if (hasMessage && cm.getPktClass() == CAN_NMT && cm.getNid() == MY_NID && cm.getSid() == receiveID) {
								// If no error
								if (cm.getType() == CAN_NMT_PGM_ACK) {
									// Start sending program data..
									byteSent = 0;
									pgs = dState.SEND_PGM_DATA;
								}
								else if (cm.getType() == CAN_NMT_PGM_NACK ) { 
									// else ..
									Console.WriteLine("Nack on CAN_NMT_PGM_START.");
									pgs = dState.DONE;
								}
							}

							break;

						case dState.SEND_PGM_DATA:
							//Console.WriteLine("byteSent = "+byteSent);
							Console.Write(".");
							// Send program data.
							for (ulong i = 0; i < 6; i++) { 
								// Populate data.
								data[i+2] = hf.getByte(currentAddress + i + byteSent);
							}
							data[0] = (byte)(byteSent & 0x00FF);
							data[1] = (byte)((byteSent & 0xFF00) >> 8);
							outCm = new CanPacket(CAN_NMT, CAN_NMT_PGM_DATA, MY_NID, MY_ID, receiveID, 8, data);
							sc.writePacket(outCm);
							t = Environment.TickCount;
							byteSent += 6;
							pgs = dState.WAIT_ACK_DATA;
							break;


						case dState.WAIT_ACK_DATA:
							// Wait for pgm ack.
							if ((Environment.TickCount - t) > 2*TIMEOUT_MS) {
								// Woops, error. 
								Console.WriteLine("Timeout while waiting for data ack.");
								pgs = dState.DONE;
							}

							// If received ack.
							if (hasMessage && cm.getPktClass() == CAN_NMT && cm.getNid() == MY_NID && cm.getSid() == receiveID) {
								// If no error
								if (cm.getType() == CAN_NMT_PGM_ACK) {
									//currentAddress += 2;
									// Check if end.
									if (currentAddress + byteSent > hf.getAddrUpper()) {
										// Yes, we are done, send done.
										pgs = dState.SEND_DONE;
									}
									else { 
										// More to write.
										//byteSent = 0;
										pgs = dState.SEND_PGM_DATA;
									}
								}
								else if (cm.getType() == CAN_NMT_PGM_NACK) {
									// else ..
									Console.WriteLine("Nack on CAN_NMT_PGM_DATA.");
									pgs = dState.DONE;
								}
							}

							break;


						case dState.SEND_DONE:
							// Send done with crc
							data[0] = 0;
							data[1] = 0;
							outCm = new CanPacket(CAN_NMT, CAN_NMT_PGM_END, MY_NID, MY_ID, receiveID, 2, data);
							sc.writePacket(outCm);
							t = Environment.TickCount;
							pgs = dState.WAIT_DONE;
							break;
							
							
						case dState.WAIT_DONE:
							// Check for timeout, resend done packet in that case..
							if ((Environment.TickCount - t) > TIMEOUT_MS) {
								Console.WriteLine("Timeout while waiting for end prg ack.");
								pgs = dState.DONE;
							}

							// If received ack.
							if (hasMessage && cm.getPktClass() == CAN_NMT && cm.getNid() == MY_NID && cm.getSid() == receiveID) {
								// If no error
								if (cm.getType() == CAN_NMT_PGM_ACK) {
									Console.WriteLine("Done, successfully programmed application.");
									// Start sending program data..
									byteSent = 0;
									pgs = dState.DONE;
								}
								else if (cm.getType() == CAN_NMT_PGM_NACK) {
									// else resend addr..
									Console.WriteLine("Nack on CAN_NMT_PGM_END.");
									pgs = dState.DONE;
								}
							}

							break;

						case dState.DONE:
							down.Abort();
							break;
					}
				}

			} catch(Exception e) {
				Console.WriteLine("Aborted or done.");
				//Console.WriteLine(e);

				long tsecs = ((Environment.TickCount - timeStart) / 1000);

				double bitRate = hf.getLength() / ((double)(Environment.TickCount - timeStart) / 1000.0);

				if (tsecs <= 0) tsecs = 1;

				long mins = tsecs / 60;
				long secs = tsecs % 60;

				Console.WriteLine("Time spent: " + mins + ":" + secs + ", " + Math.Round(bitRate, 2) + " Bps");
				Console.Write("> ");
			}
		}
	}
}
