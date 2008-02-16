#ifndef HEADER_HLAE_LIST

	#define HEADER_HLAE_LIST

	#include <wx/list.h>
	#include "SettingsPageElement.h"

	class hlaeSettingsPageElement;

	struct hlaeLayoutElement
	{
		bool is_predefined;
		long id;
		wxString describtion;
		wxString layout;
	};

	WX_DECLARE_LIST(hlaeLayoutElement, hlaeLayoutList);
	WX_DECLARE_LIST(hlaeSettingsPageElement, hlaeSettingsPageList);

	

#endif // HEADER_HLAE_LIST