#include "StdAfx.h"

#include "Launcher.h"

#include <system/globals.h>
#include <system/debug.h>
#include <system/config.h>
#include <system/loader.h>

using namespace hlae;
using namespace hlae::globals;
using namespace hlae::debug;
using namespace hlae::config;

using namespace System::Windows::Forms;

System::Void Launcher::buttonOK_Click(System::Object^  sender, System::EventArgs^  e)
{
	//	build commandline:
	String ^cmds, ^s1, ^s2, ^s3;
	int i1;

	cmds = String::Concat(
		"-steam -gl -window -game ",
		this->comboBoxModSel->Text->Split(' ',1)[0]
	);

	// gfx settings
	s1 = this->comboBoxResDepth->Text;
	if( 0 < s1->Length)
		cmds = String::Concat( cmds," -", s1, "bpp" );

	if( this->checkBoxResForce->Checked )
	{
		s1 = this->textBoxResWidth->Text;
		s2 = this->textBoxResHeight->Text;
		s3 = this->comboBoxResDepth->Text;

		if( 0 < s1->Length && 0 < s2->Length && 0 < s3->Length )
			cmds = String::Concat( cmds," -mdtres ", s1,"x",s2,"x",s3 );
	} else {
		s1 = this->textBoxResWidth->Text;
		if( 0 < s1->Length) cmds = String::Concat( cmds," -w ", s1 );
		s1 = this->textBoxResHeight->Text;
		if( 0 < s1->Length) cmds = String::Concat( cmds," -h ", s1 );
	}

	// advanced

	if( this->checkBoxForceAlpha->Checked )
		cmds = String::Concat( cmds, " -mdtalpha8" );

	i1 = this->comboBoxRenderMode->SelectedIndex;
	if( 1 == i1 )
		cmds = String::Concat( cmds, " -mdtrender fbo" );
	else if( 2 == i1 )
		cmds = String::Concat( cmds, " -mdtrender memdc" );
	// custom command line

	if( this->checkBoxVisbility->Checked )
		cmds = String::Concat( cmds, " -mdtoptvis" );

	if( this->checkBoxStartDocked->Checked )
		cmds = String::Concat( cmds, " -mdtdocked" );

	s1 = this->textBoxCmdAdd->Text;
	if( 0 < s1->Length)
		cmds = String::Concat( cmds," ", s1 );

	s1 = this->textBoxExe->Text;

	VERBOSE_MESSAGE(Globals->debugMaster,String::Concat("launching \"",s1,"\" with: ",cmds));

	InitLoader( 0, s1, cmds );

	// backup config
	ConfigWrite();
}

System::Void Launcher::buttonExe_Click(System::Object^  sender, System::EventArgs^  e)
{
	if (System::Windows::Forms::DialogResult::OK == openFileDialogExe->ShowDialog(this))
	{
		this->textBoxExe->Text = openFileDialogExe->FileName;
	}
}

System::Void Launcher::ConfigRead()
{
	CLauncher ^lcfg = Globals->ConfigMaster->Config->Settings->Launcher;

	this->checkBoxRemeber->Checked = lcfg->RememberChanges;
	this->textBoxExe->Text = lcfg->GamePath;
	this->comboBoxModSel->Text = lcfg->Modification;
	this->textBoxCmdAdd->Text = lcfg->CustomCmdLine;
	this->checkBoxResForce->Checked = lcfg->GfxForce;
	this->textBoxResWidth->Text = lcfg->GfxWidth.ToString();
	this->textBoxResHeight->Text = lcfg->GfxHeight.ToString();
	this->comboBoxResDepth->Text = lcfg->GfxBpp.ToString();
	// Advanced Settings:
	this->checkBoxForceAlpha->Checked = lcfg->ForceAlpha;
	this->checkBoxDesktopRes->Checked = lcfg->OptimizeDesktopRes;
	this->checkBoxVisbility->Checked = lcfg->OptimizeVisibilty;
	this->checkBoxStartDocked->Checked = lcfg->StartDocked;
	//
	if( lcfg->RenderMode < this->comboBoxRenderMode->Items->Count )
		this->comboBoxRenderMode->SelectedIndex = lcfg->RenderMode;
	
}

System::Void Launcher::ConfigWrite()
{
	CLauncher ^lcfg = Globals->ConfigMaster->Config->Settings->Launcher;

	lcfg->RememberChanges = this->checkBoxRemeber->Checked;

	if( lcfg->RememberChanges )
	{
		lcfg->GamePath = this->textBoxExe->Text;
		lcfg->Modification = this->comboBoxModSel->Text->Split(' ',1)[0];
		lcfg->CustomCmdLine = this->textBoxCmdAdd->Text;
		lcfg->GfxForce = this->checkBoxResForce->Checked;
		UInt16::TryParse( this->textBoxResWidth->Text, lcfg->GfxWidth );
		UInt16::TryParse( this->textBoxResHeight->Text, lcfg->GfxHeight );
		Byte::TryParse( this->comboBoxResDepth->Text->Split(' ',1)[0], lcfg->GfxBpp );
		// Advanced Settings:
		lcfg->ForceAlpha = this->checkBoxForceAlpha->Checked;
		lcfg->OptimizeDesktopRes = this->checkBoxDesktopRes->Checked;
		lcfg->OptimizeVisibilty = this->checkBoxVisbility->Checked;
		lcfg->StartDocked = this->checkBoxStartDocked->Checked;
		lcfg->RenderMode = this->comboBoxRenderMode->SelectedIndex;
	}

	Globals->ConfigMaster->BackUp();
}