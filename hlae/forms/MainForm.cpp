#include "StdAfx.h"

#include "MainForm.h"
#include "Launcher.h"
#include <system/debug.h>
#include <system/demotoolswizard.h>
#include <system/basecom.h>

using namespace hlae;
using namespace hlae::debug;

void MainForm::MyCreate()
{
	hlaeBaseComServer = new ::CHlaeBcServer( this->toolStripContainer1->ContentPanel );
}

void MainForm::MyDestroy()
{
	delete hlaeBaseComServer;
}

System::Void MainForm::launchToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	hlae::Launcher ^launcher = gcnew hlae::Launcher();

	launcher->ShowDialog(this);
}

System::Void MainForm::demoToolsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	hlae::DemoToolsWizard ^demoWiz = gcnew hlae::DemoToolsWizard( this, debugMaster );

	delete demoWiz;
}

System::Void MainForm::neverDropMessagesToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	this->neverDropMessagesToolStripMenuItem->Enabled = false; // cannot be disabled

	// never drop anything in debug mode:
	debugMaster->SetFilter(DebugMessageType::DMT_ERROR,DebugFilterSetting::DFS_NODROP);
	debugMaster->SetFilter(DebugMessageType::DMT_WARNING,DebugFilterSetting::DFS_NODROP);
	debugMaster->SetFilter(DebugMessageType::DMT_INFO,DebugFilterSetting::DFS_NODROP);
	debugMaster->SetFilter(DebugMessageType::DMT_VERBOSE,DebugFilterSetting::DFS_NODROP);
	debugMaster->SetFilter(DebugMessageType::DMT_DEBUG,DebugFilterSetting::DFS_NODROP);		 
}