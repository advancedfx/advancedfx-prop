#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace hlae {

	/// <summary>
	/// Summary for DemoToolsWiz3
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class DemoToolsWiz3 : public System::Windows::Forms::Form
	{
	public:
		DemoToolsWiz3(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

		bool bCheckedMarks( void )
		{
			return this->checkBoxMarks->Checked;
		}

		System::String ^ReturnInFile()
		{
			return this->textBoxIn->Text;
		}

		System::String ^ReturnOutFile()
		{
			return this->textBoxOut->Text;
		}

	private:
		void doUpdateFinish( void )
		{
			this->buttonNext->Enabled = !(
				String::IsNullOrEmpty( this->textBoxIn->Text )
				|| String::IsNullOrEmpty( this->textBoxOut->Text )
			);
		}


	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~DemoToolsWiz3()
		{
			if (components)
			{
				delete components;
			}
		}

	protected: 





	private: System::Windows::Forms::Button^  buttonCancel;
	private: System::Windows::Forms::Button^  buttonNext;
	private: System::Windows::Forms::Button^  buttonPrev;
	private: System::Windows::Forms::Label^  labelFileSelect;
	private: System::Windows::Forms::GroupBox^  groupBoxIn;
	private: System::Windows::Forms::Button^  buttonIn;


	private: System::Windows::Forms::TextBox^  textBoxIn;
	private: System::Windows::Forms::GroupBox^  groupBoxOut;
	private: System::Windows::Forms::CheckBox^  checkBoxMarks;



	private: System::Windows::Forms::TextBox^  textBoxOut;

	private: System::Windows::Forms::Button^  buttonOut;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog;
	private: System::Windows::Forms::SaveFileDialog^  saveFileDialog;


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
			this->buttonCancel = (gcnew System::Windows::Forms::Button());
			this->buttonNext = (gcnew System::Windows::Forms::Button());
			this->buttonPrev = (gcnew System::Windows::Forms::Button());
			this->labelFileSelect = (gcnew System::Windows::Forms::Label());
			this->groupBoxIn = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxIn = (gcnew System::Windows::Forms::TextBox());
			this->buttonIn = (gcnew System::Windows::Forms::Button());
			this->groupBoxOut = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxMarks = (gcnew System::Windows::Forms::CheckBox());
			this->textBoxOut = (gcnew System::Windows::Forms::TextBox());
			this->buttonOut = (gcnew System::Windows::Forms::Button());
			this->openFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->saveFileDialog = (gcnew System::Windows::Forms::SaveFileDialog());
			this->groupBoxIn->SuspendLayout();
			this->groupBoxOut->SuspendLayout();
			this->SuspendLayout();
			// 
			// buttonCancel
			// 
			this->buttonCancel->DialogResult = System::Windows::Forms::DialogResult::Abort;
			this->buttonCancel->Location = System::Drawing::Point(385, 302);
			this->buttonCancel->Name = L"buttonCancel";
			this->buttonCancel->Size = System::Drawing::Size(75, 23);
			this->buttonCancel->TabIndex = 5;
			this->buttonCancel->Text = L"Cancel";
			this->buttonCancel->UseVisualStyleBackColor = true;
			// 
			// buttonNext
			// 
			this->buttonNext->DialogResult = System::Windows::Forms::DialogResult::Yes;
			this->buttonNext->Enabled = false;
			this->buttonNext->Location = System::Drawing::Point(278, 302);
			this->buttonNext->Name = L"buttonNext";
			this->buttonNext->Size = System::Drawing::Size(75, 23);
			this->buttonNext->TabIndex = 0;
			this->buttonNext->Text = L"Finish";
			this->buttonNext->UseVisualStyleBackColor = true;
			// 
			// buttonPrev
			// 
			this->buttonPrev->DialogResult = System::Windows::Forms::DialogResult::No;
			this->buttonPrev->Location = System::Drawing::Point(197, 302);
			this->buttonPrev->Name = L"buttonPrev";
			this->buttonPrev->Size = System::Drawing::Size(75, 23);
			this->buttonPrev->TabIndex = 1;
			this->buttonPrev->Text = L"< Back";
			this->buttonPrev->UseVisualStyleBackColor = true;
			// 
			// labelFileSelect
			// 
			this->labelFileSelect->AutoSize = true;
			this->labelFileSelect->Location = System::Drawing::Point(12, 23);
			this->labelFileSelect->Name = L"labelFileSelect";
			this->labelFileSelect->Size = System::Drawing::Size(71, 13);
			this->labelFileSelect->TabIndex = 2;
			this->labelFileSelect->Text = L"File selection:";
			// 
			// groupBoxIn
			// 
			this->groupBoxIn->Controls->Add(this->textBoxIn);
			this->groupBoxIn->Controls->Add(this->buttonIn);
			this->groupBoxIn->Location = System::Drawing::Point(17, 53);
			this->groupBoxIn->Name = L"groupBoxIn";
			this->groupBoxIn->Size = System::Drawing::Size(442, 58);
			this->groupBoxIn->TabIndex = 3;
			this->groupBoxIn->TabStop = false;
			this->groupBoxIn->Text = L"In: demo to fix";
			// 
			// textBoxIn
			// 
			this->textBoxIn->Location = System::Drawing::Point(12, 23);
			this->textBoxIn->Name = L"textBoxIn";
			this->textBoxIn->Size = System::Drawing::Size(333, 20);
			this->textBoxIn->TabIndex = 1;
			this->textBoxIn->TextChanged += gcnew System::EventHandler(this, &DemoToolsWiz3::textBoxIn_TextChanged);
			// 
			// buttonIn
			// 
			this->buttonIn->Location = System::Drawing::Point(351, 21);
			this->buttonIn->Name = L"buttonIn";
			this->buttonIn->Size = System::Drawing::Size(75, 23);
			this->buttonIn->TabIndex = 0;
			this->buttonIn->Text = L"Browse";
			this->buttonIn->UseVisualStyleBackColor = true;
			this->buttonIn->Click += gcnew System::EventHandler(this, &DemoToolsWiz3::buttonIn_Click);
			// 
			// groupBoxOut
			// 
			this->groupBoxOut->Controls->Add(this->checkBoxMarks);
			this->groupBoxOut->Controls->Add(this->textBoxOut);
			this->groupBoxOut->Controls->Add(this->buttonOut);
			this->groupBoxOut->Location = System::Drawing::Point(15, 132);
			this->groupBoxOut->Name = L"groupBoxOut";
			this->groupBoxOut->Size = System::Drawing::Size(442, 78);
			this->groupBoxOut->TabIndex = 4;
			this->groupBoxOut->TabStop = false;
			this->groupBoxOut->Text = L"Out: file to save to";
			// 
			// checkBoxMarks
			// 
			this->checkBoxMarks->AutoSize = true;
			this->checkBoxMarks->Enabled = false;
			this->checkBoxMarks->Location = System::Drawing::Point(12, 52);
			this->checkBoxMarks->Name = L"checkBoxMarks";
			this->checkBoxMarks->Size = System::Drawing::Size(135, 17);
			this->checkBoxMarks->TabIndex = 2;
			this->checkBoxMarks->Text = L"add HLAE water marks";
			this->checkBoxMarks->UseVisualStyleBackColor = true;
			// 
			// textBoxOut
			// 
			this->textBoxOut->Location = System::Drawing::Point(12, 23);
			this->textBoxOut->Name = L"textBoxOut";
			this->textBoxOut->Size = System::Drawing::Size(333, 20);
			this->textBoxOut->TabIndex = 1;
			this->textBoxOut->TextChanged += gcnew System::EventHandler(this, &DemoToolsWiz3::textBoxOut_TextChanged);
			// 
			// buttonOut
			// 
			this->buttonOut->Location = System::Drawing::Point(351, 21);
			this->buttonOut->Name = L"buttonOut";
			this->buttonOut->Size = System::Drawing::Size(75, 23);
			this->buttonOut->TabIndex = 0;
			this->buttonOut->Text = L"Browse";
			this->buttonOut->UseVisualStyleBackColor = true;
			this->buttonOut->Click += gcnew System::EventHandler(this, &DemoToolsWiz3::buttonOut_Click);
			// 
			// openFileDialog
			// 
			this->openFileDialog->DefaultExt = L"dem";
			this->openFileDialog->FileName = L"*.dem";
			this->openFileDialog->Filter = L"Half-Life Demo (*.dem)|*.dem";
			// 
			// saveFileDialog
			// 
			this->saveFileDialog->DefaultExt = L"dem";
			this->saveFileDialog->Filter = L"Half-Life Demo (*.dem)|*.dem";
			// 
			// DemoToolsWiz3
			// 
			this->ClientSize = System::Drawing::Size(474, 335);
			this->Controls->Add(this->groupBoxOut);
			this->Controls->Add(this->groupBoxIn);
			this->Controls->Add(this->labelFileSelect);
			this->Controls->Add(this->buttonPrev);
			this->Controls->Add(this->buttonNext);
			this->Controls->Add(this->buttonCancel);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"DemoToolsWiz3";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"DemoTools Wizard - File Selection";
			this->groupBoxIn->ResumeLayout(false);
			this->groupBoxIn->PerformLayout();
			this->groupBoxOut->ResumeLayout(false);
			this->groupBoxOut->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private:
		//
		// Events:
		//

		System::Void buttonIn_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if (::DialogResult::OK == openFileDialog->ShowDialog(this))
			{
				this->textBoxIn->Text = openFileDialog->FileName;
			}
		}

		System::Void buttonOut_Click(System::Object^  sender, System::EventArgs^  e){
			if (::DialogResult::OK == saveFileDialog->ShowDialog(this))
			{
				this->textBoxOut->Text = saveFileDialog->FileName;
			}
		}

		System::Void textBoxIn_TextChanged(System::Object^  sender, System::EventArgs^  e)
		{ doUpdateFinish(); }
		System::Void textBoxOut_TextChanged(System::Object^  sender, System::EventArgs^  e)
		{ doUpdateFinish(); }

	}; // class
}
