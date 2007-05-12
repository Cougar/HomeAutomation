using System;
using System.Collections.Generic;
using System.Text;

public class CanPacket {
	private byte pktclass = 0;
//	private byte type = 0;
//	private byte sid = 0;
//	private byte rid = 0;
	private byte data_length = 0;
	private uint id = 0;
	private byte[] data = new byte[8];
	private byte ext = 0;
	private byte rtr = 0;
	
	public CanPacket(byte[] raw,uint startIndex) { 
		// 17 bytes, a packet.
		// UART_START_BYTE id[0] id[1] id[2] id[3] extended remote_request data_length d[0] d[1] d[2] d[3] d[4] d[5] d[6] d[7] UART_END_BYTE
		/*
		* 000CCCCx TTTTTTTT SSSSSSSS RRRRRRRR
		*
		* <CLASS> = 00011110 00000000 00000000 00000000 = 0x1E000000	NMT, ...
		* <TYPE>  = 00000000 11111111 00000000 00000000 = 0x00FF0000	NMT[CAN_NMT_PGM_START, CAN_ID_NMT_PGM_ACK, ...]
		* <SID>   = 00000000 00000000 11111111 00000000 = 0x0000FF00	Sender ID
		* <RID>   = 00000000 00000000 00000000 11111111 = 0x000000FF	Receiver ID
		* 
		*/
		
		
		uint addr = (((uint)raw[startIndex + 3]) << 24) + (((uint)raw[startIndex + 2]) << 16) + (((uint)raw[startIndex + 1]) << 8) + (((uint)raw[startIndex + 0]));
		
		this.id = addr;
		this.pktclass = (byte)((addr & 0x1E000000) >> 25);
//		this.type =	 (byte)((addr & 0x00FF0000) >> 16);
//		this.sid =	  (byte)((addr & 0x0000FF00) >> 8);
//		this.rid =	  (byte)((addr & 0x000000FF));
		this.data_length = raw[startIndex + 6];
		
		ext = (byte)(0x0f & raw[startIndex + 4]);
		rtr = (byte)(0x0f & raw[startIndex + 5]);
		
		for (int i = 0; i < 8; i++) this.data[i] = raw[startIndex + 7 + i];
	}
	
	public CanPacket(string raw) {
//	}
//	public void setData(string raw) {
		//1e00007f 1 0 04 03 02 01 96 3f 76 15
		string [] split = null;
		split = raw.Split( new Char [] {' '} );
		if ((split.Length >= 3) && (split.Length <= 11)) {
			try {
				id = System.Convert.ToUInt32(split[0].Trim(), 16);

				ext = (byte)(System.Convert.ToUInt32(split[1].Trim(), 16)&0xFF);
				rtr = (byte)(System.Convert.ToUInt32(split[2].Trim(), 16)&0xFF);
				
				//testa ext och rtr!
				
				for (int i = 0; i < split.Length-3; i++) {
					string dummy = split[i+3].Trim();
					if (dummy.Length == 0) break;
					data_length = (byte)((i+1)&0xFF);
					//Console.WriteLine("i: " + i + " i+3: " + (i+3) + " split[i+3]: " +split[i+3]);
					data[i] = (byte)(System.Convert.ToUInt32(dummy, 16)&0xFF);
				}
			}
			catch {
				Console.WriteLine("overflow");
			}
		}
		
	}
	
//	public CanPacket(byte pktclass, byte type, byte sid, byte rid, byte data_length, byte[] data, byte rtr, byte ext) { 
//		this.pktclass=pktclass;
//		this.type=type;
//		this.sid=sid;
//		this.rid=rid;
//		this.data_length=data_length;
//		for(int i=0;i<8;i++) this.data[i]=data[i];
//		this.ext = (byte)(0x0f & ext);
//		this.rtr = (byte)(0x0f & rtr);
//	}
	
	public override int GetHashCode() {
		return base.GetHashCode();
	}
	
	public override bool  Equals(Object obj) { 
		CanPacket cpm = (CanPacket)obj;
		
		byte[] bytes = cpm.getData();
		
		for(int i=0;i<8;i++) if (this.data[i]!=bytes[i]) return false;
		
		return (this.id==cpm.getId() && this.data_length==cpm.getDataLength());
		
	}
	
	public uint getId() { return this.id; }
	public byte getPktClass(){ return this.pktclass; }
//	public uint getType(){ return this.type; }
//	public uint getSid(){ return this.sid; }
//	public uint getRid(){ return this.sid; }
	public byte getDataLength(){ return this.data_length; }
	public byte[] getData(){ return this.data; }
	
//	public override string ToString() {
//		string str = "";
		
//		str = "pktclass: "+pktclass.ToString()+", type: "+type.ToString()+", sid: "+sid.ToString()+", rid: "+rid.ToString()+",data_length: "+data_length.ToString()+", data: ";
//		for (int i = 0; i < 8; i++) str+=data[i].ToString()+" ";
		
//		return str;
//	}
	
	public string toRawString() {

		string returnstring = "PKT " + String.Format("{0:x8}", id);
		returnstring = returnstring + " " + String.Format("{0:x1}", ext) + " " + String.Format("{0:x1}", rtr);
		for (int i = 0; i < data_length; i++) {
			returnstring = returnstring + " " + String.Format("{0:x2}", data[i]);
		}
		//returnstring = returnstring + "\n";
		return returnstring;
	}
	
	public byte[] getBytes() {
		byte[] bytes = new byte[15];
		
		// 17 bytes, a packet.
		// UART_START_BYTE id[0] id[1] id[2] id[3] extended remote_request data_length d[0] d[1] d[2] d[3] d[4] d[5] d[6] d[7] UART_END_BYTE
		/*
		* 000CCCCx TTTTTTTT SSSSSSSS RRRRRRRR
		*
		* <CLASS> = 00011110 00000000 00000000 00000000 = 0x1E000000	NMT, ...
		* <TYPE>  = 00000000 11111111 00000000 00000000 = 0x00FF0000	NMT[CAN_NMT_PGM_START, CAN_ID_NMT_PGM_ACK, ...]
		* <SID>   = 00000000 00000000 11111111 00000000 = 0x0000FF00	Sender ID
		* <RID>   = 00000000 00000000 00000000 11111111 = 0x000000FF	Receiver ID
		* 
		*/
		
		
		bytes[3] = (byte)((id & 0xFF000000) >> 24);
		bytes[2] = (byte)((id & 0x00FF0000) >> 16);
		bytes[1] = (byte)((id & 0x0000FF00) >> 8);
		bytes[0] = (byte)((id & 0x000000FF));
		bytes[4]=ext;
		bytes[5]=rtr;
		bytes[6]=this.data_length;
		for(int i=0;i<8;i++) bytes[i+7]=this.data[i];
		
		return bytes;
	}
	
}
