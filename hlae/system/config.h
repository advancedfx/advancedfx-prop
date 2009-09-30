#pragma once

// Project :  Half-Life Advanced Effects
// File    :  hlae/system/config.h
// Changed :  2008-10-18
// Created :  2008-10-18

// Authors : last change / first change / name
// 2008-10-18 / 2008-10-18 / Dominik Tugend


using namespace System;
using namespace System::ComponentModel;
using namespace System::IO;
using namespace System::Threading;
using namespace System::Xml;
using namespace System::Xml::Serialization;

using namespace hlae;

namespace hlae {

public ref class CfgLauncher
{
public:
	//[DefaultValue("please select")]
	String ^GamePath;

	//[DefaultValue("cstrike")]
	String ^Modification;

	//[DefaultValue("+toggleconsole")]
	String ^CustomCmdLine;

	//[DefaultValue(true)]
	System::Boolean GfxForce;

	//[DefaultValue(800)]
	UInt16 GfxWidth;

	//[DefaultValue(600)]
	UInt16 GfxHeight;

	//[DefaultValue(32)]
	Byte GfxBpp;

	//[DefaultValue(true)]
	Boolean RememberChanges;

	// Advanced Settings:

	//[DefaultValue(false)]
	Boolean ForceAlpha;

	//[DefaultValue(true)]
	Boolean OptimizeDesktopRes;

	//[DefaultValue(true)]
	Boolean OptimizeVisibilty;

	//[DefaultValue(true)]
	Boolean StartDocked;

	//[DefaultValue(false)]
	Boolean FullScreen;

	//[DefaultValue("default")]
	Byte RenderMode;

	// String ^RenderMode;
	//[System.Xml.Serialization.XmlIgnoreAttribute]
	// bool ModificationSpecified;

public:
	void Default()
	{
		GamePath = "please select";
		Modification ="cstrike";
		CustomCmdLine = "+toggleconsole";
		GfxForce = true;
		GfxWidth = 800;
		GfxHeight = 600;
		GfxBpp = 32;
		RememberChanges = true;
		// Advanced Settings:
		ForceAlpha = false;
		OptimizeDesktopRes = false;
		OptimizeVisibilty = true;
		StartDocked = true;
		FullScreen = false;
		RenderMode = 0;		
	}
};


public ref class CfgCustomLoader
{
public:
	String ^ HookDllPath;
	String ^ ProgramPath;
	String ^ CmdLine;

public:
	void Default()
	{
		HookDllPath = "";
		ProgramPath = "";
		CmdLine = "-steam -game cstrike";
	}
};

public ref class CfgSettings
{
public:
	CfgLauncher ^Launcher;
	CfgCustomLoader ^ CustomLoader;

public:
	CfgSettings()
	{
		Launcher = gcnew CfgLauncher();
		CustomLoader = gcnew CfgCustomLoader();
	}
	void Default()
	{
		Launcher->Default();
		CustomLoader->Default();
	}

};

[XmlRootAttribute("HlaeConfig")]
public ref class CfgConfig
{
public:
	//[DefaultValue("unknown")]
	String ^Version;
	CfgSettings ^Settings;

public:
	CfgConfig()
	{
		Settings = gcnew CfgSettings();
		Default();
	}
	void Default()
	{
		Version = "unknown";
		Settings->Default();
	}
};

ref class HlaeConfig
{
public:
	static void Load(String ^ cfgPath)
	{
		m_CfgPath = cfgPath;
		m_WriteFileSyncer = gcnew System::Object();

		if( System::IO::File::Exists( cfgPath ) )
		{
			// Read config:
			XmlSerializer^ serializer = gcnew XmlSerializer( CfgConfig::typeid );
			FileStream^ fs = gcnew FileStream( cfgPath, FileMode::Open );

			m_Config = dynamic_cast<CfgConfig ^>(serializer->Deserialize( fs ));

			fs->Close();
		} else {
			m_Config = gcnew CfgConfig();
		}

		// always write back so we have the newest config version:
		BackUp();
	}

	// Threadsafe (won't crash due to concurring calls, config logic is not maintained of course)
	static bool BackUp()
	{
		return WriteToFile( m_CfgPath );
	}

	// Threadsafe (won't crash due to concurring calls, config logic is not maintained of course)
	static bool BackUp( String ^FilePath )
	{
		return WriteToFile( FilePath );
	}

	static property CfgConfig ^ Config {
		CfgConfig ^ get() { return m_Config; }
	}

private:
	static String ^ m_CfgPath;
	static CfgConfig ^ m_Config;
	static System::Object ^ m_WriteFileSyncer;

	// Threadsafe (won't crash due to concurring calls, config logic is not maintained of course)
	static bool WriteToFile( String ^FilePath )
	{
		bool bOk=false;

		try
		{
			Monitor::Enter( m_WriteFileSyncer );

			XmlSerializer^ serializer = gcnew XmlSerializer( CfgConfig::typeid );
			TextWriter^ writer = gcnew StreamWriter( FilePath );
			serializer->Serialize( writer, m_Config );
			writer->Close();

			bOk = true;
		}
		finally
		{
			Monitor::Exit( m_WriteFileSyncer );
		}

		return bOk;
	}
};

} // namespace hlae 

