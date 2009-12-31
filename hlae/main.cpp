// hlae.cpp : main project file.

#include "stdafx.h"

#include "system/debug.h"
#include "system/debug_file.h"
#include "system/config.h"
#include "system/globals.h"


#include "forms/MainForm.h"

using namespace hlae;
using namespace hlae::debug;
using namespace hlae::globals;


[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	//
	// .NET Framework Service Pack check:

	System::Version ^ envVersion = System::Environment::Version;
	System::String ^ strVer = nullptr;

	if(
		50727 == envVersion->Build
		&& 42 <= envVersion->Revision
	) {
		if(envVersion->Revision < 1433) strVer = "Original release";
		else if(envVersion->Revision < 3053) strVer = "Service Pack 1";
	}
	else strVer = "unknown";

	if(nullptr != strVer) System::Windows::Forms::MessageBox::Show(
		System::String::Concat(
			"HLAE is targeted at .NET framework 2.0 Service Pack 2 or newer.\n"
			"But your installed CLR version is ", envVersion->ToString(), " (", strVer, ").\n",
			"\n",
			"We recommend upgrading to the latest Service Pack to ensure HLAE operates as intended!"
		),
		".NET framework version warning",
		System::Windows::Forms::MessageBoxButtons::OK,
		System::Windows::Forms::MessageBoxIcon::Warning
	);


	//

	String ^BaseDir = System::Windows::Forms::Application::StartupPath;
	String ^cmdLine = System::Environment::CommandLine;

	// Init debug system:
	DebugMaster ^debugMaster = gcnew DebugMaster();
	FileDebugListener^ debugFile = nullptr;

#ifdef _DEBUG
	// attach a fileListener in debug mode:
	debugFile = gcnew FileDebugListener( debugMaster, "hlae.log.txt" );
#endif

	INFO_MESSAGE( debugMaster, "Debug Message System online" );

	// start up config system:
	HlaeConfig::Load(String::Concat( BaseDir,"\\hlaeconfig.xml"));

	// fill in globals:
	CGlobals ^Globals = gcnew CGlobals();
	Globals->debugMaster = debugMaster;

	// Create the main window and run it
	Application::Run(gcnew MainForm(Globals,debugFile));

	// remove globals:
	delete Globals;

	// shutdown debug system:

#ifdef _DEBUG
	debugMaster->SetFilter(DebugMessageType::DMT_DEBUG,DebugFilterSetting::DFS_NODROP);
	if (DebugQueueState::DQS_OK != debugMaster->GetLastQueueState())
		DEBUG_MESSAGE(debugMaster,"Queue was NOT DQS_OK");
	else
		DEBUG_MESSAGE(debugMaster,"Queue was DQS_OK");
	
	debugMaster->Flush();
	debugFile->Flush();
	delete debugFile;
#endif

	// Explicetely invoke Dispose, otherwise it can never be collected because the thread blocks
	delete debugMaster;

	return 0;
}
