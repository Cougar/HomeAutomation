using System;
using System.Collections.Generic;
using System.Text;

namespace canBootloader {
	public class CanPacket {
		private byte pktclass = 0;
		private byte type = 0;
		private byte sid = 0;
		private byte rid = 0;
		private byte data_length = 0;
		private byte[] data = new byte[8];

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


			ulong addr = (((ulong)raw[startIndex + 3]) << 24) + (((ulong)raw[startIndex + 2]) << 16) + (((ulong)raw[startIndex + 1]) << 8) + (((ulong)raw[startIndex + 0]));

			this.pktclass = (byte)((addr & 0x1E000000) >> 25);
			this.type =	 (byte)((addr & 0x00FF0000) >> 16);
			this.sid =	  (byte)((addr & 0x0000FF00) >> 8);
			this.rid =	  (byte)((addr & 0x000000FF));
			this.data_length = raw[startIndex + 6];

			for (int i = 0; i < 8; i++) this.data[i] = raw[startIndex + 7 + i];
		}

		public CanPacket(byte pktclass, byte type, byte sid, byte rid, byte data_length, byte[] data) { 
			this.pktclass=pktclass;
			this.type=type;
			this.sid=sid;
			this.rid=rid;
			this.data_length=data_length;
			for(int i=0;i<8;i++) this.data[i]=data[i];
		}

		public override int GetHashCode() {
			return base.GetHashCode();
		}

		public override bool  Equals(Object obj) { 
			CanPacket cpm = (CanPacket)obj;

			byte[] bytes = cpm.getData();

			for(int i=0;i<8;i++) if (this.data[i]!=bytes[i]) return false;

			return (this.pktclass==cpm.getPktClass() && this.type==cpm.getType() && this.sid==cpm.getSid() && this.sid==cpm.getRid() && this.data_length==cpm.getDataLength());

		}

		public byte getPktClass(){ return this.pktclass; }
		public uint getType(){ return this.type; }
		public uint getSid(){ return this.sid; }
		public uint getRid(){ return this.sid; }
		public byte getDataLength(){ return this.data_length; }
		public byte[] getData(){ return this.data; }

		public override string ToString() {
			string str = "";

			str = "pktclass: "+pktclass.ToString()+", type: "+type.ToString()+", sid: "+sid.ToString()+", rid: "+rid.ToString()+",data_length: "+data_length.ToString()+", data: ";
			for (int i = 0; i < 8; i++) str+=data[i].ToString()+" ";

			return str;
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

			ulong addr = (((ulong)this.pktclass)<<25)+(((ulong)this.type)<<16)+(((ulong)this.sid)<<8)+((ulong)this.rid);

			bytes[3] = (byte)((addr & 0xFF000000) >> 24);
			bytes[2] = (byte)((addr & 0x00FF0000) >> 16);
			bytes[1] = (byte)((addr & 0x0000FF00) >> 8);
			bytes[0] = (byte)((addr & 0x000000FF));
			bytes[4]=1;
			bytes[5]=0;
			bytes[6]=this.data_length;
			for(int i=0;i<8;i++) bytes[i+7]=this.data[i];

			return bytes;
		}


	}
}
