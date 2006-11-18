using System;
using System.Collections.Generic;
using System.Text;

namespace canWatcherLight
{
    public class CanPacket
    {
        private byte funct = 0;
        private uint funcc = 0;
        private byte nid = 0;
        private uint sid = 0;
        private byte data_length = 0;
        private byte[] data = new byte[8];

        public CanPacket(byte[] raw,uint startIndex)
        { 
	        // 17 bytes, a packet.
	        // UART_START_BYTE id[0] id[1] id[2] id[3] extended remote_request data_length d[0] d[1] d[2] d[3] d[4] d[5] d[6] d[7] UART_END_BYTE
            /*
	        * 000TTTTC CCCCCCCC CNNNNNNS SSSSSSSS
	        *
	        * <FUNCT> = 00011110 00000000 00000000 00000000 = 0x1E000000	max(0xF, 15)
	        * <FUNCC> = 00000001 11111111 10000000 00000000 = 0x01FF8000    max(0x3FF, 1023)
	        * <NID>   = 00000000 00000000 01111110 00000000 = 0x00007E00	max(0x3F, 63)
	        * <SID>   = 00000000 00000000 00000001 11111111 = 0x000001FF	max(0x1FF, 511)
	        * 
	        <FUNCT>, 4 bitar = Function Type, funktionstyp.
	        <FUNCC>, 10 bitar = Function Code, funktionskod.
	        <NID>, 6 bitar = Network ID, nätverksid.
	        <SID>, 9 bitar = Sender ID, sändarid.
	        */


            ulong addr = (((ulong)raw[startIndex + 3]) << 24) + (((ulong)raw[startIndex + 2]) << 16) + (((ulong)raw[startIndex + 1]) << 8) + (((ulong)raw[startIndex + 0]));

            this.funct = (byte)((addr & 0x1E000000)>>25);
            this.funcc = (uint)((addr & 0x01FF8000) >> 15);
            this.nid = (byte)((addr & 0x00007E00) >> 9);
            this.sid = (uint)((addr & 0x000001FF));
            this.data_length = raw[startIndex + 6];

            for (int i = 0; i < 8; i++) this.data[i] = raw[startIndex + 7 + i];


        }

        public CanPacket(byte funct, uint funcc, byte nid, uint sid, byte data_length, byte[] data)
        { 
            this.funct=funct;
	        this.funcc=funcc;
	        this.nid=nid;
	        this.sid=sid;
	        this.data_length=data_length;
	        for(int i=0;i<8;i++) this.data[i]=data[i];
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        public override bool  Equals(Object obj)
        { 
            CanPacket cpm = (CanPacket)obj;

	        byte[] bytes = cpm.getData();

	        for(int i=0;i<8;i++) if (this.data[i]!=bytes[i]) return false;

	        return (this.funct==cpm.getFunct() && this.funcc==cpm.getFuncc() && this.nid==cpm.getNid() && this.sid==cpm.getSid() && this.data_length==cpm.getDataLength());

        }

        public byte getFunct(){ return this.funct; }
        public uint getFuncc(){ return this.funcc; }
        public byte getNid(){ return this.nid; }
        public uint getSid(){ return this.sid; }
        public byte getDataLength(){ return this.data_length; }
        public byte[] getData(){ return this.data; }

        public override string ToString()
        {
            string str = "";

            str = "\t0x"+funct.ToString("X").PadLeft(1,'0')+"\t0x"+funcc.ToString("X").PadLeft(3,'0')+"\t0x"+nid.ToString("X").PadLeft(2,'0')+"\t0x"+sid.ToString("X").PadLeft(3,'0')+"\t"+data_length.ToString()+"\t";
            for (int i = 0; i < 8; i++) str +=((7 - i) < this.data_length ? "0x"+data[7 - i].ToString("X").PadLeft(2, '0') : "xxxx") + " ";

            return str;
        }


        public byte[] getBytes()
        {
            byte[] bytes = new byte[15];

	        // 17 bytes, a packet.
	        // UART_START_BYTE id[0] id[1] id[2] id[3] extended remote_request data_length d[0] d[1] d[2] d[3] d[4] d[5] d[6] d[7] UART_END_BYTE
            /*
	        * 000TTTTC CCCCCCCC CNNNNNNS SSSSSSSS
	        *
	        * <FUNCT> = 00011110 00000000 00000000 00000000 = 0x1E000000
	        * <FUNCC> = 00000001 11111111 10000000 00000000 = 0x01FF8000
	        * <NID>   = 00000000 00000000 01111110 00000000 = 0x00007E00
	        * <SID>   = 00000000 00000000 00000001 11111111 = 0x000001FF
	        * 
	        <FUNCT>, 4 bitar = Function Type, funktionstyp.
	        <FUNCC>, 10 bitar = Function Code, funktionskod.
	        <NID>, 6 bitar = Network ID, nätverksid.
	        <SID>, 9 bitar = Sender ID, sändarid.
	        */

            ulong addr = (((ulong)this.funct)<<25)+(((ulong)this.funcc)<<15)+(((ulong)this.nid)<<9)+((ulong)this.sid);

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
