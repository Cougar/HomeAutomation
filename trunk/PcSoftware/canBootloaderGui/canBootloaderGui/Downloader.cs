using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace canBootloader
{
    public class Downloader
    {
        private HexFile hf;
        private SerialConnection sc;
        private Thread down;

        private enum dState { SEND_START, WAIT_ACK, SEND_PGM_DATA, WAIT_OWN_PACKET, WAIT_PGM_ACK, RESEND_ADDR, WAIT_DONE, SEND_DONE, DONE, WAIT_ADDR_ACK, DEBUG_STATE1, DEBUG_STATE2 };

        private const byte FUNCT_BOOTLOADER = 0x0;
        private const byte FUNCC_BOOT_INIT = 0x01; //för att initiera en programmering/erase/etc.
        private const byte FUNCC_BOOT_ADDR = 0x02;  // For att ändra adress.
        private const byte FUNCC_BOOT_DONE = 0x05; //för att avsluta en programmering/erase/etc.
        private const byte FUNCC_BOOT_PGM = 0x03; //för att indikera att detta är programdata som ska skrivas.
        private const byte FUNCC_BOOT_ACK = 0x04; //för att indikera att detta är en ACK.
        private const byte MY_ID = 0x91;
        private const byte TARGET_ID = 0x78;
        private const byte MY_NID = 0x0;
        private const byte ADDRL_INDEX = 0;
        private const byte ADDRH_INDEX = 1;
        private const byte ADDRU_INDEX = 2;
        private const byte RID_INDEX = 4;
        private const byte ERR_INDEX = 4;
        private const byte ERR_NO_ERROR = 0x00; //= inget fel
        private const byte ERR_ERROR = 0x01; //= fel

        private const int TIMEOUT_MS = 100;

        private long timeStart = 0;

        private bool userAborted = false;


        public Downloader(HexFile hf, SerialConnection sc) 
        {
            this.hf = hf;
            this.sc = sc;
        }

        ~Downloader() { if (down.IsAlive) userAborted = true; down.Abort(); }

        public bool go() 
        {
            if (sc==null || hf==null || hf.getLength()==0 || !sc.isOpen()) return false;
            down = new Thread(new ThreadStart(downloader));
            down.Start();
            return true;
        }

        public void abort()
        {
            if (down != null && down.IsAlive) { userAborted = true; down.Abort(); }
        }

        public void downloader()
        {
            dState pgs = dState.SEND_START;
            int t = 0;
            CanPacket outCm = null;
            CanPacket cm = null;
            byte[] data = new byte[8];
            byte byteSent = 0;
            ulong currentAddress = 0;

            try
            {
                while (true)
                {
                    bool hasMessage = sc.getPacket(out cm);
                    if (hasMessage)
                    {
                        Array.Copy(cm.getData(), data, 8);
                    }
                    
                   
                    switch (pgs)
                    {
                        case dState.SEND_START:
                            // Send boot start packet to target.
                            // and wait for ack.
                            currentAddress = hf.getAddrLower();
                            data[RID_INDEX] = TARGET_ID;
                            data[ADDRU_INDEX] = (byte)((currentAddress & 0xFF0000) >> 16);
                            data[ADDRH_INDEX] = (byte)((currentAddress & 0xFF00) >> 8);
                            data[ADDRL_INDEX] = (byte)(currentAddress & 0xFF);
                            outCm = new CanPacket(FUNCT_BOOTLOADER, FUNCC_BOOT_INIT, MY_NID, MY_ID, 8, data);
                            sc.writePacket(outCm);
                            t = Environment.TickCount;
                            pgs = dState.WAIT_ACK;
                            timeStart = Environment.TickCount;
                            break;


                        case dState.WAIT_ACK:
                            
                            // Check for timeout, resend start packet in that case..
                            if ((Environment.TickCount - t) > TIMEOUT_MS)
                            {
                                pgs = dState.SEND_START;
                            }

                            // If received ack.
                            if (hasMessage && cm.getFunct() == FUNCT_BOOTLOADER && cm.getFuncc() == FUNCC_BOOT_ACK && cm.getNid() == MY_NID && cm.getSid() == TARGET_ID)
                            {
                                // If no error
                                if (data[ERR_INDEX] == ERR_NO_ERROR)
                                {
                                    // Start sending program data..
                                    byteSent = 0;
                                    pgs = dState.SEND_PGM_DATA;
                                }
                                else 
                                { 
                                    // else resend start..
                                    pgs = dState.SEND_START;
                                }
                            }

                            break;

                        case dState.SEND_PGM_DATA:
                            // Send program data.
                            for (ulong i = 0; i < 8; i++)
                            { 
                                // Populate data.
                                data[i] = hf.getByte(currentAddress + i + byteSent);
                            }
                            outCm = new CanPacket(FUNCT_BOOTLOADER, (uint)((uint)((uint)(Math.Floor(((double)byteSent/8.0)))<<2) + (FUNCC_BOOT_PGM)), MY_NID, MY_ID, 8, data);
                            sc.writePacket(outCm);
                            t = Environment.TickCount;
                            pgs = dState.WAIT_OWN_PACKET;
                            break;


                        case dState.WAIT_OWN_PACKET:
                            // Wait reciving own packet, if timeout, resend last.
                            if ((Environment.TickCount - t) > TIMEOUT_MS)
                            {
                                //pgs = dState.SEND_PGM_DATA;
                                pgs = dState.RESEND_ADDR;
                            }

                            // If received own packet..
                            if (hasMessage && outCm.Equals(cm))
                            {
                                byteSent += 8;

                                // Check if 64 bytes sent or not..
                                if (byteSent == 64)
                                {
                                    // Yes, wait for ack.
                                    t = Environment.TickCount;
                                    pgs = dState.WAIT_PGM_ACK;
                                }
                                else
                                { 
                                    // No, send more.
                                    pgs = dState.SEND_PGM_DATA;
                                }

                            }
                            break;


                        case dState.WAIT_PGM_ACK:
                            // Wait for pgm ack.
                            if ((Environment.TickCount - t) > 2*TIMEOUT_MS)
                            {
                                // Woops, error. Resend address and start over.
                                pgs = dState.RESEND_ADDR;
                            }

                            // If received ack.
                            if (hasMessage && cm.getFunct() == FUNCT_BOOTLOADER && cm.getFuncc() == FUNCC_BOOT_ACK && cm.getNid() == MY_NID && cm.getSid() == TARGET_ID)
                            {
                                // If no error
                                if (data[ERR_INDEX] == ERR_NO_ERROR)
                                {
                                    currentAddress += 64;
                                    // Check if end.
                                    if (currentAddress > hf.getAddrUpper())
                                    {
                                        // Yes, we are done, send done.
                                        pgs = dState.SEND_DONE;
                                    }
                                    else
                                    { 
                                        // More to write.
                                        byteSent = 0;
                                        pgs = dState.SEND_PGM_DATA;
                                    }
                                }
                                else
                                {
                                    // else resend start..
                                    pgs = dState.RESEND_ADDR;
                                }
                            }

                            break;


                        case dState.SEND_DONE:
                            // Send done
                            outCm = new CanPacket(FUNCT_BOOTLOADER, FUNCC_BOOT_DONE, MY_NID, MY_ID, 8, data);
                            sc.writePacket(outCm);
                            t = Environment.TickCount;
                            pgs = dState.WAIT_DONE;
                            break;

                        case dState.RESEND_ADDR:
                            // Resend addr.
                            data[RID_INDEX] = TARGET_ID;
                            data[ADDRU_INDEX] = (byte)((currentAddress & 0xFF0000) >> 16);
                            data[ADDRH_INDEX] = (byte)((currentAddress & 0xFF00) >> 8);
                            data[ADDRL_INDEX] = (byte)(currentAddress & 0xFF);
                            outCm = new CanPacket(FUNCT_BOOTLOADER, FUNCC_BOOT_ADDR, MY_NID, MY_ID, 8, data);
                            sc.writePacket(outCm);
                            t = Environment.TickCount;
                            pgs = dState.WAIT_ADDR_ACK;
                            break;


                        case dState.WAIT_ADDR_ACK:
                            // Check for timeout, resend addr packet in that case..
                            if ((Environment.TickCount - t) > TIMEOUT_MS)
                            {
                                pgs = dState.RESEND_ADDR;
                            }

                            // If received ack.
                            if (hasMessage && cm.getFunct() == FUNCT_BOOTLOADER && cm.getFuncc() == FUNCC_BOOT_ACK && cm.getNid() == MY_NID && cm.getSid() == TARGET_ID)
                            {
                                // If no error
                                if (data[ERR_INDEX] == ERR_NO_ERROR)
                                {
                                    // Start sending program data..
                                    byteSent = 0;
                                    pgs = dState.SEND_PGM_DATA;
                                }
                                else
                                {
                                    // else resend addr..
                                    pgs = dState.RESEND_ADDR;
                                }
                            }
                            break;

                        case dState.WAIT_DONE:
                            // Check for timeout, resend done packet in that case..
                            if ((Environment.TickCount - t) > TIMEOUT_MS)
                            {
                                pgs = dState.SEND_DONE;
                            }

                            // If received ack.
                            if (hasMessage && cm.getFunct() == FUNCT_BOOTLOADER && cm.getFuncc() == FUNCC_BOOT_ACK && cm.getNid() == MY_NID && cm.getSid() == TARGET_ID)
                            {
                                // If no error
                                if (data[ERR_INDEX] == ERR_NO_ERROR)
                                {
                                    // Start sending program data..
                                    byteSent = 0;
                                    pgs = dState.DONE;
                                }
                                else
                                {
                                    // else resend addr..
                                    pgs = dState.RESEND_ADDR;
                                }
                            }

                            break;

                        case dState.DONE:
                            userAborted = false;
                            down.Abort();
                            break;
                    }
                }

            } catch(Exception) 
            {
                threadAbort.Invoke(this, new threadAbortEvent((int)(Environment.TickCount - timeStart), (int)hf.getLength(), userAborted));
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
        private bool userAborted = false;

        public threadAbortEvent(int diffms, int bytes, bool userAborted)
        {
            if (!userAborted)
            {
                this.timems = diffms;
                this.times = this.timems/1000;
                this.bytes = bytes;
                this.bps = ((double)bytes / ((double)this.timems/1000.0));
            }
            this.userAborted = userAborted;
        }

        public double getBps() { return this.bps; }
        public int getTimeS() { return this.times; }
        public bool getUserAborted() { return this.userAborted; }
    }
}
