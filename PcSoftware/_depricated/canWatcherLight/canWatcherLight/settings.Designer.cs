namespace canWatcherLight
{
    partial class settings
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
            this.cmd_cancel = new System.Windows.Forms.Button();
            this.cmd_add = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.cmb_port = new System.Windows.Forms.ComboBox();
            this.cmb_parity = new System.Windows.Forms.ComboBox();
            this.cmb_stopbits = new System.Windows.Forms.ComboBox();
            this.cmb_databits = new System.Windows.Forms.ComboBox();
            this.txt_baud = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // cmd_cancel
            // 
            this.cmd_cancel.Location = new System.Drawing.Point(15, 149);
            this.cmd_cancel.Name = "cmd_cancel";
            this.cmd_cancel.Size = new System.Drawing.Size(84, 26);
            this.cmd_cancel.TabIndex = 32;
            this.cmd_cancel.Text = "Cancel";
            this.cmd_cancel.UseVisualStyleBackColor = true;
            this.cmd_cancel.Click += new System.EventHandler(this.cmd_cancel_Click);
            // 
            // cmd_add
            // 
            this.cmd_add.Location = new System.Drawing.Point(105, 149);
            this.cmd_add.Name = "cmd_add";
            this.cmd_add.Size = new System.Drawing.Size(84, 26);
            this.cmd_add.TabIndex = 31;
            this.cmd_add.Text = "Save";
            this.cmd_add.UseVisualStyleBackColor = true;
            this.cmd_add.Click += new System.EventHandler(this.cmd_add_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 44);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 13);
            this.label1.TabIndex = 33;
            this.label1.Text = "Port:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 71);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(36, 13);
            this.label2.TabIndex = 34;
            this.label2.Text = "Parity:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 98);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(51, 13);
            this.label3.TabIndex = 35;
            this.label3.Text = "Stop bits:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(12, 125);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(52, 13);
            this.label4.TabIndex = 36;
            this.label4.Text = "Data bits:";
            // 
            // cmb_port
            // 
            this.cmb_port.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmb_port.FormattingEnabled = true;
            this.cmb_port.Location = new System.Drawing.Point(70, 41);
            this.cmb_port.Name = "cmb_port";
            this.cmb_port.Size = new System.Drawing.Size(118, 21);
            this.cmb_port.TabIndex = 37;
            // 
            // cmb_parity
            // 
            this.cmb_parity.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmb_parity.FormattingEnabled = true;
            this.cmb_parity.Location = new System.Drawing.Point(70, 68);
            this.cmb_parity.Name = "cmb_parity";
            this.cmb_parity.Size = new System.Drawing.Size(118, 21);
            this.cmb_parity.TabIndex = 38;
            // 
            // cmb_stopbits
            // 
            this.cmb_stopbits.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmb_stopbits.FormattingEnabled = true;
            this.cmb_stopbits.Location = new System.Drawing.Point(70, 95);
            this.cmb_stopbits.Name = "cmb_stopbits";
            this.cmb_stopbits.Size = new System.Drawing.Size(118, 21);
            this.cmb_stopbits.TabIndex = 39;
            // 
            // cmb_databits
            // 
            this.cmb_databits.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmb_databits.FormattingEnabled = true;
            this.cmb_databits.Location = new System.Drawing.Point(70, 122);
            this.cmb_databits.Name = "cmb_databits";
            this.cmb_databits.Size = new System.Drawing.Size(118, 21);
            this.cmb_databits.TabIndex = 40;
            // 
            // txt_baud
            // 
            this.txt_baud.Location = new System.Drawing.Point(70, 15);
            this.txt_baud.Name = "txt_baud";
            this.txt_baud.Size = new System.Drawing.Size(118, 20);
            this.txt_baud.TabIndex = 41;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(12, 18);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(35, 13);
            this.label5.TabIndex = 42;
            this.label5.Text = "Baud:";
            // 
            // settings
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(203, 187);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.txt_baud);
            this.Controls.Add(this.cmb_databits);
            this.Controls.Add(this.cmb_stopbits);
            this.Controls.Add(this.cmb_parity);
            this.Controls.Add(this.cmb_port);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.cmd_cancel);
            this.Controls.Add(this.cmd_add);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "settings";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "settings";
            this.Load += new System.EventHandler(this.settings_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button cmd_cancel;
        private System.Windows.Forms.Button cmd_add;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox cmb_port;
        private System.Windows.Forms.ComboBox cmb_parity;
        private System.Windows.Forms.ComboBox cmb_stopbits;
        private System.Windows.Forms.ComboBox cmb_databits;
        private System.Windows.Forms.TextBox txt_baud;
        private System.Windows.Forms.Label label5;
    }
}