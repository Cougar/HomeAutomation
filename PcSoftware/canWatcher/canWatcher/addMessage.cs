using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace canWatcher
{
    public partial class addMessage : Form
    {
        TextBox[] idBoxes = new TextBox[4];
        TextBox[] dataBoxes = new TextBox[8];
        main m;
        canMessage cmEdit;

        public addMessage(main m, canMessage cmEdit)
        {
            InitializeComponent();
            idBoxes[0] = txt_id0;
            idBoxes[1] = txt_id1;
            idBoxes[2] = txt_id2;
            idBoxes[3] = txt_id3;

            dataBoxes[0] = txt_d0;
            dataBoxes[1] = txt_d1;
            dataBoxes[2] = txt_d2;
            dataBoxes[3] = txt_d3;
            dataBoxes[4] = txt_d4;
            dataBoxes[5] = txt_d5;
            dataBoxes[6] = txt_d6;
            dataBoxes[7] = txt_d7;

            this.m = m;
            this.cmEdit = cmEdit;
            if (cmEdit != null)
            {
                cmd_add.Text = "Save";
                this.Text = "Edit message";
                loadCm(cmEdit);
            }
            else if(m.addLastCm!=null)
            {
                loadCm(m.addLastCm);
                txt_period.Text = "1000";
            }
        }

        private void loadCm(canMessage cmEdit)
        {
            
            chk_extended.Checked = cmEdit.getExtended();
            txt_period.Text = m.mtOut.getPeriod(cmEdit).ToString();
            uint ident = cmEdit.getIdent();
            data_length.Value = cmEdit.getDataLength();
            byte[] data = cmEdit.getData();
            for (int i = 0; i < 4; i++)
            {
                uint er = (ident >> (8 * i));
                byte b = ((byte)(er&0x000000FF));
                idBoxes[i].Text = b.ToString("X").PadLeft(2, '0');
            }
            for (int i = 0; i < 8; i++) dataBoxes[i].Text = data[i].ToString("X").PadLeft(2, '0');
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void data_length_ValueChanged(object sender, EventArgs e)
        {
            for (int i = 0; i < dataBoxes.Length; i++)
            {
                if (data_length.Value > i) dataBoxes[i].Enabled = true;
                else dataBoxes[i].Enabled = false;
            }
        }

        private void chk_extended_CheckedChanged(object sender, EventArgs e)
        {
            idBoxes[2].Enabled = idBoxes[3].Enabled = chk_extended.Checked;
        }

        private void cmd_add_Click(object sender, EventArgs e)
        {
   
            byte[] id = new byte[4];
            byte[] data = new byte[8];
            long period = 0;

            if (checkID(out id) && checkData(out data) && checkPeriod(out period))
            { 
                // Add message
                canMessage cm = new canMessage(BitConverter.ToUInt32(id,0),chk_extended.Checked,(byte)data_length.Value,data);
                if (cmEdit != null) m.mtOut.deleteMessage(cmEdit);
                if (cmEdit == null) m.addLastCm = cm.clone();
                m.mtOut.addMessage(cm, period);
                m.refreshOutgoing();
                this.Close();
            }
        }

        private bool checkPeriod(out long period)
        {
            if (!long.TryParse(txt_period.Text, out period))
            {
                MessageBox.Show("Period does not contains an number.", "Format error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return false;
            }

            if (period<10)
            {
                MessageBox.Show("Period cannot be less then 10 ms.", "Format error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return false;
            }

            return true;
        }

        private bool checkBoxes(TextBox[] tb, string kind,out byte[] b)
        {
            b = new byte[tb.Length];

            for (int i = 0; i < tb.Length; i++)
            {
                if (!byte.TryParse(tb[i].Text,System.Globalization.NumberStyles.HexNumber,null, out b[i]))
                {
                    MessageBox.Show(kind+" box #" + i + " does not contains an hexadecimal number.", "Format error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    return false;
                }
            }

            return true;
        }

        public bool checkID(out byte[] id)
        {
            if (!checkBoxes(idBoxes, "ID", out  id)) return false;

            if ((chk_extended.Checked && id[3] > 31) || (!chk_extended.Checked && id[1] > 7))
            {
                MessageBox.Show("ID box #" + (chk_extended.Checked?"3":"1") + " has a to large value.", "Value error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return false;
            }

            return true;
        }

        public bool checkData(out byte[] data)
        {
            if (!checkBoxes(dataBoxes, "Data", out  data)) return false;

            return true;
        }

        private void addMessage_Load(object sender, EventArgs e)
        {

        }

        private void cmd_cancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }

    }
}