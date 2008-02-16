#ifndef HEADER_HLAE_SETTINGSPAGEELEMENT

	#define HEADER_HLAE_SETTINGSPAGEELEMENT

	#include <wx/window.h>
	#include "Lists.h"
	
	class hlaeSettingsPageList;

	class hlaeSettingsPageElement
	{
		private:
			hlaeSettingsPageList* m_pagelist;
			bool m_isadvanced;
			wxString m_name;
			wxWindow* m_window;

		public:
			hlaeSettingsPageElement(wxWindow* window, const wxString& name, bool is_advanced);
			~hlaeSettingsPageElement();
			bool IsAdvanced();
			const wxString& GetName();
			wxWindow* GetWindow();
			void Append(hlaeSettingsPageElement* page_element);
			hlaeSettingsPageElement* GetElement(int index);
			size_t GetCount();
	};

#endif // HEADER_HLAE_SETTINGSPAGEELEMENT