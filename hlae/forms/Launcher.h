#pragma once

#include <system/globals.h>
#include <system/config.h>

using namespace hlae;
using namespace hlae::globals;

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace hlae {

	/// <summary>
	/// Summary for Launcher
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	ref class Launcher : public System::Windows::Forms::Form
	{
	public:
		Launcher(CGlobals ^Globals)
		{
			this->Globals = Globals;

			InitializeComponent();

			// custom bellow this line
			ReadFromConfig(HlaeConfig::Config->Settings->Launcher);
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Launcher()
		{
			// custom above this line

			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::CheckBox^  checkBoxStartDocked;
	private: System::Windows::Forms::CheckBox^  checkBoxFullScreen;
	private: System::Windows::Forms::TextBox^  textBoxCustMod;


	protected: 

	private:
		CGlobals ^Globals;
		System::Void ReadFromConfig(CfgLauncher ^ cfg);
		System::Void WriteToConfig(CfgLauncher ^ cfg);


	private: System::Windows::Forms::GroupBox^  groupBoxGame;

	private: System::Windows::Forms::TextBox^  textBoxExe;
	private: System::Windows::Forms::Label^  labelExe;
	private: System::Windows::Forms::Button^  buttonExe;
	private: System::Windows::Forms::ComboBox^  comboBoxModSel;

	private: System::Windows::Forms::Label^  labelModSel;


	private: System::Windows::Forms::GroupBox^  groupBoxRes;
	private: System::Windows::Forms::CheckBox^  checkBoxResForce;
	private: System::Windows::Forms::TextBox^  textBoxResHeight;
	private: System::Windows::Forms::Label^  labelResHeight;
	private: System::Windows::Forms::TextBox^  textBoxResWidth;
	private: System::Windows::Forms::Label^  labelResWidth;
	private: System::Windows::Forms::Label^  labelResDepth;
	private: System::Windows::Forms::ComboBox^  comboBoxResDepth;
	private: System::Windows::Forms::GroupBox^  groupBoxMisc;
	private: System::Windows::Forms::ComboBox^  comboBoxRenderMode;

	private: System::Windows::Forms::Label^  labelRenderMode;

	private: System::Windows::Forms::CheckBox^  checkBoxForceAlpha;
	private: System::Windows::Forms::GroupBox^  groupBoxCmdOpts;






	private: System::Windows::Forms::Button^  buttonOK;
	private: System::Windows::Forms::Button^  buttonCancel;

	private: System::Windows::Forms::OpenFileDialog^  openFileDialogExe; 
	private: System::Windows::Forms::CheckBox^  checkBoxRemeber;
	private: System::Windows::Forms::TextBox^  textBoxCmdAdd;
	private: System::Windows::Forms::CheckBox^  checkBoxVisbility;

	private: System::Windows::Forms::CheckBox^  checkBoxDesktopRes;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->groupBoxGame = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxCustMod = (gcnew System::Windows::Forms::TextBox());
			this->comboBoxModSel = (gcnew System::Windows::Forms::ComboBox());
			this->labelModSel = (gcnew System::Windows::Forms::Label());
			this->buttonExe = (gcnew System::Windows::Forms::Button());
			this->textBoxExe = (gcnew System::Windows::Forms::TextBox());
			this->labelExe = (gcnew System::Windows::Forms::Label());
			this->groupBoxRes = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxFullScreen = (gcnew System::Windows::Forms::CheckBox());
			this->comboBoxResDepth = (gcnew System::Windows::Forms::ComboBox());
			this->labelResDepth = (gcnew System::Windows::Forms::Label());
			this->textBoxResHeight = (gcnew System::Windows::Forms::TextBox());
			this->labelResHeight = (gcnew System::Windows::Forms::Label());
			this->textBoxResWidth = (gcnew System::Windows::Forms::TextBox());
			this->labelResWidth = (gcnew System::Windows::Forms::Label());
			this->checkBoxResForce = (gcnew System::Windows::Forms::CheckBox());
			this->groupBoxMisc = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxStartDocked = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxVisbility = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxDesktopRes = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxForceAlpha = (gcnew System::Windows::Forms::CheckBox());
			this->comboBoxRenderMode = (gcnew System::Windows::Forms::ComboBox());
			this->labelRenderMode = (gcnew System::Windows::Forms::Label());
			this->groupBoxCmdOpts = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxCmdAdd = (gcnew System::Windows::Forms::TextBox());
			this->buttonOK = (gcnew System::Windows::Forms::Button());
			this->buttonCancel = (gcnew System::Windows::Forms::Button());
			this->openFileDialogExe = (gcnew System::Windows::Forms::OpenFileDialog());
			this->checkBoxRemeber = (gcnew System::Windows::Forms::CheckBox());
			this->groupBoxGame->SuspendLayout();
			this->groupBoxRes->SuspendLayout();
			this->groupBoxMisc->SuspendLayout();
			this->groupBoxCmdOpts->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBoxGame
			// 
			this->groupBoxGame->Controls->Add(this->textBoxCustMod);
			this->groupBoxGame->Controls->Add(this->comboBoxModSel);
			this->groupBoxGame->Controls->Add(this->labelModSel);
			this->groupBoxGame->Controls->Add(this->buttonExe);
			this->groupBoxGame->Controls->Add(this->textBoxExe);
			this->groupBoxGame->Controls->Add(this->labelExe);
			this->groupBoxGame->Location = System::Drawing::Point(2, 5);
			this->groupBoxGame->Name = L"groupBoxGame";
			this->groupBoxGame->Size = System::Drawing::Size(469, 86);
			this->groupBoxGame->TabIndex = 3;
			this->groupBoxGame->TabStop = false;
			this->groupBoxGame->Text = L"Game";
			// 
			// textBoxCustMod
			// 
			this->textBoxCustMod->Enabled = false;
			this->textBoxCustMod->Location = System::Drawing::Point(342, 51);
			this->textBoxCustMod->Name = L"textBoxCustMod";
			this->textBoxCustMod->Size = System::Drawing::Size(113, 20);
			this->textBoxCustMod->TabIndex = 5;
			this->textBoxCustMod->Text = L"custom";
			// 
			// comboBoxModSel
			// 
			this->comboBoxModSel->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBoxModSel->FormattingEnabled = true;
			this->comboBoxModSel->Items->AddRange(gcnew cli::array< System::Object^  >(5) {L"cstrike (Counter-Strike)", L"dod (Day of Defeat)", 
				L"tfc (Team Fortress Classic)", L"valve (Half-Life)", L"Other modification:"});
			this->comboBoxModSel->Location = System::Drawing::Point(112, 51);
			this->comboBoxModSel->Name = L"comboBoxModSel";
			this->comboBoxModSel->Size = System::Drawing::Size(224, 21);
			this->comboBoxModSel->TabIndex = 4;
			this->comboBoxModSel->SelectedIndexChanged += gcnew System::EventHandler(this, &Launcher::comboBoxModSel_SelectedIndexChanged);
			// 
			// labelModSel
			// 
			this->labelModSel->AutoSize = true;
			this->labelModSel->Location = System::Drawing::Point(7, 54);
			this->labelModSel->Name = L"labelModSel";
			this->labelModSel->Size = System::Drawing::Size(67, 13);
			this->labelModSel->TabIndex = 3;
			this->labelModSel->Text = L"Modification:";
			// 
			// buttonExe
			// 
			this->buttonExe->Location = System::Drawing::Point(342, 15);
			this->buttonExe->Name = L"buttonExe";
			this->buttonExe->Size = System::Drawing::Size(96, 23);
			this->buttonExe->TabIndex = 1;
			this->buttonExe->Text = L"Browse ...";
			this->buttonExe->UseVisualStyleBackColor = true;
			this->buttonExe->Click += gcnew System::EventHandler(this, &Launcher::buttonExe_Click);
			// 
			// textBoxExe
			// 
			this->textBoxExe->Location = System::Drawing::Point(112, 17);
			this->textBoxExe->Name = L"textBoxExe";
			this->textBoxExe->Size = System::Drawing::Size(224, 20);
			this->textBoxExe->TabIndex = 2;
			// 
			// labelExe
			// 
			this->labelExe->AutoSize = true;
			this->labelExe->Location = System::Drawing::Point(7, 20);
			this->labelExe->Name = L"labelExe";
			this->labelExe->Size = System::Drawing::Size(54, 13);
			this->labelExe->TabIndex = 0;
			this->labelExe->Text = L"hl.exe file:";
			// 
			// groupBoxRes
			// 
			this->groupBoxRes->Controls->Add(this->checkBoxFullScreen);
			this->groupBoxRes->Controls->Add(this->comboBoxResDepth);
			this->groupBoxRes->Controls->Add(this->labelResDepth);
			this->groupBoxRes->Controls->Add(this->textBoxResHeight);
			this->groupBoxRes->Controls->Add(this->labelResHeight);
			this->groupBoxRes->Controls->Add(this->textBoxResWidth);
			this->groupBoxRes->Controls->Add(this->labelResWidth);
			this->groupBoxRes->Controls->Add(this->checkBoxResForce);
			this->groupBoxRes->Location = System::Drawing::Point(2, 153);
			this->groupBoxRes->Name = L"groupBoxRes";
			this->groupBoxRes->Size = System::Drawing::Size(469, 63);
			this->groupBoxRes->TabIndex = 5;
			this->groupBoxRes->TabStop = false;
			this->groupBoxRes->Text = L"Graphic Resolution";
			// 
			// checkBoxFullScreen
			// 
			this->checkBoxFullScreen->AutoSize = true;
			this->checkBoxFullScreen->Location = System::Drawing::Point(275, 39);
			this->checkBoxFullScreen->Name = L"checkBoxFullScreen";
			this->checkBoxFullScreen->Size = System::Drawing::Size(74, 17);
			this->checkBoxFullScreen->TabIndex = 7;
			this->checkBoxFullScreen->Text = L"full screen";
			this->checkBoxFullScreen->UseVisualStyleBackColor = true;
			this->checkBoxFullScreen->Click += gcnew System::EventHandler(this, &Launcher::checkBoxFullScreen_Click);
			// 
			// comboBoxResDepth
			// 
			this->comboBoxResDepth->FormattingEnabled = true;
			this->comboBoxResDepth->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"32 (High)", L"24 (Medium)", L"16 (Low)"});
			this->comboBoxResDepth->Location = System::Drawing::Point(157, 35);
			this->comboBoxResDepth->Name = L"comboBoxResDepth";
			this->comboBoxResDepth->Size = System::Drawing::Size(88, 21);
			this->comboBoxResDepth->TabIndex = 5;
			// 
			// labelResDepth
			// 
			this->labelResDepth->AutoSize = true;
			this->labelResDepth->Location = System::Drawing::Point(154, 18);
			this->labelResDepth->Name = L"labelResDepth";
			this->labelResDepth->Size = System::Drawing::Size(66, 13);
			this->labelResDepth->TabIndex = 4;
			this->labelResDepth->Text = L"Color Depth:";
			// 
			// textBoxResHeight
			// 
			this->textBoxResHeight->Location = System::Drawing::Point(85, 35);
			this->textBoxResHeight->Name = L"textBoxResHeight";
			this->textBoxResHeight->Size = System::Drawing::Size(56, 20);
			this->textBoxResHeight->TabIndex = 3;
			// 
			// labelResHeight
			// 
			this->labelResHeight->AutoSize = true;
			this->labelResHeight->Location = System::Drawing::Point(82, 18);
			this->labelResHeight->Name = L"labelResHeight";
			this->labelResHeight->Size = System::Drawing::Size(41, 13);
			this->labelResHeight->TabIndex = 2;
			this->labelResHeight->Text = L"Height:";
			// 
			// textBoxResWidth
			// 
			this->textBoxResWidth->Location = System::Drawing::Point(10, 35);
			this->textBoxResWidth->Name = L"textBoxResWidth";
			this->textBoxResWidth->Size = System::Drawing::Size(56, 20);
			this->textBoxResWidth->TabIndex = 1;
			// 
			// labelResWidth
			// 
			this->labelResWidth->AutoSize = true;
			this->labelResWidth->Location = System::Drawing::Point(7, 19);
			this->labelResWidth->Name = L"labelResWidth";
			this->labelResWidth->Size = System::Drawing::Size(38, 13);
			this->labelResWidth->TabIndex = 0;
			this->labelResWidth->Text = L"Width:";
			// 
			// checkBoxResForce
			// 
			this->checkBoxResForce->AutoSize = true;
			this->checkBoxResForce->Location = System::Drawing::Point(275, 14);
			this->checkBoxResForce->Name = L"checkBoxResForce";
			this->checkBoxResForce->Size = System::Drawing::Size(98, 17);
			this->checkBoxResForce->TabIndex = 6;
			this->checkBoxResForce->Text = L"force resolution";
			this->checkBoxResForce->UseVisualStyleBackColor = true;
			// 
			// groupBoxMisc
			// 
			this->groupBoxMisc->Controls->Add(this->checkBoxStartDocked);
			this->groupBoxMisc->Controls->Add(this->checkBoxVisbility);
			this->groupBoxMisc->Controls->Add(this->checkBoxDesktopRes);
			this->groupBoxMisc->Controls->Add(this->checkBoxForceAlpha);
			this->groupBoxMisc->Controls->Add(this->comboBoxRenderMode);
			this->groupBoxMisc->Controls->Add(this->labelRenderMode);
			this->groupBoxMisc->Location = System::Drawing::Point(2, 222);
			this->groupBoxMisc->Name = L"groupBoxMisc";
			this->groupBoxMisc->Size = System::Drawing::Size(469, 98);
			this->groupBoxMisc->TabIndex = 6;
			this->groupBoxMisc->TabStop = false;
			this->groupBoxMisc->Text = L"Advanced Settings";
			// 
			// checkBoxStartDocked
			// 
			this->checkBoxStartDocked->AutoSize = true;
			this->checkBoxStartDocked->Checked = true;
			this->checkBoxStartDocked->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxStartDocked->Enabled = false;
			this->checkBoxStartDocked->Location = System::Drawing::Point(259, 65);
			this->checkBoxStartDocked->Name = L"checkBoxStartDocked";
			this->checkBoxStartDocked->Size = System::Drawing::Size(87, 17);
			this->checkBoxStartDocked->TabIndex = 5;
			this->checkBoxStartDocked->Text = L"Start docked";
			this->checkBoxStartDocked->UseVisualStyleBackColor = true;
			// 
			// checkBoxVisbility
			// 
			this->checkBoxVisbility->AutoSize = true;
			this->checkBoxVisbility->Location = System::Drawing::Point(10, 42);
			this->checkBoxVisbility->Name = L"checkBoxVisbility";
			this->checkBoxVisbility->Size = System::Drawing::Size(195, 17);
			this->checkBoxVisbility->TabIndex = 1;
			this->checkBoxVisbility->Text = L"Optimize window visibilty on capture";
			this->checkBoxVisbility->UseVisualStyleBackColor = true;
			// 
			// checkBoxDesktopRes
			// 
			this->checkBoxDesktopRes->AutoSize = true;
			this->checkBoxDesktopRes->Enabled = false;
			this->checkBoxDesktopRes->Location = System::Drawing::Point(10, 65);
			this->checkBoxDesktopRes->Name = L"checkBoxDesktopRes";
			this->checkBoxDesktopRes->Size = System::Drawing::Size(155, 17);
			this->checkBoxDesktopRes->TabIndex = 4;
			this->checkBoxDesktopRes->Text = L"Optimize desktop resolution";
			this->checkBoxDesktopRes->UseVisualStyleBackColor = true;
			// 
			// checkBoxForceAlpha
			// 
			this->checkBoxForceAlpha->AutoSize = true;
			this->checkBoxForceAlpha->Location = System::Drawing::Point(10, 19);
			this->checkBoxForceAlpha->Name = L"checkBoxForceAlpha";
			this->checkBoxForceAlpha->Size = System::Drawing::Size(146, 17);
			this->checkBoxForceAlpha->TabIndex = 0;
			this->checkBoxForceAlpha->Text = L"Force 8 bit alpha channel";
			this->checkBoxForceAlpha->UseVisualStyleBackColor = true;
			// 
			// comboBoxRenderMode
			// 
			this->comboBoxRenderMode->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBoxRenderMode->FormattingEnabled = true;
			this->comboBoxRenderMode->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"Standard", L"FrameBuffer Object", L"Memory DC"});
			this->comboBoxRenderMode->Location = System::Drawing::Point(259, 32);
			this->comboBoxRenderMode->Name = L"comboBoxRenderMode";
			this->comboBoxRenderMode->Size = System::Drawing::Size(181, 21);
			this->comboBoxRenderMode->TabIndex = 3;
			// 
			// labelRenderMode
			// 
			this->labelRenderMode->AutoSize = true;
			this->labelRenderMode->Location = System::Drawing::Point(256, 16);
			this->labelRenderMode->Name = L"labelRenderMode";
			this->labelRenderMode->Size = System::Drawing::Size(72, 13);
			this->labelRenderMode->TabIndex = 2;
			this->labelRenderMode->Text = L"RenderMode:";
			// 
			// groupBoxCmdOpts
			// 
			this->groupBoxCmdOpts->Controls->Add(this->textBoxCmdAdd);
			this->groupBoxCmdOpts->Location = System::Drawing::Point(2, 97);
			this->groupBoxCmdOpts->Name = L"groupBoxCmdOpts";
			this->groupBoxCmdOpts->Size = System::Drawing::Size(469, 50);
			this->groupBoxCmdOpts->TabIndex = 4;
			this->groupBoxCmdOpts->TabStop = false;
			this->groupBoxCmdOpts->Text = L"Custom command line options";
			// 
			// textBoxCmdAdd
			// 
			this->textBoxCmdAdd->Location = System::Drawing::Point(10, 19);
			this->textBoxCmdAdd->Name = L"textBoxCmdAdd";
			this->textBoxCmdAdd->Size = System::Drawing::Size(446, 20);
			this->textBoxCmdAdd->TabIndex = 0;
			// 
			// buttonOK
			// 
			this->buttonOK->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->buttonOK->Location = System::Drawing::Point(292, 330);
			this->buttonOK->Name = L"buttonOK";
			this->buttonOK->Size = System::Drawing::Size(75, 28);
			this->buttonOK->TabIndex = 0;
			this->buttonOK->Text = L"L&aunch";
			this->buttonOK->UseVisualStyleBackColor = true;
			this->buttonOK->Click += gcnew System::EventHandler(this, &Launcher::buttonOK_Click);
			// 
			// buttonCancel
			// 
			this->buttonCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->buttonCancel->Location = System::Drawing::Point(385, 330);
			this->buttonCancel->Name = L"buttonCancel";
			this->buttonCancel->Size = System::Drawing::Size(75, 28);
			this->buttonCancel->TabIndex = 1;
			this->buttonCancel->Text = L"Can&cel";
			this->buttonCancel->UseVisualStyleBackColor = true;
			// 
			// openFileDialogExe
			// 
			this->openFileDialogExe->DefaultExt = L"exe";
			this->openFileDialogExe->FileName = L"hl.exe";
			this->openFileDialogExe->Filter = L"Half-Life executeable|hl.exe";
			// 
			// checkBoxRemeber
			// 
			this->checkBoxRemeber->AutoSize = true;
			this->checkBoxRemeber->Location = System::Drawing::Point(12, 330);
			this->checkBoxRemeber->Name = L"checkBoxRemeber";
			this->checkBoxRemeber->Size = System::Drawing::Size(132, 17);
			this->checkBoxRemeber->TabIndex = 2;
			this->checkBoxRemeber->Text = L"remember my changes";
			this->checkBoxRemeber->UseVisualStyleBackColor = true;
			// 
			// Launcher
			// 
			this->AcceptButton = this->buttonOK;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->buttonCancel;
			this->ClientSize = System::Drawing::Size(472, 366);
			this->Controls->Add(this->checkBoxRemeber);
			this->Controls->Add(this->buttonCancel);
			this->Controls->Add(this->buttonOK);
			this->Controls->Add(this->groupBoxCmdOpts);
			this->Controls->Add(this->groupBoxMisc);
			this->Controls->Add(this->groupBoxRes);
			this->Controls->Add(this->groupBoxGame);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"Launcher";
			this->Text = L"Launch ...";
			this->groupBoxGame->ResumeLayout(false);
			this->groupBoxGame->PerformLayout();
			this->groupBoxRes->ResumeLayout(false);
			this->groupBoxRes->PerformLayout();
			this->groupBoxMisc->ResumeLayout(false);
			this->groupBoxMisc->PerformLayout();
			this->groupBoxCmdOpts->ResumeLayout(false);
			this->groupBoxCmdOpts->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void buttonOK_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void buttonExe_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void checkBoxFullScreen_Click(System::Object^  sender, System::EventArgs^  e) {
			if( this->checkBoxFullScreen->Checked )
			{
				System::Windows::Forms::DialogResult dr = MessageBox::Show(
					"Switching to fullscreen is not recommended:\n"
					"Fullscreen supports much fewer resolutions.\n"
					"\n"
					"Do you want to switch to fullscreen?",
					"Really switch to full screen?",
					MessageBoxButtons::YesNo,
					MessageBoxIcon::Information
				);
				if( System::Windows::Forms::DialogResult::Yes == dr )
				{					
					this->checkBoxVisbility->Checked = false;
				}
				else this->checkBoxFullScreen->Checked = false;
			}
		 }

private: System::Void comboBoxModSel_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 if( this->comboBoxModSel->SelectedIndex ==  this->comboBoxModSel->Items->Count-1)
			 {
				 this->textBoxCustMod->Enabled = true;
			 } else {
				 this->textBoxCustMod->Enabled = false;
			 }
		 }
};
}
