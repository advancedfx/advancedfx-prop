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

        static void ProcessArgsAfxHookGoldSrc(string[] args)
        {
            for (int i = 0; i < args.Length; i++)
            {
                string arg = args[i];
                switch (arg)
                {
                case "-autoStart":
                    Globals.AutoStartAfxHookGoldSrc = true;
                    break;
                case "-gamePath":
                    if (i + 1 < args.Length)
                    {
                        GlobalConfig.Instance.Settings.Launcher.GamePath = args[i + 1];
                        i++;
                    }
                    break;
                case "-modification":
                    if (i + 1 < args.Length)
                    {
                        GlobalConfig.Instance.Settings.Launcher.Modification = args[i + 1];
                        i++;
                    }
                    break;
                case "-customCmdLine":
                    if (i + 1 < args.Length)
                    {
                        GlobalConfig.Instance.Settings.Launcher.CustomCmdLine = args[i + 1];
                        i++;
                    }
                    break;
                case "-gfxForce":
                    if (i + 1 < args.Length)
                    {
                        Boolean.TryParse(args[i + 1], out GlobalConfig.Instance.Settings.Launcher.GfxForce);
                        i++;
                    }
                    break;
                case "-gfxWidth":
                    if (i + 1 < args.Length)
                    {
                        UInt16.TryParse(args[i + 1], out GlobalConfig.Instance.Settings.Launcher.GfxWidth);
                        i++;
                    }
                    break;
                case "-gfxHeight":
                    if (i + 1 < args.Length)
                    {
                        UInt16.TryParse(args[i + 1], out GlobalConfig.Instance.Settings.Launcher.GfxHeight);
                        i++;
                    }
                    break;
                case "-gfxBpp":
                    if (i + 1 < args.Length)
                    {
                        Byte.TryParse(args[i + 1], out GlobalConfig.Instance.Settings.Launcher.GfxBpp);
                        i++;
                    }
                    break;
                //case "-rememberChanges":
                case "-foreceAlpha":
                    if (i + 1 < args.Length)
                    {
                        Boolean.TryParse(args[i + 1], out GlobalConfig.Instance.Settings.Launcher.ForceAlpha);
                        i++;
                    }
                    break;
                case "-optimizeDesktopRes":
                    if (i + 1 < args.Length)
                    {
                        Boolean.TryParse(args[i + 1], out GlobalConfig.Instance.Settings.Launcher.OptimizeDesktopRes);
                        i++;
                    }
                    break;
                case "-optimizeVisibilty":
                    if (i + 1 < args.Length)
                    {
                        Boolean.TryParse(args[i + 1], out GlobalConfig.Instance.Settings.Launcher.OptimizeVisibilty);
                        i++;
                    }
                    break;
                case "-fullScreen":
                    if (i + 1 < args.Length)
                    {
                        Boolean.TryParse(args[i + 1], out GlobalConfig.Instance.Settings.Launcher.FullScreen);
                        i++;
                    }
                    break;
                case "-renderMode":
                    if (i + 1 < args.Length)
                    {
                        Byte.TryParse(args[i + 1], out GlobalConfig.Instance.Settings.Launcher.RenderMode);
                        i++;
                    }
                    break;
                }
            }
        }

        static void ProcessCommandLine()
        {
            string [] argv = new string[0];
            
            try
            {
                argv = System.Environment.GetCommandLineArgs();
            }
            catch(NotSupportedException)
            {
            }

            if (0 < argv.Length)
            {
                string[] tmp = new string[argv.Length - 1];
                Array.Copy(argv, 1, tmp, 0, argv.Length - 1);
                argv = tmp;
            }

            Globals.EnableHlaeRemote = Array.Exists<string>(argv, p => p == "-ipcremote");

            if (Array.Exists<string>(argv, p => p == "-afxHookGoldSrc"))
            {
                ProcessArgsAfxHookGoldSrc(argv);
            }
        }

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

            ProcessCommandLine();

            Application.Run(new MainForm());

            GlobalConfig.Instance.BackUp();

            GlobalUpdateCheck.Instance.Dispose();
        }
    }
}
