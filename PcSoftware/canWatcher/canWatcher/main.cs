using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.Threading;
using System.IO.Ports;

namespace canWatcher
{
    

    public partial class main : Form
    {
        public const byte UART_START_BYTE = 253;
        public const byte UART_END_BYTE = 250;

        byte[] iBuff = new byte[256];
        int iBuffPointer=0;
        public messageTracker mt = new messageTracker();
        public outgoingKeeper mtOut = new outgoingKeeper();

        public main()
        {
            InitializeComponent();
        }

        private void dg_schedule_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }

        private void cmd_connect_Click(object sender, EventArgs e)
        {
            if (serial_conn.IsOpen)
            {
                lab_status.Text = "Status: Disconnected.";
                serial_conn.Close();
            }
            else 
            {
                lab_status.Text = "Status: Connected.";
                serial_conn.ReadBufferSize = 256;
                serial_conn.BaudRate = 19200;
                serial_conn.Parity = System.IO.Ports.Parity.None;
                serial_conn.PortName = "COM2";
                serial_conn.RtsEnable = false;
                serial_conn.StopBits = System.IO.Ports.StopBits.One;
                serial_conn.DataBits = 8;
                serial_conn.Open();
                
            }
        }

        private void txt_debug_out_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (serial_conn.IsOpen)
            {
                byte[] b = new byte [1];

                b[0] = (byte)e.KeyChar;

                serial_conn.Write(b,0,1);
                
            }
            else
            {
                lab_status.Text = "Status: Serial port is not connected.";
            }
        }

        private void serial_conn_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {

            System.IO.Ports.SerialPort port = (System.IO.Ports.SerialPort)sender;
            byte[] data = new byte[port.BytesToRead];

            int bytesToRead = port.BytesToRead;

            // Fyll på array hela tiden i slutet.
            port.Read(data, 0, data.Length);
            Array.Copy(data, 0, iBuff, iBuffPointer, data.Length);
            iBuffPointer += bytesToRead;

         
            // När array större eller lika med 16...
            while (iBuffPointer >= 16)
            {
                int startIndex = 0;

                // Sök igenom efter start byte från början.
                for (int i = 0; i < iBuffPointer; i++)
                {
                    // Poppa alla bytes som inte är startbyte.
                    if (iBuff[i] != UART_START_BYTE) startIndex++; 
                    else
                    {
                        // När startbyte hittas, kolla om återstående längd är större eller lika med 16 (inkl startbyte)
                        if ((iBuffPointer - startIndex) >= 16)
                        {
                            //om så, kolla 15 byte fram.
                            if (iBuff[startIndex + 15] == UART_END_BYTE)
                            {
                                // Om byte 15 är slutbyte så extraktas startIndex till slutbyteindex.
                                canMessage cm = new canMessage(iBuff, startIndex + 1);
                                mt.addMessage(cm);
                                
                                newIncommingCanMessage.Invoke(this, EventArgs.Empty);

                                // Sätt ny startindex och avsluta loop.
                                startIndex += 16;
                                break;
                            }
                        }
                    }
                }
                // och i slutet göra en array copy.
                // Flytta ner allt efter slutbyte till index 0 i array.
                Array.Copy(iBuff, startIndex, iBuff, 0, iBuffPointer - 16);
                iBuffPointer -= startIndex;
            }
            
            
            
   

        }

        public event EventHandler newIncommingCanMessage;

        private void main_Load(object sender, EventArgs e)
        {
            this.newIncommingCanMessage += new EventHandler(main_newIncommingCanMessage);
        }

        void main_newIncommingCanMessage(object sender, EventArgs e)
        {
            this.BeginInvoke((ThreadStart)delegate
            {
                int scrollPosition = dg_incomming.FirstDisplayedScrollingRowIndex;
                
                dg_incomming.Rows.Clear();
                foreach (DictionaryEntry de in mt.getMessages())
                {
                    canMessage cm = (canMessage)de.Value;
                    dg_incomming.Rows.Add(cm.getDataGridViewRow(dg_incomming, mt));
                }

                if (scrollPosition > 0) dg_incomming.FirstDisplayedScrollingRowIndex = scrollPosition;
            });
        }

        public void refreshOutgoing()
        {
            
            this.BeginInvoke((ThreadStart)delegate
            {
                int scrollPosition = dg_outgoing.FirstDisplayedScrollingRowIndex;
                
                dg_outgoing.Rows.Clear();
                foreach (DictionaryEntry de in mtOut.getMessages())
                {
                    canMessage cm = (canMessage)de.Key;
                    dg_outgoing.Rows.Add(cm.getDataGridViewRow(dg_outgoing,mtOut));
                }

                if (scrollPosition > 0) dg_outgoing.FirstDisplayedScrollingRowIndex = scrollPosition;
  
            });
            
            
        }




        private void main_FormClosing(object sender, FormClosingEventArgs e)
        {
            serial_conn.Close();
        }

        private void cmd_add_message_Click(object sender, EventArgs e)
        {
            addMessage am = new addMessage(this);
            am.Show();
        }

        private void tmr_check_out_Tick(object sender, EventArgs e)
        {
            if (serial_conn.IsOpen)
            {
                foreach (DictionaryEntry de in mtOut.getMessages())
                {
                    canMessage cm = (canMessage)de.Key;
                    if (mtOut.timeToSend(cm))
                    { 
                        // send
                        sendMessage(cm);
                        mtOut.flagAsSent(cm);
                    }
                }
            }
        }

        private void sendMessage(canMessage cm)
        {
            if (serial_conn.IsOpen)
            {
                byte[] b = new byte[16];
                b[0] = UART_START_BYTE;
                Array.Copy(cm.getRaw(), 0, b, 1, 14);
                b[15] = UART_END_BYTE;
                serial_conn.Write(b, 0, 16);
            }
        }

        private void tmr_refresher_Tick(object sender, EventArgs e)
        {
            refreshOutgoing();
        }


    }

    public class canMessage
    {
        private uint ident;
        private bool extended;
        private byte data_length;
        private byte[] data = new byte[8];

        public canMessage(uint ident,bool extended,byte data_length,byte[] data)
        {
            this.ident = ident;
            this.extended = extended;
            if (this.extended) this.ident &= 0x1FFFFFFF; else this.ident &= 0x000007FF; 
            this.data_length = data_length;
            this.data = data;
        }

        public canMessage(byte[] raw,int startIndex)
        {
            this.ident = BitConverter.ToUInt32(raw, startIndex);
            this.extended = (raw[startIndex+4] != 0);
            if (this.extended) this.ident &= 0x1FFFFFFF; else this.ident &= 0x000007FF; 
            this.data_length = raw[startIndex+5];
            Array.Copy(raw, startIndex+6, this.data, 0, 8);
            Console.WriteLine(this.data[7] + " " + this.data[0]);
        }

        public uint getIdent() { return ident; }
        public bool getExtended() { return extended; }
        public byte getDataLength() { return data_length; }
        public byte[] getData() { return data; }
        
        
        public DataGridViewRow getDataGridViewRow(DataGridView dg, KeeperTracker mt)
        {
            DataGridViewRow row = new DataGridViewRow();
            row.CreateCells(dg);

            string dgIdentifier = this.ident.ToString("X").PadLeft(8, '0') + "h";
            string dgLength = this.data_length.ToString();
            string dgData = byteArrayToHexString(this.data, this.data_length);
            string dgPeriod = mt.getPeriod(this).ToString();
            string dgCount = mt.getCount(this).ToString();

            ArrayList a = new ArrayList();
            a.Add(dgIdentifier);
            a.Add(dgLength);
            a.Add(dgData);
            a.Add(dgPeriod);
            a.Add(dgCount);
            a.Add(this.ident);
            row.SetValues(a.ToArray());

            return row;
        }
        

        private string byteArrayToHexString(byte[] data, byte data_length)
        {
            string str = "";

            for (int i = 0;i < 8; i++)
            {
                if (i < data_length)
                    str += data[data_length - i-1].ToString("X");
                else
                    str += "--";
                if (i!=8) str += " ";
            }

            return str;
        }


        public byte[] getRaw()
        {
            byte[] b = new byte[14];
            byte[] b2 = new byte[8];
            b2 = BitConverter.GetBytes(this.ident);
            Array.Copy(b2, 0, b, 0, 4);
            b[4] = (byte)(this.extended?1:0);
            b[5] = this.data_length;
            b2 = new byte[8];
            Array.Copy(this.data, 0, b, 6, 8);
            //Array.Copy(this.data, 0, b2, 0, 8);
            //Array.Reverse(b2);
            //Array.Copy(b2, 0, b, 6, 8);
            return b;
        }
 
    }

    public class outgoingKeeper : KeeperTracker
    {
        Hashtable outgoingMessages = new Hashtable();

        public outgoingKeeper() { }

        public void addMessage(canMessage cm,long period)
        {
            outgoingMessages[cm]=new messageState(period);
        }

        public Hashtable getMessages()
        {
            return outgoingMessages;
        }

        public long getPeriod(canMessage cm)
        {
            messageState ms = (messageState)outgoingMessages[cm];
            if (ms == null) return 0;
            return ms.period;
        }

        public int getCount(canMessage cm)
        {
            messageState ms = (messageState)outgoingMessages[cm];
            if (ms == null) return 0;
            return ms.count;
        }

        public bool timeToSend(canMessage cm)
        {
            messageState ms = (messageState)outgoingMessages[cm];
            if (ms == null) return false;
            return (Environment.TickCount > ms.nextTimeToSend);
        }

        public void flagAsSent(canMessage cm)
        {
            messageState ms = (messageState)outgoingMessages[cm];
            if (ms == null) return;

            ms.flagAsSent();
        }

        

        internal class messageState
        {
            public long period;
            public int count;
            public long nextTimeToSend = 0;

            public messageState(long period)
            {
                this.period = period;
                this.nextTimeToSend = Environment.TickCount + period;
            }

            public void flagAsSent()
            {
                this.count++;
                this.nextTimeToSend = Environment.TickCount + this.period;
            }
        }
    }

    public class messageTracker : KeeperTracker
    {
        Hashtable incommingMessages = new Hashtable();
        Hashtable allMessages = new Hashtable();

        public messageTracker() {  }

        public void addMessage(canMessage cm)
        {
            allMessages[cm.getIdent()] = cm;
            stamp(cm);
        }

        public Hashtable getMessages()
        {
            return allMessages;
        }

        private void stamp(canMessage cm)
        {
            messageState ms = (messageState)incommingMessages[cm.getIdent()];
            if (ms == null)
            {
                ms = new messageState();
            }
            ms.stamp();
            incommingMessages[cm.getIdent()] = ms;

        }

        public long getPeriod(canMessage cm) 
        {
            messageState ms = (messageState)incommingMessages[cm.getIdent()];
            if (ms == null) return 0;
            return ms.period;
        }
        public int getCount(canMessage cm) 
        {

            messageState ms = (messageState)incommingMessages[cm.getIdent()];

            if (ms == null) return 0;
            return ms.count;
        }


        internal class messageState
        {
            public int count;
            public long period;

            private long lastStamp;

            public messageState()
            {
                lastStamp = Environment.TickCount;
                this.count = 0;
                this.period = 0;
            }

            public void stamp()
            {
                this.count++;
                this.period = Environment.TickCount - lastStamp;
                lastStamp = Environment.TickCount;
            }

        }

    }

}