using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ShaderBuilder
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private LogForm m_LogForm;

        private void MainForm_Load(object sender, EventArgs e)
        {
            textBoxInputFxc.Text = Properties.Settings.Default.InputFxc;
            textBoxOutputPrefix.Text = Properties.Settings.Default.OutputPrefix;
            textBoxOutputFolder.Text = Properties.Settings.Default.OutputFolder;
            {
                foreach (var value in System.Enum.GetValues(typeof(FxcCompile.Profile)))
                {
                    comboBoxProfile.Items.Add(value);
                }
                comboBoxProfile.SelectedIndex = comboBoxProfile.FindString(Properties.Settings.Default.Profile);
            }

            m_LogForm = new LogForm();
        }

        private void buttonInputFxc_Click(object sender, EventArgs e)
        {
            openFileDialogInput.FileName = textBoxInputFxc.Text;
            string initialDirectory = null;
            try
            {
                initialDirectory = System.IO.Path.GetDirectoryName(openFileDialogInput.FileName);
            }
            catch (ArgumentException)
            {
                initialDirectory = null;
            }
            if (null != initialDirectory) openFileDialogInput.InitialDirectory = initialDirectory;
            if (openFileDialogInput.ShowDialog(this) == DialogResult.OK)
                textBoxInputFxc.Text = openFileDialogInput.FileName;
        }

        private void buttonOutputFolder_Click(object sender, EventArgs e)
        {
            folderBrowserDialogOutput.SelectedPath = textBoxOutputFolder.Text;
            if (folderBrowserDialogOutput.ShowDialog(this) == DialogResult.OK)
                textBoxOutputFolder.Text = folderBrowserDialogOutput.SelectedPath;
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            Properties.Settings.Default.InputFxc = textBoxInputFxc.Text;
            Properties.Settings.Default.OutputPrefix = textBoxOutputPrefix.Text;
            Properties.Settings.Default.OutputFolder = textBoxOutputFolder.Text;
            Properties.Settings.Default.Profile = comboBoxProfile.GetItemText(comboBoxProfile.SelectedItem);

            Properties.Settings.Default.Save();
        }

        private void fxcCompile_Progress(FxcCompile o, double relativeValue)
        {
            progressBar1.Value = progressBar1.Minimum +(int)(relativeValue*(progressBar1.Maximum - progressBar1.Minimum));
            Application.DoEvents();
        }

        private void fxcCompile_Status(FxcCompile o, string message)
        {
            toolStripStatusLabel1.Text = message;
        }

        private void fxcCompile_Error(FxcCompile o, string message)
        {
            m_LogForm.AddText(message);
        }

        private void buttonCompile_Click(object sender, EventArgs e)
        {
            progressBar1.Value = progressBar1.Minimum;
            m_LogForm.Clear();

            FxcCompile.Profile profile;
            if(!System.Enum.TryParse<FxcCompile.Profile>(comboBoxProfile.Text, out profile))
            {
                m_LogForm.AddText("Invalid Profile selected!");
                m_LogForm.ShowDialog(this);
                return;
            }

            FxcCompile fxcCompile = new FxcCompile();

            fxcCompile.Error = fxcCompile_Error;
            fxcCompile.Status = fxcCompile_Status;
            fxcCompile.Progress = fxcCompile_Progress;

            if(!fxcCompile.Compile(textBoxInputFxc.Text, textBoxOutputFolder.Text+"\\"+textBoxOutputPrefix.Text,profile))
            {
                m_LogForm.AddText("Compile failed!");
                m_LogForm.ShowDialog(this);
                return;
            }
        }
    }
}
