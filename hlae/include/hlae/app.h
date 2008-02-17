#ifndef _HLAE_APP_H_

	#define _HLAE_APP_H_

	#define wxUSE_NO_MANIFEST 0

	#include <wx/app.h>

	class hlaeApp : public wxApp {

		public:

			bool OnInit();

	};

	DECLARE_APP(hlaeApp)

#endif // _HLAE_APP_H_