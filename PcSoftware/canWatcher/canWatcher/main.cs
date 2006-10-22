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
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;

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

        public Hashtable sets = new Hashtable();

        public canMessage addLastCm = null;

        public main()
        {
            InitializeComponent();
        }

        private void dg_schedule_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }


        private void txt_debug_out_KeyPress(object sender, KeyPressEventArgs e)
        {
            
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
            loadSettings();
        }

        void main_newIncommingCanMessage(object sender, EventArgs e)
        {
            this.BeginInvoke((ThreadStart)delegate
            {
                int scrollPosition = dg_incomming.FirstDisplayedScrollingRowIndex;

                DataGridViewColumn sortedColumn = dg_incomming.SortedColumn;
                ListSortDirection sorder = new ListSortDirection();
                bool doSort = true;
                if (dg_incomming.SortOrder == SortOrder.Ascending) sorder = ListSortDirection.Ascending;
                else if (dg_incomming.SortOrder == SortOrder.Descending) sorder = ListSortDirection.Descending;
                else doSort = false;

                Point pp = Point.Empty;
                if (dg_incomming.SelectedCells.Count > 0)
                    pp = dg_incomming.CurrentCellAddress;

                dg_incomming.Rows.Clear();
                foreach (DictionaryEntry de in mt.getMessages())
                {
                    canMessage cm = (canMessage)de.Value;
                    dg_incomming.Rows.Add(cm.getDataGridViewRow(dg_incomming, mt));
                }

                if (scrollPosition > 0) dg_incomming.FirstDisplayedScrollingRowIndex = scrollPosition;

                if (doSort) dg_incomming.Sort(sortedColumn, (ListSortDirection)sorder);

                if (dg_incomming.Rows.Count > 0)
                    dg_incomming.CurrentCell = dg_incomming.Rows[pp.Y].Cells[pp.X];
            
            });
        }

        public void refreshOutgoing()
        {

            this.BeginInvoke((ThreadStart)delegate
            {
                int scrollPosition = dg_outgoing.FirstDisplayedScrollingRowIndex;

                DataGridViewColumn sortedColumn = dg_outgoing.SortedColumn;
                ListSortDirection sorder = new ListSortDirection();
                bool doSort = true;
                if (dg_outgoing.SortOrder == SortOrder.Ascending) sorder = ListSortDirection.Ascending;
                else if (dg_outgoing.SortOrder == SortOrder.Descending) sorder = ListSortDirection.Descending;
                else doSort = false;


                Point pp = Point.Empty;
                if (dg_outgoing.SelectedCells.Count > 0)
                    pp = dg_outgoing.CurrentCellAddress;

                dg_outgoing.Rows.Clear();
                foreach (DictionaryEntry de in mtOut.getMessages())
                {
                    canMessage cm = (canMessage)de.Key;
                    dg_outgoing.Rows.Add(cm.getDataGridViewRow(dg_outgoing,mtOut));
                }

                if (doSort) dg_outgoing.Sort(sortedColumn,(ListSortDirection) sorder);

                if (scrollPosition > 0) dg_outgoing.FirstDisplayedScrollingRowIndex = scrollPosition;

                if (dg_outgoing.Rows.Count > 0 && pp.Y < dg_outgoing.Rows.Count)
                    dg_outgoing.CurrentCell = dg_outgoing.Rows[pp.Y].Cells[pp.X];

                
            });
       
            
        }




        private void main_FormClosing(object sender, FormClosingEventArgs e)
        {
            serial_conn.Close();
        }

        private void cmd_add_message_Click(object sender, EventArgs e)
        {
            addMessage am = new addMessage(this,null);
            am.ShowDialog();
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

        private void cmd_edit_message_Click(object sender, EventArgs e)
        {
            if (dg_outgoing.SelectedCells.Count > 0)
            {
                addMessage am = new addMessage(this, (canMessage)dg_outgoing["cmO", dg_outgoing.CurrentCell.RowIndex].Value);
                am.ShowDialog();
            }
            else MessageBox.Show("You need to select a message to edit.", "Row missing", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void cmd_delete_message_Click(object sender, EventArgs e)
        {
            if (dg_outgoing.SelectedCells.Count > 0)
            {
                mtOut.deleteMessage((canMessage)dg_outgoing["cmO", dg_outgoing.CurrentCell.RowIndex].Value);
                refreshOutgoing();
            }
            else MessageBox.Show("You need to select a message to delete.","Row missing",MessageBoxButtons.OK,MessageBoxIcon.Information);
        }

        public bool setConnected(bool connected)
        {
            if (connected)
            {
                if (serial_conn.IsOpen)
                {
                    lab_status.Text = "Err! Port alredy open.";
                    Console.Beep();
                    return false;
                }
                try
                {
                    serial_conn.ReadBufferSize = 256;
                    serial_conn.BaudRate = 19200;
                    serial_conn.RtsEnable = false;
                    serial_conn.Parity = (System.IO.Ports.Parity)Enum.Parse(typeof(System.IO.Ports.Parity), sets["parity"].ToString());
                    serial_conn.PortName = (string)sets["port"];
                    serial_conn.StopBits = (System.IO.Ports.StopBits)Enum.Parse(typeof(System.IO.Ports.StopBits), sets["stopbits"].ToString());
                    serial_conn.DataBits = (int)sets["databits"];
                    serial_conn.Open();
                }
                catch (Exception e) { MessageBox.Show("Error during com port connection: " + e.Message, "Connection error", MessageBoxButtons.OK, MessageBoxIcon.Error); }
            }
            else
            {
                if (!serial_conn.IsOpen)
                {
                    lab_status.Text = "Err! Port alredy closed.";
                    Console.Beep();
                    return false;
                } 
                try
                {
                    serial_conn.Close();
                }
                catch (Exception e) { MessageBox.Show("Error during com port connection: " + e.Message, "Connection error", MessageBoxButtons.OK, MessageBoxIcon.Error); }
           
            }

            if (serial_conn.IsOpen) lab_status.Text = "Connected."; else lab_status.Text = "Disconnected.";
            cmd_connect.Enabled = !serial_conn.IsOpen;
            cmd_disconnect.Enabled = serial_conn.IsOpen;
            return true;
        }

        public void saveSettings()
        {
            IFormatter formatter = new BinaryFormatter();
            Stream st = File.OpenWrite(Application.StartupPath + "/settings.dat");
            formatter.Serialize(st, sets);
            st.Close();
        }

        public void loadSettings()
        {
            IFormatter formatter = new BinaryFormatter();
            if (File.Exists(Application.StartupPath + "/settings.dat"))
            {
                Stream st = File.OpenRead(Application.StartupPath + "/settings.dat");
                sets = (Hashtable)formatter.Deserialize(st);
                st.Close();
            }
        }




        private void cmd_connect_Click(object sender, EventArgs e)
        {

            setConnected(true);
        }

        private void cmd_disconnect_Click(object sender, EventArgs e)
        {
            setConnected(false);
        }

        private void button1_Click(object sender, EventArgs e)
        {

        }

        private void cmd_send_Click(object sender, EventArgs e)
        {
            if (serial_conn.IsOpen)
            {
                byte[] b = Encoding.ASCII.GetBytes(txt_debug_out.Text);

                serial_conn.Write(b, 0, b.Length);

            }
            else
            {
                lab_status.Text = "Err! Not connected!";
            }
        }

        private void cmd_send_message_Click(object sender, EventArgs e)
        {
            if (dg_outgoing.SelectedCells.Count > 0)
            {
                if (serial_conn.IsOpen)
                {
                    canMessage cm = (canMessage)dg_outgoing["cmO", dg_outgoing.CurrentCell.RowIndex].Value;
                    sendMessage(cm);
                    mtOut.flagAsSent(cm);
                }
                else lab_status.Text = "Err! Not connected!";
            }
            else MessageBox.Show("You need to select a message to send.", "Row missing", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void cmd_settings_Click(object sender, EventArgs e)
        {
            settings s = new settings(this);
            s.ShowDialog();
        }

        private void cmd_save_outgoing_Click(object sender, EventArgs e)
        {
            SaveFileDialog sf = new SaveFileDialog();
            sf.AddExtension = true;
            sf.Filter = "Messages (*.can)|*.can";
            sf.Title = "Select where to save message file";
            sf.CheckPathExists = true;
            if (sf.ShowDialog() == DialogResult.OK)
            {
                IFormatter formatter = new BinaryFormatter();
                Stream st = File.OpenWrite(sf.FileName);
                formatter.Serialize(st, mtOut);
                st.Close();
            }
        }

        private void cmd_load_outgoing_Click(object sender, EventArgs e)
        {
            OpenFileDialog fd = new OpenFileDialog();
            fd.Multiselect = false;
            fd.Filter = "Messages (*.can)|*.can";
            fd.Title = "Select message file to load";
            fd.CheckFileExists = true;
            if (fd.ShowDialog() == DialogResult.OK)
            {
                IFormatter formatter = new BinaryFormatter();
                Stream st = File.OpenRead(fd.FileName);
                mtOut = (outgoingKeeper)formatter.Deserialize(st);
                st.Close();
                refreshOutgoing();

            }
        }



    }

    [Serializable]
    public class canMessage
    {
        private uint ident;
        private bool extended;
        private byte data_length;
        private byte[] data = new byte[8];

        public canMessage clone()
        {
            return new canMessage(this.ident, this.extended, this.data_length, this.data);
        }

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
            a.Add(this);
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

    [Serializable]
    public class outgoingKeeper : KeeperTracker
    {
        Hashtable outgoingMessages = new Hashtable();

        public outgoingKeeper() { }

        public void addMessage(canMessage cm,long period)
        {
            outgoingMessages[cm]=new messageState(period);
        }

        public void deleteMessage(canMessage cm)
        {
            outgoingMessages.Remove(cm);
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


        [Serializable]
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

    [Serializable]
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

        [Serializable]
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