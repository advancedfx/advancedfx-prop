#include "StdAfx.h"

#include "Launcher.h"

#include <system/globals.h>
#include <system/debug.h>
#include <system/config.h>
#include <AfxGoldSrc/AfxGoldSrc.h>

using namespace hlae;
using namespace hlae::globals;
using namespace hlae::debug;

using namespace System::Windows::Forms;

System::Void Launcher::buttonOK_Click(System::Object^  sender, System::EventArgs^  e)
{
	CfgLauncher ^ cfg = gcnew CfgLauncher();
	WriteToConfig(cfg);

	AfxGoldSrc ^ ag = AfxGoldSrc::GetOrCreate();
	ag->Settings->CopyFrom(cfg);

	if(!ag->Launch(gamePanel))
		MessageBox::Show(
			"Launching failed.",
			"Error",
			MessageBoxButtons::OK,
			MessageBoxIcon::Error
		);
	else if(cfg->RememberChanges) {
		HlaeConfig::Config->Settings->Launcher = cfg;
		HlaeConfig::BackUp();
	}
}

System::Void Launcher::buttonExe_Click(System::Object^  sender, System::EventArgs^  e)
{
	if (System::Windows::Forms::DialogResult::OK == openFileDialogExe->ShowDialog(this))
	{
		this->textBoxExe->Text = openFileDialogExe->FileName;
	}
}

System::Void Launcher::ReadFromConfig(CfgLauncher ^ cfg)
{
	this->checkBoxRemeber->Checked = cfg->RememberChanges;
	this->textBoxExe->Text = cfg->GamePath;

	int iInd = -1;
	for( int i = 0; i < this->comboBoxModSel->Items->Count; i++)
	{

		System::String ^stext = this->comboBoxModSel->Items[i]->ToString();
		stext = stext->Split(' ',1)[0];

		if(0 == System::String::Compare(stext,cfg->Modification))
		{
			iInd = i;
			break;
		}
	}

	if(iInd<0 || iInd >= this->comboBoxModSel->Items->Count-1)
	{
		this->textBoxCustMod->Enabled = true;
		this->comboBoxModSel->SelectedIndex = this->comboBoxModSel->Items->Count - 1;
		this->textBoxCustMod->Text = cfg->Modification;
	}
	else
	{
		this->textBoxCustMod->Enabled = false;
		this->comboBoxModSel->SelectedIndex = iInd;
	}

	this->textBoxCmdAdd->Text = cfg->CustomCmdLine;
	this->checkBoxResForce->Checked = cfg->GfxForce;
	this->textBoxResWidth->Text = cfg->GfxWidth.ToString();
	this->textBoxResHeight->Text = cfg->GfxHeight.ToString();
	this->comboBoxResDepth->Text = cfg->GfxBpp.ToString();
	// Advanced Settings:
	this->checkBoxForceAlpha->Checked = cfg->ForceAlpha;
	this->checkBoxDesktopRes->Checked = cfg->OptimizeDesktopRes;
	this->checkBoxVisbility->Checked = cfg->OptimizeVisibilty;
	//
	if( cfg->RenderMode < this->comboBoxRenderMode->Items->Count )
		this->comboBoxRenderMode->SelectedIndex = cfg->RenderMode;
	//
	checkBoxFullScreen->Checked = cfg->FullScreen;
}

System::Void Launcher::WriteToConfig(CfgLauncher ^ cfg)
{
	cfg->RememberChanges = this->checkBoxRemeber->Checked;

	cfg->GamePath = this->textBoxExe->Text;

	if( this->comboBoxModSel->SelectedIndex ==  this->comboBoxModSel->Items->Count-1)
	{
		cfg->Modification = this->textBoxCustMod->Text;
	} else {
		cfg->Modification = this->comboBoxModSel->Text->Split(' ',1)[0];
	}

	cfg->CustomCmdLine = this->textBoxCmdAdd->Text;
	cfg->GfxForce = this->checkBoxResForce->Checked;
	UInt16::TryParse( this->textBoxResWidth->Text, cfg->GfxWidth );
	UInt16::TryParse( this->textBoxResHeight->Text, cfg->GfxHeight );
	Byte::TryParse( this->comboBoxResDepth->Text->Split(' ',1)[0], cfg->GfxBpp );

	// Advanced Settings:
	cfg->ForceAlpha = this->checkBoxForceAlpha->Checked;
	cfg->OptimizeDesktopRes = this->checkBoxDesktopRes->Checked;
	cfg->OptimizeVisibilty = this->checkBoxVisbility->Checked;
	cfg->RenderMode = this->comboBoxRenderMode->SelectedIndex;
	cfg->FullScreen = this->checkBoxFullScreen->Checked;
}