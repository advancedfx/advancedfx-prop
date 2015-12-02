namespace ShaderBuilder
{
    partial class MainForm
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
            this.groupBoxCompileOptions = new System.Windows.Forms.GroupBox();
            this.label7 = new System.Windows.Forms.Label();
            this.buttonTemp = new System.Windows.Forms.Button();
            this.textBoxTemp = new System.Windows.Forms.TextBox();
            this.comboBoxProfile = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.buttonFxcExe = new System.Windows.Forms.Button();
            this.textBoxFxcExe = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBoxInput = new System.Windows.Forms.GroupBox();
            this.buttonInputFxc = new System.Windows.Forms.Button();
            this.textBoxInputFxc = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.groupBoxOutput = new System.Windows.Forms.GroupBox();
            this.textBoxOutputPrefix = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.buttonOutputFolder = new System.Windows.Forms.Button();
            this.textBoxOutputFolder = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.buttonCompile = new System.Windows.Forms.Button();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.folderBrowserDialogOutput = new System.Windows.Forms.FolderBrowserDialog();
            this.openFileDialogFxcExe = new System.Windows.Forms.OpenFileDialog();
            this.openFileDialogInput = new System.Windows.Forms.OpenFileDialog();
            this.folderBrowserDialogTemp = new System.Windows.Forms.FolderBrowserDialog();
            this.groupBoxCompileOptions.SuspendLayout();
            this.groupBoxInput.SuspendLayout();
            this.groupBoxOutput.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBoxCompileOptions
            // 
            this.groupBoxCompileOptions.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBoxCompileOptions.Controls.Add(this.label7);
            this.groupBoxCompileOptions.Controls.Add(this.buttonTemp);
            this.groupBoxCompileOptions.Controls.Add(this.textBoxTemp);
            this.groupBoxCompileOptions.Controls.Add(this.comboBoxProfile);
            this.groupBoxCompileOptions.Controls.Add(this.label2);
            this.groupBoxCompileOptions.Controls.Add(this.buttonFxcExe);
            this.groupBoxCompileOptions.Controls.Add(this.textBoxFxcExe);
            this.groupBoxCompileOptions.Controls.Add(this.label1);
            this.groupBoxCompileOptions.Location = new System.Drawing.Point(6, 180);
            this.groupBoxCompileOptions.Name = "groupBoxCompileOptions";
            this.groupBoxCompileOptions.Size = new System.Drawing.Size(572, 109);
            this.groupBoxCompileOptions.TabIndex = 0;
            this.groupBoxCompileOptions.TabStop = false;
            this.groupBoxCompileOptions.Text = " Compile Options ";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(8, 74);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(66, 13);
            this.label7.TabIndex = 7;
            this.label7.Text = "TEMP folder";
            // 
            // buttonTemp
            // 
            this.buttonTemp.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonTemp.Location = new System.Drawing.Point(476, 69);
            this.buttonTemp.Name = "buttonTemp";
            this.buttonTemp.Size = new System.Drawing.Size(90, 23);
            this.buttonTemp.TabIndex = 6;
            this.buttonTemp.Text = "browse";
            this.buttonTemp.UseVisualStyleBackColor = true;
            this.buttonTemp.Click += new System.EventHandler(this.buttonTemp_Click);
            // 
            // textBoxTemp
            // 
            this.textBoxTemp.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxTemp.Location = new System.Drawing.Point(93, 71);
            this.textBoxTemp.Name = "textBoxTemp";
            this.textBoxTemp.Size = new System.Drawing.Size(377, 20);
            this.textBoxTemp.TabIndex = 5;
            // 
            // comboBoxProfile
            // 
            this.comboBoxProfile.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.comboBoxProfile.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxProfile.FormattingEnabled = true;
            this.comboBoxProfile.Location = new System.Drawing.Point(93, 44);
            this.comboBoxProfile.Name = "comboBoxProfile";
            this.comboBoxProfile.Size = new System.Drawing.Size(377, 21);
            this.comboBoxProfile.TabIndex = 4;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(8, 47);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(36, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "Profile";
            // 
            // buttonFxcExe
            // 
            this.buttonFxcExe.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonFxcExe.Location = new System.Drawing.Point(476, 15);
            this.buttonFxcExe.Name = "buttonFxcExe";
            this.buttonFxcExe.Size = new System.Drawing.Size(90, 23);
            this.buttonFxcExe.TabIndex = 2;
            this.buttonFxcExe.Text = "browse";
            this.buttonFxcExe.UseVisualStyleBackColor = true;
            this.buttonFxcExe.Click += new System.EventHandler(this.buttonFxcExe_Click);
            // 
            // textBoxFxcExe
            // 
            this.textBoxFxcExe.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxFxcExe.Location = new System.Drawing.Point(93, 17);
            this.textBoxFxcExe.Name = "textBoxFxcExe";
            this.textBoxFxcExe.Size = new System.Drawing.Size(377, 20);
            this.textBoxFxcExe.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(7, 20);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(41, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "fxc.exe";
            // 
            // groupBoxInput
            // 
            this.groupBoxInput.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBoxInput.Controls.Add(this.buttonInputFxc);
            this.groupBoxInput.Controls.Add(this.textBoxInputFxc);
            this.groupBoxInput.Controls.Add(this.label3);
            this.groupBoxInput.Location = new System.Drawing.Point(6, 7);
            this.groupBoxInput.Name = "groupBoxInput";
            this.groupBoxInput.Size = new System.Drawing.Size(572, 51);
            this.groupBoxInput.TabIndex = 1;
            this.groupBoxInput.TabStop = false;
            this.groupBoxInput.Text = " Input ";
            // 
            // buttonInputFxc
            // 
            this.buttonInputFxc.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonInputFxc.Location = new System.Drawing.Point(476, 15);
            this.buttonInputFxc.Name = "buttonInputFxc";
            this.buttonInputFxc.Size = new System.Drawing.Size(90, 23);
            this.buttonInputFxc.TabIndex = 3;
            this.buttonInputFxc.Text = "browse";
            this.buttonInputFxc.UseVisualStyleBackColor = true;
            this.buttonInputFxc.Click += new System.EventHandler(this.buttonInputFxc_Click);
            // 
            // textBoxInputFxc
            // 
            this.textBoxInputFxc.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxInputFxc.Location = new System.Drawing.Point(93, 17);
            this.textBoxInputFxc.Name = "textBoxInputFxc";
            this.textBoxInputFxc.Size = new System.Drawing.Size(377, 20);
            this.textBoxInputFxc.TabIndex = 2;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(7, 20);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(40, 13);
            this.label3.TabIndex = 0;
            this.label3.Text = ".fxc file";
            // 
            // groupBoxOutput
            // 
            this.groupBoxOutput.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBoxOutput.Controls.Add(this.textBoxOutputPrefix);
            this.groupBoxOutput.Controls.Add(this.label6);
            this.groupBoxOutput.Controls.Add(this.label5);
            this.groupBoxOutput.Controls.Add(this.buttonOutputFolder);
            this.groupBoxOutput.Controls.Add(this.textBoxOutputFolder);
            this.groupBoxOutput.Controls.Add(this.label4);
            this.groupBoxOutput.Location = new System.Drawing.Point(6, 64);
            this.groupBoxOutput.Name = "groupBoxOutput";
            this.groupBoxOutput.Size = new System.Drawing.Size(572, 110);
            this.groupBoxOutput.TabIndex = 2;
            this.groupBoxOutput.TabStop = false;
            this.groupBoxOutput.Text = " Output ";
            // 
            // textBoxOutputPrefix
            // 
            this.textBoxOutputPrefix.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxOutputPrefix.Location = new System.Drawing.Point(93, 20);
            this.textBoxOutputPrefix.Name = "textBoxOutputPrefix";
            this.textBoxOutputPrefix.Size = new System.Drawing.Size(377, 20);
            this.textBoxOutputPrefix.TabIndex = 6;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(8, 22);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(33, 13);
            this.label6.TabIndex = 5;
            this.label6.Text = "Prefix";
            // 
            // label5
            // 
            this.label5.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label5.Location = new System.Drawing.Point(8, 72);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(558, 27);
            this.label5.TabIndex = 4;
            this.label5.Text = "Files will be build in that directory as PREFIX_VALVESTATICCOMBO_VALVEDYNAMICCOMB" +
    "O_AFXCOMBO.fxo files.";
            // 
            // buttonOutputFolder
            // 
            this.buttonOutputFolder.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonOutputFolder.Location = new System.Drawing.Point(476, 46);
            this.buttonOutputFolder.Name = "buttonOutputFolder";
            this.buttonOutputFolder.Size = new System.Drawing.Size(90, 23);
            this.buttonOutputFolder.TabIndex = 3;
            this.buttonOutputFolder.Text = "browse";
            this.buttonOutputFolder.UseVisualStyleBackColor = true;
            this.buttonOutputFolder.Click += new System.EventHandler(this.buttonOutputFolder_Click);
            // 
            // textBoxOutputFolder
            // 
            this.textBoxOutputFolder.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxOutputFolder.Location = new System.Drawing.Point(93, 46);
            this.textBoxOutputFolder.Name = "textBoxOutputFolder";
            this.textBoxOutputFolder.Size = new System.Drawing.Size(377, 20);
            this.textBoxOutputFolder.TabIndex = 2;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(7, 48);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(49, 13);
            this.label4.TabIndex = 0;
            this.label4.Text = "Directory";
            // 
            // buttonCompile
            // 
            this.buttonCompile.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonCompile.Location = new System.Drawing.Point(7, 295);
            this.buttonCompile.Name = "buttonCompile";
            this.buttonCompile.Size = new System.Drawing.Size(571, 31);
            this.buttonCompile.TabIndex = 3;
            this.buttonCompile.Text = "COMPILE";
            this.buttonCompile.UseVisualStyleBackColor = true;
            this.buttonCompile.Click += new System.EventHandler(this.buttonCompile_Click);
            // 
            // progressBar1
            // 
            this.progressBar1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.progressBar1.Location = new System.Drawing.Point(7, 332);
            this.progressBar1.Maximum = 10000;
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(571, 23);
            this.progressBar1.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            this.progressBar1.TabIndex = 4;
            // 
            // folderBrowserDialogOutput
            // 
            this.folderBrowserDialogOutput.Description = "Select output folder";
            // 
            // openFileDialogFxcExe
            // 
            this.openFileDialogFxcExe.FileName = "fxc.exe";
            this.openFileDialogFxcExe.Filter = "fxc.exe (DirectX SDK)|fxc.exe";
            this.openFileDialogFxcExe.Title = "Find fxc.exe ...";
            // 
            // openFileDialogInput
            // 
            this.openFileDialogInput.FileName = "*.fxc";
            this.openFileDialogInput.Filter = "Valve shader source file (.fxc)|*.fxc";
            this.openFileDialogInput.Title = "Find .fxc input file ...";
            // 
            // folderBrowserDialogTemp
            // 
            this.folderBrowserDialogTemp.Description = "Select temporary folder ...";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(584, 361);
            this.Controls.Add(this.progressBar1);
            this.Controls.Add(this.buttonCompile);
            this.Controls.Add(this.groupBoxOutput);
            this.Controls.Add(this.groupBoxInput);
            this.Controls.Add(this.groupBoxCompileOptions);
            this.MinimumSize = new System.Drawing.Size(600, 400);
            this.Name = "MainForm";
            this.Padding = new System.Windows.Forms.Padding(3);
            this.Text = "ShaderBuilder";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.groupBoxCompileOptions.ResumeLayout(false);
            this.groupBoxCompileOptions.PerformLayout();
            this.groupBoxInput.ResumeLayout(false);
            this.groupBoxInput.PerformLayout();
            this.groupBoxOutput.ResumeLayout(false);
            this.groupBoxOutput.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBoxCompileOptions;
        private System.Windows.Forms.ComboBox comboBoxProfile;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button buttonFxcExe;
        private System.Windows.Forms.TextBox textBoxFxcExe;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBoxInput;
        private System.Windows.Forms.Button buttonInputFxc;
        private System.Windows.Forms.TextBox textBoxInputFxc;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.GroupBox groupBoxOutput;
        private System.Windows.Forms.Button buttonOutputFolder;
        private System.Windows.Forms.TextBox textBoxOutputFolder;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox textBoxOutputPrefix;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button buttonCompile;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.FolderBrowserDialog folderBrowserDialogOutput;
        private System.Windows.Forms.OpenFileDialog openFileDialogFxcExe;
        private System.Windows.Forms.OpenFileDialog openFileDialogInput;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Button buttonTemp;
        private System.Windows.Forms.TextBox textBoxTemp;
        private System.Windows.Forms.FolderBrowserDialog folderBrowserDialogTemp;
    }
}

