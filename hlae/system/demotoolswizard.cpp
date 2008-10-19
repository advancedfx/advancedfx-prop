#include <stdafx.h>

#include <system/debug.h>
#include <system/globals.h>

#include <forms/DemoToolsWiz1.h>
#include <forms/DemoToolsWiz2.h>
#include <forms/DemoToolsWiz3.h>
#include <system/demotools.h>

#include "demotoolswizard.h"

using namespace System;
using namespace System::Windows::Forms;

using namespace hlae;
using namespace hlae::globals;
using namespace hlae::debug;

DemoToolsWizard::DemoToolsWizard( System::Windows::Forms::Form ^parentWindow, CGlobals ^Globals)
{
	this->debugMaster = Globals->debugMaster;

	DemoToolsWiz1 ^wiz1 = gcnew DemoToolsWiz1();
	DemoToolsWiz2 ^wiz2 = gcnew DemoToolsWiz2();
	DemoToolsWiz3 ^wiz3 = gcnew DemoToolsWiz3();

	DialogResult dResult;
	int iShowDialog = 0;

	while (iShowDialog>=0 && iShowDialog<3)
	{
		switch (iShowDialog)
		{
		case 0:
			dResult = wiz1->ShowDialog( parentWindow );
			if (DialogResult::Yes != dResult)
				iShowDialog = -1;
			else
				iShowDialog = wiz1->bCheckedCleanup() ? 1 : 2;
			break;
		case 1:
			dResult = wiz2->ShowDialog( parentWindow );
			switch (dResult)
			{
			case DialogResult::No:
				iShowDialog = 0;
				break;
			case DialogResult::Yes:
				iShowDialog = 2;
				break;
			default:
				iShowDialog = -1;
			};
			break;
		case 2:
			dResult = wiz3->ShowDialog( parentWindow );
			switch (dResult)
			{
			case DialogResult::No:
				iShowDialog = wiz1->bCheckedCleanup() ? 1 : 0;
				break;
			case DialogResult::Yes:
				iShowDialog = 3;
				break;
			default:
				iShowDialog = -1;
			};
			break;
		default:
			iShowDialog = -1;
		};
	}

	if (iShowDialog==3)
	{
		// Finished
		CHlaeDemoFix ^dtool = gcnew CHlaeDemoFix( parentWindow, debugMaster );

		dtool->EnableDirectoryFix( wiz1->bCheckedFix() );
		dtool->EnableHltvFix( wiz1->bCheckedStuck() );
		dtool->EnableWaterMarks( wiz3->bCheckedMarks() );

		if (wiz1->bCheckedCleanup())
		{
			System::String ^astr;

			bool bEnabledSth = false;
			
			for (int i=0;i<3;i++)
			{
				astr = wiz2->ReturnMapping( 2*i );
				if ( !System::String::IsNullOrEmpty( astr ) )
				{
					dtool->AddCommandMapping( astr, wiz2->ReturnMapping( 2*i+1 ) );
					bEnabledSth = true;
				}
			}
			
			dtool->EnableDemoCleanUp( bEnabledSth );
		}

		dtool->Run( wiz3->ReturnInFile(), wiz3->ReturnOutFile() );

		delete dtool;
	}

	delete wiz3;
	delete wiz2;
	delete wiz1;
}
