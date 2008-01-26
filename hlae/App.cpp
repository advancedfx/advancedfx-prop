/* includes */
#include <hlae/App.h>
#include <hlae/FrameMain.h>

/* defines */
#define wxUSE_NO_MANIFEST 0 // to avoid problems with duplicate manifests

/* implementation */
bool hlaeApp::OnInit() {
	hlaeFrameMain* h_framemain = new hlaeFrameMain();
	SetTopWindow(h_framemain); // set as main window
	h_framemain->Show();
	return true;
}

IMPLEMENT_APP(hlaeApp) // entrypoint = hlaeApp::OnInit()