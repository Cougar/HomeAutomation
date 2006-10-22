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
            this.dg_incomming = new System.Windows.Forms.DataGridView();
            this.ident = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.length = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.data = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.period = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.count = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.hash = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.txt_debug_out = new System.Windows.Forms.TextBox();
            this.serial_conn = new System.IO.Ports.SerialPort(this.components);
            this.cmd_connect = new System.Windows.Forms.Button();
            this.lab_status = new System.Windows.Forms.Label();
            this.dg_outgoing = new System.Windows.Forms.DataGridView();
            this.cmd_add_message = new System.Windows.Forms.Button();
            this.tmr_check_out = new System.Windows.Forms.Timer(this.components);
            this.tmr_refresher = new System.Windows.Forms.Timer(this.components);
            this.dataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn3 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn4 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn5 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn6 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.cm = new System.Windows.Forms.DataGridViewTextBoxColumn();
            ((System.ComponentModel.ISupportInitialize)(this.dg_incomming)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dg_outgoing)).BeginInit();
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
            this.hash});
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle2.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.dg_incomming.DefaultCellStyle = dataGridViewCellStyle2;
            this.dg_incomming.Location = new System.Drawing.Point(12, 12);
            this.dg_incomming.MultiSelect = false;
            this.dg_incomming.Name = "dg_incomming";
            this.dg_incomming.Size = new System.Drawing.Size(600, 238);
            this.dg_incomming.TabIndex = 8;
            this.dg_incomming.CellContentClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.dg_schedule_CellContentClick);
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
            this.data.Width = 200;
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
            this.count.Width = 80;
            // 
            // hash
            // 
            this.hash.HeaderText = "hash";
            this.hash.Name = "hash";
            this.hash.Visible = false;
            // 
            // txt_debug_out
            // 
            this.txt_debug_out.Location = new System.Drawing.Point(12, 500);
            this.txt_debug_out.Name = "txt_debug_out";
            this.txt_debug_out.Size = new System.Drawing.Size(202, 20);
            this.txt_debug_out.TabIndex = 9;
            this.txt_debug_out.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.txt_debug_out_KeyPress);
            // 
            // serial_conn
            // 
            this.serial_conn.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serial_conn_DataReceived);
            // 
            // cmd_connect
            // 
            this.cmd_connect.Location = new System.Drawing.Point(522, 507);
            this.cmd_connect.Name = "cmd_connect";
            this.cmd_connect.Size = new System.Drawing.Size(89, 24);
            this.cmd_connect.TabIndex = 10;
            this.cmd_connect.Text = "C/D";
            this.cmd_connect.UseVisualStyleBackColor = true;
            this.cmd_connect.Click += new System.EventHandler(this.cmd_connect_Click);
            // 
            // lab_status
            // 
            this.lab_status.AutoSize = true;
            this.lab_status.Location = new System.Drawing.Point(220, 507);
            this.lab_status.Name = "lab_status";
            this.lab_status.Size = new System.Drawing.Size(38, 13);
            this.lab_status.TabIndex = 11;
            this.lab_status.Text = "status:";
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
            this.dataGridViewTextBoxColumn6,
            this.cm});
            dataGridViewCellStyle4.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle4.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle4.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle4.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle4.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle4.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle4.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.dg_outgoing.DefaultCellStyle = dataGridViewCellStyle4;
            this.dg_outgoing.Location = new System.Drawing.Point(12, 256);
            this.dg_outgoing.MultiSelect = false;
            this.dg_outgoing.Name = "dg_outgoing";
            this.dg_outgoing.Size = new System.Drawing.Size(600, 238);
            this.dg_outgoing.TabIndex = 12;
            // 
            // cmd_add_message
            // 
            this.cmd_add_message.Location = new System.Drawing.Point(419, 507);
            this.cmd_add_message.Name = "cmd_add_message";
            this.cmd_add_message.Size = new System.Drawing.Size(97, 24);
            this.cmd_add_message.TabIndex = 13;
            this.cmd_add_message.Text = "Add message";
            this.cmd_add_message.UseVisualStyleBackColor = true;
            this.cmd_add_message.Click += new System.EventHandler(this.cmd_add_message_Click);
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
            this.dataGridViewTextBoxColumn3.Width = 200;
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
            this.dataGridViewTextBoxColumn5.Width = 80;
            // 
            // dataGridViewTextBoxColumn6
            // 
            this.dataGridViewTextBoxColumn6.HeaderText = "hash";
            this.dataGridViewTextBoxColumn6.Name = "dataGridViewTextBoxColumn6";
            this.dataGridViewTextBoxColumn6.Visible = false;
            // 
            // cm
            // 
            this.cm.HeaderText = "cm";
            this.cm.Name = "cm";
            this.cm.Visible = false;
            // 
            // main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(623, 533);
            this.Controls.Add(this.cmd_add_message);
            this.Controls.Add(this.dg_outgoing);
            this.Controls.Add(this.lab_status);
            this.Controls.Add(this.cmd_connect);
            this.Controls.Add(this.txt_debug_out);
            this.Controls.Add(this.dg_incomming);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "main";
            this.Text = "canWatcher";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.main_FormClosing);
            this.Load += new System.EventHandler(this.main_Load);
            ((System.ComponentModel.ISupportInitialize)(this.dg_incomming)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dg_outgoing)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.DataGridView dg_incomming;
        private System.Windows.Forms.TextBox txt_debug_out;
        private System.IO.Ports.SerialPort serial_conn;
        private System.Windows.Forms.Button cmd_connect;
        private System.Windows.Forms.Label lab_status;
        private System.Windows.Forms.DataGridViewTextBoxColumn ident;
        private System.Windows.Forms.DataGridViewTextBoxColumn length;
        private System.Windows.Forms.DataGridViewTextBoxColumn data;
        private System.Windows.Forms.DataGridViewTextBoxColumn period;
        private System.Windows.Forms.DataGridViewTextBoxColumn count;
        private System.Windows.Forms.DataGridViewTextBoxColumn hash;
        private System.Windows.Forms.DataGridView dg_outgoing;
        private System.Windows.Forms.Button cmd_add_message;
        private System.Windows.Forms.Timer tmr_check_out;
        private System.Windows.Forms.Timer tmr_refresher;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn2;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn3;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn4;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn5;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn6;
        private System.Windows.Forms.DataGridViewTextBoxColumn cm;
    }
}

