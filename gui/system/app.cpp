#include <wx/wx.h>

#include <system/config.h>
#include <windows/hlae.h>

#include "app.h"


bool hlaeApp::OnInit()
{
	// Initialize the config system
	g_config.Initialize();

	// Create the main window
	hlaeMainWindow* main_window = new hlaeMainWindow();
	SetTopWindow(main_window);
	main_window->Show();

	return true;
}