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
    // TODO - History for incomming data per address
    // TODO - Raw log export as xml (maybe).

    public partial class main : Form
    {
        public messageTracker mt = new messageTracker();
        public outgoingKeeper mtOut = new outgoingKeeper();

        public Hashtable sets = new Hashtable();

        public canMessage addLastCm = null;

        public serialCanConnection canconnection;

        public rawLog rlog;

        public dataHistory dhistory = new dataHistory();

        public main()
        {
            InitializeComponent();
        }

        private void main_Load(object sender, EventArgs e)
        {
            loadSettings();
            createSerialCanConnection();
            rlog = new rawLog();
        }

        private void createSerialCanConnection()
        {
            string str;
            try
            {
                if (canconnection != null && canconnection.isConnected()) canconnection.disconnect(out str);
                setConnected(false);
                canconnection = new serialCanConnection((int)sets["baud"], (System.IO.Ports.Parity)Enum.Parse(typeof(System.IO.Ports.Parity), sets["parity"].ToString()), (string)sets["port"], (System.IO.Ports.StopBits)Enum.Parse(typeof(System.IO.Ports.StopBits), sets["stopbits"].ToString()), (int)sets["databits"], false);
            }
            catch (Exception e2) { MessageBox.Show("Error during port initlization. Check settings. Error message: " + e2.Message, "Connection error", MessageBoxButtons.OK, MessageBoxIcon.Error); }

            if (canconnection != null) canconnection.newIncommingCanMessage += new EventHandler(main_newIncommingCanMessage);
        }

        void main_newIncommingCanMessage(object sender, EventArgs e)
        {
          

            this.BeginInvoke((ThreadStart)delegate
            {

                canMessage newcm = canconnection.getMessage();
                if (newcm != null)
                {
                    mt.addMessage(newcm);
                    rlog.addMessage(newcm);
                    dhistory.createHistory(newcm);
                }
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
            string str;
            canconnection.disconnect(out str);
            canconnection = null;
        }

        private void cmd_add_message_Click(object sender, EventArgs e)
        {
            addMessage am = new addMessage(this,null);
            am.ShowDialog();
        }

        private void tmr_check_out_Tick(object sender, EventArgs e)
        {
            if (canconnection!=null && canconnection.isConnected())
            {
                foreach (DictionaryEntry de in mtOut.getMessages())
                {
                    canMessage cm = (canMessage)de.Key;
                    if (mtOut.timeToSend(cm))
                    { 
                        // send
                        canconnection.sendMessage(cm);
                        mtOut.flagAsSent(cm);
                    }
                }
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
            string err;
            if (canconnection != null && connected)
            {
                if (!canconnection.connect(out err)) MessageBox.Show("Error during com port connection: " + err, "Connection error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            else if (canconnection!=null && !connected)
            {
                if (!canconnection.disconnect(out err)) MessageBox.Show("Error during com port disconnection: " + err, "Connection error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            if (canconnection != null && canconnection.isConnected()) lab_status.Text = "Connected."; else lab_status.Text = "Disconnected.";
            cmd_connect.Enabled = (canconnection == null || !canconnection.isConnected());
            cmd_disconnect.Enabled = !cmd_connect.Enabled;
            return true;
        }

        public void saveSettings()
        {
            createSerialCanConnection();
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


        private void cmd_send_Click(object sender, EventArgs e)
        {
            if (canconnection != null && canconnection.isConnected())
            {
                byte[] b = Encoding.ASCII.GetBytes(txt_debug_out.Text);
                canconnection.writeRaw(b, b.Length);
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
                if (canconnection != null && canconnection.isConnected())
                {
                    canMessage cm = (canMessage)dg_outgoing["cmO", dg_outgoing.CurrentCell.RowIndex].Value;
                    canconnection.sendMessage(cm);
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

        private void cmd_clear_incomming_Click(object sender, EventArgs e)
        {
            mt = new messageTracker();
            main_newIncommingCanMessage(this, EventArgs.Empty);
        }

        private void dg_outgoing_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            cmd_edit_message_Click(this, EventArgs.Empty);
        }

        private void cmd_show_raw_log_Click(object sender, EventArgs e)
        {
            rlog.Show();
        }

        private void dg_incomming_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            if (dg_incomming.SelectedCells.Count > 0)
            {
                dataHistoryViewer dhw = new dataHistoryViewer(this, (canMessage)dg_incomming["cmI", dg_incomming.CurrentCell.RowIndex].Value);
                dhw.Show();
            }
            else MessageBox.Show("You need to select a message to view data log for.", "Row missing", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void dg_incomming_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }



    }

    

    

    

}