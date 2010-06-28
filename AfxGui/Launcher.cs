using System;
using System.Windows.Forms;

namespace AfxGui {

class Launcher
{
    public static bool RunLauncher(IWin32Window dialogOwner, ScrollableControl gameWindowParent)
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

                Afx.AfxGoldSrc ag = GlobalAfxGoldSrc.GetInstanceOrCreate();

                bOk = ag.Start(cfg.MakeStartSettings(gameWindowParent));

                if (!bOk)
                    MessageBox.Show("Launching failed.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);

                if (cfg.RememberChanges)
                    GlobalConfig.Instance.Settings.Launcher.CopyFrom(cfg);
            }
            else
                bOk = true;

        }

        return bOk;

    }


}

} // namespace AfxGui {
