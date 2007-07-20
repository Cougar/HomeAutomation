using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using canConfigurator;

namespace canBootloader
{
    public class Downloader
    {
        private enum dState { SEND_START, WAIT_ACK, SEND_PGM_DATA, WAIT_OWN_PACKET, WAIT_PGM_ACK, RESEND_ADDR, WAIT_DONE, SEND_DONE, DONE, WAIT_ADDR_ACK, DEBUG_STATE1, DEBUG_STATE2, CHANGE_ID_START, CHANGE_ID_WAIT_ACK_PACKET, CHANGE_ID_DONE, SEND_CRC, WAIT_CRC_ACK };
        public enum downloadMode { PROGRAM, CHANGE_ID };
        public enum abortMode { USER, PROGRAM, CHANGE_ID };

        private byte MY_ID = 0;
        private byte TARGET_ID = 0;
        
        private HexFile hf;
        private SerialConnection sc;
        private Thread down;
        private dState pgs;
        private downloadMode downloadmode;

        private const int TIMEOUT_MS = 100;

        private long timeStart = 0;

        private abortMode abortmode;
        private bool hasFoundNode = false;

        private byte tmp_new_id = 0;

        public string lastError = "";

        public Downloader(HexFile hf, SerialConnection sc, byte myId, byte targetId, downloadMode downloadmode, byte tmp_new_id) 
        {
            this.hf = hf;
            this.sc = sc;
            this.MY_ID = myId;
            this.TARGET_ID = targetId;
            this.downloadmode = downloadmode;
            this.tmp_new_id = tmp_new_id;
        }

        ~Downloader() { if (down.IsAlive) abortmode = abortMode.USER; down.Abort(); }

        public bool foundNode() { return hasFoundNode; }

        public bool go() 
        {
            if (sc == null) { lastError = "No serial connection established!"; return false; }
            if (!sc.isOpen()) { lastError = "Serial port is not open!"; return false; }
            if(downloadmode == downloadMode.PROGRAM && (hf == null || hf.getLength() == 0)) {lastError="No hex file loaded or hexfile has length 0."; return false;}

            if (downloadmode == downloadMode.PROGRAM)
            {
                
                pgs = dState.SEND_START;
            }
            else if (downloadmode == downloadMode.CHANGE_ID)
            {
                pgs = dState.CHANGE_ID_START;
            }
            down = new Thread(new ThreadStart(downloader));
            down.Start();
            return true;
        }

        public void abort()
        {
            if (down != null && down.IsAlive) { abortmode = abortMode.USER; down.Abort(); }
        }

        public void downloader()
        {
            int t = 0;
            CanPacket outCp = null;
            CanPacket cp = null;
            byte[] data = new byte[8];
            byte offset = 0;
            ulong currentAddress = 0;

            try
            {
                while (true)
                {
                    bool hasMessage = sc.getPacket(out cp);
                    if (hasMessage)
                    {
                        Array.Copy(cp.data, data, 8);
                    }
                    bool hasMessageFromTarget = (hasMessage && cp.type== td.PACKET_TYPE.ptBOOT && cp.boot.rid==MY_ID && cp.sid == TARGET_ID);

                    
                   
                    switch (pgs)
                    {
                        case dState.SEND_START:
                            // Send boot start packet to target.
                            // and wait for ack.
                            currentAddress = hf.getAddrLower();
                            data[2] = (byte)((currentAddress & 0xFF0000) >> 16);
                            data[1] = (byte)((currentAddress & 0xFF00) >> 8);
                            data[0] = (byte)(currentAddress & 0xFF);
                            outCp = new CanPacket(new CanPacket.canBOOTPacket(td.BOOT_TYPE.btADDR, 0, TARGET_ID), MY_ID, 3, data);

                            sc.writePacket(outCp);
                            t = Environment.TickCount;
                            pgs = dState.WAIT_ACK;
                            timeStart = Environment.TickCount;
                            hasFoundNode = false;
                            break;



                        case dState.WAIT_ACK:

                            // Check for timeout, resend start packet in that case..
                            if ((Environment.TickCount - t) > TIMEOUT_MS)
                            {
                                pgs = dState.SEND_START;
                            }

                            // If received ack.
                            if (hasMessageFromTarget && cp.boot.type == td.BOOT_TYPE.btACK)
                            {
                                // Start sending program data..
                                offset = 0;
                                pgs = dState.SEND_PGM_DATA;
                                hasFoundNode = true;
                            }
                            // if nack
                            if (hasMessageFromTarget && cp.boot.type == td.BOOT_TYPE.btNACK)
                            {    
                                // else resend start..
                                pgs = dState.SEND_START;
                            }

                            break;


                        case dState.SEND_PGM_DATA:
                            // Send program data.
                            for (ulong i = 0; i < 8; i++)
                            {
                                // Populate data.
                                data[i] = hf.getByte(currentAddress + i + offset);
                            }
                            outCp = new CanPacket(new CanPacket.canBOOTPacket(td.BOOT_TYPE.btPGM, offset, TARGET_ID), MY_ID, 8, data);
                            sc.writePacket(outCp);
                            t = Environment.TickCount;
                            pgs = dState.WAIT_OWN_PACKET;
                            break;


                        case dState.WAIT_OWN_PACKET:
                            // Wait reciving own packet, if timeout, resend last.
                            if ((Environment.TickCount - t) > TIMEOUT_MS)
                            {
                                pgs = dState.SEND_PGM_DATA;
                            }

                            // If received own packet..
                            if (hasMessage && outCp.Equals(cp))
                            {
                                offset += 8;

                                // Check if 64 bytes sent or not..
                                if (offset % 64 == 0)
                                {
                                    // Yes, send crc.
                                    t = Environment.TickCount;
                                    pgs = dState.SEND_CRC;
                                }
                                else
                                {
                                    // No, send more.
                                    pgs = dState.SEND_PGM_DATA;
                                }

                            }
                            break;


                        case dState.SEND_CRC:
                            byte[] pgmData = new byte[64];
                            for(ulong i=0;i<64;i++) pgmData[i] = hf.getByte(i+currentAddress);

                            ushort crc = crc16.calc_crc(pgmData, 64);
                            data[4] = (byte)((crc & 0xFF00) >> 8);
                            data[3] = (byte)(crc & 0xFF);
                            data[2] = (byte)(((currentAddress + offset) & 0xFF0000) >> 16);
                            data[1] = (byte)(((currentAddress + offset) & 0xFF00) >> 8);
                            data[0] = (byte)((currentAddress + offset) & 0xFF);
                            outCp = new CanPacket(new CanPacket.canBOOTPacket(td.BOOT_TYPE.btCRC, offset, TARGET_ID), MY_ID, 8, data);
                            sc.writePacket(outCp);
                            t = Environment.TickCount;
                            pgs = dState.WAIT_CRC_ACK;
                            break;


                        case dState.WAIT_CRC_ACK:
                            // Check for timeout, resend start packet in that case..
                            if ((Environment.TickCount - t) > TIMEOUT_MS)
                            {
                                pgs = dState.SEND_CRC;
                            }

                            // If received ack.
                            if (hasMessageFromTarget && cp.boot.type == td.BOOT_TYPE.btACK)
                            {
                                // Start send more program data..
                                currentAddress += offset;

                                offset = 0;
                                pgs = dState.SEND_PGM_DATA;
                                hasFoundNode = true;
 
                                if (currentAddress > hf.getAddrUpper())
                                {
                                    // Yes, we are done, send done.
                                    pgs = dState.SEND_DONE;
                                }
                            }
                            // if nack
                            if (hasMessageFromTarget && cp.boot.type == td.BOOT_TYPE.btNACK)
                            {
                                // else resend pgm data...
                                offset = 0;
                                pgs = dState.SEND_PGM_DATA;
                            }

                            break;


                        case dState.SEND_DONE:
                            outCp = new CanPacket(new CanPacket.canBOOTPacket(td.BOOT_TYPE.btDONE, offset, TARGET_ID), MY_ID, 8, data);
                            sc.writePacket(outCp);
                            t = Environment.TickCount;
                            pgs = dState.DONE;
                            break;


                        case dState.DONE:
                            abortmode = abortMode.PROGRAM;
                            down.Abort();
                            break;


 // -------------------------------- CHANGE ID NID ------------------------------------


                            
                        case dState.CHANGE_ID_START:
                            // Send change id packet
                            // and wait for own packet.
                            data[0] = tmp_new_id;
                            outCp = new CanPacket(new CanPacket.canBOOTPacket(td.BOOT_TYPE.btCHANGEID, 0, TARGET_ID), MY_ID, 1, data);

                            sc.writePacket(outCp);
                            t = Environment.TickCount;
                            pgs = dState.CHANGE_ID_WAIT_ACK_PACKET;
                            timeStart = Environment.TickCount;
                            break;

                        case dState.CHANGE_ID_WAIT_ACK_PACKET:
                            // Wait reciving own packet, if timeout, resend last.
                            if ((Environment.TickCount - t) > TIMEOUT_MS)
                            {
                                pgs = dState.CHANGE_ID_START;
                            }

                            if (hasMessage && cp.type == td.PACKET_TYPE.ptBOOT && cp.sid == tmp_new_id && cp.boot.type == td.BOOT_TYPE.btACK)
                            {
                                pgs = dState.CHANGE_ID_DONE;
                            }

                            break;

                        case dState.CHANGE_ID_DONE:
                            abortmode = abortMode.CHANGE_ID;
                            down.Abort();
                            break;
                            
                    }

                    Thread.Sleep(0);
                }

            } catch(Exception) 
            {
                int hfLength = 0;
                if (hf != null) hfLength = (int)hf.getLength();
                threadAbort.Invoke(this, new threadAbortEvent((int)(Environment.TickCount - timeStart), hfLength, abortmode));
            }

            
        }

        public event EventHandler threadAbort;
    }

    internal class threadAbortEvent : EventArgs
    {
        private int timems = 0;
        private int times = 0;
        private double bps = 0;
        private int bytes = 0;
        private Downloader.abortMode abortmode;

        public threadAbortEvent(int diffms, int bytes, Downloader.abortMode abortmode)
        {
            if (abortmode == Downloader.abortMode.PROGRAM)
            {
                this.timems = diffms;
                this.times = this.timems/1000;
                this.bytes = bytes;
                this.bps = ((double)bytes / ((double)this.timems/1000.0));
            }
            this.abortmode = abortmode;
        }

        public double getBps() { return this.bps; }
        public int getTimeS() { return this.times; }
        public Downloader.abortMode getAbortMode() { return this.abortmode; }
    }

}
