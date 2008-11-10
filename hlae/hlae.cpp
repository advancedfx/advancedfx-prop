// hlae.cpp : main project file.

#include "stdafx.h"

#include "system/debug.h"
#include "system/debug_file.h"
#include "system/config.h"
#include "system/globals.h"


#include "forms/MainForm.h"

using namespace hlae;
using namespace hlae::debug;
using namespace hlae::config;
using namespace hlae::globals;


[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

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
	CConfigMaster ^ConfigMaster = gcnew CConfigMaster( debugMaster, String::Concat( BaseDir,"\\hlaeconfig.xml") );

	// fill in globals:
	CGlobals ^Globals = gcnew CGlobals();
	Globals->debugMaster = debugMaster;
	Globals->ConfigMaster = ConfigMaster;

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
