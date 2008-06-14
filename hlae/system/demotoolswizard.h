#pragma once

#include <system/debug.h>

namespace hlae {

	using namespace hlae::debug;

	ref class DemoToolsWizard
	{
	public:
		DemoToolsWizard( System::Windows::Forms::Form ^parentWindow, DebugMaster ^debugMaster);
	private: DebugMaster ^debugMaster;
	};
}