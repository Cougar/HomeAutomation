namespace canBootloader
{
    partial class manageTargetsDialog
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(manageTargetsDialog));
            this.cmd_save = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.txt_tid = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.txt_expl = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // cmd_save
            // 
            this.cmd_save.Location = new System.Drawing.Point(240, 30);
            this.cmd_save.Name = "cmd_save";
            this.cmd_save.Size = new System.Drawing.Size(68, 20);
            this.cmd_save.TabIndex = 0;
            this.cmd_save.Text = "OK";
            this.cmd_save.UseVisualStyleBackColor = true;
            this.cmd_save.Click += new System.EventHandler(this.cmd_save_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(33, 14);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(52, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Target ID";
            // 
            // txt_tid
            // 
            this.txt_tid.Location = new System.Drawing.Point(36, 31);
            this.txt_tid.Name = "txt_tid";
            this.txt_tid.Size = new System.Drawing.Size(60, 20);
            this.txt_tid.TabIndex = 3;
            this.txt_tid.Text = "00";
            this.txt_tid.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(12, 33);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(18, 13);
            this.label4.TabIndex = 6;
            this.label4.Text = "0x";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(99, 14);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(38, 13);
            this.label5.TabIndex = 7;
            this.label5.Text = "Name:";
            // 
            // txt_expl
            // 
            this.txt_expl.Location = new System.Drawing.Point(102, 30);
            this.txt_expl.MaxLength = 50;
            this.txt_expl.Name = "txt_expl";
            this.txt_expl.Size = new System.Drawing.Size(120, 20);
            this.txt_expl.TabIndex = 8;
            // 
            // manageTargetsDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(322, 62);
            this.Controls.Add(this.txt_expl);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.txt_tid);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.cmd_save);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "manageTargetsDialog";
            this.Text = "manageTargetsDialog";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button cmd_save;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txt_tid;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox txt_expl;
    }
}