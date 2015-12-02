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
            textBoxFxcExe.Text = Properties.Settings.Default.FxcExe;
            {
                foreach (var value in System.Enum.GetValues(typeof(FxcCompile.Profile)))
                {
                    comboBoxProfile.Items.Add(value);
                }
                comboBoxProfile.SelectedIndex = comboBoxProfile.FindString(Properties.Settings.Default.Profile);
            }
            textBoxTemp.Text = Properties.Settings.Default.TempFolder;

            m_LogForm = new LogForm();
        }

        private void buttonFxcExe_Click(object sender, EventArgs e)
        {
            openFileDialogFxcExe.FileName = textBoxFxcExe.Text;
            if (openFileDialogFxcExe.ShowDialog(this) == DialogResult.OK)
                textBoxFxcExe.Text = openFileDialogFxcExe.FileName;
        }

        private void buttonInputFxc_Click(object sender, EventArgs e)
        {
            openFileDialogInput.FileName = textBoxInputFxc.Text;
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
            Properties.Settings.Default.FxcExe = textBoxFxcExe.Text;
            Properties.Settings.Default.Profile = comboBoxProfile.GetItemText(comboBoxProfile.SelectedItem);
            Properties.Settings.Default.TempFolder = textBoxTemp.Text;

            Properties.Settings.Default.Save();
        }

        private void buttonTemp_Click(object sender, EventArgs e)
        {
            folderBrowserDialogTemp.SelectedPath = textBoxTemp.Text;
            if (folderBrowserDialogTemp.ShowDialog(this) == DialogResult.OK)
                textBoxTemp.Text = folderBrowserDialogTemp.SelectedPath;
        }

        private void fxcCompile_Progress(FxcCompile o, double relativeValue)
        {
            progressBar1.Value = progressBar1.Minimum +(int)(relativeValue*(progressBar1.Maximum - progressBar1.Minimum));
            Application.DoEvents();
        }

        private void fxcCompile_Error(FxcCompile o, string error)
        {
            m_LogForm.AddText(error);
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
            fxcCompile.Progress = fxcCompile_Progress;

            if(!fxcCompile.Compile(textBoxInputFxc.Text, textBoxOutputFolder.Text+"\\"+textBoxOutputPrefix.Text,textBoxFxcExe.Text, profile, textBoxTemp.Text))
            {
                m_LogForm.AddText("Compile failed!");
                m_LogForm.ShowDialog(this);
                return;
            }
        }
    }
}
