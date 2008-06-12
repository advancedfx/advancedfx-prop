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
#ifdef _DEBUG
	// attach a fileListener in debug mode:
	FileDebugListener^ fileListener = gcnew FileDebugListener( debugMaster, "hlae.log.txt" );
#endif

	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 


	debugMaster->PostMessage( "Test!", DebugMessageType::DMT_INFO );
	debugMaster->PostMessage( "Test2", DebugMessageType::DMT_INFO );
	debugMaster->PostMessage( "Test3", DebugMessageType::DMT_INFO );
	DEBUG_MESSAGE( debugMaster, "TestDebug" );

	// Create the main window and run it
	Application::Run(gcnew MainForm());


#ifdef _DEBUG
	delete fileListener;
#endif
	// Explicetely invoke Dispose, otherwise it can never be collected because the thread blocks
	delete debugMaster;

	return 0;
}
