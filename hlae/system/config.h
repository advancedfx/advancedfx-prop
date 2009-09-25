#pragma once

// Project :  Half-Life Advanced Effects
// File    :  hlae/system/config.h
// Changed :  2008-10-18
// Created :  2008-10-18

// Authors : last change / first change / name
// 2008-10-18 / 2008-10-18 / Dominik Tugend

//#using <System.Xml.dll>
//#using <System.dll>

#include <system/debug.h>

using namespace hlae;
using namespace hlae::debug;

using namespace System;
using namespace System::ComponentModel;
using namespace System::IO;
using namespace System::Xml;
using namespace System::Xml::Serialization;

namespace hlae {
namespace config {

public ref class CLauncher
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

public ref class CSettings
{
public:
	CLauncher ^Launcher;

public:
	CSettings()
	{
		Launcher = gcnew CLauncher();
	}
	void Default()
	{
		Launcher->Default();
	}

};

[XmlRootAttribute("HlaeConfig")]
public ref class CConfig
{
public:
	//[DefaultValue("unknown")]
	String ^Version;
	CSettings ^Settings;

public:
	CConfig()
	{
		Settings = gcnew CSettings();
		Default();
	}
	void Default()
	{
		Version = "unknown";
		Settings->Default();
	}
};

ref class CConfigMaster
{
public:
	CConfig ^Config;

public:

	CConfigMaster( DebugMaster ^debugMaster, String ^CfgPath )
	{
		this->debugMaster = debugMaster;
		this->CfgPath = CfgPath;
		WriteFileSyncer = gcnew System::Object();

		VERBOSE_MESSAGE( debugMaster, "Creating CConfigMaster" );

		if( System::IO::File::Exists( CfgPath ) )
		{
			// Read config:
			XmlSerializer^ serializer = gcnew XmlSerializer( CConfig::typeid );
			FileStream^ fs = gcnew FileStream( CfgPath, FileMode::Open );

			Config = dynamic_cast<CConfig^>(serializer->Deserialize( fs ));

			fs->Close();
		} else {
			Config = gcnew CConfig();
		}
		// always write back so we have the newest config version:
		BackUp();
	}

	// Threadsafe (won't crash due to concurring calls, config logic is not maintained of course)
	bool BackUp()
	{
		return WriteToFile( CfgPath );
	}

	// Threadsafe (won't crash due to concurring calls, config logic is not maintained of course)
	bool BackUp( String ^FilePath )
	{
		return WriteToFile( FilePath );
	}

private:
	System::Object ^WriteFileSyncer;
	DebugMaster ^debugMaster;
	String ^CfgPath;

	// Threadsafe (won't crash due to concurring calls, config logic is not maintained of course)
	bool WriteToFile( String ^FilePath )
	{
		bool bOk=false;

		try
		{
			Monitor::Enter( WriteFileSyncer );

			XmlSerializer^ serializer = gcnew XmlSerializer( CConfig::typeid );
			TextWriter^ writer = gcnew StreamWriter( FilePath );
			serializer->Serialize( writer, Config );
			writer->Close();

			bOk = true;
		}
		finally
		{
			Monitor::Exit( WriteFileSyncer );
		}

		return bOk;
	}
};

} // namespace config
} // namespace hlae 
