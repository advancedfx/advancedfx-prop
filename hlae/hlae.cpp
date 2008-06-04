// hlae.cpp : main project file.

#include "stdafx.h"
#include "system/debug.h"
#include "forms/MainForm.h"

using namespace hlae;
using namespace hlae::debug;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Init debug system:

	DebugMaster ^debugMaster = gcnew DebugMaster();

	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	ExampleDebugListener^ testListener = gcnew ExampleDebugListener( debugMaster );
	debugMaster->PostMessage( "Test!", DebugMessageType::DMT_INFO );

	// Create the main window and run it
	Application::Run(gcnew MainForm());


	delete debugMaster; // shutdown DebugMaster

	return 0;
}
