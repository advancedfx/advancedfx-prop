// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-06-19 by dominik.matrixstorm.com
//
// First changes:
// 2008-10-18 by dominik.matrixstorm.com

using System;
using System.ComponentModel;
using System.IO;
using System.Threading;
using System.Xml;
using System.Xml.Serialization;

namespace AfxGui {

public class CfgLauncher
{
    public String CustomCmdLine;
    public Boolean ForceAlpha;
    public Boolean FullScreen;
    public String GamePath;
    public Boolean GfxForce;
    public UInt16 GfxWidth;
    public UInt16 GfxHeight;
    public Byte GfxBpp;
    public String Modification;
    public Boolean OptimizeDesktopRes;
    public Boolean OptimizeVisibilty;
    public Boolean RememberChanges;
    public Byte RenderMode;

    internal void Default()
	{
		GamePath = "please select";
		Modification ="cstrike";
		CustomCmdLine = "+toggleconsole";
		GfxForce = true;
		GfxWidth = 800;
		GfxHeight = 600;
		GfxBpp = 32;
		RememberChanges = true;
		ForceAlpha = false;
		OptimizeDesktopRes = false;
		OptimizeVisibilty = true;
		FullScreen = false;
		RenderMode = 0;		
	}

    internal void CopyFrom(CfgLauncher from)
    {
		this.GamePath = String.Copy(from.GamePath);
		this.Modification = String.Copy(from.Modification);
		this.CustomCmdLine = String.Copy(from.CustomCmdLine);
		this.GfxForce = from.GfxForce;
		this.GfxWidth = from.GfxWidth;
		this.GfxHeight = from.GfxHeight;
		this.GfxBpp = from.GfxBpp;
		this.RememberChanges = from.RememberChanges;
		this.ForceAlpha = from.ForceAlpha;
		this.OptimizeDesktopRes = from.OptimizeDesktopRes;
		this.OptimizeVisibilty = from.OptimizeVisibilty;
		this.FullScreen = from.FullScreen;
		this.RenderMode = from.RenderMode;
	}

    internal Afx.AfxGoldSrc.StartSettings MakeStartSettings()
    {
        Afx.AfxGoldSrc.StartSettings s = new Afx.AfxGoldSrc.StartSettings(
            this.GamePath,
            this.Modification
        );

		s.Alpha8 = this.ForceAlpha;
		s.Bpp = this.GfxBpp;
		s.CustomLaunchOptions = this.CustomCmdLine;
		s.ForceRes = this.GfxForce;
		s.FullScreen = this.FullScreen;
		s.HalfLifePath = this.GamePath;
		s.Height = this.GfxHeight;
		s.Modification = this.Modification;
		s.OptWindowVisOnRec = this.OptimizeVisibilty;
		switch(this.RenderMode) {
		case 1:
            s.RenderMode = Afx.AfxGoldSrc.RenderMode.FrameBufferObject;
			break;
		case 2:
            s.RenderMode = Afx.AfxGoldSrc.RenderMode.MemoryDC;
			break;
		default:
            s.RenderMode = Afx.AfxGoldSrc.RenderMode.Default;
            break;
		}
		s.Width = this.GfxWidth;

        return s;
    }
}


public class CfgCustomLoader
{
	public String CmdLine;
    public String HookDllPath;
    public String ProgramPath;

    internal void Default()
	{
		HookDllPath = "";
		ProgramPath = "";
		CmdLine = "-steam -insecure -window -console -game cstrike";
	}
}


public class CfgDemoTools
{
    public String OutputFolder;

    internal void Default()
    {
        OutputFolder = "";
    }
}


public class CfgSettings
{
    public CfgLauncher Launcher;
    public CfgCustomLoader CustomLoader;
    public CfgDemoTools DemoTools;
    public SByte UpdateCheck;
    public Guid IgnoreUpdateGuid;

	public CfgSettings()
	{
		Launcher = new CfgLauncher();
		CustomLoader = new CfgCustomLoader();
		DemoTools = new CfgDemoTools();
	}

    internal void Default()
	{
		Launcher.Default();
		CustomLoader.Default();
		DemoTools.Default();

        UpdateCheck = 0;
	}

}

[XmlRootAttribute("HlaeConfig")]
public class Config
{
    //
    // Public members:

	public String Version;
	public CfgSettings Settings;

    public Config()
	{
		Settings = new CfgSettings();

        m_CfgPath = "hlaeconfig.xml";

        Default();
	}

    //
    // Internal members:

    internal static Config LoadOrCreate(String cfgPath)
    {
        Config config = Load(cfgPath);
        if(null == config)
        {
            config = new Config();
            config.CfgPath = cfgPath;
        }

        return config;
    }

    internal static Config Load(String cfgPath)
	{
        Config config = null;

		if( File.Exists( cfgPath ) )
		{
            FileStream fs = null;

            try
            {
                XmlSerializer serializer = new XmlSerializer(typeof(Config));
    			fs = new FileStream( cfgPath, FileMode.Open );

                config = serializer.Deserialize(fs) as Config;
            }
            catch(Exception)
            {
                config = null;
            }
        }

        if(null != config)
            config.CfgPath = cfgPath;

        return config;
	}

    internal bool BackUp()
	{
		return WriteToFile( m_CfgPath );
	}

    internal bool BackUp(String filePath)
	{
		return WriteToFile( filePath );
	}

    internal void Default()
    {
        Settings.Default();

        Version = "unknown";
    }

    //
    // Internal properties:

    internal String CfgPath
    {
        get
        {
            return m_CfgPath;
        }
        set
        {
            m_CfgPath = value;
        }
    }

    //
    // Private members:

	String m_CfgPath;

	bool WriteToFile( String filePath )
	{
		bool bOk=false;

        TextWriter writer = null;

		try
		{
			XmlSerializer serializer = new XmlSerializer( typeof(Config) );
            writer = new StreamWriter(filePath);

			serializer.Serialize( writer, this );

			bOk = true;
		}
		catch(Exception)
		{
			bOk = false;
		}

        if(null != writer)
            writer.Close();

		return bOk;
	}
}

} // namespace AfxGui
