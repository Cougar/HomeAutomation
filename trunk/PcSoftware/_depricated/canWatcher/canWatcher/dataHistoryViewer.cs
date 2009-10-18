using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;

namespace canWatcher
{
    public partial class dataHistoryViewer : Form
    {
        canMessage cm;
        main m;

        public dataHistoryViewer(main m, canMessage cm)
        {
            InitializeComponent();
            this.cm = cm;
            this.m = m;
            lab_status.Text = "Last 0 data for "+cm.getIdent().ToString("X").PadLeft(8,'0')+"h";
            this.Text = cm.getIdent().ToString("X").PadLeft(8, '0') + "h history";
        }

        public void refreshList()
        {
            int prevpos = lst_history.SelectedIndex;

            clearList(lst_history);

            LinkedList<identHistory.dataStamp> lst = m.dhistory.getHistory(cm.getIdent()).getStamps();

            lst_history.BeginUpdate();
            foreach (identHistory.dataStamp ds in lst) lst_history.Items.Insert(1, ds);
            lst_history.EndUpdate();

            if (prevpos > -1 && prevpos < lst_history.Items.Count) lst_history.SelectedIndex = prevpos;


            lab_status.Text = "Last "+lst.Count+" data for " + cm.getIdent().ToString("X").PadLeft(8, '0') + "h";
        }

        private void cmd_clear_history_Click(object sender, EventArgs e)
        {
            m.dhistory.clearHistory(cm.getIdent());
            clearList(lst_history);
        }

        private void clearList(ListBox lst)
        {
            lst.Items.Clear();
            lst.Items.Add("Time\t\tDLC\tData[7]..[0]");
        }

        private void cmd_export_history_Click(object sender, EventArgs e)
        {
            exportFunctions.exportToFile("Data log (*.log)|*.log", "Select where to save data log file", lst_history, exportFunctions.exportFormat.TXT);
        }



        private void dataHistoryViewer_Load(object sender, EventArgs e)
        {

        }

        private void tmr_refresh_Tick(object sender, EventArgs e)
        {
            refreshList();
        }

    }
}