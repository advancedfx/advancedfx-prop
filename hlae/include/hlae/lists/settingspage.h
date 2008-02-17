#ifndef _HLAE_LISTSETTINGSPAGE_H_

	#define _HLAE_LISTSETTINGSPAGE_H_

	#include <wx/list.h>

	#include <hlae/settingspages/template.h>

	class hlaeListElementSettingsPage;	

	WX_DECLARE_LIST(hlaeListElementSettingsPage, hlaeListSettingsPage);

	class hlaeListElementSettingsPage
	{
		private:
			hlaeListSettingsPage* m_pagelist;
			bool m_isadvanced;
			wxString m_name;
			hlaeSettingsPageTemplate* m_window;

		public:
			hlaeListElementSettingsPage(hlaeSettingsPageTemplate* window,
				const wxString& name, bool is_advanced);
			~hlaeListElementSettingsPage();
			bool IsAdvanced();
			const wxString& GetName();
			hlaeSettingsPageTemplate* GetWindow();
			void Append(hlaeListElementSettingsPage* page_element);
			hlaeListElementSettingsPage* GetElement(int index);
			size_t GetCount();
	};

#endif // _HLAE_LISTSETTINGSPAGE_H_