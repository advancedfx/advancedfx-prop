// ipc_remoting.cpp : main project file.

//	This is the Half-Life Advanced Effects remoting example (C++).
//
//	It assumes that HLAE has been already launched with the -ipcremote option,
//  the user has made his or her default settings already (path to game etc.)
//  and the game has not been launched yet.
//
//	The sample is puzzled together out of various Microsoft Developer Network
//	(MSDN) Library samples, for more information about .NET remoting see here:
//	.NET Remoting
//	  http://msdn.microsoft.com/en-us/library/72x4h507(VS.80).aspx
//
//	For more information see the HLAEwiki.

#using <System.Runtime.Remoting.dll>

using namespace System;
using namespace System::Runtime::Remoting;
using namespace System::Runtime::Remoting::Channels;
using namespace System::Runtime::Remoting::Channels::Ipc;

namespace hlae {
namespace remoting {

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
	//    Tells HLAE to launch the engine
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

} // namespace remoting
} // namespace hlae

using namespace hlae::remoting;

int main(array<System::String ^> ^args)
{
	Console::WriteLine( "Launch \"hlae.exe -ipcremote\" and press [ENTER] to continue" );
	Console::ReadLine();

	IpcChannel ^channel = gcnew IpcChannel();
	ChannelServices::RegisterChannel( channel );

	// Instead of creating a new object, this obtains a reference
	// to the server's single instance of the ServiceClass object.
	IHlaeRemote_1 ^myremote = (IHlaeRemote_1 ^)Activator::GetObject(
		IHlaeRemote_1::typeid,
		"ipc://localhost:31337/Hlae.Remote.1"
	);

	try
	{
		// check if the interface is current or if it is deprecated (will be removed or replaced soon):
		if(myremote->IsDeprecated())
		{
			Console::WriteLine("WARNING: This program uses a deprecated interface, please tell the author to update to the new interface version.");
		}

		// get the users customargs:
		String ^custargs = myremote->GetCustomArgs();

		Console::WriteLine( "Users current CustomArgs are: {0}", custargs); 

		// appends s.th. to the user's custormargs:
		// For our example we will append s.th. that we'll be able to read in the console later.
		// Please note: some HLAE hooks might not yet be in place when those commands get executed!
		custargs = String::Concat( custargs, " +echo HelloWorldFromRemoting" );

		Console::WriteLine( "Launching with overriden new CustomArgs: {0}", custargs); 

		// Launche the game:
		if(!myremote->LaunchEx(custargs))
		{
			Console::WriteLine("ERROR: HLAE Failed launching.");
		} else {
			Console::WriteLine("HLAE didn't report any problem when launching.");
		}

		
	}
	catch (Exception ^ex)
	{
		Console::WriteLine( String::Format("Exception of type: {0} occurred.", ex->ToString()) );
		Console::WriteLine( "Details: {0}", ex->Message );
	}

    return 0;
}
