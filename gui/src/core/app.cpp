#include <hlae/core/app.h>
#include <hlae/windows/main.h>
#include <hlae/core/debug.h>

IMPLEMENT_APP(hlaeApp)

hlaeApp::hlaeApp()
{
	g_debug.SetApp(this);
}

bool hlaeApp::OnInit()
{
	hlaeMainWindow* h_framemain = new hlaeMainWindow();
	SetTopWindow(h_framemain);
	h_framemain->Show();

	return true;
}