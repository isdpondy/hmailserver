namespace hMailServer.Shared
{
    partial class formEnterPassword
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
           System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(formEnterPassword));
           this.labelPassword = new System.Windows.Forms.Label();
           this.groupBox1 = new System.Windows.Forms.GroupBox();
           this.btnCancel = new System.Windows.Forms.Button();
           this.btnOK = new System.Windows.Forms.Button();
           this.textPassword = new hMailServer.Shared.ucText();
           this.SuspendLayout();
           // 
           // labelPassword
           // 
           this.labelPassword.AutoSize = true;
           this.labelPassword.Location = new System.Drawing.Point(9, 11);
           this.labelPassword.Name = "labelPassword";
           this.labelPassword.Size = new System.Drawing.Size(132, 13);
           this.labelPassword.TabIndex = 0;
           this.labelPassword.Text = "Please enter the password";
           // 
           // groupBox1
           // 
           this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.groupBox1.Location = new System.Drawing.Point(8, 51);
           this.groupBox1.Name = "groupBox1";
           this.groupBox1.Size = new System.Drawing.Size(272, 7);
           this.groupBox1.TabIndex = 10;
           this.groupBox1.TabStop = false;
           // 
           // btnCancel
           // 
           this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
           this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
           this.btnCancel.Location = new System.Drawing.Point(190, 67);
           this.btnCancel.Name = "btnCancel";
           this.btnCancel.Size = new System.Drawing.Size(89, 25);
           this.btnCancel.TabIndex = 9;
           this.btnCancel.Text = "Cancel";
           this.btnCancel.UseVisualStyleBackColor = true;
           this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
           // 
           // btnOK
           // 
           this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
           this.btnOK.Location = new System.Drawing.Point(94, 67);
           this.btnOK.Name = "btnOK";
           this.btnOK.Size = new System.Drawing.Size(89, 25);
           this.btnOK.TabIndex = 8;
           this.btnOK.Text = "OK";
           this.btnOK.UseVisualStyleBackColor = true;
           this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
           // 
           // textPassword
           // 
           this.textPassword.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.textPassword.Location = new System.Drawing.Point(13, 27);
           this.textPassword.Name = "textPassword";
           this.textPassword.Number = 0;
           this.textPassword.Numeric = false;
           this.textPassword.PasswordChar = '*';
           this.textPassword.Size = new System.Drawing.Size(270, 20);
           this.textPassword.TabIndex = 1;
           // 
           // formEnterPassword
           // 
           this.AcceptButton = this.btnOK;
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.CancelButton = this.btnCancel;
           this.ClientSize = new System.Drawing.Size(295, 102);
           this.ControlBox = false;
           this.Controls.Add(this.groupBox1);
           this.Controls.Add(this.btnCancel);
           this.Controls.Add(this.btnOK);
           this.Controls.Add(this.textPassword);
           this.Controls.Add(this.labelPassword);
           this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
           this.Name = "formEnterPassword";
           this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
           this.Text = "Enter password";
           this.Load += new System.EventHandler(this.formEnterPassword_Load);
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label labelPassword;
        private hMailServer.Shared.ucText textPassword;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnOK;
    }
}