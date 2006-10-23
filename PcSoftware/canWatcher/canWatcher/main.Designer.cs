namespace canWatcher
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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle3 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle4 = new System.Windows.Forms.DataGridViewCellStyle();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(main));
            this.dg_incomming = new System.Windows.Forms.DataGridView();
            this.ident = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.length = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.data = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.period = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.count = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Column1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.hash = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.cmI = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dg_outgoing = new System.Windows.Forms.DataGridView();
            this.dataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn3 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn4 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn5 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Column2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn6 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.cmO = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.tmr_check_out = new System.Windows.Forms.Timer(this.components);
            this.tmr_refresher = new System.Windows.Forms.Timer(this.components);
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.strip_tool = new System.Windows.Forms.ToolStrip();
            this.cmd_connect = new System.Windows.Forms.ToolStripButton();
            this.cmd_disconnect = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.cmd_clear_incomming = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripLabel1 = new System.Windows.Forms.ToolStripLabel();
            this.cmd_add_message = new System.Windows.Forms.ToolStripButton();
            this.cmd_edit_message = new System.Windows.Forms.ToolStripButton();
            this.cmd_delete_message = new System.Windows.Forms.ToolStripButton();
            this.cmd_send_message = new System.Windows.Forms.ToolStripButton();
            this.cmd_settings = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.toolStripLabel2 = new System.Windows.Forms.ToolStripLabel();
            this.lab_status = new System.Windows.Forms.ToolStripLabel();
            this.cmd_save_outgoing = new System.Windows.Forms.ToolStripButton();
            this.cmd_load_outgoing = new System.Windows.Forms.ToolStripButton();
            this.toolStripLabel3 = new System.Windows.Forms.ToolStripLabel();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.cmd_send = new System.Windows.Forms.ToolStripButton();
            this.txt_debug_out = new System.Windows.Forms.ToolStripTextBox();
            this.toolStripLabel4 = new System.Windows.Forms.ToolStripLabel();
            this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
            ((System.ComponentModel.ISupportInitialize)(this.dg_incomming)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dg_outgoing)).BeginInit();
            this.strip_tool.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // dg_incomming
            // 
            this.dg_incomming.AllowUserToAddRows = false;
            this.dg_incomming.AllowUserToDeleteRows = false;
            this.dg_incomming.AllowUserToResizeRows = false;
            dataGridViewCellStyle1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.dg_incomming.AlternatingRowsDefaultCellStyle = dataGridViewCellStyle1;
            this.dg_incomming.BackgroundColor = System.Drawing.SystemColors.InactiveBorder;
            this.dg_incomming.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.dg_incomming.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dg_incomming.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.ident,
            this.length,
            this.data,
            this.period,
            this.count,
            this.Column1,
            this.hash,
            this.cmI});
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle2.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.dg_incomming.DefaultCellStyle = dataGridViewCellStyle2;
            this.dg_incomming.Location = new System.Drawing.Point(15, 41);
            this.dg_incomming.MultiSelect = false;
            this.dg_incomming.Name = "dg_incomming";
            this.dg_incomming.Size = new System.Drawing.Size(610, 238);
            this.dg_incomming.TabIndex = 8;
            // 
            // ident
            // 
            this.ident.HeaderText = "Identifier";
            this.ident.Name = "ident";
            this.ident.ReadOnly = true;
            // 
            // length
            // 
            this.length.HeaderText = "Length";
            this.length.Name = "length";
            this.length.ReadOnly = true;
            this.length.Width = 80;
            // 
            // data
            // 
            this.data.HeaderText = "Data";
            this.data.Name = "data";
            this.data.ReadOnly = true;
            this.data.Width = 130;
            // 
            // period
            // 
            this.period.HeaderText = "Period";
            this.period.Name = "period";
            this.period.ReadOnly = true;
            this.period.Width = 80;
            // 
            // count
            // 
            this.count.HeaderText = "Count";
            this.count.Name = "count";
            this.count.ReadOnly = true;
            this.count.Width = 60;
            // 
            // Column1
            // 
            this.Column1.HeaderText = "Time";
            this.Column1.Name = "Column1";
            this.Column1.Width = 80;
            // 
            // hash
            // 
            this.hash.HeaderText = "hash";
            this.hash.Name = "hash";
            this.hash.Visible = false;
            // 
            // cmI
            // 
            this.cmI.HeaderText = "cmcm";
            this.cmI.Name = "cmI";
            this.cmI.Visible = false;
            // 
            // dg_outgoing
            // 
            this.dg_outgoing.AllowUserToAddRows = false;
            this.dg_outgoing.AllowUserToDeleteRows = false;
            this.dg_outgoing.AllowUserToResizeRows = false;
            dataGridViewCellStyle3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.dg_outgoing.AlternatingRowsDefaultCellStyle = dataGridViewCellStyle3;
            this.dg_outgoing.BackgroundColor = System.Drawing.SystemColors.InactiveBorder;
            this.dg_outgoing.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.dg_outgoing.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dg_outgoing.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.dataGridViewTextBoxColumn1,
            this.dataGridViewTextBoxColumn2,
            this.dataGridViewTextBoxColumn3,
            this.dataGridViewTextBoxColumn4,
            this.dataGridViewTextBoxColumn5,
            this.Column2,
            this.dataGridViewTextBoxColumn6,
            this.cmO});
            dataGridViewCellStyle4.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle4.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle4.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle4.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle4.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle4.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle4.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.dg_outgoing.DefaultCellStyle = dataGridViewCellStyle4;
            this.dg_outgoing.Location = new System.Drawing.Point(15, 298);
            this.dg_outgoing.MultiSelect = false;
            this.dg_outgoing.Name = "dg_outgoing";
            this.dg_outgoing.Size = new System.Drawing.Size(610, 238);
            this.dg_outgoing.TabIndex = 12;
            this.dg_outgoing.CellDoubleClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.dg_outgoing_CellDoubleClick);
            // 
            // dataGridViewTextBoxColumn1
            // 
            this.dataGridViewTextBoxColumn1.HeaderText = "Identifier";
            this.dataGridViewTextBoxColumn1.Name = "dataGridViewTextBoxColumn1";
            this.dataGridViewTextBoxColumn1.ReadOnly = true;
            // 
            // dataGridViewTextBoxColumn2
            // 
            this.dataGridViewTextBoxColumn2.HeaderText = "Length";
            this.dataGridViewTextBoxColumn2.Name = "dataGridViewTextBoxColumn2";
            this.dataGridViewTextBoxColumn2.ReadOnly = true;
            this.dataGridViewTextBoxColumn2.Width = 80;
            // 
            // dataGridViewTextBoxColumn3
            // 
            this.dataGridViewTextBoxColumn3.HeaderText = "Data";
            this.dataGridViewTextBoxColumn3.Name = "dataGridViewTextBoxColumn3";
            this.dataGridViewTextBoxColumn3.ReadOnly = true;
            this.dataGridViewTextBoxColumn3.Width = 130;
            // 
            // dataGridViewTextBoxColumn4
            // 
            this.dataGridViewTextBoxColumn4.HeaderText = "Period";
            this.dataGridViewTextBoxColumn4.Name = "dataGridViewTextBoxColumn4";
            this.dataGridViewTextBoxColumn4.ReadOnly = true;
            this.dataGridViewTextBoxColumn4.Width = 80;
            // 
            // dataGridViewTextBoxColumn5
            // 
            this.dataGridViewTextBoxColumn5.HeaderText = "Count";
            this.dataGridViewTextBoxColumn5.Name = "dataGridViewTextBoxColumn5";
            this.dataGridViewTextBoxColumn5.ReadOnly = true;
            this.dataGridViewTextBoxColumn5.Width = 60;
            // 
            // Column2
            // 
            this.Column2.HeaderText = "Time";
            this.Column2.Name = "Column2";
            this.Column2.Width = 80;
            // 
            // dataGridViewTextBoxColumn6
            // 
            this.dataGridViewTextBoxColumn6.HeaderText = "hash";
            this.dataGridViewTextBoxColumn6.Name = "dataGridViewTextBoxColumn6";
            this.dataGridViewTextBoxColumn6.Visible = false;
            // 
            // cmO
            // 
            this.cmO.HeaderText = "cm";
            this.cmO.Name = "cmO";
            this.cmO.Visible = false;
            // 
            // tmr_check_out
            // 
            this.tmr_check_out.Enabled = true;
            this.tmr_check_out.Interval = 4;
            this.tmr_check_out.Tick += new System.EventHandler(this.tmr_check_out_Tick);
            // 
            // tmr_refresher
            // 
            this.tmr_refresher.Enabled = true;
            this.tmr_refresher.Interval = 1000;
            this.tmr_refresher.Tick += new System.EventHandler(this.tmr_refresher_Tick);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(12, 25);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(67, 13);
            this.label1.TabIndex = 14;
            this.label1.Text = "Incomming";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(12, 282);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(58, 13);
            this.label2.TabIndex = 15;
            this.label2.Text = "Outgoing";
            // 
            // strip_tool
            // 
            this.strip_tool.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.cmd_connect,
            this.cmd_disconnect,
            this.toolStripSeparator1,
            this.cmd_clear_incomming,
            this.toolStripSeparator4,
            this.toolStripLabel1,
            this.cmd_add_message,
            this.cmd_edit_message,
            this.cmd_delete_message,
            this.cmd_send_message,
            this.cmd_settings,
            this.toolStripSeparator2});
            this.strip_tool.Location = new System.Drawing.Point(0, 0);
            this.strip_tool.Name = "strip_tool";
            this.strip_tool.Size = new System.Drawing.Size(637, 25);
            this.strip_tool.TabIndex = 18;
            this.strip_tool.Text = "toolStrip1";
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
            // cmd_clear_incomming
            // 
            this.cmd_clear_incomming.Image = ((System.Drawing.Image)(resources.GetObject("cmd_clear_incomming.Image")));
            this.cmd_clear_incomming.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.cmd_clear_incomming.Name = "cmd_clear_incomming";
            this.cmd_clear_incomming.Size = new System.Drawing.Size(104, 22);
            this.cmd_clear_incomming.Text = "Clear incomming";
            this.cmd_clear_incomming.Click += new System.EventHandler(this.cmd_clear_incomming_Click);
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStripLabel1
            // 
            this.toolStripLabel1.Image = ((System.Drawing.Image)(resources.GetObject("toolStripLabel1.Image")));
            this.toolStripLabel1.Name = "toolStripLabel1";
            this.toolStripLabel1.Size = new System.Drawing.Size(71, 22);
            this.toolStripLabel1.Text = "Outgoing:";
            // 
            // cmd_add_message
            // 
            this.cmd_add_message.Image = ((System.Drawing.Image)(resources.GetObject("cmd_add_message.Image")));
            this.cmd_add_message.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.cmd_add_message.Name = "cmd_add_message";
            this.cmd_add_message.Size = new System.Drawing.Size(46, 22);
            this.cmd_add_message.Text = "Add";
            this.cmd_add_message.Click += new System.EventHandler(this.cmd_add_message_Click);
            // 
            // cmd_edit_message
            // 
            this.cmd_edit_message.Image = ((System.Drawing.Image)(resources.GetObject("cmd_edit_message.Image")));
            this.cmd_edit_message.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.cmd_edit_message.Name = "cmd_edit_message";
            this.cmd_edit_message.Size = new System.Drawing.Size(45, 22);
            this.cmd_edit_message.Text = "Edit";
            this.cmd_edit_message.Click += new System.EventHandler(this.cmd_edit_message_Click);
            // 
            // cmd_delete_message
            // 
            this.cmd_delete_message.Image = ((System.Drawing.Image)(resources.GetObject("cmd_delete_message.Image")));
            this.cmd_delete_message.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.cmd_delete_message.Name = "cmd_delete_message";
            this.cmd_delete_message.Size = new System.Drawing.Size(58, 22);
            this.cmd_delete_message.Text = "Delete";
            this.cmd_delete_message.Click += new System.EventHandler(this.cmd_delete_message_Click);
            // 
            // cmd_send_message
            // 
            this.cmd_send_message.Image = ((System.Drawing.Image)(resources.GetObject("cmd_send_message.Image")));
            this.cmd_send_message.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.cmd_send_message.Name = "cmd_send_message";
            this.cmd_send_message.Size = new System.Drawing.Size(74, 22);
            this.cmd_send_message.Text = "Send now";
            this.cmd_send_message.Click += new System.EventHandler(this.cmd_send_message_Click);
            // 
            // cmd_settings
            // 
            this.cmd_settings.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.cmd_settings.Image = ((System.Drawing.Image)(resources.GetObject("cmd_settings.Image")));
            this.cmd_settings.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.cmd_settings.Name = "cmd_settings";
            this.cmd_settings.Size = new System.Drawing.Size(66, 22);
            this.cmd_settings.Text = "Settings";
            this.cmd_settings.Click += new System.EventHandler(this.cmd_settings_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStrip1
            // 
            this.toolStrip1.AutoSize = false;
            this.toolStrip1.CanOverflow = false;
            this.toolStrip1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.toolStrip1.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripLabel2,
            this.lab_status,
            this.cmd_save_outgoing,
            this.cmd_load_outgoing,
            this.toolStripLabel3,
            this.toolStripSeparator3,
            this.cmd_send,
            this.txt_debug_out,
            this.toolStripLabel4,
            this.toolStripSeparator5});
            this.toolStrip1.Location = new System.Drawing.Point(0, 546);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(637, 25);
            this.toolStrip1.TabIndex = 22;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // toolStripLabel2
            // 
            this.toolStripLabel2.Name = "toolStripLabel2";
            this.toolStripLabel2.Size = new System.Drawing.Size(42, 22);
            this.toolStripLabel2.Text = "Status:";
            // 
            // lab_status
            // 
            this.lab_status.AutoSize = false;
            this.lab_status.Name = "lab_status";
            this.lab_status.Overflow = System.Windows.Forms.ToolStripItemOverflow.Never;
            this.lab_status.Size = new System.Drawing.Size(100, 22);
            // 
            // cmd_save_outgoing
            // 
            this.cmd_save_outgoing.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.cmd_save_outgoing.Image = ((System.Drawing.Image)(resources.GetObject("cmd_save_outgoing.Image")));
            this.cmd_save_outgoing.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.cmd_save_outgoing.Name = "cmd_save_outgoing";
            this.cmd_save_outgoing.Size = new System.Drawing.Size(51, 22);
            this.cmd_save_outgoing.Text = "Save";
            this.cmd_save_outgoing.Click += new System.EventHandler(this.cmd_save_outgoing_Click);
            // 
            // cmd_load_outgoing
            // 
            this.cmd_load_outgoing.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.cmd_load_outgoing.Image = ((System.Drawing.Image)(resources.GetObject("cmd_load_outgoing.Image")));
            this.cmd_load_outgoing.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.cmd_load_outgoing.Name = "cmd_load_outgoing";
            this.cmd_load_outgoing.Size = new System.Drawing.Size(50, 22);
            this.cmd_load_outgoing.Text = "Load";
            this.cmd_load_outgoing.Click += new System.EventHandler(this.cmd_load_outgoing_Click);
            // 
            // toolStripLabel3
            // 
            this.toolStripLabel3.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.toolStripLabel3.Image = ((System.Drawing.Image)(resources.GetObject("toolStripLabel3.Image")));
            this.toolStripLabel3.Name = "toolStripLabel3";
            this.toolStripLabel3.Size = new System.Drawing.Size(121, 22);
            this.toolStripLabel3.Text = "Outgoing messages:";
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(6, 25);
            // 
            // cmd_send
            // 
            this.cmd_send.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.cmd_send.Image = ((System.Drawing.Image)(resources.GetObject("cmd_send.Image")));
            this.cmd_send.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.cmd_send.Name = "cmd_send";
            this.cmd_send.Size = new System.Drawing.Size(51, 22);
            this.cmd_send.Text = "Send";
            this.cmd_send.Click += new System.EventHandler(this.cmd_send_Click);
            // 
            // txt_debug_out
            // 
            this.txt_debug_out.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.txt_debug_out.Name = "txt_debug_out";
            this.txt_debug_out.Size = new System.Drawing.Size(100, 25);
            // 
            // toolStripLabel4
            // 
            this.toolStripLabel4.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.toolStripLabel4.Name = "toolStripLabel4";
            this.toolStripLabel4.Size = new System.Drawing.Size(81, 22);
            this.toolStripLabel4.Text = "Send raw data:";
            // 
            // toolStripSeparator5
            // 
            this.toolStripSeparator5.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.toolStripSeparator5.Name = "toolStripSeparator5";
            this.toolStripSeparator5.Size = new System.Drawing.Size(6, 25);
            // 
            // main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(637, 571);
            this.Controls.Add(this.toolStrip1);
            this.Controls.Add(this.strip_tool);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.dg_outgoing);
            this.Controls.Add(this.dg_incomming);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "main";
            this.Text = "canWatcher";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.main_FormClosing);
            this.Load += new System.EventHandler(this.main_Load);
            ((System.ComponentModel.ISupportInitialize)(this.dg_incomming)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dg_outgoing)).EndInit();
            this.strip_tool.ResumeLayout(false);
            this.strip_tool.PerformLayout();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.DataGridView dg_incomming;
        private System.Windows.Forms.DataGridView dg_outgoing;
        private System.Windows.Forms.Timer tmr_check_out;
        private System.Windows.Forms.Timer tmr_refresher;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ToolStrip strip_tool;
        private System.Windows.Forms.ToolStripButton cmd_connect;
        private System.Windows.Forms.ToolStripButton cmd_disconnect;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton cmd_add_message;
        private System.Windows.Forms.ToolStripButton cmd_edit_message;
        private System.Windows.Forms.ToolStripButton cmd_delete_message;
        private System.Windows.Forms.ToolStripLabel toolStripLabel1;
        private System.Windows.Forms.ToolStripButton cmd_send_message;
        private System.Windows.Forms.ToolStripButton cmd_settings;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripLabel toolStripLabel2;
        private System.Windows.Forms.ToolStripLabel lab_status;
        private System.Windows.Forms.ToolStripButton cmd_save_outgoing;
        private System.Windows.Forms.ToolStripButton cmd_load_outgoing;
        private System.Windows.Forms.ToolStripLabel toolStripLabel3;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripLabel toolStripLabel4;
        private System.Windows.Forms.ToolStripTextBox txt_debug_out;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator5;
        private System.Windows.Forms.ToolStripButton cmd_send;
        private System.Windows.Forms.ToolStripButton cmd_clear_incomming;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.DataGridViewTextBoxColumn ident;
        private System.Windows.Forms.DataGridViewTextBoxColumn length;
        private System.Windows.Forms.DataGridViewTextBoxColumn data;
        private System.Windows.Forms.DataGridViewTextBoxColumn period;
        private System.Windows.Forms.DataGridViewTextBoxColumn count;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column1;
        private System.Windows.Forms.DataGridViewTextBoxColumn hash;
        private System.Windows.Forms.DataGridViewTextBoxColumn cmI;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn2;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn3;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn4;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn5;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column2;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn6;
        private System.Windows.Forms.DataGridViewTextBoxColumn cmO;
    }
}

