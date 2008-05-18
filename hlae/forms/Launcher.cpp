#include "StdAfx.h"

#include "Launcher.h"
#include "../system/loader.h"


using namespace hlae;
using namespace System::Windows::Forms;

System::Void Launcher::buttonOK_Click(System::Object^  sender, System::EventArgs^  e)
{
	InitLoader( 0, this->textBoxExe->Text, this->textBoxCmdPrev->Text );
}

System::Void Launcher::buttonExe_Click(System::Object^  sender, System::EventArgs^  e)
{
	if (System::Windows::Forms::DialogResult::OK == openFileDialogExe->ShowDialog(this))
	{
		this->textBoxExe->Text = openFileDialogExe->FileName;
	}
}