#include "StdAfx.h"

#include "MainForm.h"

#include <AfxGoldSrc/AfxGoldSrc.h>
#include <system/debug.h>

#include "Launcher.h"


using namespace hlae;
using namespace hlae::debug;
using namespace hlae::remoting;


System::Void MainForm::MainForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e)
{
	if(nullptr != AfxGoldSrc::Get())
		delete AfxGoldSrc::Get();
}

void MainForm::MyCreate()
{
	m_UpdaterCheckedNotification = gcnew UpdaterNotificationTarget(
		this,
		gcnew UpdaterCheckedHandler(this, &MainForm::UpdaterChecked)
	);

	Updater::Singelton->BeginCheckedNotification(m_UpdaterCheckedNotification);

	if(0 < HlaeConfig::Config->Settings->UpdateCheck) {
		this->menuAutoCheck->Checked = true;
		this->statusLabelUpdate->Text = "Checking for updates ...";

		Updater::Singelton->StartCheck();
	}

	// start up public remoting system (if requested):
	if( System::Environment::CommandLine->Contains( "-ipcremote" ) )
		remotingSystem = gcnew HlaeRemoting( Globals, this->panelGame );

	hlaeConsole->WriteLn(
		String::Concat("Version GUID: ", Updater::Singelton->OwnGuid->ToString())
	);
}

void MainForm::MyDestroy()
{
	// shutdown remoting system (if it was started):
	if( remotingSystem )
		delete remotingSystem;

	Updater::Singelton->EndCheckedNotification(m_UpdaterCheckedNotification);
	delete m_UpdaterCheckedNotification;
}

void MainForm::UpdaterChecked(System::Object ^sender, IUpdaterCheckResult ^checkResult) {
	if(checkResult) {
		// Has result:
		if(checkResult->IsUpdated) {
			// Updated:
			statusStrip1->Visible = true;
			statusLabelUpdate->IsLink = true;
			statusLabelUpdate->Tag = checkResult->Uri ? checkResult->Uri->ToString() : "http://advancedfx.org/";
			statusLabelUpdate->Text = "Update available!";
			statusLabelUpdate->ForeColor = Color::Black;
			statusLabelUpdate->BackColor = Color::Orange;
		}
		else {
			// Is recent:
			statusLabelUpdate->IsLink = false;
			statusLabelUpdate->Text = "Your version is up to date :)";
			statusLabelUpdate->ForeColor = Color::Black;
			statusLabelUpdate->BackColor = Color::LightGreen;
		}
	}
	else {
		// Has no result (s.th. went wrong):
		statusStrip1->Visible = true;
		statusLabelUpdate->IsLink = true;
		statusLabelUpdate->Tag = "http://advancedfx.org/";
		statusLabelUpdate->Text = "Update check failed :(";
		statusLabelUpdate->ForeColor = Color::Black;
		statusLabelUpdate->BackColor = Color::LightCoral;
	}
}

System::Void MainForm::launchToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	hlae::Launcher ^launcher = gcnew hlae::Launcher(Globals, panelGame);

	launcher->ShowDialog(this);
}

System::Void MainForm::demoToolsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	AfxCppCli::old::tools::DemoToolsWizard ^ demoWiz = gcnew AfxCppCli::old::tools::DemoToolsWizard();

	demoWiz->OutputPath = HlaeConfig::Config->Settings->DemoTools->OutputFolder;

	if(demoWiz->ShowDialog(this))
	{
		HlaeConfig::Config->Settings->DemoTools->OutputFolder = demoWiz->OutputPath;
		HlaeConfig::BackUp();
	}

	delete demoWiz;
}

System::Void MainForm::neverDropMessagesToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	this->neverDropMessagesToolStripMenuItem->Enabled = false; // cannot be disabled

	// never drop anything in debug mode:
	Globals->debugMaster->SetFilter(DebugMessageType::DMT_ERROR,DebugFilterSetting::DFS_NODROP);
	Globals->debugMaster->SetFilter(DebugMessageType::DMT_WARNING,DebugFilterSetting::DFS_NODROP);
	Globals->debugMaster->SetFilter(DebugMessageType::DMT_INFO,DebugFilterSetting::DFS_NODROP);
	Globals->debugMaster->SetFilter(DebugMessageType::DMT_VERBOSE,DebugFilterSetting::DFS_NODROP);
	Globals->debugMaster->SetFilter(DebugMessageType::DMT_DEBUG,DebugFilterSetting::DFS_NODROP);		 
}