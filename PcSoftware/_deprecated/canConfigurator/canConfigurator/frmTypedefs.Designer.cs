namespace canConfigurator
{
    partial class frmTypedefs
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
            this.label1 = new System.Windows.Forms.Label();
            this.txtClassName = new System.Windows.Forms.TextBox();
            this.txtClassComment = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.cmdClassAdd = new System.Windows.Forms.Button();
            this.cmdClassUpdate = new System.Windows.Forms.Button();
            this.cmdClassDelete = new System.Windows.Forms.Button();
            this.cmdIdDelete = new System.Windows.Forms.Button();
            this.cmdIdUpdate = new System.Windows.Forms.Button();
            this.cmdIdAdd = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.txtIdComment = new System.Windows.Forms.TextBox();
            this.txtIdName = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.txtCsharpPath = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.cmdBrowseCsharp = new System.Windows.Forms.Button();
            this.cmdBrowseC = new System.Windows.Forms.Button();
            this.label6 = new System.Windows.Forms.Label();
            this.txtCPath = new System.Windows.Forms.TextBox();
            this.cmdGenerate = new System.Windows.Forms.Button();
            this.lstClasses = new System.Windows.Forms.ListBox();
            this.lstIds = new System.Windows.Forms.ListBox();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Underline, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(43, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Classes";
            // 
            // txtClassName
            // 
            this.txtClassName.Location = new System.Drawing.Point(184, 25);
            this.txtClassName.Name = "txtClassName";
            this.txtClassName.Size = new System.Drawing.Size(130, 20);
            this.txtClassName.TabIndex = 2;
            // 
            // txtClassComment
            // 
            this.txtClassComment.Location = new System.Drawing.Point(184, 51);
            this.txtClassComment.Multiline = true;
            this.txtClassComment.Name = "txtClassComment";
            this.txtClassComment.Size = new System.Drawing.Size(130, 69);
            this.txtClassComment.TabIndex = 3;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(181, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(102, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "Name and comment";
            // 
            // cmdClassAdd
            // 
            this.cmdClassAdd.Location = new System.Drawing.Point(320, 25);
            this.cmdClassAdd.Name = "cmdClassAdd";
            this.cmdClassAdd.Size = new System.Drawing.Size(86, 22);
            this.cmdClassAdd.TabIndex = 5;
            this.cmdClassAdd.Text = "Add class";
            this.cmdClassAdd.UseVisualStyleBackColor = true;
            this.cmdClassAdd.Click += new System.EventHandler(this.cmdClassAdd_Click);
            // 
            // cmdClassUpdate
            // 
            this.cmdClassUpdate.Location = new System.Drawing.Point(320, 56);
            this.cmdClassUpdate.Name = "cmdClassUpdate";
            this.cmdClassUpdate.Size = new System.Drawing.Size(86, 22);
            this.cmdClassUpdate.TabIndex = 6;
            this.cmdClassUpdate.Text = "Update class";
            this.cmdClassUpdate.UseVisualStyleBackColor = true;
            this.cmdClassUpdate.Click += new System.EventHandler(this.cmdClassUpdate_Click);
            // 
            // cmdClassDelete
            // 
            this.cmdClassDelete.Location = new System.Drawing.Point(320, 87);
            this.cmdClassDelete.Name = "cmdClassDelete";
            this.cmdClassDelete.Size = new System.Drawing.Size(86, 22);
            this.cmdClassDelete.TabIndex = 7;
            this.cmdClassDelete.Text = "Delete class";
            this.cmdClassDelete.UseVisualStyleBackColor = true;
            this.cmdClassDelete.Click += new System.EventHandler(this.cmdClassDelete_Click);
            // 
            // cmdIdDelete
            // 
            this.cmdIdDelete.Location = new System.Drawing.Point(320, 210);
            this.cmdIdDelete.Name = "cmdIdDelete";
            this.cmdIdDelete.Size = new System.Drawing.Size(86, 22);
            this.cmdIdDelete.TabIndex = 15;
            this.cmdIdDelete.Text = "Delete ID";
            this.cmdIdDelete.UseVisualStyleBackColor = true;
            this.cmdIdDelete.Click += new System.EventHandler(this.cmdIdDelete_Click);
            // 
            // cmdIdUpdate
            // 
            this.cmdIdUpdate.Location = new System.Drawing.Point(320, 179);
            this.cmdIdUpdate.Name = "cmdIdUpdate";
            this.cmdIdUpdate.Size = new System.Drawing.Size(86, 22);
            this.cmdIdUpdate.TabIndex = 14;
            this.cmdIdUpdate.Text = "Update ID";
            this.cmdIdUpdate.UseVisualStyleBackColor = true;
            this.cmdIdUpdate.Click += new System.EventHandler(this.cmdIdUpdate_Click);
            // 
            // cmdIdAdd
            // 
            this.cmdIdAdd.Location = new System.Drawing.Point(320, 148);
            this.cmdIdAdd.Name = "cmdIdAdd";
            this.cmdIdAdd.Size = new System.Drawing.Size(86, 22);
            this.cmdIdAdd.TabIndex = 13;
            this.cmdIdAdd.Text = "Add ID";
            this.cmdIdAdd.UseVisualStyleBackColor = true;
            this.cmdIdAdd.Click += new System.EventHandler(this.cmdIdAdd_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(181, 132);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(102, 13);
            this.label3.TabIndex = 12;
            this.label3.Text = "Name and comment";
            // 
            // txtIdComment
            // 
            this.txtIdComment.Location = new System.Drawing.Point(184, 174);
            this.txtIdComment.Multiline = true;
            this.txtIdComment.Name = "txtIdComment";
            this.txtIdComment.Size = new System.Drawing.Size(130, 68);
            this.txtIdComment.TabIndex = 11;
            // 
            // txtIdName
            // 
            this.txtIdName.Location = new System.Drawing.Point(184, 148);
            this.txtIdName.Name = "txtIdName";
            this.txtIdName.Size = new System.Drawing.Size(130, 20);
            this.txtIdName.TabIndex = 10;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Underline, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(12, 132);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(23, 13);
            this.label4.TabIndex = 9;
            this.label4.Text = "IDs";
            // 
            // txtCsharpPath
            // 
            this.txtCsharpPath.Location = new System.Drawing.Point(66, 257);
            this.txtCsharpPath.Name = "txtCsharpPath";
            this.txtCsharpPath.Size = new System.Drawing.Size(248, 20);
            this.txtCsharpPath.TabIndex = 16;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(12, 260);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(48, 13);
            this.label5.TabIndex = 17;
            this.label5.Text = "C# path:";
            // 
            // cmdBrowseCsharp
            // 
            this.cmdBrowseCsharp.Location = new System.Drawing.Point(320, 257);
            this.cmdBrowseCsharp.Name = "cmdBrowseCsharp";
            this.cmdBrowseCsharp.Size = new System.Drawing.Size(86, 20);
            this.cmdBrowseCsharp.TabIndex = 18;
            this.cmdBrowseCsharp.Text = "Browse...";
            this.cmdBrowseCsharp.UseVisualStyleBackColor = true;
            this.cmdBrowseCsharp.Click += new System.EventHandler(this.cmdBrowseCsharp_Click);
            // 
            // cmdBrowseC
            // 
            this.cmdBrowseC.Location = new System.Drawing.Point(320, 283);
            this.cmdBrowseC.Name = "cmdBrowseC";
            this.cmdBrowseC.Size = new System.Drawing.Size(86, 20);
            this.cmdBrowseC.TabIndex = 21;
            this.cmdBrowseC.Text = "Browse...";
            this.cmdBrowseC.UseVisualStyleBackColor = true;
            this.cmdBrowseC.Click += new System.EventHandler(this.cmdBrowseC_Click);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(12, 286);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(41, 13);
            this.label6.TabIndex = 20;
            this.label6.Text = "C path:";
            // 
            // txtCPath
            // 
            this.txtCPath.Location = new System.Drawing.Point(66, 283);
            this.txtCPath.Name = "txtCPath";
            this.txtCPath.Size = new System.Drawing.Size(248, 20);
            this.txtCPath.TabIndex = 19;
            // 
            // cmdGenerate
            // 
            this.cmdGenerate.Location = new System.Drawing.Point(320, 320);
            this.cmdGenerate.Name = "cmdGenerate";
            this.cmdGenerate.Size = new System.Drawing.Size(86, 22);
            this.cmdGenerate.TabIndex = 22;
            this.cmdGenerate.Text = "Generate!";
            this.cmdGenerate.UseVisualStyleBackColor = true;
            this.cmdGenerate.Click += new System.EventHandler(this.cmdGenerate_Click);
            // 
            // lstClasses
            // 
            this.lstClasses.FormattingEnabled = true;
            this.lstClasses.Location = new System.Drawing.Point(15, 25);
            this.lstClasses.Name = "lstClasses";
            this.lstClasses.Size = new System.Drawing.Size(163, 95);
            this.lstClasses.TabIndex = 23;
            this.lstClasses.SelectedIndexChanged += new System.EventHandler(this.lstClasses_SelectedIndexChanged);
            // 
            // lstIds
            // 
            this.lstIds.FormattingEnabled = true;
            this.lstIds.Location = new System.Drawing.Point(15, 147);
            this.lstIds.Name = "lstIds";
            this.lstIds.Size = new System.Drawing.Size(163, 95);
            this.lstIds.TabIndex = 24;
            this.lstIds.SelectedIndexChanged += new System.EventHandler(this.lstIds_SelectedIndexChanged);
            // 
            // frmTypedefs
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(418, 351);
            this.Controls.Add(this.lstIds);
            this.Controls.Add(this.lstClasses);
            this.Controls.Add(this.cmdGenerate);
            this.Controls.Add(this.cmdBrowseC);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.txtCPath);
            this.Controls.Add(this.cmdBrowseCsharp);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.txtCsharpPath);
            this.Controls.Add(this.cmdIdDelete);
            this.Controls.Add(this.cmdIdUpdate);
            this.Controls.Add(this.cmdIdAdd);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.txtIdComment);
            this.Controls.Add(this.txtIdName);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.cmdClassDelete);
            this.Controls.Add(this.cmdClassUpdate);
            this.Controls.Add(this.cmdClassAdd);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.txtClassComment);
            this.Controls.Add(this.txtClassName);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "frmTypedefs";
            this.Text = "Type definitions";
            this.Load += new System.EventHandler(this.frmTypedefs_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtClassName;
        private System.Windows.Forms.TextBox txtClassComment;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button cmdClassAdd;
        private System.Windows.Forms.Button cmdClassUpdate;
        private System.Windows.Forms.Button cmdClassDelete;
        private System.Windows.Forms.Button cmdIdDelete;
        private System.Windows.Forms.Button cmdIdUpdate;
        private System.Windows.Forms.Button cmdIdAdd;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox txtIdComment;
        private System.Windows.Forms.TextBox txtIdName;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox txtCsharpPath;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button cmdBrowseCsharp;
        private System.Windows.Forms.Button cmdBrowseC;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox txtCPath;
        private System.Windows.Forms.Button cmdGenerate;
        private System.Windows.Forms.ListBox lstClasses;
        private System.Windows.Forms.ListBox lstIds;
    }
}