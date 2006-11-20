using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Threading;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace canBootloader
{
    public partial class main : Form
    {

        private HexFile hf;
        private SerialConnection sc;
        private Downloader dl;
        private string currentLoadedFile = "";
        private nodeTarget currentTarget;
        private uint myid;

        private uint tmp_new_id = 0;
        private byte tmp_new_nid = 0;

        private enum TrayIcon {NORMAL,DOWNLOADING,FAIL,OK};
        private System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(trayIcons));
        private System.Timers.Timer iconTimer = new System.Timers.Timer(3000.0);
        private System.Timers.Timer downloadTimeout = new System.Timers.Timer(5000.0);

        private LinkedList<string> recentFiles = new LinkedList<string>();
        public Hashtable serialSettings = new Hashtable();
        public LinkedList<nodeTarget> targets = new LinkedList<nodeTarget>();

        public main()
        {
            InitializeComponent();
            iconTimer.Elapsed += new System.Timers.ElapsedEventHandler(iconTimer_Elapsed);
            downloadTimeout.Elapsed += new System.Timers.ElapsedEventHandler(downloadTimeout_Elapsed);
            iconTimer.Enabled = false;
            downloadTimeout.Enabled = false;
        }

        void downloadTimeout_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            if (dl != null && !dl.foundNode())
            {
                menu_action_abort_Click(this, EventArgs.Empty);
                this.BeginInvoke((ThreadStart)delegate { log("Download timeout (" + (downloadTimeout.Interval / 1000) + " s), no reply from target."); });
                downloadTimeout.Enabled = false;
                setTrayIcon(TrayIcon.FAIL);
            }
            else downloadTimeout.Enabled = false;
        }

        void iconTimer_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            setTrayIcon(TrayIcon.NORMAL);
            iconTimer.Enabled = false;
        }

        private void main_Load(object sender, EventArgs e)
        {
            loadSettings();
            refreshRecentFiles();
            refreshTargets();
        }

        private void log(string str)
        {
            if (str.Equals("")) { txtLog.Text += "\r\n"; return; }
            string lstr = "[" + DateTime.Now.ToLongTimeString() + "] " + str;
            txtLog.Text += lstr + "\r\n";
        }

        private void menu_file_quit_Click(object sender, EventArgs e)
        {
            if(dl!=null) dl.abort();
            Application.Exit();
        }

        private void menu_file_load_recent_item_Click(object sender, EventArgs e)
        {
            recentItem tsmi = (recentItem)sender;
            loadFile(tsmi.filepath);
        }


        private void loadFile(string filepath)
        { 

            // If file does not exist, show open file dialog.
            if (!File.Exists(filepath))
            {
                OpenFileDialog fd = new OpenFileDialog();
                fd.Multiselect = false;
                fd.Filter = "inhex32 files (*.hex)|*.hex";
                fd.Title = "Select HEX file to download";
                fd.CheckFileExists = true;
                if (fd.ShowDialog() == DialogResult.OK)
                {
                    FileStream file = new FileStream(fd.FileName, FileMode.Open, FileAccess.Read);
                    StreamReader sr = new StreamReader(file);
                    filepath = fd.FileName;
                    sr.Close();
                }
                else return;
            }

            hf = new HexFile();
            if (dl != null) dl.abort(); dl = null;
            if (hf.loadHex(filepath))
            {
                currentLoadedFile = filepath;

                // Handle recent list.
                if (recentFiles.Contains(filepath))
                {
                    recentFiles.Remove(filepath);
                }
                recentFiles.AddFirst(filepath);
                refreshRecentFiles();
                saveSettings();

                log("");
                log("File " + filepath + " loaded.");
                log("Size: " + hf.getLength().ToString() + " bytes, end adress " + hf.getAddrUpper().ToString() + "(0x" + hf.getAddrUpper().ToString("X") + ").");
            }
            else { log("Error loading " + filepath + "!"); hf = null;  }
        }

        private void refreshRecentFiles()
        {
            menu_file_load_recent.DropDownItems.Clear();

            while (recentFiles.Count > 5) { recentFiles.RemoveLast(); }
            int i = 0;
            foreach (string s in recentFiles)
            {
                recentItem ri = new recentItem(s);
                ri.Text = (i + 1).ToString() + ". " + shortString(s);
                ri.Click += new EventHandler(menu_file_load_recent_item_Click);
                menu_file_load_recent.DropDownItems.Add(ri);
                i++;
            }
        }

        public void refreshTargets()
        {
            menu_target.DropDownItems.Clear();

            menu_target.DropDownItems.Add(new ToolStripMenuItem("Add target...",null,menu_target_add_Click,"menu_target_add"));
            menu_target.DropDownItems.Add(new ToolStripSeparator());

            foreach (nodeTarget nt in targets)
            {
                targetItem ti = new targetItem(nt);
                menu_target.DropDownItems.Add(ti);
                ti.Click += new EventHandler(targetItem_Click);
                ti.DoubleClick += new EventHandler(ti_DoubleClick);
                ti.Checked = ti.getNodeTarget().Equals(currentTarget);
                targetItemDelete tidel = new targetItemDelete(ti);
                tidel.Click += new EventHandler(tidel_Click);
                ti.DropDownItems.Add(tidel);
            }

        }

        void ti_DoubleClick(object sender, EventArgs e)
        {
            manageTargetsDialog mtd = new manageTargetsDialog(this, targets, manageTargetsDialog.manageMode.EDIT_TARGET, ((targetItem)sender).getNodeTarget());
            mtd.ShowDialog();
        }

        void tidel_Click(object sender, EventArgs e)
        {
            targetItemDelete tidel = (targetItemDelete)sender;
            if (tidel.getTargetItem().getNodeTarget().Equals(currentTarget)) currentTarget = null;
            targets.Remove(tidel.getTargetItem().getNodeTarget());
            refreshTargets();
            saveSettings();
        }

        private void targetItem_Click(object sender, EventArgs e)
        {
            targetItem ti = (targetItem)sender;
            currentTarget = ti.getNodeTarget();
            for (int i = 2; i < menu_target.DropDownItems.Count; i++)
            {
                ((targetItem)menu_target.DropDownItems[i]).Checked = false;
            }
            ti.Checked = true;
            saveSettings();
        }

        private string shortString(string str)
        {
            return str;
        }

        private void download(Downloader.downloadMode downloadmode)
        {
            if (dl != null) { log("Process in progress..."); return; }

            if (downloadmode == Downloader.downloadMode.PROGRAM)
            {
                loadFile(currentLoadedFile);

                if (hf == null) { log("No hexfile loaded."); return; }
            }

            if (currentTarget == null) { log("No target selected."); return; }

            sc = new SerialConnection();
            try
            {
                sc.setConfiguration((int)serialSettings["baud"], (System.IO.Ports.Parity)Enum.Parse(typeof(System.IO.Ports.Parity), serialSettings["parity"].ToString()), (string)serialSettings["port"], (System.IO.Ports.StopBits)Enum.Parse(typeof(System.IO.Ports.StopBits), serialSettings["stopbits"].ToString()), (int)serialSettings["databits"], false);
            }
            catch (Exception e) { log("Error setting serial connection settings: " + e.Message); setTrayIcon(TrayIcon.FAIL); return; }
            if (!sc.open()) { log("Error opening port."); setTrayIcon(TrayIcon.FAIL); return; }

            dl = new Downloader(hf, sc, myid, currentTarget.getNid(), currentTarget.getTargetId(), downloadmode, tmp_new_id, tmp_new_nid);
            if (!dl.go()) { log("Error downloading/ID change..."); setTrayIcon(TrayIcon.FAIL); return; }
            setTrayIcon(TrayIcon.DOWNLOADING);
            downloadTimeout.Stop();
            downloadTimeout.Start();
            downloadTimeout.Enabled = true;
            if (downloadmode == Downloader.downloadMode.PROGRAM)
            {
                log("Downloading...");
            }
            else if (downloadmode == Downloader.downloadMode.CHANGE_ID_NID) 
            {
                log("Changeing ID/NID.");
            }
            dl.threadAbort += new EventHandler(dl_threadAbort);

        }

        private void setTrayIcon(TrayIcon ti)
        {
            switch (ti)
            { 
                case TrayIcon.OK:
                    this.nicon.Icon = ((System.Drawing.Icon)(resources.GetObject("canBootloader_done_ok")));
                    iconTimer.Stop();
                    iconTimer.Start();
                    iconTimer.Enabled = true;
                    break;
                case TrayIcon.DOWNLOADING:
                    this.nicon.Icon = ((System.Drawing.Icon)(resources.GetObject("canBootloader_downloading")));
                    break;
                case TrayIcon.FAIL:
                    this.nicon.Icon = ((System.Drawing.Icon)(resources.GetObject("canBootloader_done_fail")));
                    iconTimer.Stop();
                    iconTimer.Start();
                    iconTimer.Enabled = true;
                    break;
                default:
                    this.nicon.Icon = ((System.Drawing.Icon)(resources.GetObject("canBootloader_B")));
                    break;
            }
        }


        void dl_threadAbort(object sender, EventArgs e)
        {
            try
            {
                this.BeginInvoke((ThreadStart)delegate
                {
                    threadAbortEvent e2 = (threadAbortEvent)e;
                    switch(e2.getAbortMode())
                    {
                        case Downloader.abortMode.CHANGE_ID_NID:
                            log("ID and NID changed.");
                            setTrayIcon(TrayIcon.OK);
                            break;

                        case Downloader.abortMode.PROGRAM:
                            log("Download done. Time spent: " + e2.getTimeS().ToString() + " seconds. " + Math.Round(e2.getBps(), 0) + " Bps");
                            setTrayIcon(TrayIcon.OK);
                            break;

                        case Downloader.abortMode.USER:
                            log("Download aborted.");
                            break;
                      
                    }

                    sc.close(); sc = null;
                    dl = null;
                    downloadTimeout.Enabled = false;
                });
            }
            catch (InvalidOperationException) { }
        }

        private void menu_file_load_Click(object sender, EventArgs e)
        {
            loadFile("");
        }

        private void menu_action_download_Click(object sender, EventArgs e)
        {
            download(Downloader.downloadMode.PROGRAM);
        }

        public void saveSettings()
        { 
            Hashtable saveblock = new Hashtable();
            saveblock["recentFiles"] = recentFiles;
            saveblock["serialSettings"] = serialSettings;
            saveblock["targets"] = targets;
            saveblock["currentTarget"] = currentTarget;
            saveblock["myid"] = myid;
            saveblock["currentLoadedFile"] = currentLoadedFile;

            IFormatter formatter = new BinaryFormatter();
            Stream st = File.OpenWrite(Application.StartupPath + "/settings.dat");
            formatter.Serialize(st, saveblock);
            st.Close();
        }

        public void loadSettings()
        {
            Hashtable saveblock;
            IFormatter formatter = new BinaryFormatter();
            if (File.Exists(Application.StartupPath + "/settings.dat"))
            {
                try
                {
                    Stream st = File.OpenRead(Application.StartupPath + "/settings.dat");
                    saveblock = (Hashtable)formatter.Deserialize(st);
                    st.Close();
                    recentFiles = (LinkedList<string>)saveblock["recentFiles"];
                    serialSettings = (Hashtable)saveblock["serialSettings"];
                    targets = (LinkedList<nodeTarget>)saveblock["targets"];
                    currentTarget = (nodeTarget)saveblock["currentTarget"];
                    currentLoadedFile = (string)saveblock["currentLoadedFile"];
                    myid = (uint)saveblock["myid"];
                }
                catch (Exception) { }
            }
        }

        private void menu_settings_serialconnection_Click(object sender, EventArgs e)
        {
            serialSettingsDialog ssd = new serialSettingsDialog(this, serialSettings);
            ssd.ShowDialog();
        }

        private void menu_action_abort_Click(object sender, EventArgs e)
        {
            if (dl != null) dl.abort();
            setTrayIcon(TrayIcon.NORMAL);
        }

        private void txtLog_TextChanged(object sender, EventArgs e)
        {
            txtLog.SelectionStart = txtLog.Text.Length;
            txtLog.ScrollToCaret();
            txtLog.DeselectAll();
        }

        private void nicon_DoubleClick(object sender, EventArgs e)
        {
            MouseEventArgs mea = (MouseEventArgs)e;
            if (mea.Button == MouseButtons.Left) download(Downloader.downloadMode.PROGRAM);
        }


        private void menu_target_add_Click(object sender, EventArgs e)
        {
            manageTargetsDialog mtd = new manageTargetsDialog(this, targets,manageTargetsDialog.manageMode.ADD_TARGET,null);
            mtd.ShowDialog();
        }


        private void menu_settings_myid_DropDownClosed(object sender, EventArgs e)
        {
            if (!uint.TryParse(menu_settings_myid_txt.Text, System.Globalization.NumberStyles.HexNumber, null, out myid) || myid > 0x1FFFFFFF)
            { MessageBox.Show("Id need to be a hexadecimal number between 0 and 0x1FFFFFFF.", "Format error", MessageBoxButtons.OK, MessageBoxIcon.Error); return; }
            saveSettings();
        }

        private void menu_settings_DropDownOpened(object sender, EventArgs e)
        {
            menu_settings_myid.Text = "My ID: 0x"+myid.ToString("X");
            menu_settings_myid_txt.Text = myid.ToString("X");
        }

        private void main_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (e.CloseReason == CloseReason.UserClosing)
            {
                e.Cancel = true;
                this.WindowState = FormWindowState.Minimized;
            }
        }

        private void menu_action_change_target_idnid_Click(object sender, EventArgs e)
        {
            manageTargetsDialog mtd = new manageTargetsDialog(this, targets, manageTargetsDialog.manageMode.CHANGE_ID_NID, null);
            mtd.ShowDialog();
        }


        internal void changeIdNid(uint tid, byte nid)
        {
            tmp_new_id = tid;
            tmp_new_nid = nid;
            download(Downloader.downloadMode.CHANGE_ID_NID);
        }
    }

    internal class recentItem : ToolStripMenuItem
    {
        public string filepath;
        public recentItem(string filepath) { this.filepath = filepath; }
        public recentItem() { filepath = ""; }
    }

    [Serializable]
    public class nodeTarget
    {
        private string expl;
        private uint targetId;
        private byte nid;
        public nodeTarget() { expl = "(NULL)";  }
        public nodeTarget(string expl, uint targetId, byte nid) 
        {
            this.expl = expl;
            this.targetId = targetId;
            this.nid = nid;
        }
        public string getExpl() { return this.expl; }
        public void setExpl(string expl) { this.expl = expl; }
        public uint getTargetId() { return this.targetId; }
        public byte getNid() { return this.nid; }

        public override bool Equals(object obj)
        {
            if (obj == null) return false;
            nodeTarget ti = (nodeTarget)obj;
            return (ti.getTargetId() == this.targetId && ti.getNid() == this.nid);
        }
        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

    }


    public class targetItemDelete : ToolStripMenuItem
    {
        targetItem ti;

        public targetItemDelete(targetItem ti) { this.ti = ti; }
        public targetItem getTargetItem() { return this.ti; }
        public override string Text
        {
            get
            {
                return "Delete";
            }
            set
            {
            }
        }
    }

    
    public class targetItem : ToolStripMenuItem
    {
        private nodeTarget nt;

        public targetItem(nodeTarget nt) 
        {
            this.nt = nt;
            if (nt == null) nt = new nodeTarget();
        }

        public nodeTarget getNodeTarget() { return nt; }

        public override string ToString()
        {
            return this.nt.getExpl() + " (0x " + this.nt.getNid().ToString("X").PadLeft(1, '0') + ", 0x" + this.nt.getTargetId().ToString("X").PadLeft(3, '0') + ")";
        }
        public override string Text
        {
            get
            {
                return this.ToString();
            }
            set
            {
                this.nt.setExpl(value);
            }
        }
        public override bool Equals(object obj)
        {
            if (obj == null) return false;
            targetItem ti = (targetItem)obj;
            return (this.nt.Equals(ti.getNodeTarget()));
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }
    }
}