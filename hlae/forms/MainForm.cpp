#include "StdAfx.h"

#include "MainForm.h"
#include "Launcher.h"
#include "../system/basecom.h"


using namespace hlae;

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