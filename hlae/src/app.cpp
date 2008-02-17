#include <hlae/app.h>
#include <hlae/frames/main.h>

IMPLEMENT_APP(hlaeApp)

bool hlaeApp::OnInit() {

	hlaeFrameMain* h_framemain = new hlaeFrameMain();

	SetTopWindow(h_framemain);

	h_framemain->Show();

	return true;
}