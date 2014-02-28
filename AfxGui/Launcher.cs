using System;
using System.Windows.Forms;

namespace AfxGui {

class Launcher
{
    public static bool RunLauncher(IWin32Window dialogOwner, Afx.AfxGoldSrc afxGoldSrc)
    {
        bool bOk;

        using (LauncherForm frm = new LauncherForm())
        {
            frm.Icon = Program.Icon;
            frm.ShowInTaskbar = false;

            CfgLauncher cfg = new CfgLauncher();

            cfg.CopyFrom(GlobalConfig.Instance.Settings.Launcher);

            frm.ReadFromConfig(cfg);

            if (DialogResult.OK == frm.ShowDialog(dialogOwner))
            {
                frm.WriteToConfig(cfg);

                bOk = afxGoldSrc.Start(cfg.MakeStartSettings());

                if (!bOk)
                    MessageBox.Show("Launching failed.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);

                if (cfg.RememberChanges)
                {
                    GlobalConfig.Instance.Settings.Launcher.CopyFrom(cfg);
                    GlobalConfig.Instance.BackUp();
                }
            }
            else
                bOk = true;

        }

        return bOk;

    }


}

} // namespace AfxGui {
