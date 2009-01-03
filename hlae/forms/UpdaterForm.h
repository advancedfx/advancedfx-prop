#pragma once

#include <system/globals.h>
#include <system/updater.h>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace hlae::updater;
using namespace hlae::globals;

namespace hlae {


	/// <summary>
	/// Summary for UpdaterForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class UpdaterForm : public System::Windows::Forms::Form
	{
	public:
		UpdaterForm(CGlobals ^Globals)
		{
			this->Globals = Globals;
			InitializeComponent();

			DoTheCheck();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~UpdaterForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		CGlobals ^Globals;

		System::Void DoTheCheck()
		{
			Updater ^updater = gcnew Updater(Globals);

			UpdateCheckSate us = updater->CheckForUpdate();

			switch(us)
			{
			case UpdateCheckSate::US_RECENT:
			case UpdateCheckSate::US_OLD:
				this->linkLabelDownload->Text = "Download Link";
				this->linkLabelDownload->Enabled = true;
				this->linkLabelDownload->Links[0]->LinkData = updater->GetUrl();
				this->textBoxTitle->Text = updater->GetTitle();
				this->textBoxDescription->Text = updater->GetDescription();

				if( UpdateCheckSate::US_RECENT == us )
				{
					this->labelResState->Text = "You are up to date.";
					this->labelResState->BackColor = System::Drawing::Color::LightGreen;
				} else {
					this->labelResState->Text = "Update available.";
					this->labelResState->BackColor = System::Drawing::Color::Orange;
				}
				break;
			 default:
				this->textBoxTitle->Text = "n/a";
				this->textBoxDescription->Text = "n/a";
				this->linkLabelDownload->Enabled = false;
				this->linkLabelDownload->Text = "n/a";
				this->labelResState->Text = "CHECK FAILED";
				this->labelResState->BackColor = System::Drawing::Color::Red;
			 }
		}

	private: System::Windows::Forms::GroupBox^  groupBoxResult;
	private: System::Windows::Forms::Label^  labelResState;

	private: System::Windows::Forms::LinkLabel^  linkLabelDownload;

	private: System::Windows::Forms::TextBox^  textBoxDescription;

	private: System::Windows::Forms::TextBox^  textBoxTitle;


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
			this->groupBoxResult = (gcnew System::Windows::Forms::GroupBox());
			this->linkLabelDownload = (gcnew System::Windows::Forms::LinkLabel());
			this->textBoxDescription = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTitle = (gcnew System::Windows::Forms::TextBox());
			this->labelResState = (gcnew System::Windows::Forms::Label());
			this->groupBoxResult->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBoxResult
			// 
			this->groupBoxResult->Controls->Add(this->linkLabelDownload);
			this->groupBoxResult->Controls->Add(this->textBoxDescription);
			this->groupBoxResult->Controls->Add(this->textBoxTitle);
			this->groupBoxResult->Controls->Add(this->labelResState);
			this->groupBoxResult->Location = System::Drawing::Point(12, 12);
			this->groupBoxResult->Name = L"groupBoxResult";
			this->groupBoxResult->Size = System::Drawing::Size(268, 211);
			this->groupBoxResult->TabIndex = 1;
			this->groupBoxResult->TabStop = false;
			this->groupBoxResult->Text = L"Result";
			// 
			// linkLabelDownload
			// 
			this->linkLabelDownload->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->linkLabelDownload->Enabled = false;
			this->linkLabelDownload->Location = System::Drawing::Point(6, 179);
			this->linkLabelDownload->Name = L"linkLabelDownload";
			this->linkLabelDownload->Size = System::Drawing::Size(256, 22);
			this->linkLabelDownload->TabIndex = 3;
			this->linkLabelDownload->TabStop = true;
			this->linkLabelDownload->Text = L"n/a";
			this->linkLabelDownload->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->linkLabelDownload->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &UpdaterForm::linkLabelDownload_LinkClicked);
			// 
			// textBoxDescription
			// 
			this->textBoxDescription->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->textBoxDescription->Location = System::Drawing::Point(6, 78);
			this->textBoxDescription->Multiline = true;
			this->textBoxDescription->Name = L"textBoxDescription";
			this->textBoxDescription->ReadOnly = true;
			this->textBoxDescription->Size = System::Drawing::Size(256, 93);
			this->textBoxDescription->TabIndex = 2;
			this->textBoxDescription->Text = L"n/a";
			// 
			// textBoxTitle
			// 
			this->textBoxTitle->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->textBoxTitle->Location = System::Drawing::Point(6, 52);
			this->textBoxTitle->Name = L"textBoxTitle";
			this->textBoxTitle->ReadOnly = true;
			this->textBoxTitle->Size = System::Drawing::Size(256, 20);
			this->textBoxTitle->TabIndex = 1;
			this->textBoxTitle->Text = L"n/a";
			// 
			// labelResState
			// 
			this->labelResState->BackColor = System::Drawing::Color::Gray;
			this->labelResState->ForeColor = System::Drawing::Color::Black;
			this->labelResState->Location = System::Drawing::Point(6, 16);
			this->labelResState->Name = L"labelResState";
			this->labelResState->Size = System::Drawing::Size(256, 26);
			this->labelResState->TabIndex = 0;
			this->labelResState->Text = L"PLEASE WAIT";
			this->labelResState->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// UpdaterForm
			// 
			this->ClientSize = System::Drawing::Size(292, 233);
			this->Controls->Add(this->groupBoxResult);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"UpdaterForm";
			this->ShowIcon = false;
			this->Text = L"HLAE Update Check";
			this->groupBoxResult->ResumeLayout(false);
			this->groupBoxResult->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

private: System::Void linkLabelDownload_LinkClicked(System::Object^  sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^  e) {
			 System::Diagnostics::Process::Start(e->Link->LinkData->ToString());
		 }
};
}
