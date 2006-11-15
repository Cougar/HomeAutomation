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
        public enum manageMode {ADD_TARGET,EDIT_TARGET,CHANGE_ID_NID };

        main m;
        LinkedList<nodeTarget> targets;
        manageMode mmode;
        nodeTarget targetToEdit = null;


        public manageTargetsDialog(main m,LinkedList<nodeTarget> targets,manageMode mmode,nodeTarget targetToEdit)
        {
            InitializeComponent();
            this.m = m;
            this.targets = targets;
            this.targetToEdit = targetToEdit;
            setMode(mmode);
        }

        private void setMode(manageMode mmode)
        {
            this.mmode = mmode;
            if (mmode == manageMode.CHANGE_ID_NID)
            {
                txt_expl.Enabled = false;
                this.Text = "Enter new ID and NID";
            }
            if (mmode == manageMode.ADD_TARGET || mmode == manageMode.EDIT_TARGET)
            {
                this.Text = "Add/edit target ID, NID and description";
            }
        }


        private void cmd_save_Click(object sender, EventArgs e)
        {
            uint tid; byte nid;
            if (!uint.TryParse(txt_tid.Text, System.Globalization.NumberStyles.HexNumber, null, out tid) || tid > 0x1FFFFFFF)
            { MessageBox.Show("Target id need to be a hexadecimal number between 0 and 0x1FFFFFFF.","Format error",MessageBoxButtons.OK,MessageBoxIcon.Error); return; }
            if (!byte.TryParse(txt_nid.Text, System.Globalization.NumberStyles.HexNumber, null, out nid) || nid > 0xF)
            { MessageBox.Show("Network id need to be a hexadecimal number between 0 and 0xF.", "Format error", MessageBoxButtons.OK, MessageBoxIcon.Error); return; }

            if (mmode == manageMode.EDIT_TARGET)
            {
                targets.Remove(targetToEdit);
            }

            if (mmode == manageMode.ADD_TARGET || mmode == manageMode.EDIT_TARGET)
            {
                targets.AddFirst(new nodeTarget(txt_expl.Text, tid, nid));
                m.saveSettings();
                m.refreshTargets();
            }

            if (mmode == manageMode.CHANGE_ID_NID)
            {
                m.changeIdNid(tid,nid);
            }

            this.Close();
        }
    }
}