// hlae.cpp : main project file.

#include "stdafx.h"
#include "system/debug.h"
#include "system/debug_file.h"
#include "forms/MainForm.h"

using namespace hlae;
using namespace hlae::debug;


[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Init debug system:
	DebugMaster ^debugMaster = gcnew DebugMaster();
	FileDebugListener^ debugFile = nullptr;

#ifdef _DEBUG
	// attach a fileListener in debug mode:
	debugFile = gcnew FileDebugListener( debugMaster, "hlae.log.txt" );
#endif

	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	INFO_MESSAGE( debugMaster, "Debug Message System online started." );

	// Create the main window and run it
	Application::Run(gcnew MainForm(debugMaster,debugFile));

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
