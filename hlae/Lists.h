#ifndef HLAELISTS_H
	#define HLAELISTS_H

	/* includes */
	#include <wx/list.h>

	/* declaration */
	struct hlaeLayoutElement {
		bool is_predefined;
		long id;
		wxString describtion;
		wxString layout;
	};

	WX_DECLARE_LIST(hlaeLayoutElement, hlaeLayoutList);

#endif // HLAELISTS_H