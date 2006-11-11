using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace canBootloader
{
    public partial class manageTargetsDialog : Form
    {
        main m;
        LinkedList<nodeTarget> targets;

        public manageTargetsDialog(main m,LinkedList<nodeTarget> targets)
        {
            InitializeComponent();
            this.m = m;
            this.targets = targets;
        }

        private void manageTargetsDialog_Load(object sender, EventArgs e)
        {

        }

        private void cmd_save_Click(object sender, EventArgs e)
        {
            uint tid; byte nid;
            if (!uint.TryParse(txt_tid.Text, System.Globalization.NumberStyles.HexNumber, null, out tid) || tid > 0x1FFFFFFF)
            { MessageBox.Show("Target id need to be a hexadecimal number between 0 and 0x1FFFFFFF.","Format error",MessageBoxButtons.OK,MessageBoxIcon.Error); return; }
            if (!byte.TryParse(txt_nid.Text, System.Globalization.NumberStyles.HexNumber, null, out nid) || nid > 0xF)
            { MessageBox.Show("Network id need to be a hexadecimal number between 0 and 0xF.", "Format error", MessageBoxButtons.OK, MessageBoxIcon.Error); return; }

            targets.AddFirst(new nodeTarget(txt_expl.Text, tid, nid));

            m.saveSettings();
            m.refreshTargets();
            this.Close();
        }
    }
}