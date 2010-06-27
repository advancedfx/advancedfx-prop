using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace AfxGui
{
    static class Program
    {


        //
        // Internal members:

        internal static String BaseDir
        {
            get
            {
                return m_BaseDir;
            }
        }

        internal static System.Drawing.Icon Icon
        {
            get
            {
                return m_Icon;
            }
        }


        //
        // Private members:

        static String m_BaseDir;
        static System.Drawing.Icon m_Icon;

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            m_BaseDir = System.Windows.Forms.Application.StartupPath;

            m_Icon = System.Drawing.Icon.ExtractAssociatedIcon(System.Windows.Forms.Application.ExecutablePath);

            GlobalConfig.Instance = Config.LoadOrCreate(String.Concat(m_BaseDir, "\\hlaeconfig.xml"));
            GlobalUpdateCheck.Instance = new UpdateCheck();

            Application.Run(new MainForm());

            GlobalConfig.Instance.BackUp();

            GlobalUpdateCheck.Instance.Dispose();
        }
    }
}
