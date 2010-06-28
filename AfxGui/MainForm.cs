// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-06-19 by dominik.matrixstorm.com
//
// First changes:
// 2008-05-18 by dominik.matrixstorm.com

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace AfxGui
{
    public partial class MainForm : Form
    {
        //
        // Public members:

        public MainForm()
        {
            InitializeComponent();
            this.Icon = Program.Icon;

	        m_UpdateCheckNotification = new UpdateCheckNotificationTarget(this, new UpdateCheckedDelegate(OnUpdateChecked));
        }

        //
        // Internal members:

        //
        // Private members:

        Tools.Calculator m_Calculator;
        hlae.remoting.HlaeRemoting m_HlaeRemoting;
        UpdateCheckNotificationTarget m_UpdateCheckNotification;

        private void Calculator_FormClosed(object sender, FormClosedEventArgs e)
        {
            m_Calculator = null;
        }

        void OnUpdateChecked(object o, IUpdateCheckResult checkResult)
        {
	        if(null != checkResult)
            {
		        // Has result:
		        if(checkResult.IsUpdated) 
                {
			        // Updated:
			        statusStrip.Visible = true;
			        statusLabelUpdate.IsLink = true;
			        statusLabelUpdate.Tag = null != checkResult.Uri ? checkResult.Uri.ToString() : "http://advancedfx.org/";
			        statusLabelUpdate.Text = "Update available!";
			        statusLabelUpdate.ForeColor = Color.Black;
			        statusLabelUpdate.BackColor = Color.Orange;
		        }
		        else
                {
			        // Is recent:
			        statusLabelUpdate.IsLink = false;
			        statusLabelUpdate.Text = "Your version is up to date :)";
			        statusLabelUpdate.ForeColor = Color.Black;
			        statusLabelUpdate.BackColor = Color.LightGreen;
		        }
	        }
	        else
            {
		        // Has no result (s.th. went wrong):
		        statusStrip.Visible = true;
		        statusLabelUpdate.IsLink = true;
		        statusLabelUpdate.Tag = "http://advancedfx.org/";
		        statusLabelUpdate.Text = "Update check failed :(";
		        statusLabelUpdate.ForeColor = Color.Black;
		        statusLabelUpdate.BackColor = Color.LightCoral;
	        }
        }

        void StartUpdateCheck()
        {
			this.statusLabelUpdate.IsLink = false;
			this.statusLabelUpdate.Text = "Checking for updates ...";
			this.statusLabelUpdate.ForeColor = Color.FromKnownColor(KnownColor.ControlText);
			this.statusLabelUpdate.BackColor = Color.FromKnownColor(KnownColor.Control);

			GlobalUpdateCheck.Instance.StartCheck();
        }

        private void menuFileSize_Click(object sender, EventArgs e)
        {
            if (null == m_Calculator)
            {
                m_Calculator = new Tools.Calculator();
                m_Calculator.FormClosed += Calculator_FormClosed;

                m_Calculator.Show();
            }
            else
            {
                m_Calculator.WindowState = FormWindowState.Normal;
                m_Calculator.BringToFront();
            }
        }

        private void menuAdvancedFxOrg_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start("http://advancedfx.org/");
        }

        private void checkNowToolStripMenuItem_Click(object sender, EventArgs e)
        {
            StartUpdateCheck();
            this.statusStrip.Visible = true;
        }

        private void statusBarToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.statusStrip.Visible = !this.menuStatusBar.Checked;
        }

        private void MainForm_Shown(object sender, EventArgs e)
        {
            GlobalUpdateCheck.Instance.BeginCheckedNotification(m_UpdateCheckNotification);

            if (0 < GlobalConfig.Instance.Settings.UpdateCheck)
            {
                this.menuAutoUpdateCheck.Checked = true;
                StartUpdateCheck();
            }
            else if (0 == GlobalConfig.Instance.Settings.UpdateCheck)
            {
                this.stripEnableUpdateCheck.Visible = true;
            }

	        // start up public remoting system (if requested):
	        if( System.Environment.CommandLine.Contains( "-ipcremote" ) )
                m_HlaeRemoting = new hlae.remoting.HlaeRemoting(gameWindowPanel);

        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            // close down remoting:
            if (null != m_HlaeRemoting) m_HlaeRemoting.Dispose();

            // close down ag:
            Afx.AfxGoldSrc ag = GlobalAfxGoldSrc.Instance;
            if (null != ag) ag.Dispose();

            GlobalUpdateCheck.Instance.EndCheckedNotification(m_UpdateCheckNotification);
        }

        private void statusLabelUpdate_Click(object sender, EventArgs e)
        {
			 if(statusLabelUpdate.IsLink)
             {
                System.Diagnostics.Process.Start(
					 statusLabelUpdate.Tag.ToString()
				);
			 }
        }

        private void statusStrip_VisibleChanged(object sender, EventArgs e)
        {
            this.menuStatusBar.Checked = this.statusStrip.Visible;
        }

        private void statusLabelHide_Click(object sender, EventArgs e)
        {
            this.statusStrip.Visible = false;
        }

        private void menuAutoUpdateCheck_Click(object sender, EventArgs e)
        {
            this.stripEnableUpdateCheck.Visible = false;

            menuAutoUpdateCheck.Checked = !menuAutoUpdateCheck.Checked;
            GlobalConfig.Instance.Settings.UpdateCheck = (SByte)(menuAutoUpdateCheck.Checked ? 1 : -1);
        }

        private void statusLabelAuto_Click(object sender, EventArgs e)
        {
            this.stripEnableUpdateCheck.Visible = false;

            if (this.statusLabelAutoYes == sender)
            {
                GlobalConfig.Instance.Settings.UpdateCheck = (SByte)1;
                StartUpdateCheck();
            }
            else
            {
                GlobalConfig.Instance.Settings.UpdateCheck = (SByte)(-1);
            }
        }

        private void demoToolsToolStripMenuItem_Click(object sender, EventArgs e)
        {
	        AfxCppCli.old.tools.DemoToolsWizard demoWiz = new AfxCppCli.old.tools.DemoToolsWizard();

            demoWiz.OutputPath = GlobalConfig.Instance.Settings.DemoTools.OutputFolder;

	        if(demoWiz.ShowDialog(this))
	        {
                GlobalConfig.Instance.Settings.DemoTools.OutputFolder = demoWiz.OutputPath;
                GlobalConfig.Instance.BackUp();
	        }

            //demoWiz.Dispose();
        }

        private void skyToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AfxCppCli.old.tools.skymanager sm = new AfxCppCli.old.tools.skymanager(GlobalConfig.Instance.Settings.Launcher.GamePath);
			sm.Show(this);
        }

        private void menuExit_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void menuCustomLoader_Click(object sender, EventArgs e)
        {
            AfxGui.Tools.CustomLoader.RunCustomLoader(this);
        }

        private void menuLaunch_Click(object sender, EventArgs e)
        {
            Launcher.RunLauncher(this, this.gameWindowPanel);
        }

        private void menuGuidToClipBoard_Click(object sender, EventArgs e)
        {
            Clipboard.SetText(GlobalUpdateCheck.Instance.Guid.ToString());           
        }
    }
}
