#ifndef _HLAE_LISTLAYOUT_H_

	#define _HLAE_LISTLAYOUT_H_

	#include <wx/list.h>

	struct hlaeListElementLayout
	{
		bool is_predefined;
		long id;
		wxString describtion;
		wxString layout;
	};

	WX_DECLARE_LIST(hlaeListElementLayout, hlaeListLayout);

#endif // _HLAE_LISTLAYOUT_H_