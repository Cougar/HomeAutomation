using System;
using System.Collections.Generic;
using System.Text;

namespace canBootloader
{
    public class CanPacket
    {
        public enum PACKET_TYPE : byte {ptBOOT=0x0,ptPGM=0x1};
        public enum BOOT_TYPE : byte { btADDR = 0x0, btPGM = 0x1, btDONE = 0x2, btACK = 0x4, btNACK = 0x5, btCRC = 0x6, btCHANGEID = 0x7 };
        public enum PGM_CLASS : byte {pcCTRL=0x0,pcSENSOR=0x1,pcACTUATOR=0x2};
        
        public enum PGM_CTRL_TYPE : ushort {pctBOOTBOOT=0x0,pctAPPBOOT=0x1,pctHEARTBEAT=0x2};

        public enum PGM_SENSOR_TYPE : ushort
        {	
					pstIR=0x0,
					pstTEMP_INSIDE=0x1, pstTEMP_OUTSIDE=0x2, pstTEMP_FREEZER=0x3, pstTEMP_REFRIGERATOR=0x4,
					pstDOOR_FREEZER=0x5, pstDOOR_REFRIGERATOR=0x6,
					pstBLIND_TR=0x7
				};


        public enum PGM_ACTUATOR_TYPE : ushort
        {	
					patBLIND_TR=0x7
				};



        public struct canPGMPacket
        {
            public canPGMPacket(PGM_CLASS _class, ushort id)
            {
                this._class = _class;
                this.id = (ushort)((int)id & 0x7FFF);
            }
            public PGM_CLASS _class; //4
            public ushort id;  //15
           
        };

        public struct canBOOTPacket
        {
            public canBOOTPacket(BOOT_TYPE type,byte offset,byte rid) 
            {
                this.type = type;
                this.offset = offset;
                this.rid = rid;
            }
            public BOOT_TYPE type; //3
            public byte offset;    //8
            public byte rid;       //8
        };

        public PACKET_TYPE type;
        public canPGMPacket pgm = new canPGMPacket();
        public canBOOTPacket boot = new canBOOTPacket();
        public byte sid; //8
        public byte length;
        public byte[] data = new byte[8];


        /// <summary>
        /// Creates a new CanPacket from a raw data starting at index startIndex.
        /// </summary>
        /// <param name="raw">Raw data formatted like: id[0] id[1] id[2] id[3] extended remote_request data_length d[0] d[1] d[2] d[3] d[4] d[5] d[6] d[7]</param>
        /// <param name="startIndex">The startIndex in the raw byte to parse from.</param>
        public CanPacket(byte[] raw,uint startIndex)
        { 
	        // 17 bytes, a packet.
	        // id[0] id[1] id[2] id[3] extended remote_request data_length d[0] d[1] d[2] d[3] d[4] d[5] d[6] d[7]

            ulong addr = (((ulong)raw[startIndex + 3]) << 24) + (((ulong)raw[startIndex + 2]) << 16) + (((ulong)raw[startIndex + 1]) << 8) + (((ulong)raw[startIndex + 0]));

            /*
             x x x P P T T T  O O O O O O O O  R R R R R R R R  S S S S S S S S
             x x x P P C C C  C I I I I I I I  I I I I I I I I  S S S S S S S S
             */

            // addr[28..27] PMODE 0x18000000 >> 27
            //PMODE = BOOT
            /*
            [26..24] TYPE    0x07000000 >> 24
            [23..16] OFFSET  0x00FF0000 >> 16
            [15...8] RID     0x0000FF00 >>  8
            [7....0] SID     0x000000FF
            */

            //PMODE = PGM
            /*
            [26..23] CLASS   0x07800000 >> 23
            [22...8] ID      0x007FFF00 >>  8
            [7....0] SID     0x000000FF
            */

            this.type = (PACKET_TYPE)((addr & 0x18000000) >> 27);

            if (this.type == PACKET_TYPE.ptBOOT)
            {
                boot.type = (BOOT_TYPE)((addr & 0x07000000) >> 24);
                boot.offset = (byte)((addr & 0x00FF0000) >> 16);
                boot.rid = (byte)((addr & 0x0000FF00) >> 8);
            }
            else
            {
                pgm._class = (PGM_CLASS)((addr & 0x07800000) >> 23);
                pgm.id = (ushort)((addr & 0x007FFF00) >> 8);
            }
            this.sid = (byte)(addr & 0x000000FF);

            this.length = raw[startIndex + 6];
            for (int i = 0; i < 8; i++) this.data[i] = raw[startIndex + 7 + i];

        }

        /// <summary>
        /// Create a new canPacket from a canBOOTPacket.
        /// </summary>
        /// <param name="boot">The canBOOTPacket to create from.</param>
        /// <param name="sid">The sender id.</param>
        /// <param name="length">Length of data.</param>
        /// <param name="data">Data.</param>
        public CanPacket(canBOOTPacket boot, byte sid, byte length, byte[] data)
        {
            this.type = PACKET_TYPE.ptBOOT;
            this.boot = boot;
            this.sid = sid;
            this.length = length;
            for (int i = 0; i < 8; i++) this.data[i] = data[i];
        }

        /// <summary>
        /// Create a new canPacket from a canPGMPacket.
        /// </summary>
        /// <param name="pgm">The canPGMPacket to create from.</param>
        /// <param name="sid">The sender id.</param>
        /// <param name="length">Length of data.</param>
        /// <param name="data">Data.</param>
        public CanPacket(canPGMPacket pgm, byte sid, byte length, byte[] data)
        {
            this.type = PACKET_TYPE.ptPGM;
            this.pgm = pgm;
            this.sid = sid;
            this.length = length;
            for (int i = 0; i < 8; i++) this.data[i] = data[i];
        }


        public override int GetHashCode()
        {
            return base.GetHashCode();
        }


        public override bool  Equals(Object obj)
        { 
            CanPacket cpm = (CanPacket)obj;

	        byte[] bytes = cpm.data;

	        for(int i=0;i<8;i++) if (this.data[i]!=bytes[i]) return false;

            if (this.type != cpm.type || this.sid != cpm.sid || this.length != cpm.length) return false;

            if (this.type == PACKET_TYPE.ptBOOT && !pgm.Equals(cpm.pgm)) return false;

            if (this.type == PACKET_TYPE.ptPGM && !boot.Equals(cpm.boot)) return false;

	        return true;

        }

        /// <summary>
        /// Get string representation of a canPacket object.
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            string str = "";

            if (this.type == PACKET_TYPE.ptBOOT)
            {
                str = "BOOT: type: " + this.boot.type.ToString() + ", offset: 0x" + this.boot.offset.ToString("X").PadLeft(2, '0') + ", rid: 0x" + this.boot.rid.ToString("X").PadLeft(2, '0') + ",";
            }
            else
            {
                str = "PGM: class: " + this.pgm._class.ToString() + ", id: 0x" + this.pgm.id.ToString("X").PadLeft(4, '0') + ",";
            }
            str += " sid: 0x" + this.sid.ToString("X").PadLeft(2, '0') + ", length: " + this.length.ToString() + ", data: ";

            for (int i = 0; i < 8; i++) str+="0x"+data[i].ToString("X").PadLeft(2,'0')+" ";

            return str;
        }


        /// <summary>
        /// Get raw byte representation of object.
        /// </summary>
        /// <returns>Raw byte representation.</returns>
        public byte[] getBytes()
        {
            byte[] bytes = new byte[15];

	        // 15 bytes, a packet.
	        // id[0] id[1] id[2] id[3] extended remote_request data_length d[0] d[1] d[2] d[3] d[4] d[5] d[6] d[7]

            /*
             x x x P P T T T  O O O O O O O O  R R R R R R R R  S S S S S S S S
             x x x P P C C C  C I I I I I I I  I I I I I I I I  S S S S S S S S
             */
 
            // addr[28..27] PMODE 0x18000000 >> 27
            //PMODE = BOOT
            /*
            [26..24] TYPE    0x07000000 >> 24
            [23..16] OFFSET  0x00FF0000 >> 16
            [15...8] RID     0x0000FF00 >>  8
            [7....0] SID     0x000000FF
            */

            //PMODE = PGM
            /*
            [26..23] CLASS   0x07800000 >> 23
            [22...8] ID      0x007FFF00 >>  8
            [7....0] SID     0x000000FF
            */

            ulong addr = (ulong)(((int)this.type & 0x3) << 27) + (ulong)(this.sid & 0xFF);

            if (this.type == PACKET_TYPE.ptBOOT)
            {
                addr += (ulong)(((int)this.boot.type & 0x7) << 24) + (ulong)(((int)this.boot.offset & 0xFF) << 16) + (ulong)(((int)this.boot.rid & 0xFF) << 8);
            }
            else
            {
                addr += ((ulong)((int)this.pgm._class & 0xF) << 23) + (ulong)(((int)this.pgm.id & 0x7FFF) << 8);
            }
            bytes[3] = (byte)((addr & 0xFF000000) >> 24);
            bytes[2] = (byte)((addr & 0x00FF0000) >> 16);
            bytes[1] = (byte)((addr & 0x0000FF00) >> 8);
            bytes[0] = (byte)((addr & 0x000000FF));
	        bytes[4]=1;
	        bytes[5]=0;
	        bytes[6]=this.length;
	        for(int i=0;i<8;i++) bytes[i+7]=this.data[i];
            

            return bytes;
        }


    }
}
