#pragma once

#include <system/debug.h>
#include <system/globals.h>

namespace hlae {

	using namespace hlae::debug;
	using namespace hlae::globals;

	ref class DemoToolsWizard
	{
	public:
		DemoToolsWizard( System::Windows::Forms::Form ^parentWindow, CGlobals ^Globals);
	private: DebugMaster ^debugMaster;
	};
}