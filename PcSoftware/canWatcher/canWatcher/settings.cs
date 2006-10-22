using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace canWatcher
{
    public partial class settings : Form
    {
        main m;

        public settings(main m)
        {
            InitializeComponent();
            this.m = m;
        }

        private void cmd_add_Click(object sender, EventArgs e)
        {
            m.sets["port"] = cmb_port.SelectedItem.ToString();
            m.sets["parity"] = cmb_parity.SelectedItem.ToString();
            m.sets["stopbits"] = cmb_stopbits.SelectedItem.ToString();
            m.sets["databits"] = (int)cmb_databits.SelectedItem;
            m.saveSettings();
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

            if (m.sets["port"] == null) cmb_port.SelectedIndex = 1; else cmb_port.SelectedItem = m.sets["port"];
            if (m.sets["parity"] == null) cmb_parity.SelectedIndex = 2; else cmb_parity.SelectedItem = m.sets["parity"];
            if (m.sets["stopbits"] == null) cmb_stopbits.SelectedIndex = 1; else cmb_stopbits.SelectedItem = m.sets["stopbits"];
            if (m.sets["databits"] == null) cmb_databits.SelectedIndex = 7; else cmb_databits.SelectedItem = m.sets["databits"];

        }
    }
}