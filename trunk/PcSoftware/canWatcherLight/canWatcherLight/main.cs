using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.Serialization;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using System.Threading;

namespace canWatcherLight
{
    public partial class main : Form
    {
        public serialCanConnection canconnection;
        public Hashtable sets = new Hashtable();

        public main()
        {
            InitializeComponent();
        }

        private void cmd_clear_Click(object sender, EventArgs e)
        {
            clearIncomming();
        }

        private void main_Load(object sender, EventArgs e)
        {
            loadSettings();
            createSerialCanConnection();
            clearIncomming();
        }

        public void saveSettings()
        {
            createSerialCanConnection();
            IFormatter formatter = new BinaryFormatter();
            Stream st = File.OpenWrite(Application.StartupPath + "/settings.dat");
            formatter.Serialize(st, sets);
            st.Close();
        }

        public void clearIncomming()
        {
            lst_incomming.Items.Clear();
            lst_incomming.Items.Add("time\t\tfunct\tfuncc\tnid\tsid\tDLC\t\tdata[7..0]");
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

            if (canconnection != null) canconnection.newIncommingCanMessage += new EventHandler(canconnection_newIncommingCanMessage);
        }

        void canconnection_newIncommingCanMessage(object sender, EventArgs e)
        {
            this.BeginInvoke((ThreadStart)delegate
            {
                CanPacket cm = null;
                if (canconnection.getMessage(out cm))
                { 
                    lst_incomming.Items.Insert(1,"["+DateTime.Now.ToLongTimeString()+"] "+cm.ToString());
                }
            });
        }

        public bool setConnected(bool connected)
        {
            string err;
            if (canconnection != null && connected)
            {
                if (!canconnection.connect(out err)) MessageBox.Show("Error during com port connection: " + err, "Connection error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            else if (canconnection != null && !connected)
            {
                if (!canconnection.disconnect(out err)) MessageBox.Show("Error during com port disconnection: " + err, "Connection error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            if (canconnection != null && canconnection.isConnected()) lab_status.Text = "Connected."; else lab_status.Text = "Disconnected.";
            cmd_connect.Enabled = (canconnection == null || !canconnection.isConnected());
            cmd_disconnect.Enabled = !cmd_connect.Enabled;
            return true;
        }

        private void cmd_connect_Click(object sender, EventArgs e)
        {
            setConnected(true);
        }

        private void cmd_disconnect_Click(object sender, EventArgs e)
        {
            setConnected(false);
        }

        private void cmd_settings_Click(object sender, EventArgs e)
        {
            settings s = new settings(this,this.sets);
            s.ShowDialog();
        }


    }
}