#ifndef _HLAE_SETTINGSPAGETEMPLATE_H_

	#define _HLAE_SETTINGSPAGETEMPLATE_H_

	#include <wx/window.h>

	class hlaeSettingsPageTemplate : public wxWindow 
	{
		public:

			hlaeSettingsPageTemplate(wxWindow* parent);

			virtual void ShowPage(bool is_advanced);
			virtual void ApplyChanges();
			
	};

#endif // _HLAE_SETTINGSPAGETEMPLATE_H_