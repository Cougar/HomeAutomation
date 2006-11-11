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
        HexFile hf;
        SerialConnection sc;
        Downloader dl;
        string currentLoadedFile = "";

        LinkedList<string> recentFiles = new LinkedList<string>();
        public Hashtable serialSettings = new Hashtable();

        public main()
        {
            InitializeComponent();
        }

        private void main_Load(object sender, EventArgs e)
        {
            loadSettings();
            refreshRecentFiles();
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

        private string shortString(string str)
        {
            return str;
        }

        private void download()
        {
            if (dl != null) { log("Download in progress..."); return; }

            loadFile(currentLoadedFile);

            if (hf == null) { log("No hexfile loaded."); return; }

            sc = new SerialConnection();
            try
            {
                sc.setConfiguration((int)serialSettings["baud"], (System.IO.Ports.Parity)Enum.Parse(typeof(System.IO.Ports.Parity), serialSettings["parity"].ToString()), (string)serialSettings["port"], (System.IO.Ports.StopBits)Enum.Parse(typeof(System.IO.Ports.StopBits), serialSettings["stopbits"].ToString()), (int)serialSettings["databits"], false);
            }
            catch (Exception e) { log("Error setting serial connection settings: " + e.Message); return; }
            if (!sc.open()) { log("Error opening port."); return; }

            dl = new Downloader(hf, sc);
            if (!dl.go()) { log("Error gooing..."); return; }
            log("Downloading...");
            dl.threadAbort += new EventHandler(dl_threadAbort);

        }

        void dl_threadAbort(object sender, EventArgs e)
        {
            try
            {
                this.BeginInvoke((ThreadStart)delegate
                {
                    threadAbortEvent e2 = (threadAbortEvent)e;
                    if (!e2.getUserAborted())
                    {
                        log("Download done. Time spent: " + e2.getTimeS().ToString() + " seconds. " + Math.Round(e2.getBps(), 0) + " Bps");
                    }
                    else log("Download aborted.");
                    sc.close(); sc = null;
                    dl = null;
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
            download();
        }

        public void saveSettings()
        { 
            Hashtable saveblock = new Hashtable();
            saveblock["recentFiles"]=recentFiles;
            saveblock["serialSettings"]=serialSettings;

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
                Stream st = File.OpenRead(Application.StartupPath + "/settings.dat");
                saveblock = (Hashtable)formatter.Deserialize(st);
                st.Close();
                recentFiles = (LinkedList<string>)saveblock["recentFiles"];
                serialSettings = (Hashtable)saveblock["serialSettings"];

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
        }

        private void txtLog_TextChanged(object sender, EventArgs e)
        {
            txtLog.SelectionStart = txtLog.Text.Length;
            txtLog.ScrollToCaret();
        }


    }

    internal class recentItem : ToolStripMenuItem
    {
        public string filepath;
        public recentItem(string filepath) { this.filepath = filepath; }
        public recentItem() { filepath = ""; }
    }
}