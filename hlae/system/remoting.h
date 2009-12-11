#pragma once

// Project :  Half-Life Advanced Effects
// File    :  hlae/system/remoting.h
// Purpose :  .NET remoting classes

// Authors : last change / first change / name
// 2008-11-02 / 2008-11-02 / Dominik Tugend

#using <System.Runtime.Remoting.dll>

#include <system/loader.h>

namespace hlae {
namespace remoting {

#include <system/debug.h>
#include <system/config.h>
#include <system/globals.h>

using namespace System;
using namespace System::Runtime::Remoting;
using namespace System::Runtime::Remoting::Channels;
using namespace System::Runtime::Remoting::Channels::Ipc;

using namespace hlae::debug;
using namespace hlae::globals;

#define HLAE_REMOTING_OBJ_URI_HlaeRemote_1 "Hlae.Remote.1"

//	HlaeRemote_1
//
//	This is the main remoting interface class, which
//  shall be exposed for people wanting to use it.
//
//  make sure this interface is defined in the hlae::remoting namespace
public interface class IHlaeRemote_1
{
public:
	//	IsDeprecated
	//
	//  Retruns true in case the interface is deprecated and will go away
	//  in futur updates.
	bool IsDeprecated();

	//	GetCustomArgs
	//  
	//	Returns:
	//    null reference on error,
	//    otherwise the cusomargs the user set in the launcher as String
	String ^ GetCustomArgs();

	//  Launch
	//    Launches the engine
	//	  Warning: HLAE currently does no checks if the game is running etc.,
	//    so you might want to check if hl.exe is already running yourself.
	//  Returns:
	//    false on error, otherwise true
	bool Launch();

	//	LaunchEx
	//    Extends Launch
	//  Params:
	//    OverrideCustomArgs: use these instead of the user's default CustomArgs
	//    (Also see GetCustomArgs).
	bool LaunchEx( String^ OverrideCustomArgs );
};

////////////////////////////////////////////////////////////////////////////////

public ref class HlaeRemote_1 : public MarshalByRefObject, public IHlaeRemote_1
{
private:
	CGlobals ^globals;
public:
	HlaeRemote_1( CGlobals ^globals )
	{
		if( this->globals ) return;
		this->globals = globals;
	}

	virtual bool IsDeprecated()
	{
		return false;
	}

	virtual String ^ GetCustomArgs()
	{
		return HlaeConfig::Config->Settings->Launcher->CustomCmdLine;
	}

	virtual bool Launch()
	{
		return LaunchEx( HlaeConfig::Config->Settings->Launcher->CustomCmdLine );
	}

	virtual bool LaunchEx( String^ OverrideCustomArgs )
	{
		// first check if HL.exe is already running:
		array<System::Diagnostics::Process^>^procs = System::Diagnostics::Process::GetProcessesByName( "hl" );
		if(0 < procs->Length)
			return false;

		CfgLauncher ^cfg = gcnew CfgLauncher();

		cfg->CopyFrom(HlaeConfig::Config->Settings->Launcher);
		cfg->CustomCmdLine = OverrideCustomArgs;

		return AfxGoldSrcLaunch(cfg);
	}
};



//	HlaeRemoting
//
//	Warning this is a Singelton class!
//
ref class HlaeRemoting
{
private:
	CGlobals ^globals;
	IpcChannel ^serverChannel;
	HlaeRemote_1 ^hlaeRemote_1;
	ObjRef ^ref_HlaeRemote_1;
public:
	HlaeRemoting( CGlobals ^globals )
	{
		this->globals = globals;

		VERBOSE_MESSAGE( globals->debugMaster, "Starting up IPC remoting class ..." );

		//
		//	Init global scope class that will be accessed by the remote:

		hlaeRemote_1 = gcnew HlaeRemote_1( this->globals );
		
		//
		//	Start remoting server:

		// based on MS example: http://msdn.microsoft.com/en-us/library/system.runtime.remoting.channels.ipc.ipcchannel(VS.80).aspx

		// Create the channel:
		serverChannel = gcnew IpcChannel( "localhost:31337" );

		// Register the channel:
		DEBUG_MESSAGE( globals->debugMaster, "Registering IPC channel ..." );
		ChannelServices::RegisterChannel( serverChannel );

		DEBUG_MESSAGE( globals->debugMaster, String::Format( "The name of the channel is {0}.", serverChannel->ChannelName ) );
		DEBUG_MESSAGE( globals->debugMaster, String::Format( "The priority of the channel is {0}.", serverChannel->ChannelPriority ) );

		DEBUG_MESSAGE( globals->debugMaster, "Marshalling HlaeRemote_1 ..." );
		DEBUG_MESSAGE( globals->debugMaster, String::Format("Mtypeid: {0}",IHlaeRemote_1::typeid) );
		ref_HlaeRemote_1 = RemotingServices::Marshal( hlaeRemote_1, HLAE_REMOTING_OBJ_URI_HlaeRemote_1, IHlaeRemote_1::typeid );

		DEBUG_MESSAGE( globals->debugMaster, String::Format( "ref_HlaeRemote_1->URI is {0}.", ref_HlaeRemote_1->URI ) );

	}

	~HlaeRemoting()
	{
		VERBOSE_MESSAGE( globals->debugMaster, String::Format( "Shutting down up IPC remoting class ..." ) );

		DEBUG_MESSAGE( globals->debugMaster, "Disconnecting HlaeRemote_1 ..." );
		RemotingServices::Disconnect( hlaeRemote_1 );

		DEBUG_MESSAGE( globals->debugMaster, "Unregistering IPC channel ..." );
		ChannelServices::UnregisterChannel( serverChannel );

		delete hlaeRemote_1;
	}
};

} // namespace remoting
} // namespace hlae
