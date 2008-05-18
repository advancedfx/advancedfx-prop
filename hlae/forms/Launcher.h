#pragma once

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
	public ref class Launcher : public System::Windows::Forms::Form
	{
	public:
		Launcher(void)
		{
			InitializeComponent();

			// custom bellow this line
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
	private: System::Windows::Forms::ComboBox^  comboBoxCaptureMode;
	private: System::Windows::Forms::Label^  labelCaptureMode;
	private: System::Windows::Forms::CheckBox^  checkBoxForceAlpha;
	private: System::Windows::Forms::GroupBox^  groupBoxCmdOpts;
	private: System::Windows::Forms::Label^  labelCmdCust;
	private: System::Windows::Forms::TextBox^  textBoxCmdPrev;
	private: System::Windows::Forms::Label^  labelCmdPrev;
	private: System::Windows::Forms::TextBox^  textBoxCmdAdd;


	private: System::Windows::Forms::Button^  buttonOK;
	private: System::Windows::Forms::Button^  buttonCancel;

	private: System::Windows::Forms::OpenFileDialog^  openFileDialogExe; 

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
			this->comboBoxModSel = (gcnew System::Windows::Forms::ComboBox());
			this->labelModSel = (gcnew System::Windows::Forms::Label());
			this->buttonExe = (gcnew System::Windows::Forms::Button());
			this->textBoxExe = (gcnew System::Windows::Forms::TextBox());
			this->labelExe = (gcnew System::Windows::Forms::Label());
			this->groupBoxRes = (gcnew System::Windows::Forms::GroupBox());
			this->comboBoxResDepth = (gcnew System::Windows::Forms::ComboBox());
			this->labelResDepth = (gcnew System::Windows::Forms::Label());
			this->textBoxResHeight = (gcnew System::Windows::Forms::TextBox());
			this->labelResHeight = (gcnew System::Windows::Forms::Label());
			this->textBoxResWidth = (gcnew System::Windows::Forms::TextBox());
			this->labelResWidth = (gcnew System::Windows::Forms::Label());
			this->checkBoxResForce = (gcnew System::Windows::Forms::CheckBox());
			this->groupBoxMisc = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxForceAlpha = (gcnew System::Windows::Forms::CheckBox());
			this->comboBoxCaptureMode = (gcnew System::Windows::Forms::ComboBox());
			this->labelCaptureMode = (gcnew System::Windows::Forms::Label());
			this->groupBoxCmdOpts = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxCmdPrev = (gcnew System::Windows::Forms::TextBox());
			this->labelCmdPrev = (gcnew System::Windows::Forms::Label());
			this->textBoxCmdAdd = (gcnew System::Windows::Forms::TextBox());
			this->labelCmdCust = (gcnew System::Windows::Forms::Label());
			this->buttonOK = (gcnew System::Windows::Forms::Button());
			this->buttonCancel = (gcnew System::Windows::Forms::Button());
			this->openFileDialogExe = (gcnew System::Windows::Forms::OpenFileDialog());
			this->groupBoxGame->SuspendLayout();
			this->groupBoxRes->SuspendLayout();
			this->groupBoxMisc->SuspendLayout();
			this->groupBoxCmdOpts->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBoxGame
			// 
			this->groupBoxGame->Controls->Add(this->comboBoxModSel);
			this->groupBoxGame->Controls->Add(this->labelModSel);
			this->groupBoxGame->Controls->Add(this->buttonExe);
			this->groupBoxGame->Controls->Add(this->textBoxExe);
			this->groupBoxGame->Controls->Add(this->labelExe);
			this->groupBoxGame->Location = System::Drawing::Point(2, 5);
			this->groupBoxGame->Name = L"groupBoxGame";
			this->groupBoxGame->Size = System::Drawing::Size(469, 86);
			this->groupBoxGame->TabIndex = 2;
			this->groupBoxGame->TabStop = false;
			this->groupBoxGame->Text = L"Game";
			// 
			// comboBoxModSel
			// 
			this->comboBoxModSel->FormattingEnabled = true;
			this->comboBoxModSel->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"cstrike (Counter-Strike)", L"dod (Day of Defeat)", 
				L"tfc (Team Fortress Classic)"});
			this->comboBoxModSel->Location = System::Drawing::Point(112, 51);
			this->comboBoxModSel->Name = L"comboBoxModSel";
			this->comboBoxModSel->Size = System::Drawing::Size(224, 21);
			this->comboBoxModSel->TabIndex = 4;
			// 
			// labelModSel
			// 
			this->labelModSel->AutoSize = true;
			this->labelModSel->Location = System::Drawing::Point(9, 54);
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
			this->textBoxExe->Text = L"Select the hl.exe of your game ...";
			// 
			// labelExe
			// 
			this->labelExe->AutoSize = true;
			this->labelExe->Location = System::Drawing::Point(9, 20);
			this->labelExe->Name = L"labelExe";
			this->labelExe->Size = System::Drawing::Size(54, 13);
			this->labelExe->TabIndex = 0;
			this->labelExe->Text = L"hl.exe file:";
			// 
			// groupBoxRes
			// 
			this->groupBoxRes->Controls->Add(this->comboBoxResDepth);
			this->groupBoxRes->Controls->Add(this->labelResDepth);
			this->groupBoxRes->Controls->Add(this->textBoxResHeight);
			this->groupBoxRes->Controls->Add(this->labelResHeight);
			this->groupBoxRes->Controls->Add(this->textBoxResWidth);
			this->groupBoxRes->Controls->Add(this->labelResWidth);
			this->groupBoxRes->Controls->Add(this->checkBoxResForce);
			this->groupBoxRes->Location = System::Drawing::Point(2, 97);
			this->groupBoxRes->Name = L"groupBoxRes";
			this->groupBoxRes->Size = System::Drawing::Size(262, 98);
			this->groupBoxRes->TabIndex = 3;
			this->groupBoxRes->TabStop = false;
			this->groupBoxRes->Text = L"Resolution";
			// 
			// comboBoxResDepth
			// 
			this->comboBoxResDepth->Enabled = false;
			this->comboBoxResDepth->FormattingEnabled = true;
			this->comboBoxResDepth->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"32 (High)", L"24 (Medium)", L"16 (Low)"});
			this->comboBoxResDepth->Location = System::Drawing::Point(159, 62);
			this->comboBoxResDepth->Name = L"comboBoxResDepth";
			this->comboBoxResDepth->Size = System::Drawing::Size(88, 21);
			this->comboBoxResDepth->TabIndex = 6;
			// 
			// labelResDepth
			// 
			this->labelResDepth->AutoSize = true;
			this->labelResDepth->Enabled = false;
			this->labelResDepth->Location = System::Drawing::Point(156, 45);
			this->labelResDepth->Name = L"labelResDepth";
			this->labelResDepth->Size = System::Drawing::Size(66, 13);
			this->labelResDepth->TabIndex = 5;
			this->labelResDepth->Text = L"Color Depth:";
			// 
			// textBoxResHeight
			// 
			this->textBoxResHeight->Enabled = false;
			this->textBoxResHeight->Location = System::Drawing::Point(87, 62);
			this->textBoxResHeight->Name = L"textBoxResHeight";
			this->textBoxResHeight->Size = System::Drawing::Size(56, 20);
			this->textBoxResHeight->TabIndex = 4;
			this->textBoxResHeight->Text = L"600";
			// 
			// labelResHeight
			// 
			this->labelResHeight->AutoSize = true;
			this->labelResHeight->Enabled = false;
			this->labelResHeight->Location = System::Drawing::Point(84, 45);
			this->labelResHeight->Name = L"labelResHeight";
			this->labelResHeight->Size = System::Drawing::Size(41, 13);
			this->labelResHeight->TabIndex = 3;
			this->labelResHeight->Text = L"Height:";
			// 
			// textBoxResWidth
			// 
			this->textBoxResWidth->Enabled = false;
			this->textBoxResWidth->Location = System::Drawing::Point(12, 62);
			this->textBoxResWidth->Name = L"textBoxResWidth";
			this->textBoxResWidth->Size = System::Drawing::Size(56, 20);
			this->textBoxResWidth->TabIndex = 2;
			this->textBoxResWidth->Text = L"800";
			// 
			// labelResWidth
			// 
			this->labelResWidth->AutoSize = true;
			this->labelResWidth->Enabled = false;
			this->labelResWidth->Location = System::Drawing::Point(9, 46);
			this->labelResWidth->Name = L"labelResWidth";
			this->labelResWidth->Size = System::Drawing::Size(38, 13);
			this->labelResWidth->TabIndex = 1;
			this->labelResWidth->Text = L"Width:";
			// 
			// checkBoxResForce
			// 
			this->checkBoxResForce->AutoSize = true;
			this->checkBoxResForce->Location = System::Drawing::Point(12, 19);
			this->checkBoxResForce->Name = L"checkBoxResForce";
			this->checkBoxResForce->Size = System::Drawing::Size(56, 17);
			this->checkBoxResForce->TabIndex = 0;
			this->checkBoxResForce->Text = L"Force:";
			this->checkBoxResForce->UseVisualStyleBackColor = true;
			// 
			// groupBoxMisc
			// 
			this->groupBoxMisc->Controls->Add(this->checkBoxForceAlpha);
			this->groupBoxMisc->Controls->Add(this->comboBoxCaptureMode);
			this->groupBoxMisc->Controls->Add(this->labelCaptureMode);
			this->groupBoxMisc->Location = System::Drawing::Point(270, 97);
			this->groupBoxMisc->Name = L"groupBoxMisc";
			this->groupBoxMisc->Size = System::Drawing::Size(200, 98);
			this->groupBoxMisc->TabIndex = 5;
			this->groupBoxMisc->TabStop = false;
			this->groupBoxMisc->Text = L"Miscellaneous";
			// 
			// checkBoxForceAlpha
			// 
			this->checkBoxForceAlpha->AutoSize = true;
			this->checkBoxForceAlpha->Location = System::Drawing::Point(9, 19);
			this->checkBoxForceAlpha->Name = L"checkBoxForceAlpha";
			this->checkBoxForceAlpha->Size = System::Drawing::Size(146, 17);
			this->checkBoxForceAlpha->TabIndex = 2;
			this->checkBoxForceAlpha->Text = L"Force 8 bit alpha channel";
			this->checkBoxForceAlpha->UseVisualStyleBackColor = true;
			// 
			// comboBoxCaptureMode
			// 
			this->comboBoxCaptureMode->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBoxCaptureMode->FormattingEnabled = true;
			this->comboBoxCaptureMode->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"(none)", L"undock (un-dock, default)", 
				L"fbo (FrameBuffer Object)", L"memdc (Memory DC)"});
			this->comboBoxCaptureMode->Location = System::Drawing::Point(9, 62);
			this->comboBoxCaptureMode->Name = L"comboBoxCaptureMode";
			this->comboBoxCaptureMode->Size = System::Drawing::Size(181, 21);
			this->comboBoxCaptureMode->TabIndex = 1;
			// 
			// labelCaptureMode
			// 
			this->labelCaptureMode->AutoSize = true;
			this->labelCaptureMode->Location = System::Drawing::Point(6, 45);
			this->labelCaptureMode->Name = L"labelCaptureMode";
			this->labelCaptureMode->Size = System::Drawing::Size(76, 13);
			this->labelCaptureMode->TabIndex = 0;
			this->labelCaptureMode->Text = L"Capture mode:";
			// 
			// groupBoxCmdOpts
			// 
			this->groupBoxCmdOpts->Controls->Add(this->textBoxCmdPrev);
			this->groupBoxCmdOpts->Controls->Add(this->labelCmdPrev);
			this->groupBoxCmdOpts->Controls->Add(this->textBoxCmdAdd);
			this->groupBoxCmdOpts->Controls->Add(this->labelCmdCust);
			this->groupBoxCmdOpts->Location = System::Drawing::Point(2, 201);
			this->groupBoxCmdOpts->Name = L"groupBoxCmdOpts";
			this->groupBoxCmdOpts->Size = System::Drawing::Size(469, 114);
			this->groupBoxCmdOpts->TabIndex = 4;
			this->groupBoxCmdOpts->TabStop = false;
			this->groupBoxCmdOpts->Text = L"Launch Options";
			// 
			// textBoxCmdPrev
			// 
			this->textBoxCmdPrev->BackColor = System::Drawing::SystemColors::Control;
			this->textBoxCmdPrev->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->textBoxCmdPrev->Location = System::Drawing::Point(107, 46);
			this->textBoxCmdPrev->Multiline = true;
			this->textBoxCmdPrev->Name = L"textBoxCmdPrev";
			this->textBoxCmdPrev->ReadOnly = true;
			this->textBoxCmdPrev->Size = System::Drawing::Size(350, 56);
			this->textBoxCmdPrev->TabIndex = 3;
			this->textBoxCmdPrev->Text = L"-steam -game cstrike -gl -window";
			// 
			// labelCmdPrev
			// 
			this->labelCmdPrev->AutoSize = true;
			this->labelCmdPrev->Location = System::Drawing::Point(9, 46);
			this->labelCmdPrev->Name = L"labelCmdPrev";
			this->labelCmdPrev->Size = System::Drawing::Size(48, 13);
			this->labelCmdPrev->TabIndex = 2;
			this->labelCmdPrev->Text = L"Preview:";
			// 
			// textBoxCmdAdd
			// 
			this->textBoxCmdAdd->Location = System::Drawing::Point(107, 17);
			this->textBoxCmdAdd->Name = L"textBoxCmdAdd";
			this->textBoxCmdAdd->Size = System::Drawing::Size(350, 20);
			this->textBoxCmdAdd->TabIndex = 1;
			this->textBoxCmdAdd->Text = L"-demoedit";
			// 
			// labelCmdCust
			// 
			this->labelCmdCust->AutoSize = true;
			this->labelCmdCust->Location = System::Drawing::Point(10, 16);
			this->labelCmdCust->Name = L"labelCmdCust";
			this->labelCmdCust->Size = System::Drawing::Size(56, 13);
			this->labelCmdCust->TabIndex = 0;
			this->labelCmdCust->Text = L"Additional:";
			// 
			// buttonOK
			// 
			this->buttonOK->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->buttonOK->Location = System::Drawing::Point(292, 330);
			this->buttonOK->Name = L"buttonOK";
			this->buttonOK->Size = System::Drawing::Size(75, 28);
			this->buttonOK->TabIndex = 1;
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
			this->buttonCancel->TabIndex = 0;
			this->buttonCancel->Text = L"Can&cel";
			this->buttonCancel->UseVisualStyleBackColor = true;
			// 
			// openFileDialogExe
			// 
			this->openFileDialogExe->DefaultExt = L"exe";
			this->openFileDialogExe->FileName = L"hl.exe";
			this->openFileDialogExe->Filter = L"Half-Life executeable|hl.exe";
			// 
			// Launcher
			// 
			this->AcceptButton = this->buttonOK;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->buttonCancel;
			this->ClientSize = System::Drawing::Size(472, 364);
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

		}
#pragma endregion
	private: System::Void buttonOK_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void buttonExe_Click(System::Object^  sender, System::EventArgs^  e);


};
}
