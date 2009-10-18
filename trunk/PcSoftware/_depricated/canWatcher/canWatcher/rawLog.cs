using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace canWatcher
{
    public partial class rawLog : Form
    {
        private bool doLog = false;

        public rawLog()
        {
            InitializeComponent();
        }

        private void cmd_clear_log_Click(object sender, EventArgs e)
        {
            lst_raw_log.Items.Clear();
            lst_raw_log.Items.Add("Time\t\tIdent\t\tRTR\tDLC\tData[7]..[0]");
        }

        private void rawLog_Load(object sender, EventArgs e)
        {
            cmd_clear_log_Click(this, EventArgs.Empty);
        }

        internal void addMessage(canMessage newcm)
        {
            lst_raw_log.BeginUpdate();
            if (doLog) lst_raw_log.Items.Insert(1, newcm);
            lst_raw_log.EndUpdate();
        }

        private void cmd_start_log_Click(object sender, EventArgs e)
        {
            setDoLog(true);
        }

        private void setDoLog(bool isOn)
        {
            this.doLog = isOn;
            cmd_start_log.Enabled = !doLog;
            cmd_stop_log.Enabled = !cmd_start_log.Enabled;
        }

        private void cmd_stop_log_Click(object sender, EventArgs e)
        {
            setDoLog(false);
        }

        private void rawLog_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (e.CloseReason == CloseReason.UserClosing)
            {
                setDoLog(false);
                e.Cancel = true;
                this.Hide();
            }
        }

        private void cmd_export_log_Click(object sender, EventArgs e)
        {
            exportFunctions.exportToFile("Message log (*.log)|*.log", "Select where to save message log file",lst_raw_log,exportFunctions.exportFormat.TXT);
        }

        

    }
}