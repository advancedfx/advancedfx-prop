#ifndef _HLAE_SETTINGSPAGEGENERAL_H_

	#define _HLAE_SETTINGSPAGEGENERAL_H_

	#include <hlae/settingspages/template.h>

	class hlaeSettingsPageGeneral : public hlaeSettingsPageTemplate 
	{
		public:

			hlaeSettingsPageGeneral(wxWindow* parent);
			void ShowPage(bool is_advanced);
			void ApplyChanges();

	};

#endif // _HLAE_SETTINGSPAGEGENERAL_H_