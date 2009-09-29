#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace hlae {

	/// <summary>
	/// Summary for CustomLoaderForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CustomLoaderForm : public System::Windows::Forms::Form
	{
	public:
		CustomLoaderForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

		property String ^ HookDll { String ^ get() { return this->textDll->Text; } }
		property String ^ Program { String ^ get() { return this->textProgram->Text; } }
		property String ^ CmdLine { String ^ get() { return this->textCmdLine->Text; } }

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CustomLoaderForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  textDll;
	protected: 

	private: System::Windows::Forms::Label^  labelDll;
	private: System::Windows::Forms::Label^  labelProgram;
	private: System::Windows::Forms::TextBox^  textProgram;


	protected: 



	private: System::Windows::Forms::Label^  labelCmdLine;
	private: System::Windows::Forms::TextBox^  textCmdLine;



	private: System::Windows::Forms::Button^  buttonOk;
	private: System::Windows::Forms::Button^  buttonAbort;
	private: System::Windows::Forms::Button^  buttonSelectHook;
	private: System::Windows::Forms::Button^  buttonSelectProgram;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog;

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
			this->textDll = (gcnew System::Windows::Forms::TextBox());
			this->labelDll = (gcnew System::Windows::Forms::Label());
			this->labelProgram = (gcnew System::Windows::Forms::Label());
			this->textProgram = (gcnew System::Windows::Forms::TextBox());
			this->labelCmdLine = (gcnew System::Windows::Forms::Label());
			this->textCmdLine = (gcnew System::Windows::Forms::TextBox());
			this->buttonOk = (gcnew System::Windows::Forms::Button());
			this->buttonAbort = (gcnew System::Windows::Forms::Button());
			this->buttonSelectHook = (gcnew System::Windows::Forms::Button());
			this->buttonSelectProgram = (gcnew System::Windows::Forms::Button());
			this->openFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->SuspendLayout();
			// 
			// textDll
			// 
			this->textDll->Location = System::Drawing::Point(105, 12);
			this->textDll->Name = L"textDll";
			this->textDll->Size = System::Drawing::Size(262, 20);
			this->textDll->TabIndex = 1;
			// 
			// labelDll
			// 
			this->labelDll->AutoSize = true;
			this->labelDll->Location = System::Drawing::Point(12, 15);
			this->labelDll->Name = L"labelDll";
			this->labelDll->Size = System::Drawing::Size(56, 13);
			this->labelDll->TabIndex = 0;
			this->labelDll->Text = L"HookDLL:";
			// 
			// labelProgram
			// 
			this->labelProgram->AutoSize = true;
			this->labelProgram->Location = System::Drawing::Point(12, 41);
			this->labelProgram->Name = L"labelProgram";
			this->labelProgram->Size = System::Drawing::Size(71, 13);
			this->labelProgram->TabIndex = 3;
			this->labelProgram->Text = L"ProgramPath:";
			// 
			// textProgram
			// 
			this->textProgram->Location = System::Drawing::Point(105, 38);
			this->textProgram->Name = L"textProgram";
			this->textProgram->Size = System::Drawing::Size(262, 20);
			this->textProgram->TabIndex = 4;
			// 
			// labelCmdLine
			// 
			this->labelCmdLine->AutoSize = true;
			this->labelCmdLine->Location = System::Drawing::Point(12, 67);
			this->labelCmdLine->Name = L"labelCmdLine";
			this->labelCmdLine->Size = System::Drawing::Size(77, 13);
			this->labelCmdLine->TabIndex = 6;
			this->labelCmdLine->Text = L"CommandLine:";
			// 
			// textCmdLine
			// 
			this->textCmdLine->Location = System::Drawing::Point(105, 64);
			this->textCmdLine->Name = L"textCmdLine";
			this->textCmdLine->Size = System::Drawing::Size(352, 20);
			this->textCmdLine->TabIndex = 7;
			// 
			// buttonOk
			// 
			this->buttonOk->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->buttonOk->Location = System::Drawing::Point(15, 108);
			this->buttonOk->Name = L"buttonOk";
			this->buttonOk->Size = System::Drawing::Size(123, 31);
			this->buttonOk->TabIndex = 8;
			this->buttonOk->Text = L"&Ok";
			this->buttonOk->UseVisualStyleBackColor = true;
			// 
			// buttonAbort
			// 
			this->buttonAbort->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->buttonAbort->Location = System::Drawing::Point(334, 108);
			this->buttonAbort->Name = L"buttonAbort";
			this->buttonAbort->Size = System::Drawing::Size(123, 31);
			this->buttonAbort->TabIndex = 9;
			this->buttonAbort->Text = L"&Abort";
			this->buttonAbort->UseVisualStyleBackColor = true;
			// 
			// buttonSelectHook
			// 
			this->buttonSelectHook->Location = System::Drawing::Point(373, 10);
			this->buttonSelectHook->Name = L"buttonSelectHook";
			this->buttonSelectHook->Size = System::Drawing::Size(84, 23);
			this->buttonSelectHook->TabIndex = 2;
			this->buttonSelectHook->Text = L"browse";
			this->buttonSelectHook->UseVisualStyleBackColor = true;
			this->buttonSelectHook->Click += gcnew System::EventHandler(this, &CustomLoaderForm::buttonSelectHook_Click);
			// 
			// buttonSelectProgram
			// 
			this->buttonSelectProgram->Location = System::Drawing::Point(373, 36);
			this->buttonSelectProgram->Name = L"buttonSelectProgram";
			this->buttonSelectProgram->Size = System::Drawing::Size(84, 23);
			this->buttonSelectProgram->TabIndex = 5;
			this->buttonSelectProgram->Text = L"browse";
			this->buttonSelectProgram->UseVisualStyleBackColor = true;
			this->buttonSelectProgram->Click += gcnew System::EventHandler(this, &CustomLoaderForm::buttonSelectProgram_Click);
			// 
			// openFileDialog
			// 
			this->openFileDialog->AutoUpgradeEnabled = false;
			this->openFileDialog->Title = L"Select ...";
			// 
			// CustomLoaderForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(469, 151);
			this->Controls->Add(this->buttonSelectProgram);
			this->Controls->Add(this->buttonSelectHook);
			this->Controls->Add(this->buttonAbort);
			this->Controls->Add(this->buttonOk);
			this->Controls->Add(this->labelCmdLine);
			this->Controls->Add(this->textCmdLine);
			this->Controls->Add(this->labelProgram);
			this->Controls->Add(this->textProgram);
			this->Controls->Add(this->labelDll);
			this->Controls->Add(this->textDll);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"CustomLoaderForm";
			this->Text = L"CustomLoaderForm";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

private:
	System::Void buttonSelectHook_Click(System::Object^  sender, System::EventArgs^  e) {
		openFileDialog->DefaultExt = "*.dll";
		openFileDialog->Filter = "Hook DLL (.dll)|*.dll";
		if(::DialogResult::OK == openFileDialog->ShowDialog(this))
			this->textDll->Text = openFileDialog->FileName;
	}
	System::Void buttonSelectProgram_Click(System::Object^  sender, System::EventArgs^  e) {
		openFileDialog->DefaultExt = "*.exe";
		openFileDialog->Filter = "Program to launch and hook (.exe)|*.exe";
		if(::DialogResult::OK == openFileDialog->ShowDialog(this))
			this->textProgram->Text = openFileDialog->FileName;
	}
};
}
