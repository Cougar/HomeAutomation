namespace canWatcherLight
{
    partial class main
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(main));
            this.lst_incomming = new System.Windows.Forms.ListBox();
            this.tstrip = new System.Windows.Forms.ToolStrip();
            this.cmd_connect = new System.Windows.Forms.ToolStripButton();
            this.cmd_disconnect = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.cmd_clear = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.cmd_settings = new System.Windows.Forms.ToolStripButton();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.lab_status = new System.Windows.Forms.ToolStripLabel();
            this.tstrip.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // lst_incomming
            // 
            this.lst_incomming.FormattingEnabled = true;
            this.lst_incomming.Location = new System.Drawing.Point(12, 28);
            this.lst_incomming.Name = "lst_incomming";
            this.lst_incomming.Size = new System.Drawing.Size(574, 134);
            this.lst_incomming.TabIndex = 0;
            // 
            // tstrip
            // 
            this.tstrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.cmd_connect,
            this.cmd_disconnect,
            this.toolStripSeparator1,
            this.cmd_clear,
            this.toolStripSeparator2,
            this.cmd_settings});
            this.tstrip.Location = new System.Drawing.Point(0, 0);
            this.tstrip.Name = "tstrip";
            this.tstrip.Size = new System.Drawing.Size(598, 25);
            this.tstrip.TabIndex = 1;
            this.tstrip.Text = "tstrip";
            // 
            // cmd_connect
            // 
            this.cmd_connect.Image = ((System.Drawing.Image)(resources.GetObject("cmd_connect.Image")));
            this.cmd_connect.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.cmd_connect.Name = "cmd_connect";
            this.cmd_connect.Size = new System.Drawing.Size(67, 22);
            this.cmd_connect.Text = "Connect";
            this.cmd_connect.Click += new System.EventHandler(this.cmd_connect_Click);
            // 
            // cmd_disconnect
            // 
            this.cmd_disconnect.Enabled = false;
            this.cmd_disconnect.Image = ((System.Drawing.Image)(resources.GetObject("cmd_disconnect.Image")));
            this.cmd_disconnect.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.cmd_disconnect.Name = "cmd_disconnect";
            this.cmd_disconnect.Size = new System.Drawing.Size(79, 22);
            this.cmd_disconnect.Text = "Disconnect";
            this.cmd_disconnect.Click += new System.EventHandler(this.cmd_disconnect_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // cmd_clear
            // 
            this.cmd_clear.Image = ((System.Drawing.Image)(resources.GetObject("cmd_clear.Image")));
            this.cmd_clear.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.cmd_clear.Name = "cmd_clear";
            this.cmd_clear.Size = new System.Drawing.Size(52, 22);
            this.cmd_clear.Text = "Clear";
            this.cmd_clear.Click += new System.EventHandler(this.cmd_clear_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
            // 
            // cmd_settings
            // 
            this.cmd_settings.Image = ((System.Drawing.Image)(resources.GetObject("cmd_settings.Image")));
            this.cmd_settings.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.cmd_settings.Name = "cmd_settings";
            this.cmd_settings.Size = new System.Drawing.Size(66, 22);
            this.cmd_settings.Text = "Settings";
            this.cmd_settings.Click += new System.EventHandler(this.cmd_settings_Click);
            // 
            // toolStrip1
            // 
            this.toolStrip1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lab_status});
            this.toolStrip1.Location = new System.Drawing.Point(0, 172);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(598, 25);
            this.toolStrip1.TabIndex = 2;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // lab_status
            // 
            this.lab_status.Name = "lab_status";
            this.lab_status.Size = new System.Drawing.Size(0, 22);
            // 
            // main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(598, 197);
            this.Controls.Add(this.toolStrip1);
            this.Controls.Add(this.tstrip);
            this.Controls.Add(this.lst_incomming);
            this.Name = "main";
            this.Text = "canWatcherLight";
            this.Load += new System.EventHandler(this.main_Load);
            this.tstrip.ResumeLayout(false);
            this.tstrip.PerformLayout();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListBox lst_incomming;
        private System.Windows.Forms.ToolStrip tstrip;
        private System.Windows.Forms.ToolStripButton cmd_connect;
        private System.Windows.Forms.ToolStripButton cmd_disconnect;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton cmd_clear;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripButton cmd_settings;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripLabel lab_status;
    }
}

