using System.Threading;
using System;

public class CanNMT {
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

	private bool HWID_INUSE=false;

	CanPacket cp;
	private bool running = false;
	
	public CanNMT(bool HWID_INUSE) {
		cp = new CanPacket();
		cp.setExt(1);
		this.HWID_INUSE = HWID_INUSE;
	}

	public CanNMT(CanPacket cp, bool HWID_INUSE) {
		this.cp = cp;
		this.cp.setExt(1);
		this.HWID_INUSE = HWID_INUSE;
	}
	
	public byte getSender() {
		return (byte)((cp.getId()>>8)&0xFF);
	}
	
	public bool isNMTPacket() {
		if (((cp.getId()>>24)&0xFF) == 0) {
			return true;
		}
		return false;
	}

	public byte getType() {
		return (byte)((cp.getId()>>16)&0xFF);
	}
	
	public byte getReceiver() {
		return (byte)((cp.getId()>>0)&0xFF);
	}
	
	public void setSender(byte id) {
		cp.setId( ( ( cp.getId()&0x00FF00FF )|((uint)id<<8) ) ); 
	}
	public void setReceiver(byte id) {
		cp.setId( ( ( cp.getId()&0x00FFFF00 )|((uint)id) ) ); 
	}
	
	public CanPacket getResetPacket() {
		cp.setId( ( cp.getId()&0x0000FFFF )|( CAN_NMT_RESET<<16 ) ); 
		if (HWID_INUSE) {
		   cp.setDataLength(4);
		} else {
		   cp.setDataLength(0);
		}
		return cp;
	}

	public CanPacket getStartPacket() {
		cp.setId( ( cp.getId()&0x0000FFFF )|( CAN_NMT_START_APP<<16 ) ); 
		if (HWID_INUSE) {
		   cp.setDataLength(4);
		} else {
		   cp.setDataLength(0);
		}
		return cp;
	}

	public CanPacket getPgmStartPacket(byte[] data) {
		cp.setId( ( cp.getId()&0x0000FFFF )|( CAN_NMT_PGM_START<<16 ) ); 
		cp.setData(data);
		if (HWID_INUSE) {
		   cp.setDataLength(8);
		} else {
		   cp.setDataLength(4);
		}
		return cp;
	}

	public CanPacket getPgmDataPacket(byte[] data, byte datalen) {
		cp.setId( ( cp.getId()&0x0000FFFF )|( CAN_NMT_PGM_DATA<<16 ) ); 
		cp.setData(data);
		cp.setDataLength(datalen);
		return cp;
	}

	public CanPacket getPgmEndPacket() {
		cp.setId( ( cp.getId()&0x0000FFFF )|( CAN_NMT_PGM_END<<16 ) ); 
		cp.setDataLength(0);
		return cp;
	}

	public CanPacket getPgmCopyPacket(byte[] data) {
		cp.setId( ( cp.getId()&0x0000FFFF )|( CAN_NMT_PGM_COPY<<16 ) ); 
		cp.setData(data);
		cp.setDataLength(6);
		return cp;
	}
	
	public bool isBiosStart(CanPacket cp, byte sender, uint hwid) {
		bool returnval = false;
		uint id = cp.getId();
		if (HWID_INUSE) {
			byte[] data = cp.getData();
			if ((id&0x00FF0000) == (CAN_NMT_BIOS_START<<16) && data[4]==((hwid)&0xff) && data[5]==((hwid>>8)&0xff) && data[6]==((hwid>>16)&0xff) && data[7]==((hwid>>24)&0xff)) {
				returnval = true;
				string appinfo = "Application installed";
				if (data[2] == 0) {
					appinfo = "No application installed";
				}
				if (data[3]!=0 && data[3]!=0xff) {
				   appinfo += ", application can-id "+data[3];
				}
				Console.WriteLine("Bios started on node 0x" + String.Format("{0:x2}", sender) + 
									", bios version 0x" + String.Format("{0:x2}", data[1]) + String.Format("{0:x2}", data[0]) +
									", " + appinfo
									);
			}
		} else {
			if ((id&0x00FF0000) == (CAN_NMT_BIOS_START<<16) && (id&0x0000FF00) == (sender<<8)) {
				returnval = true;
				byte[] data = cp.getData();
				string appinfo = "Application installed";
				if (data[2] == 0) {
					appinfo = "No application installed";
				}
				Console.WriteLine("Bios started on node 0x" + String.Format("{0:x2}", sender) + 
									", bios version 0x" + String.Format("{0:x2}", data[1]) + String.Format("{0:x2}", data[0]) +
									", " + appinfo
									);
			}
		}
		return returnval;
	}

	public bool doStart(DaemonConnection dc, byte receiver, uint hwid) {
		return doStart(dc, receiver, 0, hwid);
	}
	
	public bool doStart(DaemonConnection dc, byte receiver, byte sender, uint hwid) {
		bool success = true;

		  if (HWID_INUSE) {
		   byte[] data = new byte[8];
		   data[0] = (byte)(hwid&0xff); data[1] = (byte)((hwid>>8)&0xff); data[2] = (byte)((hwid>>16)&0xff); data[3] = (byte)((hwid>>24)&0xff);  
		   cp.setData(data);
		   cp.setDataLength(4);
		  }		
		setReceiver(receiver);
		dc.sendCanPacket(getStartPacket());
		return success;
	}

	public bool doReset(DaemonConnection dc, byte receiver) {
		return doReset(dc, receiver, 0);
	}

	public bool doReset(DaemonConnection dc, byte receiver, uint hwid) {
		return doReset(dc, receiver, 0, hwid);
	}

	public bool doReset(DaemonConnection dc, byte receiver, byte sender, uint hwid) {
		bool success = false;

      if (HWID_INUSE) {
	   byte[] data = new byte[8];
	   data[0] = (byte)(hwid&0xff); data[1] = (byte)((hwid>>8)&0xff); data[2] = (byte)((hwid>>16)&0xff); data[3] = (byte)((hwid>>24)&0xff);  
	   cp.setData(data);
	   cp.setDataLength(4);
      } else {
		setReceiver(receiver);
      }      
		
		dc.sendCanPacket(getResetPacket());
		CanPacket getcp;
		running = true;
		dc.flushData();
		for (int i = 0; i < 150; i++) {
			Thread.Sleep(50);
			if (dc.getData(out getcp)) {
				if (isBiosStart(getcp, receiver, hwid)) {
					success = true;
					break;
				}
			}
			
			if (!running) {
				break;
			}
		}
		if (!success) {
			Console.WriteLine("Timed out while waiting for node to reset");
		}
		return success;
	}
	
	public void stop() {
		running = false;
	}
	
}
