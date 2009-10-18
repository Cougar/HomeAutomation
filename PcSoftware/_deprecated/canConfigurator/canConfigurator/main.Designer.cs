namespace canConfigurator
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
            this.cmdConfigureTypedefs = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.cmdConfigureIrTable = new System.Windows.Forms.Button();
            this.cmdSerialSettings = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // cmdConfigureTypedefs
            // 
            this.cmdConfigureTypedefs.Location = new System.Drawing.Point(15, 25);
            this.cmdConfigureTypedefs.Name = "cmdConfigureTypedefs";
            this.cmdConfigureTypedefs.Size = new System.Drawing.Size(89, 22);
            this.cmdConfigureTypedefs.TabIndex = 0;
            this.cmdConfigureTypedefs.Text = "Typedefs";
            this.cmdConfigureTypedefs.UseVisualStyleBackColor = true;
            this.cmdConfigureTypedefs.Click += new System.EventHandler(this.cmdConfigureTypedefs_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(15, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(89, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Configurations for";
            // 
            // cmdConfigureIrTable
            // 
            this.cmdConfigureIrTable.Location = new System.Drawing.Point(15, 53);
            this.cmdConfigureIrTable.Name = "cmdConfigureIrTable";
            this.cmdConfigureIrTable.Size = new System.Drawing.Size(89, 22);
            this.cmdConfigureIrTable.TabIndex = 2;
            this.cmdConfigureIrTable.Text = "IR-table";
            this.cmdConfigureIrTable.UseVisualStyleBackColor = true;
            this.cmdConfigureIrTable.Click += new System.EventHandler(this.cmdConfigureIrTable_Click);
            // 
            // cmdSerialSettings
            // 
            this.cmdSerialSettings.Location = new System.Drawing.Point(15, 93);
            this.cmdSerialSettings.Name = "cmdSerialSettings";
            this.cmdSerialSettings.Size = new System.Drawing.Size(89, 22);
            this.cmdSerialSettings.TabIndex = 3;
            this.cmdSerialSettings.Text = "Connection";
            this.cmdSerialSettings.UseVisualStyleBackColor = true;
            this.cmdSerialSettings.Click += new System.EventHandler(this.cmdSerialSettings_Click);
            // 
            // main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(116, 127);
            this.Controls.Add(this.cmdSerialSettings);
            this.Controls.Add(this.cmdConfigureIrTable);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.cmdConfigureTypedefs);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "main";
            this.Text = "canConfigurator";
            this.Load += new System.EventHandler(this.main_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button cmdConfigureTypedefs;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button cmdConfigureIrTable;
        private System.Windows.Forms.Button cmdSerialSettings;
    }
}

