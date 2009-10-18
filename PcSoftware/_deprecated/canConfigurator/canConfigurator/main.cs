using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace canConfigurator
{
    public partial class main : Form
    {
        Hashtable serialSettings = new Hashtable();
        serialCanConnection canconnection;

        public main()
        {
            InitializeComponent();
            loadSettings();
            createSerialCanConnection();
        }

        private void cmdConfigureTypedefs_Click(object sender, EventArgs e)
        {
            frmTypedefs frmtypes = new frmTypedefs();
            frmtypes.ShowDialog();
        }

        private void cmdConfigureIrTable_Click(object sender, EventArgs e)
        {
            frmIrtable frmirtable = new frmIrtable(this.canconnection);
            frmirtable.ShowDialog();
        }

        private void cmdSerialSettings_Click(object sender, EventArgs e)
        {
            serialSettingsDialog ssd = new serialSettingsDialog(serialSettings);
            ssd.ShowDialog();
            saveSettings();
            createSerialCanConnection();
        }

        private void createSerialCanConnection()
        {
            string str;
            try
            {
                if (canconnection != null && canconnection.isConnected()) canconnection.disconnect(out str);
                canconnection = new serialCanConnection((int)serialSettings["baud"], (System.IO.Ports.Parity)Enum.Parse(typeof(System.IO.Ports.Parity), serialSettings["parity"].ToString()), (string)serialSettings["port"], (System.IO.Ports.StopBits)Enum.Parse(typeof(System.IO.Ports.StopBits), serialSettings["stopbits"].ToString()), (int)serialSettings["databits"], false);
            }
            catch (Exception e2) { MessageBox.Show("Error during port initlization. Check settings. Error message: " + e2.Message, "Connection error", MessageBoxButtons.OK, MessageBoxIcon.Error); }
        }


        private void saveSettings()
        {
            IFormatter formatter = new BinaryFormatter();
            Stream st = File.OpenWrite(Application.StartupPath + "/settings.dat");
            Hashtable temphash = new Hashtable();
            temphash["serialSettings"] = serialSettings;
            formatter.Serialize(st, temphash);
            st.Close();
        }

        private void loadSettings()
        {
            IFormatter formatter = new BinaryFormatter();
            if (File.Exists(Application.StartupPath + "/settings.dat"))
            {
                Stream st = File.OpenRead(Application.StartupPath + "/settings.dat");
                Hashtable temphash = (Hashtable)formatter.Deserialize(st);
                if (temphash != null)
                {
                    serialSettings = (Hashtable)temphash["serialSettings"];
                    if (serialSettings == null) serialSettings = new Hashtable();
                }
                st.Close();
            }
        }

        private void main_Load(object sender, EventArgs e)
        {

        }
    }
}