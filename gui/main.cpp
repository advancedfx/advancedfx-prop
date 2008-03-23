#include <wx/wx.h>

#include "defines.h"

#include "system/app.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	wxCmdLineArgType lpCmdLine, int nCmdShow)                          
{
	// Check for build mistakes
	wxAppConsole::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, HLAE_NAME);

	// Initialize the app
	wxAppInitializer app_initializer((wxAppInitializerFunction) new hlaeApp);

	// Entrypoint of wxWidgets
    return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}