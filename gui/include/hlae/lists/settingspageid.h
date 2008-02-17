#ifndef _HLAE_LISTSETTINGSPAGEID_H_

	#define _HLAE_LISTSETTINGSPAGEID_H_

	#include <wx/list.h>
	#include <wx/treebase.h>

	#include <hlae/settingspages/template.h>

	struct hlaeListElementSettingsPageID
	{
		wxTreeItemId id;
		hlaeSettingsPageTemplate* window;
	};

	WX_DECLARE_LIST(hlaeListElementSettingsPageID, hlaeListSettingsPageID);
	

#endif // _HLAE_LISTSETTINGSPAGEID_H_