namespace canBootloader
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(main));
            this.menu = new System.Windows.Forms.MenuStrip();
            this.menu_file = new System.Windows.Forms.ToolStripMenuItem();
            this.menu_file_load = new System.Windows.Forms.ToolStripMenuItem();
            this.menu_file_load_recent = new System.Windows.Forms.ToolStripMenuItem();
            this.menu_file_quit = new System.Windows.Forms.ToolStripMenuItem();
            this.menu_action = new System.Windows.Forms.ToolStripMenuItem();
            this.menu_action_download = new System.Windows.Forms.ToolStripMenuItem();
            this.menu_action_abort = new System.Windows.Forms.ToolStripMenuItem();
            this.menu_settings = new System.Windows.Forms.ToolStripMenuItem();
            this.menu_settings_serialconnection = new System.Windows.Forms.ToolStripMenuItem();
            this.menu_settings_myid = new System.Windows.Forms.ToolStripMenuItem();
            this.menu_settings_myid_txt = new System.Windows.Forms.ToolStripTextBox();
            this.menu_target = new System.Windows.Forms.ToolStripMenuItem();
            this.txtLog = new System.Windows.Forms.TextBox();
            this.nicon = new System.Windows.Forms.NotifyIcon(this.components);
            this.menu.SuspendLayout();
            this.SuspendLayout();
            // 
            // menu
            // 
            this.menu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menu_file,
            this.menu_action,
            this.menu_settings,
            this.menu_target});
            this.menu.Location = new System.Drawing.Point(0, 0);
            this.menu.Name = "menu";
            this.menu.Size = new System.Drawing.Size(432, 24);
            this.menu.TabIndex = 0;
            this.menu.Text = "menu";
            // 
            // menu_file
            // 
            this.menu_file.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menu_file_load,
            this.menu_file_load_recent,
            this.menu_file_quit});
            this.menu_file.Name = "menu_file";
            this.menu_file.Size = new System.Drawing.Size(35, 20);
            this.menu_file.Text = "File";
            // 
            // menu_file_load
            // 
            this.menu_file_load.Name = "menu_file_load";
            this.menu_file_load.Size = new System.Drawing.Size(131, 22);
            this.menu_file_load.Text = "Load...";
            this.menu_file_load.Click += new System.EventHandler(this.menu_file_load_Click);
            // 
            // menu_file_load_recent
            // 
            this.menu_file_load_recent.Name = "menu_file_load_recent";
            this.menu_file_load_recent.Size = new System.Drawing.Size(131, 22);
            this.menu_file_load_recent.Text = "Load recent";
            // 
            // menu_file_quit
            // 
            this.menu_file_quit.Name = "menu_file_quit";
            this.menu_file_quit.Size = new System.Drawing.Size(131, 22);
            this.menu_file_quit.Text = "Quit";
            this.menu_file_quit.Click += new System.EventHandler(this.menu_file_quit_Click);
            // 
            // menu_action
            // 
            this.menu_action.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menu_action_download,
            this.menu_action_abort});
            this.menu_action.Name = "menu_action";
            this.menu_action.Size = new System.Drawing.Size(49, 20);
            this.menu_action.Text = "Action";
            // 
            // menu_action_download
            // 
            this.menu_action_download.Name = "menu_action_download";
            this.menu_action_download.ShortcutKeys = System.Windows.Forms.Keys.F10;
            this.menu_action_download.Size = new System.Drawing.Size(146, 22);
            this.menu_action_download.Text = "Download";
            this.menu_action_download.Click += new System.EventHandler(this.menu_action_download_Click);
            // 
            // menu_action_abort
            // 
            this.menu_action_abort.Name = "menu_action_abort";
            this.menu_action_abort.ShortcutKeys = System.Windows.Forms.Keys.F12;
            this.menu_action_abort.Size = new System.Drawing.Size(146, 22);
            this.menu_action_abort.Text = "Abort";
            this.menu_action_abort.Click += new System.EventHandler(this.menu_action_abort_Click);
            // 
            // menu_settings
            // 
            this.menu_settings.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menu_settings_serialconnection,
            this.menu_settings_myid});
            this.menu_settings.Name = "menu_settings";
            this.menu_settings.Size = new System.Drawing.Size(58, 20);
            this.menu_settings.Text = "Settings";
            this.menu_settings.DropDownOpened += new System.EventHandler(this.menu_settings_DropDownOpened);
            // 
            // menu_settings_serialconnection
            // 
            this.menu_settings_serialconnection.Name = "menu_settings_serialconnection";
            this.menu_settings_serialconnection.Size = new System.Drawing.Size(167, 22);
            this.menu_settings_serialconnection.Text = "Serial connection...";
            this.menu_settings_serialconnection.Click += new System.EventHandler(this.menu_settings_serialconnection_Click);
            // 
            // menu_settings_myid
            // 
            this.menu_settings_myid.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menu_settings_myid_txt});
            this.menu_settings_myid.Name = "menu_settings_myid";
            this.menu_settings_myid.Size = new System.Drawing.Size(167, 22);
            this.menu_settings_myid.Text = "My ID: ";
            this.menu_settings_myid.DropDownClosed += new System.EventHandler(this.menu_settings_myid_DropDownClosed);
            // 
            // menu_settings_myid_txt
            // 
            this.menu_settings_myid_txt.Name = "menu_settings_myid_txt";
            this.menu_settings_myid_txt.Size = new System.Drawing.Size(100, 21);
            // 
            // menu_target
            // 
            this.menu_target.Name = "menu_target";
            this.menu_target.Size = new System.Drawing.Size(51, 20);
            this.menu_target.Text = "Target";
            // 
            // txtLog
            // 
            this.txtLog.BackColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.txtLog.Location = new System.Drawing.Point(12, 27);
            this.txtLog.Multiline = true;
            this.txtLog.Name = "txtLog";
            this.txtLog.ReadOnly = true;
            this.txtLog.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtLog.Size = new System.Drawing.Size(408, 159);
            this.txtLog.TabIndex = 1;
            this.txtLog.TextChanged += new System.EventHandler(this.txtLog_TextChanged);
            // 
            // nicon
            // 
            this.nicon.Icon = ((System.Drawing.Icon)(resources.GetObject("nicon.Icon")));
            this.nicon.Text = "canBootloader";
            this.nicon.Visible = true;
            this.nicon.Click += new System.EventHandler(this.nicon_Click);
            this.nicon.DoubleClick += new System.EventHandler(this.nicon_DoubleClick);
            // 
            // main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(432, 198);
            this.Controls.Add(this.txtLog);
            this.Controls.Add(this.menu);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menu;
            this.Name = "main";
            this.Text = "canBootloader";
            this.Load += new System.EventHandler(this.main_Load);
            this.menu.ResumeLayout(false);
            this.menu.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menu;
        private System.Windows.Forms.ToolStripMenuItem menu_file;
        private System.Windows.Forms.ToolStripMenuItem menu_file_load;
        private System.Windows.Forms.ToolStripMenuItem menu_file_load_recent;
        private System.Windows.Forms.ToolStripMenuItem menu_file_quit;
        private System.Windows.Forms.ToolStripMenuItem menu_action;
        private System.Windows.Forms.ToolStripMenuItem menu_action_download;
        private System.Windows.Forms.ToolStripMenuItem menu_settings;
        private System.Windows.Forms.ToolStripMenuItem menu_settings_serialconnection;
        private System.Windows.Forms.ToolStripMenuItem menu_action_abort;
        private System.Windows.Forms.TextBox txtLog;
        private System.Windows.Forms.NotifyIcon nicon;
        private System.Windows.Forms.ToolStripMenuItem menu_target;
        private System.Windows.Forms.ToolStripMenuItem menu_settings_myid;
        private System.Windows.Forms.ToolStripTextBox menu_settings_myid_txt;
    }
}

