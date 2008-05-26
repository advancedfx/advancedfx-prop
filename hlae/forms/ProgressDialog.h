#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace hlae {

	/// <summary>
	/// Summary for ProgressDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class ProgressDialog : public System::Windows::Forms::Form
	{
	public:
		ProgressDialog(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

		void TempSet( int pos )
		{
			this->progressBar->Value = pos;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~ProgressDialog()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ProgressBar^  progressBar;
	private: System::Windows::Forms::Button^  buttonAction;
	protected: 

	protected: 

	private: System::Windows::Forms::Label^  labelLelapsed;

	private: System::Windows::Forms::Label^  labelLremaining;

	private: System::Windows::Forms::Label^  labelLestimated;







	private: System::Windows::Forms::Label^  labelInfo;
	private: System::Windows::Forms::Label^  labelElapsed;
	private: System::Windows::Forms::Label^  labelRemaining;
	private: System::Windows::Forms::Label^  labelEstimated;






	private: System::Windows::Forms::Timer^  timerUpdate;


	private: System::ComponentModel::IContainer^  components;



	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->progressBar = (gcnew System::Windows::Forms::ProgressBar());
			this->buttonAction = (gcnew System::Windows::Forms::Button());
			this->labelLelapsed = (gcnew System::Windows::Forms::Label());
			this->labelLremaining = (gcnew System::Windows::Forms::Label());
			this->labelLestimated = (gcnew System::Windows::Forms::Label());
			this->labelInfo = (gcnew System::Windows::Forms::Label());
			this->labelElapsed = (gcnew System::Windows::Forms::Label());
			this->labelRemaining = (gcnew System::Windows::Forms::Label());
			this->labelEstimated = (gcnew System::Windows::Forms::Label());
			this->timerUpdate = (gcnew System::Windows::Forms::Timer(this->components));
			this->SuspendLayout();
			// 
			// progressBar
			// 
			this->progressBar->Location = System::Drawing::Point(8, 10);
			this->progressBar->Name = L"progressBar";
			this->progressBar->Size = System::Drawing::Size(276, 22);
			this->progressBar->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
			this->progressBar->TabIndex = 0;
			this->progressBar->Value = 50;
			// 
			// buttonAction
			// 
			this->buttonAction->Enabled = false;
			this->buttonAction->Location = System::Drawing::Point(109, 128);
			this->buttonAction->Name = L"buttonAction";
			this->buttonAction->Size = System::Drawing::Size(75, 23);
			this->buttonAction->TabIndex = 1;
			this->buttonAction->Text = L"Cancel";
			this->buttonAction->UseVisualStyleBackColor = true;
			// 
			// labelLelapsed
			// 
			this->labelLelapsed->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->labelLelapsed->Location = System::Drawing::Point(20, 64);
			this->labelLelapsed->Name = L"labelLelapsed";
			this->labelLelapsed->Size = System::Drawing::Size(120, 16);
			this->labelLelapsed->TabIndex = 2;
			this->labelLelapsed->Text = L"Time elapsed:";
			this->labelLelapsed->TextAlign = System::Drawing::ContentAlignment::TopRight;
			// 
			// labelLremaining
			// 
			this->labelLremaining->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->labelLremaining->Location = System::Drawing::Point(20, 82);
			this->labelLremaining->Name = L"labelLremaining";
			this->labelLremaining->Size = System::Drawing::Size(120, 16);
			this->labelLremaining->TabIndex = 3;
			this->labelLremaining->Text = L"Time remaining:";
			this->labelLremaining->TextAlign = System::Drawing::ContentAlignment::TopRight;
			// 
			// labelLestimated
			// 
			this->labelLestimated->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->labelLestimated->Location = System::Drawing::Point(20, 100);
			this->labelLestimated->Name = L"labelLestimated";
			this->labelLestimated->Size = System::Drawing::Size(120, 16);
			this->labelLestimated->TabIndex = 4;
			this->labelLestimated->Text = L"Time estimated:";
			this->labelLestimated->TextAlign = System::Drawing::ContentAlignment::TopRight;
			// 
			// labelInfo
			// 
			this->labelInfo->AutoEllipsis = true;
			this->labelInfo->ForeColor = System::Drawing::SystemColors::Highlight;
			this->labelInfo->Location = System::Drawing::Point(8, 37);
			this->labelInfo->Name = L"labelInfo";
			this->labelInfo->Size = System::Drawing::Size(276, 20);
			this->labelInfo->TabIndex = 6;
			this->labelInfo->Text = L"Please Wait ...";
			this->labelInfo->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->labelInfo->UseMnemonic = false;
			// 
			// labelElapsed
			// 
			this->labelElapsed->Location = System::Drawing::Point(152, 64);
			this->labelElapsed->Name = L"labelElapsed";
			this->labelElapsed->Size = System::Drawing::Size(120, 16);
			this->labelElapsed->TabIndex = 7;
			this->labelElapsed->Text = L"00:00:00";
			this->labelElapsed->UseMnemonic = false;
			// 
			// labelRemaining
			// 
			this->labelRemaining->Location = System::Drawing::Point(152, 82);
			this->labelRemaining->Name = L"labelRemaining";
			this->labelRemaining->Size = System::Drawing::Size(120, 16);
			this->labelRemaining->TabIndex = 8;
			this->labelRemaining->Text = L"unknown";
			this->labelRemaining->UseMnemonic = false;
			// 
			// labelEstimated
			// 
			this->labelEstimated->Location = System::Drawing::Point(152, 100);
			this->labelEstimated->Name = L"labelEstimated";
			this->labelEstimated->Size = System::Drawing::Size(120, 16);
			this->labelEstimated->TabIndex = 9;
			this->labelEstimated->Text = L"unknown";
			this->labelEstimated->UseMnemonic = false;
			// 
			// timerUpdate
			// 
			this->timerUpdate->Interval = 200;
			// 
			// ProgressDialog
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(292, 158);
			this->ControlBox = false;
			this->Controls->Add(this->labelEstimated);
			this->Controls->Add(this->labelRemaining);
			this->Controls->Add(this->labelElapsed);
			this->Controls->Add(this->labelInfo);
			this->Controls->Add(this->labelLestimated);
			this->Controls->Add(this->labelLremaining);
			this->Controls->Add(this->labelLelapsed);
			this->Controls->Add(this->buttonAction);
			this->Controls->Add(this->progressBar);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"ProgressDialog";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->Text = L"Initzializing ...";
			this->ResumeLayout(false);

		}
#pragma endregion
	};
}
