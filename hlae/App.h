#ifndef HLAEAPP_H
#define HLAEAPP_H

	/* includes */
	#include <wx/app.h>

	/* declaration */
	class hlaeApp : public wxApp {

		public:
			bool OnInit(); // entrypoint of the app

	};

	DECLARE_APP(hlaeApp); // declares the entrypoint as hlaeApp::OnInit()

#endif // HLAEAPP_H