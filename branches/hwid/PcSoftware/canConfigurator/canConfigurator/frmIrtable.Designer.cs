namespace canConfigurator
{
    partial class frmIrtable
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
            this.txtIrAddr = new System.Windows.Forms.TextBox();
            this.txtIrData = new System.Windows.Forms.TextBox();
            this.cmbClass = new System.Windows.Forms.ComboBox();
            this.cmbId = new System.Windows.Forms.ComboBox();
            this.txtData1 = new System.Windows.Forms.TextBox();
            this.txtData0 = new System.Windows.Forms.TextBox();
            this.txtIrRepeat = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.cmdAdd = new System.Windows.Forms.Button();
            this.cmdUpdate = new System.Windows.Forms.Button();
            this.cmdDelete = new System.Windows.Forms.Button();
            this.lstIrcommands = new System.Windows.Forms.ListBox();
            this.cmdDownload = new System.Windows.Forms.Button();
            this.cmdAbort = new System.Windows.Forms.Button();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.labStatus = new System.Windows.Forms.ToolStripStatusLabel();
            this.statusStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // txtIrAddr
            // 
            this.txtIrAddr.Location = new System.Drawing.Point(152, 6);
            this.txtIrAddr.MaxLength = 2;
            this.txtIrAddr.Name = "txtIrAddr";
            this.txtIrAddr.Size = new System.Drawing.Size(31, 20);
            this.txtIrAddr.TabIndex = 0;
            // 
            // txtIrData
            // 
            this.txtIrData.Location = new System.Drawing.Point(264, 6);
            this.txtIrData.MaxLength = 2;
            this.txtIrData.Name = "txtIrData";
            this.txtIrData.Size = new System.Drawing.Size(31, 20);
            this.txtIrData.TabIndex = 1;
            // 
            // cmbClass
            // 
            this.cmbClass.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbClass.FormattingEnabled = true;
            this.cmbClass.Location = new System.Drawing.Point(49, 32);
            this.cmbClass.Name = "cmbClass";
            this.cmbClass.Size = new System.Drawing.Size(134, 21);
            this.cmbClass.TabIndex = 2;
            this.cmbClass.SelectedIndexChanged += new System.EventHandler(this.cmbClass_SelectedIndexChanged);
            // 
            // cmbId
            // 
            this.cmbId.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbId.FormattingEnabled = true;
            this.cmbId.Location = new System.Drawing.Point(264, 32);
            this.cmbId.Name = "cmbId";
            this.cmbId.Size = new System.Drawing.Size(153, 21);
            this.cmbId.TabIndex = 3;
            // 
            // txtData1
            // 
            this.txtData1.Location = new System.Drawing.Point(66, 59);
            this.txtData1.MaxLength = 2;
            this.txtData1.Name = "txtData1";
            this.txtData1.Size = new System.Drawing.Size(31, 20);
            this.txtData1.TabIndex = 4;
            // 
            // txtData0
            // 
            this.txtData0.Location = new System.Drawing.Point(152, 59);
            this.txtData0.MaxLength = 2;
            this.txtData0.Name = "txtData0";
            this.txtData0.Size = new System.Drawing.Size(31, 20);
            this.txtData0.TabIndex = 5;
            // 
            // txtIrRepeat
            // 
            this.txtIrRepeat.Location = new System.Drawing.Point(386, 6);
            this.txtIrRepeat.MaxLength = 2;
            this.txtIrRepeat.Name = "txtIrRepeat";
            this.txtIrRepeat.Size = new System.Drawing.Size(31, 20);
            this.txtIrRepeat.TabIndex = 6;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(134, 13);
            this.label1.TabIndex = 7;
            this.label1.Text = "When receiving IR addr 0x";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(195, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(63, 13);
            this.label2.TabIndex = 8;
            this.label2.Text = "and data 0x";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(301, 9);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(79, 13);
            this.label3.TabIndex = 9;
            this.label3.Text = ", send every 0x";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(12, 35);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(31, 13);
            this.label4.TabIndex = 10;
            this.label4.Text = "class";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(234, 35);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(18, 13);
            this.label5.TabIndex = 11;
            this.label5.Text = "ID";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(12, 62);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(48, 13);
            this.label6.TabIndex = 12;
            this.label6.Text = "data1 0x";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(103, 62);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(48, 13);
            this.label7.TabIndex = 13;
            this.label7.Text = "data0 0x";
            // 
            // cmdAdd
            // 
            this.cmdAdd.Location = new System.Drawing.Point(192, 59);
            this.cmdAdd.Name = "cmdAdd";
            this.cmdAdd.Size = new System.Drawing.Size(71, 22);
            this.cmdAdd.TabIndex = 14;
            this.cmdAdd.Text = "Add";
            this.cmdAdd.UseVisualStyleBackColor = true;
            this.cmdAdd.Click += new System.EventHandler(this.cmdAdd_Click);
            // 
            // cmdUpdate
            // 
            this.cmdUpdate.Location = new System.Drawing.Point(269, 59);
            this.cmdUpdate.Name = "cmdUpdate";
            this.cmdUpdate.Size = new System.Drawing.Size(71, 22);
            this.cmdUpdate.TabIndex = 15;
            this.cmdUpdate.Text = "Update";
            this.cmdUpdate.UseVisualStyleBackColor = true;
            this.cmdUpdate.Click += new System.EventHandler(this.cmdUpdate_Click);
            // 
            // cmdDelete
            // 
            this.cmdDelete.Location = new System.Drawing.Point(346, 59);
            this.cmdDelete.Name = "cmdDelete";
            this.cmdDelete.Size = new System.Drawing.Size(71, 22);
            this.cmdDelete.TabIndex = 16;
            this.cmdDelete.Text = "Delete";
            this.cmdDelete.UseVisualStyleBackColor = true;
            this.cmdDelete.Click += new System.EventHandler(this.cmdDelete_Click);
            // 
            // lstIrcommands
            // 
            this.lstIrcommands.FormattingEnabled = true;
            this.lstIrcommands.Location = new System.Drawing.Point(15, 87);
            this.lstIrcommands.Name = "lstIrcommands";
            this.lstIrcommands.Size = new System.Drawing.Size(402, 95);
            this.lstIrcommands.TabIndex = 17;
            this.lstIrcommands.SelectedIndexChanged += new System.EventHandler(this.lstIrcommands_SelectedIndexChanged);
            // 
            // cmdDownload
            // 
            this.cmdDownload.Location = new System.Drawing.Point(304, 188);
            this.cmdDownload.Name = "cmdDownload";
            this.cmdDownload.Size = new System.Drawing.Size(113, 22);
            this.cmdDownload.TabIndex = 18;
            this.cmdDownload.Text = "Save && download!";
            this.cmdDownload.UseVisualStyleBackColor = true;
            this.cmdDownload.Click += new System.EventHandler(this.cmdDownload_Click);
            // 
            // cmdAbort
            // 
            this.cmdAbort.Location = new System.Drawing.Point(237, 188);
            this.cmdAbort.Name = "cmdAbort";
            this.cmdAbort.Size = new System.Drawing.Size(61, 22);
            this.cmdAbort.TabIndex = 20;
            this.cmdAbort.Text = "Abort";
            this.cmdAbort.UseVisualStyleBackColor = true;
            this.cmdAbort.Click += new System.EventHandler(this.cmdAbort_Click);
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.labStatus});
            this.statusStrip1.Location = new System.Drawing.Point(0, 214);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(428, 22);
            this.statusStrip1.TabIndex = 21;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // labStatus
            // 
            this.labStatus.Name = "labStatus";
            this.labStatus.Size = new System.Drawing.Size(93, 17);
            this.labStatus.Text = "Status: Waiting...";
            // 
            // frmIrtable
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(428, 236);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.cmdAbort);
            this.Controls.Add(this.cmdDownload);
            this.Controls.Add(this.lstIrcommands);
            this.Controls.Add(this.cmdDelete);
            this.Controls.Add(this.cmdUpdate);
            this.Controls.Add(this.cmdAdd);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txtIrRepeat);
            this.Controls.Add(this.txtData0);
            this.Controls.Add(this.txtData1);
            this.Controls.Add(this.cmbId);
            this.Controls.Add(this.cmbClass);
            this.Controls.Add(this.txtIrData);
            this.Controls.Add(this.txtIrAddr);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "frmIrtable";
            this.Text = "IR-table";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.frmIrtable_FormClosed);
            this.Load += new System.EventHandler(this.frmIrtable_Load);
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox txtIrAddr;
        private System.Windows.Forms.TextBox txtIrData;
        private System.Windows.Forms.ComboBox cmbClass;
        private System.Windows.Forms.ComboBox cmbId;
        private System.Windows.Forms.TextBox txtData1;
        private System.Windows.Forms.TextBox txtData0;
        private System.Windows.Forms.TextBox txtIrRepeat;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Button cmdAdd;
        private System.Windows.Forms.Button cmdUpdate;
        private System.Windows.Forms.Button cmdDelete;
        private System.Windows.Forms.ListBox lstIrcommands;
        private System.Windows.Forms.Button cmdDownload;
        private System.Windows.Forms.Button cmdAbort;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel labStatus;
    }
}