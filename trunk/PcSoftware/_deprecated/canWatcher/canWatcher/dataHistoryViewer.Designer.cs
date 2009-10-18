namespace canWatcher
{
    partial class dataHistoryViewer
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(dataHistoryViewer));
            this.lab_status = new System.Windows.Forms.Label();
            this.lst_history = new System.Windows.Forms.ListBox();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.cmd_clear_history = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.cmd_export_history = new System.Windows.Forms.ToolStripButton();
            this.tmr_refresh = new System.Windows.Forms.Timer(this.components);
            this.toolStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // lab_status
            // 
            resources.ApplyResources(this.lab_status, "lab_status");
            this.lab_status.Name = "lab_status";
            // 
            // lst_history
            // 
            this.lst_history.FormattingEnabled = true;
            resources.ApplyResources(this.lst_history, "lst_history");
            this.lst_history.Name = "lst_history";
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.cmd_clear_history,
            this.toolStripSeparator1,
            this.cmd_export_history});
            resources.ApplyResources(this.toolStrip1, "toolStrip1");
            this.toolStrip1.Name = "toolStrip1";
            // 
            // cmd_clear_history
            // 
            resources.ApplyResources(this.cmd_clear_history, "cmd_clear_history");
            this.cmd_clear_history.Name = "cmd_clear_history";
            this.cmd_clear_history.Click += new System.EventHandler(this.cmd_clear_history_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            resources.ApplyResources(this.toolStripSeparator1, "toolStripSeparator1");
            // 
            // cmd_export_history
            // 
            resources.ApplyResources(this.cmd_export_history, "cmd_export_history");
            this.cmd_export_history.Name = "cmd_export_history";
            this.cmd_export_history.Click += new System.EventHandler(this.cmd_export_history_Click);
            // 
            // tmr_refresh
            // 
            this.tmr_refresh.Enabled = true;
            this.tmr_refresh.Interval = 250;
            this.tmr_refresh.Tick += new System.EventHandler(this.tmr_refresh_Tick);
            // 
            // dataHistoryViewer
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.toolStrip1);
            this.Controls.Add(this.lst_history);
            this.Controls.Add(this.lab_status);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "dataHistoryViewer";
            this.Load += new System.EventHandler(this.dataHistoryViewer_Load);
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lab_status;
        private System.Windows.Forms.ListBox lst_history;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton cmd_clear_history;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton cmd_export_history;
        private System.Windows.Forms.Timer tmr_refresh;
    }
}