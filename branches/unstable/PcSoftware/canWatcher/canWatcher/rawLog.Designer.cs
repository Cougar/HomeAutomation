namespace canWatcher
{
    partial class rawLog
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(rawLog));
            this.strp_top = new System.Windows.Forms.ToolStrip();
            this.cmd_start_log = new System.Windows.Forms.ToolStripButton();
            this.cmd_stop_log = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.cmd_clear_log = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.cmd_export_log = new System.Windows.Forms.ToolStripButton();
            this.lst_raw_log = new System.Windows.Forms.ListBox();
            this.strp_top.SuspendLayout();
            this.SuspendLayout();
            // 
            // strp_top
            // 
            this.strp_top.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.cmd_start_log,
            this.cmd_stop_log,
            this.toolStripSeparator1,
            this.cmd_clear_log,
            this.toolStripSeparator2,
            this.cmd_export_log});
            this.strp_top.Location = new System.Drawing.Point(0, 0);
            this.strp_top.Name = "strp_top";
            this.strp_top.Size = new System.Drawing.Size(451, 25);
            this.strp_top.TabIndex = 0;
            this.strp_top.Text = "toolStrip1";
            // 
            // cmd_start_log
            // 
            this.cmd_start_log.Image = ((System.Drawing.Image)(resources.GetObject("cmd_start_log.Image")));
            this.cmd_start_log.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.cmd_start_log.Name = "cmd_start_log";
            this.cmd_start_log.Size = new System.Drawing.Size(51, 22);
            this.cmd_start_log.Text = "Start";
            this.cmd_start_log.Click += new System.EventHandler(this.cmd_start_log_Click);
            // 
            // cmd_stop_log
            // 
            this.cmd_stop_log.Enabled = false;
            this.cmd_stop_log.Image = ((System.Drawing.Image)(resources.GetObject("cmd_stop_log.Image")));
            this.cmd_stop_log.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.cmd_stop_log.Name = "cmd_stop_log";
            this.cmd_stop_log.Size = new System.Drawing.Size(49, 22);
            this.cmd_stop_log.Text = "Stop";
            this.cmd_stop_log.Click += new System.EventHandler(this.cmd_stop_log_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // cmd_clear_log
            // 
            this.cmd_clear_log.Image = ((System.Drawing.Image)(resources.GetObject("cmd_clear_log.Image")));
            this.cmd_clear_log.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.cmd_clear_log.Name = "cmd_clear_log";
            this.cmd_clear_log.Size = new System.Drawing.Size(52, 22);
            this.cmd_clear_log.Text = "Clear";
            this.cmd_clear_log.Click += new System.EventHandler(this.cmd_clear_log_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
            // 
            // cmd_export_log
            // 
            this.cmd_export_log.Image = ((System.Drawing.Image)(resources.GetObject("cmd_export_log.Image")));
            this.cmd_export_log.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.cmd_export_log.Name = "cmd_export_log";
            this.cmd_export_log.Size = new System.Drawing.Size(71, 22);
            this.cmd_export_log.Text = "Export...";
            this.cmd_export_log.ToolTipText = "Export...";
            this.cmd_export_log.Click += new System.EventHandler(this.cmd_export_log_Click);
            // 
            // lst_raw_log
            // 
            this.lst_raw_log.FormattingEnabled = true;
            this.lst_raw_log.Location = new System.Drawing.Point(12, 28);
            this.lst_raw_log.Name = "lst_raw_log";
            this.lst_raw_log.Size = new System.Drawing.Size(427, 342);
            this.lst_raw_log.TabIndex = 1;
            // 
            // rawLog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(451, 388);
            this.Controls.Add(this.lst_raw_log);
            this.Controls.Add(this.strp_top);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "rawLog";
            this.Text = "Raw log";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.rawLog_FormClosing);
            this.Load += new System.EventHandler(this.rawLog_Load);
            this.strp_top.ResumeLayout(false);
            this.strp_top.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolStrip strp_top;
        private System.Windows.Forms.ToolStripButton cmd_start_log;
        private System.Windows.Forms.ToolStripButton cmd_stop_log;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton cmd_clear_log;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripButton cmd_export_log;
        private System.Windows.Forms.ListBox lst_raw_log;
    }
}