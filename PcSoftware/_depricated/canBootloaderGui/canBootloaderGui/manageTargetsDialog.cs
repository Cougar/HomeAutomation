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
        public enum manageMode {ADD_TARGET,EDIT_TARGET,CHANGE_ID };

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
            if (mmode == manageMode.CHANGE_ID)
            {
                txt_expl.Enabled = false;
                this.Text = "Enter new ID";
            }
            if (mmode == manageMode.ADD_TARGET || mmode == manageMode.EDIT_TARGET)
            {
                this.Text = "Add/edit target ID and description";
            }
        }


        private void cmd_save_Click(object sender, EventArgs e)
        {
            byte tid;
            if (!byte.TryParse(txt_tid.Text, System.Globalization.NumberStyles.HexNumber, null, out tid) || tid > 0xFF)
            { MessageBox.Show("Target id need to be a hexadecimal number between 0 and 0xFF.","Format error",MessageBoxButtons.OK,MessageBoxIcon.Error); return; }

            if (mmode == manageMode.EDIT_TARGET)
            {
                targets.Remove(targetToEdit);
            }

            if (mmode == manageMode.ADD_TARGET || mmode == manageMode.EDIT_TARGET)
            {
                targets.AddFirst(new nodeTarget(txt_expl.Text, tid));
                m.saveSettings();
                m.refreshTargets();
            }

            if (mmode == manageMode.CHANGE_ID)
            {
                m.changeId(tid);
            }

            this.Close();
        }
    }
}