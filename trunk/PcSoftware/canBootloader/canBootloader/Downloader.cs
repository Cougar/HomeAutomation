using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace canBootloader {
	public class Downloader {
		private HexFile hf;
		private SerialConnection sc;
		private bool isBiosUpdate;
		private ulong dlOffset = 0;
		//private Thread down;
		private byte receiveID;

		private enum dState { SEND_START, WAIT_ACK_PRG, SEND_PGM_DATA, WAIT_ACK_DATA, RESEND_ADDR, WAIT_DONE, SEND_DONE, SEND_BIOS_UPDATE, SEND_RESET, DONE, DEBUG_STATE2 };
		
		
		
		private const byte CAN_NMT				= 0x00;
		
		private const byte CAN_NMT_RESET 		= 0x04;
		private const byte CAN_NMT_BIOS_START 	= 0x08;
		private const byte CAN_NMT_PGM_START 	= 0x0C;
		private const byte CAN_NMT_PGM_DATA 	= 0x10;
		private const byte CAN_NMT_PGM_END 		= 0x14;
		private const byte CAN_NMT_PGM_COPY		= 0x18;
		private const byte CAN_NMT_PGM_ACK 		= 0x1C;
		private const byte CAN_NMT_PGM_NACK 	= 0x20;
		private const byte CAN_NMT_START_APP 	= 0x24;
		private const byte CAN_NMT_APP_START 	= 0x28;
		private const byte CAN_NMT_HEARTBEAT 	= 0x2C;
		
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
		
		public Downloader(HexFile hf, SerialConnection sc, byte receiveID, bool isBiosUpdate) {
			this.hf = hf;
			this.sc = sc;
			this.receiveID = receiveID;
			this.isBiosUpdate = isBiosUpdate;
		}

		public int calcCRC(HexFile hf) {
			int crc=0;
			for (ulong i=hf.getAddrLower(); i <= hf.getAddrUpper(); i++) {
				crc = crc16_update(crc, hf.getByte(i));
			}
			return crc;
		}
		
		public int crc16_update(int crc, byte a) {
			int i;
			crc ^= a;
			for (i = 0; i < 8; ++i) {
				if ((crc & 1) != 0)
					crc = (crc >> 1) ^ 0xA001;
				else
					crc = (crc >> 1);
			}
			return crc;
		}

		public bool go() {
			if (sc==null || hf==null || hf.getLength()==0 || !sc.isOpen()) return false;
            downloader();
			return true;
		}

		public void abort() {
			//if (down != null && down.IsAlive) down.Abort();
		}

		public void downloader() {
			dState pgs = dState.SEND_START;
			int t = 0;
			CanPacket outCm = null;
			CanPacket cm = null;
			byte[] data = new byte[8];
			uint byteSent = 0;
			ulong currentAddress = 0;
            bool done = false;
            bool errorOccured = false;
            
            try {
            	if (isBiosUpdate) {
            		dlOffset = hf.getAddrLower();
            		//Console.WriteLine(dlOffset);
            		//dlOffset = 200;
            	}
				while (!done) {
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
							data[ADDR0_INDEX] = (byte)((currentAddress-dlOffset) & 0x0000FF);
							data[ADDR1_INDEX] = (byte)(((currentAddress-dlOffset) & 0x00FF00) >> 8);
							data[ADDR2_INDEX] = (byte)(((currentAddress-dlOffset) & 0xFF0000) >> 16);
							data[ADDR3_INDEX] = 0;
							outCm = new CanPacket(CAN_NMT, CAN_NMT_PGM_START, MY_ID, receiveID, 4, data);
							sc.writePacket(outCm);
							t = Environment.TickCount;
							pgs = dState.WAIT_ACK_PRG;
							timeStart = Environment.TickCount;
							break;


						case dState.WAIT_ACK_PRG:
							// Check for timeout, resend start packet in that case..
							if ((Environment.TickCount - t) > TIMEOUT_MS) {
								Console.WriteLine("Timeout while waiting for start prg ack.");
								errorOccured = true;
								pgs = dState.SEND_RESET;
							}
							Thread.Sleep(1);
							// If received ack.
							if (hasMessage && cm.getPktClass() == CAN_NMT && cm.getSid() == receiveID) {
								// If no error
								if (cm.getType() == CAN_NMT_PGM_ACK) {
									// Start sending program data..
									byteSent = 0;
									pgs = dState.SEND_PGM_DATA;
								}
								else if (cm.getType() == CAN_NMT_PGM_NACK ) { 
									// else ..
									Console.WriteLine("Nack on CAN_NMT_PGM_START.");
									errorOccured = true;
									pgs = dState.SEND_RESET;
								}
							}

							break;

						case dState.SEND_PGM_DATA:
							//Console.WriteLine("byteSent = "+byteSent);
							Console.Write(".");
							// Send program data.
							byte datalength = 2;
							for (ulong i = 0; i < 6; i++) { 
								// Populate data.
								data[i+2] = hf.getByte(currentAddress + i + byteSent);
								if (currentAddress + i + byteSent > hf.getAddrUpper()) {
									break;	//then done
								}
								datalength++;
							}
							data[0] = (byte)(byteSent & 0x00FF);
							data[1] = (byte)((byteSent & 0xFF00) >> 8);
							outCm = new CanPacket(CAN_NMT, CAN_NMT_PGM_DATA, MY_ID, receiveID, datalength, data);
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
								errorOccured = true;
								pgs = dState.SEND_RESET;
							}
							Thread.Sleep(1);
							// If received ack.
							if (hasMessage && cm.getPktClass() == CAN_NMT && cm.getSid() == receiveID) {
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
									errorOccured = true;
									pgs = dState.SEND_RESET;
								}
							}

							break;


						case dState.SEND_DONE:
							// Send done
							//data[0] = 0;
							//data[1] = 0;
							outCm = new CanPacket(CAN_NMT, CAN_NMT_PGM_END, MY_ID, receiveID, 0, data);
							sc.writePacket(outCm);
							t = Environment.TickCount;
							pgs = dState.WAIT_DONE;
							break;

							
						case dState.WAIT_DONE:
							// Check for timeout, resend done packet in that case..
							if ((Environment.TickCount - t) > TIMEOUT_MS) {
								Console.WriteLine("Timeout while waiting for end prg ack.");
								errorOccured = true;
								pgs = dState.SEND_RESET;
							}
							Thread.Sleep(1);
							// If received ack.
							if (hasMessage && cm.getPktClass() == CAN_NMT && cm.getSid() == receiveID) {
								// If no error
								if (cm.getType() == CAN_NMT_PGM_ACK) {
									Console.WriteLine("");
									//
									int crc = calcCRC(hf);
									
									if (cm.getDataLength() == 2) {
										byte[] cmdata = cm.getData();
										if ((cmdata[0] == (crc & 0xFF)) && (cmdata[1] == (crc>>8))) {
											if (!isBiosUpdate) {
												Console.WriteLine("Done, successfully programmed application.");
												pgs = dState.SEND_RESET;
											} else {
												Console.WriteLine("Done, successfully programmed bios to application space.");
												pgs = dState.SEND_BIOS_UPDATE;
											}
										} else {
											string nodecrc = String.Format("{0:x4}", (int)cmdata[0] + (int)(cmdata[1]<<8));
											nodecrc = "0x"+nodecrc;
											string realcrc = String.Format("{0:x4}", (int)(crc & 0xFF) + (int)(crc & 0xFF00));
											realcrc = "0x"+realcrc;
											Console.WriteLine("CRC failed. Node sent CRC "+nodecrc+" but should be "+realcrc+".");
											errorOccured = true;
											pgs = dState.SEND_RESET;
										}

										byteSent = 0;
									}
									//Console.WriteLine("crc: " + crc + " " + (crc>>8) + " " + (crc & 0xFF));
									//Console.WriteLine("ACKpkg with CRC: " + cm.ToString());
									// 
								}
								else if (cm.getType() == CAN_NMT_PGM_NACK) {
									// else resend addr..
									Console.WriteLine("Nack on CAN_NMT_PGM_END.");
									errorOccured = true;
									pgs = dState.SEND_RESET;
								}
							}
							break;

						case dState.SEND_BIOS_UPDATE:
							data[0] = (byte)(0 & 0x00FF);
							data[1] = (byte)((0 & 0xFF00) >> 8);
							data[2] = (byte)(dlOffset & 0x00FF);
							data[3] = (byte)((dlOffset & 0xFF00) >> 8);
							data[4] = (byte)((hf.getAddrUpper()-hf.getAddrLower()+1) & 0x00FF);
							data[5] = (byte)(((hf.getAddrUpper()-hf.getAddrLower()+1) & 0xFF00) >> 8);
							outCm = new CanPacket(CAN_NMT, CAN_NMT_PGM_COPY, MY_ID, receiveID, 6, data);
							Console.WriteLine("Now say a prayer, moving bios");
							sc.writePacket(outCm);
							t = Environment.TickCount;
							pgs = dState.DONE;
							break;

						case dState.SEND_RESET:
							// Send reset
							outCm = new CanPacket(CAN_NMT, CAN_NMT_RESET, MY_ID, receiveID, 0, data);
							sc.writePacket(outCm);
							t = Environment.TickCount;
							pgs = dState.DONE;
							break;
							

						case dState.DONE:
                            done = true;
                            //down.Abort();
                            if (!errorOccured) {
								long tsecs = ((Environment.TickCount - timeStart) / 1000);
								double bitRate = hf.getLength() / ((double)(Environment.TickCount - timeStart) / 1000.0);
								
								if (tsecs <= 0) tsecs = 1;
								long mins = tsecs / 60;
								long secs = tsecs % 60;
								string pad = "";
								if (secs < 10) {
									pad = "0";
								}
								Console.WriteLine("Time spent: " + mins + ":" + pad + secs + ", " + Math.Round(bitRate, 2) + " bytes/s");
							}
							//Console.Write("> ");
                            break;
					}
				}

			} catch(Exception) {
				Console.WriteLine("Aborted or done.");
				//Console.WriteLine(e);
				//skriva ut antal bytes skrivna vore bra här, kanske nån annan form av hjälp för debug, något gick snett liksom
				
				Console.Write("> ");
			}
		}
	}
}
