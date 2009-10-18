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
using System.Threading;

namespace canConfigurator
{
    public partial class frmIrtable : Form
    {
        serialCanConnection sc;
        eeDownloader downloader;

        public frmIrtable(serialCanConnection sc)
        {
            InitializeComponent();
            this.sc = sc;
            string err;
            if (!sc.connect(out err))
            {
                MessageBox.Show("Error opening port. Description: " + err, "Port error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                this.Close();
            }
            sc.newIncommingCanMessage += new EventHandler(sc_newIncommingCanMessage);
            loadTable();
        }

        void sc_newIncommingCanMessage(object sender, EventArgs e)
        {
            try
            {

                this.BeginInvoke((ThreadStart)delegate
                {
                    CanPacket cp;
                    if (sc.getPacket(out cp))
                    {
                        if (cp.type == td.PACKET_TYPE.ptPGM && cp.pgm._class == td.PGM_CLASS.pcSENSOR && cp.pgm.id == (ushort)td.PGM_SENSOR_TYPE.pstIR)
                        {
                            log("New IR. Toggle: " + cp.data[2].ToString() + " 0x" + cp.data[1].ToString("X").PadLeft(2, '0') + cp.data[0].ToString("X").PadLeft(2, '0'));
                            txtIrAddr.Text = cp.data[1].ToString("X");
                            txtIrData.Text = cp.data[0].ToString("X");
                        }
                    }
                });
            }
            catch (InvalidOperationException e2) { Console.WriteLine(e2.Message); }
        }

        private void log(string str)
        {
            labStatus.Text = "Status: " + str;
        }

        private void frmIrtable_Load(object sender, EventArgs e)
        {

        }

        private void frmIrtable_FormClosed(object sender, FormClosedEventArgs e)
        {
            string err;
            sc.disconnect(out err);
        }

        private void cmbClass_SelectedIndexChanged(object sender, EventArgs e)
        {
            typeClass typeclass = (typeClass)cmbClass.SelectedItem;
            if (typeclass != null)
            {
                cmbId.Items.Clear();
                foreach (typeId ti in typeclass.getIds())
                {
                    cmbId.Items.Add(ti);
                }
            }

        }

        private void cmdAdd_Click(object sender, EventArgs e)
        {
            irCommand ircommand = new irCommand(0, 0, 0, new typeClass(""), new typeId(""), new byte[2] { 0, 0 });

            if (!checkFields(out ircommand.addr, out ircommand.data, out ircommand.repeat, out ircommand.pgm_class, out ircommand.pgm_id, out ircommand.pgm_data)) return;

            lstIrcommands.Items.Add(ircommand);
        }

        private void cmdUpdate_Click(object sender, EventArgs e)
        {
            irCommand ircommand = (irCommand)lstIrcommands.SelectedItem;
            if (ircommand == null)
            {
                MessageBox.Show("You need to select a command to update.", "Missing command", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            byte addr, data, repeat;
            byte[] pgm_data;
            typeClass pgm_class;
            typeId pgm_id;

            if (!checkFields(out addr, out data, out repeat, out pgm_class, out pgm_id, out pgm_data)) return;

            ircommand.addr = addr;
            ircommand.data = data;
            ircommand.repeat = repeat;
            ircommand.pgm_data[0] = pgm_data[0];
            ircommand.pgm_data[1] = pgm_data[1];
            ircommand.pgm_class = pgm_class;
            ircommand.pgm_id = pgm_id;

            List<irCommand> irtemp = new List<irCommand>();
            foreach (irCommand ic in lstIrcommands.Items) irtemp.Add(ic);
            lstIrcommands.Items.Clear();
            foreach (irCommand ic in irtemp) lstIrcommands.Items.Add(ic);

        }

        private void cmdDelete_Click(object sender, EventArgs e)
        {
            irCommand ircommand = (irCommand)lstIrcommands.SelectedItem;
            if (ircommand == null)
            {
                MessageBox.Show("You need to select a command to delete.", "Missing command", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            lstIrcommands.Items.Remove(ircommand);
        }

        private bool checkFields(out byte addr, out byte data, out byte repeat, out typeClass pgm_class, out typeId pgm_id, out byte[] pgm_data)
        {
            addr = 0;
            data = 0;
            repeat = 0;
            pgm_class = new typeClass("");
            pgm_id = new typeId("");
            pgm_data = new byte[irCommand.PGM_DATA_LENGTH];

            if (!byte.TryParse(txtIrAddr.Text, System.Globalization.NumberStyles.HexNumber, null, out addr))
            {
                MessageBox.Show("IR address field does not contains a hex value between 0 and FF.", "Input error", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return false;
            }

            if (!byte.TryParse(txtIrData.Text, System.Globalization.NumberStyles.HexNumber, null, out data))
            {
                MessageBox.Show("IR data field does not contains a hex value between 0 and FF.", "Input error", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return false;
            }

            if (!byte.TryParse(txtIrRepeat.Text, System.Globalization.NumberStyles.HexNumber, null, out repeat))
            {
                MessageBox.Show("IR repeat field does not contains a hex value between 0 and FF.", "Input error", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return false;
            }

            pgm_class = (typeClass)cmbClass.SelectedItem;
            if (pgm_class == null)
            {
                MessageBox.Show("You need to select a class.", "Input error", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return false;
            }

            pgm_id = (typeId)cmbId.SelectedItem;
            if (pgm_id == null)
            {
                MessageBox.Show("You need to select a ID.", "Input error", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return false;
            }

            if (!byte.TryParse(txtData1.Text, System.Globalization.NumberStyles.HexNumber, null, out pgm_data[1]))
            {
                MessageBox.Show("Data1 field does not contains a hex value between 0 and FF.", "Input error", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return false;
            }

            if (!byte.TryParse(txtData0.Text, System.Globalization.NumberStyles.HexNumber, null, out pgm_data[0]))
            {
                MessageBox.Show("Data1 field does not contains a hex value between 0 and FF.", "Input error", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return false;
            }


            return true;
        }

        private void cmdDownload_Click(object sender, EventArgs e)
        {
            byte MY_ID = 0x91;
            byte TARGET_ID = 0x36;
            ulong eestart = 0;

            if (downloader != null) { log("Process in progress..."); return; }

            byte[] eebytes = new byte[eeDownloader.MAX_EE_SIZE];
            ulong eelength = 0;

            foreach (irCommand ic in lstIrcommands.Items)
            {
                byte[] icbytes = new byte[irCommand.BYTES_LENGTH];
                Array.Copy(ic.getBytes(), icbytes, irCommand.BYTES_LENGTH);
                Array.Reverse(icbytes);
                Array.Copy(icbytes, 0, eebytes, (int)eelength, (int)irCommand.BYTES_LENGTH);
                eelength += irCommand.BYTES_LENGTH;
            }

            if (eelength == 0) { log("Nothing to download."); return; }


            if (!sc.isConnected()) { log("Port is not open."); return; }

            downloader = new eeDownloader(eebytes, eelength, eestart, eeDownloader.eeType.INTERNAL, sc, MY_ID, TARGET_ID);

            if (!downloader.go()) { log("Error starting download. Reason: " + downloader.lastError); return; }

            sc.newIncommingCanMessage -= sc_newIncommingCanMessage;

            downloader.threadAbort += new EventHandler(downloader_threadAbort);

            log("Downloading...");

            saveTable();
        }

        void downloader_threadAbort(object sender, EventArgs e)
        {
            sc.newIncommingCanMessage += sc_newIncommingCanMessage;

            try
            {
                this.BeginInvoke((ThreadStart)delegate
                {
                    threadAbortEvent e2 = (threadAbortEvent)e;
                    switch (e2.getAbortMode())
                    {
                        case eeDownloader.abortMode.PROGRAM:
                            log("Download done. Time spent: " + e2.getTimeS().ToString() + " seconds. " + Math.Round(e2.getBps(), 0) + " Bps");
                            break;

                        case eeDownloader.abortMode.USER:
                            log("Download aborted.");
                            break;
                    }

                    downloader = null;
                });
            }
            catch (InvalidOperationException) { }


        }

        private void saveTable()
        {
            IFormatter formatter = new BinaryFormatter();
            Stream st = File.OpenWrite(Application.StartupPath + "/irtable.dat");
            Hashtable temphash = new Hashtable();
            List<irCommand> ircommands = new List<irCommand>();
            foreach (irCommand ic in lstIrcommands.Items)
            {
                ircommands.Add(ic);
            }
            temphash["ircommandhash"] = ircommands;
            formatter.Serialize(st, temphash);
            st.Close();
        }

        private void loadTable()
        {
            // IR-table
            IFormatter formatter = new BinaryFormatter();
            if (File.Exists(Application.StartupPath + "/irtable.dat"))
            {
                Stream st = File.OpenRead(Application.StartupPath + "/irtable.dat");
                Hashtable temphash = null;
                if (st.Length > 0) temphash = (Hashtable)formatter.Deserialize(st);
                if (temphash != null)
                {
                    List<irCommand> ircommands = (List<irCommand>)temphash["ircommandhash"];
                    if (ircommands != null)
                    {
                        foreach (irCommand ic in ircommands)
                        {
                            lstIrcommands.Items.Add(ic);
                        }
                    }
                }
                st.Close();
            }

            // Typedefs
            formatter = new BinaryFormatter();
            if (File.Exists(Application.StartupPath + "/typedefs.dat"))
            {
                Stream st = File.OpenRead(Application.StartupPath + "/typedefs.dat");
                Hashtable temphash = (Hashtable)formatter.Deserialize(st);
                if (temphash != null)
                {
                    List<typeClass> classes = (List<typeClass>)temphash["classhash"];
                    if (classes != null)
                    {
                        foreach (typeClass tc in classes)
                        {
                            cmbClass.Items.Add(tc);
                        }
                    }
                }
                st.Close();
            }
        }

        private void lstIrcommands_SelectedIndexChanged(object sender, EventArgs e)
        {
            irCommand ircommand = (irCommand)lstIrcommands.SelectedItem;
            if (ircommand != null)
            {
                txtIrAddr.Text = ircommand.addr.ToString("X");
                txtIrData.Text = ircommand.data.ToString("X");
                txtIrRepeat.Text = ircommand.repeat.ToString("X");
                txtData0.Text = ircommand.pgm_data[0].ToString("X");
                txtData1.Text = ircommand.pgm_data[1].ToString("X");
                cmbClass.SelectedItem = ircommand.pgm_class;
                cmbId.SelectedItem = ircommand.pgm_id;
            }
        }

        private void cmdAbort_Click(object sender, EventArgs e)
        {
            if (downloader != null) downloader.abort();
        }
    }

    [Serializable]
    internal class irCommand
    {
        public const int PGM_DATA_LENGTH = 2;
        public const int BYTES_LENGTH = 8;

        public byte repeat;
        public byte addr;
        public byte data;
        public typeClass pgm_class;
        public typeId pgm_id;
        public byte[] pgm_data = new byte[PGM_DATA_LENGTH];

        public irCommand(byte repeat, byte addr, byte data, typeClass pgm_class, typeId pgm_id, byte[] pgm_data)
        {
            this.repeat = repeat;
            this.addr = addr;
            this.data = data;
            this.pgm_class = pgm_class;
            this.pgm_id = pgm_id;
            for (int i = 0; i < this.pgm_data.Length; i++) this.pgm_data[i] = pgm_data[i];
        }

        public override string ToString()
        {
            return "IR 0x" + this.addr.ToString("X").PadLeft(2, '0') + this.data.ToString("X").PadLeft(2, '0') + " => " + this.pgm_class.name + "." + this.pgm_id.name + " [0x" + this.pgm_data[1].ToString("X").PadLeft(2, '0') + "][0x" + this.pgm_data[0].ToString("X").PadLeft(2, '0') + "] " + this.repeat + " times.";
        }

        public byte[] getBytes()
        {
            byte[] b = new byte[BYTES_LENGTH];
            b[7] = this.repeat;
            b[6] = this.addr;
            b[5] = this.data;
            b[4] = (byte)(0xF & pgm_class.value);
            b[3] = (byte)((0xFF00 & pgm_id.value) >> 8);
            b[2] = (byte)(0xFF & pgm_id.value);
            b[1] = pgm_data[1];
            b[0] = pgm_data[0];
            return b;
        }

    }

}