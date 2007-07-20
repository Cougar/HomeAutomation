using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;

namespace canConfigurator
{
    public partial class serialSettingsDialog : Form
    {
        Hashtable sets;

        public serialSettingsDialog(Hashtable sets)
        {
            InitializeComponent();
            this.sets = sets;
        }

        private void cmd_add_Click(object sender, EventArgs e)
        {
            int baud;
            if (!int.TryParse(txt_baud.Text, out baud) || baud < 1)
            {
                MessageBox.Show("Baud rate must be a integer value larger than one.", "Input error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            sets["baud"] = (int)baud;
            sets["port"] = cmb_port.SelectedItem.ToString();
            sets["parity"] = cmb_parity.SelectedItem.ToString();
            sets["stopbits"] = cmb_stopbits.SelectedItem.ToString();
            sets["databits"] = (int)cmb_databits.SelectedItem;
            this.Close();
        }

        private void cmd_cancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void settings_Load(object sender, EventArgs e)
        {
            for (int i = 1; i < 43; i++) cmb_port.Items.Add("COM"+i);
            string[] str = new string[5] { System.IO.Ports.Parity.Even.ToString(), System.IO.Ports.Parity.Mark.ToString(), System.IO.Ports.Parity.None.ToString(), System.IO.Ports.Parity.Odd.ToString(), System.IO.Ports.Parity .Space.ToString()};
            cmb_parity.Items.AddRange(str);
            str = new string[4] {System.IO.Ports.StopBits.None.ToString(),System.IO.Ports.StopBits.One.ToString(),System.IO.Ports.StopBits.OnePointFive.ToString(),System.IO.Ports.StopBits.Two.ToString() };
            cmb_stopbits.Items.AddRange(str);
            for (int i = 1; i < 11; i++) cmb_databits.Items.Add(i);

            if (sets["baud"] == null) txt_baud.Text = "19200"; else txt_baud.Text = sets["baud"].ToString();
            if (sets["port"] == null) cmb_port.SelectedIndex = 1; else cmb_port.SelectedItem = sets["port"];
            if (sets["parity"] == null) cmb_parity.SelectedIndex = 2; else cmb_parity.SelectedItem = sets["parity"];
            if (sets["stopbits"] == null) cmb_stopbits.SelectedIndex = 1; else cmb_stopbits.SelectedItem = sets["stopbits"];
            if (sets["databits"] == null) cmb_databits.SelectedIndex = 7; else cmb_databits.SelectedItem = sets["databits"];

        }
    }
}