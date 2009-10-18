using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace canConfigurator
{
    public class eeDownloader
    {
        public const ulong MAX_EE_SIZE = 524288;
        private const int TIMEOUT_MS = 100;
        private const int DONE_TO_SEND = 10;

        private enum dState { SEND_START, WAIT_ACK, SEND_PGM_DATA, WAIT_OWN_PACKET, WAIT_PGM_ACK, WAIT_DONE, SEND_DONE, DONE, SEND_WAIT_DONE, SEND_CRC, WAIT_CRC_ACK };
        public enum abortMode { USER, PROGRAM };
        public enum eeType {EXTERNAL=1,INTERNAL=0 };

        private byte MY_ID = 0;
        private byte TARGET_ID = 0;

        private byte[] eebytes = new byte[MAX_EE_SIZE];
        private ulong eelength;
        private ulong eestart;
        private eeType eetype;
        private serialCanConnection sc;
        private Thread down;
        private dState pgs;
        private abortMode abortmode;
        
        private long timeStart = 0;
        private bool hasFoundNode = false;
        public string lastError = "";

        public eeDownloader(byte[] eebytes,ulong eelength,ulong eestart,eeType eetype, serialCanConnection sc, byte myId, byte targetId) 
        {
            for (ulong i = 0; (i < eelength && i < MAX_EE_SIZE); i++) this.eebytes[i] = eebytes[i];
            for (ulong i = eelength; i < MAX_EE_SIZE; i++) this.eebytes[i] = 0xFF;
            this.eelength = eelength;
            this.eestart = eestart;
            this.eetype = eetype;
            this.sc = sc;
            this.MY_ID = myId;
            this.TARGET_ID = targetId;
        }

        ~eeDownloader() { if (down.IsAlive) abortmode = abortMode.USER; down.Abort(); }

        public bool foundNode() { return hasFoundNode; }

        public bool go() 
        {
            if (sc == null) { lastError = "No serial connection established!"; return false; }
            if (!sc.isConnected()) { lastError = "Serial port is not open!"; return false; }
            if(eelength == 0) {lastError="EEprom size is zero!"; return false;}
            if (eestart > MAX_EE_SIZE) { lastError = "Start address is greater than max!"; return false; }

            pgs = dState.SEND_START;
           
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

            int doneCount = DONE_TO_SEND;

            try
            {
                while (true)
                {
                    bool hasMessage = sc.getPacket(out cp);
                    if (hasMessage)
                    {
                        Array.Copy(cp.data, data, 8);
                    }
                    bool hasMessageFromTarget = (hasMessage && cp.type== td.PACKET_TYPE.ptPGM && cp.pgm._class == td.PGM_CLASS.pcCTRL && cp.sid == TARGET_ID);

                    
                   
                    switch (pgs)
                    {
                        case dState.SEND_START:
                            // Send boot start packet to target.
                            // and wait for ack.
                            currentAddress = eestart;
                            data[4] = TARGET_ID;
                            data[3] = (byte)eetype;
                            data[2] = (byte)((currentAddress & 0xFF0000) >> 16);
                            data[1] = (byte)((currentAddress & 0xFF00) >> 8);
                            data[0] = (byte)(currentAddress & 0xFF);
                            outCp = new CanPacket(new CanPacket.canPGMPacket(td.PGM_CLASS.pcCTRL,(ushort)td.PGM_CTRL_TYPE.pctEE_START),MY_ID,5,data);

                            sc.sendPacket(outCp);
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
                            if (hasMessageFromTarget && cp.pgm.id==(ushort)td.PGM_CTRL_TYPE.pctEE_ACK)
                            {
                                // Start sending program data..
                                offset = 0;
                                pgs = dState.SEND_PGM_DATA;
                                hasFoundNode = true;
                            }
                            // if nack
                            if (hasMessageFromTarget && cp.pgm.id == (ushort)td.PGM_CTRL_TYPE.pctEE_NACK)
                            {    
                                // else resend start..
                                pgs = dState.SEND_START;
                            }

                            break;


                        case dState.SEND_PGM_DATA:
                            // Send program data.
                            for (ulong i = 0; i < 7; i++)
                            {
                                // Populate data.
                                data[i] = eebytes[currentAddress + i + offset];
                            }
                            data[7] = offset;

                            outCp = new CanPacket(new CanPacket.canPGMPacket(td.PGM_CLASS.pcCTRL,(ushort)td.PGM_CTRL_TYPE.pctEE_PGM), MY_ID, 8, data);
                            sc.sendPacket(outCp);
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
                                offset += 7;

                                // Check if 56 bytes sent or not..
                                if (offset % 56 == 0)
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
                            byte[] pgmData = new byte[56];
                            for(ulong i=0;i<56;i++) pgmData[i] = eebytes[i+currentAddress];

                            ushort crc = crc16.calc_crc(pgmData, 56);
                            data[4] = (byte)((crc & 0xFF00) >> 8);
                            data[3] = (byte)(crc & 0xFF);
                            data[2] = (byte)(((currentAddress + offset) & 0xFF0000) >> 16);
                            data[1] = (byte)(((currentAddress + offset) & 0xFF00) >> 8);
                            data[0] = (byte)((currentAddress + offset) & 0xFF);
                            outCp = new CanPacket(new CanPacket.canPGMPacket(td.PGM_CLASS.pcCTRL,(ushort)td.PGM_CTRL_TYPE.pctEE_CRC), MY_ID, 8, data);
                            sc.sendPacket(outCp);
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
                            if (hasMessageFromTarget && cp.pgm.id == (ushort)td.PGM_CTRL_TYPE.pctEE_ACK)
                            {
                                // Start send more program data..
                                currentAddress += offset;

                                offset = 0;
                                pgs = dState.SEND_PGM_DATA;
                                hasFoundNode = true;
 
                                if (currentAddress > (eelength+eestart))
                                {
                                    // Yes, we are done, send done.
                                    pgs = dState.SEND_DONE;
                                }
                            }
                            // if nack
                            if (hasMessageFromTarget && cp.pgm.id == (ushort)td.PGM_CTRL_TYPE.pctEE_NACK)
                            {
                                // else resend pgm data...
                                offset = 0;
                                pgs = dState.SEND_PGM_DATA;
                            }

                            break;


                        case dState.SEND_DONE:
                            data[2] = (byte)(((eelength) & 0xFF0000) >> 16);
                            data[1] = (byte)(((eelength) & 0xFF00) >> 8);
                            data[0] = (byte)((eelength) & 0xFF);
                            outCp = new CanPacket(new CanPacket.canPGMPacket(td.PGM_CLASS.pcCTRL,(ushort)td.PGM_CTRL_TYPE.pctEE_DONE), MY_ID, 3, data);
                            sc.sendPacket(outCp);
                            t = Environment.TickCount;
                            pgs = dState.SEND_WAIT_DONE;
                            doneCount--;
                            break;

                        case dState.SEND_WAIT_DONE:
                            if (doneCount < 0)
                            {
                                pgs = dState.DONE;
                            }
                            else if ((Environment.TickCount-t)>TIMEOUT_MS)
                            {
                                pgs = dState.SEND_DONE;
                            }


                            break;

                        case dState.DONE:
                            abortmode = abortMode.PROGRAM;
                            down.Abort();
                            break;

                    }


                    Thread.Sleep(10);
                }

            } catch(Exception) 
            {
                threadAbort.Invoke(this, new threadAbortEvent((int)(Environment.TickCount - timeStart), eelength, abortmode));
            }


            
        }

        public event EventHandler threadAbort;
    }

    internal class threadAbortEvent : EventArgs
    {
        private int timems = 0;
        private int times = 0;
        private double bps = 0;
        private ulong bytes = 0;
        private eeDownloader.abortMode abortmode;

        public threadAbortEvent(int diffms, ulong bytes, eeDownloader.abortMode abortmode)
        {
            if (abortmode == eeDownloader.abortMode.PROGRAM)
            {
                this.timems = diffms-1000;
                this.times = this.timems/1000;
                this.bytes = bytes;
                this.bps = ((double)bytes / ((double)this.timems/1000.0));
            }
            this.abortmode = abortmode;
        }

        public double getBps() { return this.bps; }
        public int getTimeS() { return this.times; }
        public eeDownloader.abortMode getAbortMode() { return this.abortmode; }
    }
}
