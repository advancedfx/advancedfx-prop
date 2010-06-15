#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace AfxCppCli {
namespace old {
namespace tools {

	/// <summary>
	/// Summary for DemoToolsWiz2
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	ref class DemoToolsWiz2 : public System::Windows::Forms::Form
	{
	public:
		DemoToolsWiz2(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

		///	<summary>
		/// Summary for ReturnMapping
		///
		/// Returns the text content of the command mapping cells where odd numbers
		/// return the source cell and even numbers return the destination cell.
		///	The first cell is 0.
		/// If iNumber is out of range an empty string is returned.
		/// </summary>
		System::String ^ReturnMapping ( int iNumber )
		{
			switch( iNumber )
			{
			case 0:
				return this->textBoxCmd1S->Text;
			case 1:
				return this->textBoxCmd1D->Text;
			case 2:
				return this->textBoxCmd2S->Text;
			case 3:
				return this->textBoxCmd2D->Text;
			case 4:
				return this->textBoxCmd3S->Text;
			case 5:
				return this->textBoxCmd3D->Text;
			default:
				return gcnew System::String("");
			}
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~DemoToolsWiz2()
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
	private: System::Windows::Forms::Label^  labelDemoCleanUp;
	private: System::Windows::Forms::Label^  labelCleanUpHelp;
	private: System::Windows::Forms::Label^  labelCmd1;

	private: System::Windows::Forms::TextBox^  textBoxCmd1S;

	private: System::Windows::Forms::TextBox^  textBoxCmd1D;
	private: System::Windows::Forms::TextBox^  textBoxCmd2D;

	private: System::Windows::Forms::TextBox^  textBoxCmd2S;
	private: System::Windows::Forms::Label^  labelCmd2;


	private: System::Windows::Forms::TextBox^  textBoxCmd3D;

	private: System::Windows::Forms::TextBox^  textBoxCmd3S;
	private: System::Windows::Forms::Label^  labelCmd3;


	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Label^  label1;



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
			this->labelDemoCleanUp = (gcnew System::Windows::Forms::Label());
			this->labelCleanUpHelp = (gcnew System::Windows::Forms::Label());
			this->labelCmd1 = (gcnew System::Windows::Forms::Label());
			this->textBoxCmd1S = (gcnew System::Windows::Forms::TextBox());
			this->textBoxCmd1D = (gcnew System::Windows::Forms::TextBox());
			this->textBoxCmd2D = (gcnew System::Windows::Forms::TextBox());
			this->textBoxCmd2S = (gcnew System::Windows::Forms::TextBox());
			this->labelCmd2 = (gcnew System::Windows::Forms::Label());
			this->textBoxCmd3D = (gcnew System::Windows::Forms::TextBox());
			this->textBoxCmd3S = (gcnew System::Windows::Forms::TextBox());
			this->labelCmd3 = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBox1->SuspendLayout();
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
			this->buttonNext->Location = System::Drawing::Point(278, 302);
			this->buttonNext->Name = L"buttonNext";
			this->buttonNext->Size = System::Drawing::Size(75, 23);
			this->buttonNext->TabIndex = 0;
			this->buttonNext->Text = L"Next >";
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
			// labelDemoCleanUp
			// 
			this->labelDemoCleanUp->AutoSize = true;
			this->labelDemoCleanUp->Location = System::Drawing::Point(12, 20);
			this->labelDemoCleanUp->Name = L"labelDemoCleanUp";
			this->labelDemoCleanUp->Size = System::Drawing::Size(79, 13);
			this->labelDemoCleanUp->TabIndex = 2;
			this->labelDemoCleanUp->Text = L"DemoCleanUp:";
			// 
			// labelCleanUpHelp
			// 
			this->labelCleanUpHelp->AutoSize = true;
			this->labelCleanUpHelp->Location = System::Drawing::Point(12, 42);
			this->labelCleanUpHelp->Name = L"labelCleanUpHelp";
			this->labelCleanUpHelp->Size = System::Drawing::Size(309, 26);
			this->labelCleanUpHelp->TabIndex = 3;
			this->labelCleanUpHelp->Text = L"By default the scoreboard and the commandmenu is removed.\r\nIf that is fine with y" 
				L"ou just continue to the next page (Click Next).";
			// 
			// labelCmd1
			// 
			this->labelCmd1->AutoSize = true;
			this->labelCmd1->Location = System::Drawing::Point(143, 27);
			this->labelCmd1->Name = L"labelCmd1";
			this->labelCmd1->Size = System::Drawing::Size(16, 13);
			this->labelCmd1->TabIndex = 1;
			this->labelCmd1->Text = L"->";
			// 
			// textBoxCmd1S
			// 
			this->textBoxCmd1S->Location = System::Drawing::Point(12, 24);
			this->textBoxCmd1S->Name = L"textBoxCmd1S";
			this->textBoxCmd1S->Size = System::Drawing::Size(120, 20);
			this->textBoxCmd1S->TabIndex = 0;
			this->textBoxCmd1S->Text = L"+showscores";
			// 
			// textBoxCmd1D
			// 
			this->textBoxCmd1D->Location = System::Drawing::Point(168, 24);
			this->textBoxCmd1D->Name = L"textBoxCmd1D";
			this->textBoxCmd1D->Size = System::Drawing::Size(120, 20);
			this->textBoxCmd1D->TabIndex = 2;
			this->textBoxCmd1D->Text = L"+sh0wscores";
			// 
			// textBoxCmd2D
			// 
			this->textBoxCmd2D->Location = System::Drawing::Point(168, 60);
			this->textBoxCmd2D->Name = L"textBoxCmd2D";
			this->textBoxCmd2D->Size = System::Drawing::Size(120, 20);
			this->textBoxCmd2D->TabIndex = 5;
			this->textBoxCmd2D->Text = L"+c0mmandmenu";
			// 
			// textBoxCmd2S
			// 
			this->textBoxCmd2S->Location = System::Drawing::Point(12, 60);
			this->textBoxCmd2S->Name = L"textBoxCmd2S";
			this->textBoxCmd2S->Size = System::Drawing::Size(120, 20);
			this->textBoxCmd2S->TabIndex = 3;
			this->textBoxCmd2S->Text = L"+commandmenu";
			// 
			// labelCmd2
			// 
			this->labelCmd2->AutoSize = true;
			this->labelCmd2->Location = System::Drawing::Point(143, 63);
			this->labelCmd2->Name = L"labelCmd2";
			this->labelCmd2->Size = System::Drawing::Size(16, 13);
			this->labelCmd2->TabIndex = 4;
			this->labelCmd2->Text = L"->";
			// 
			// textBoxCmd3D
			// 
			this->textBoxCmd3D->Location = System::Drawing::Point(168, 96);
			this->textBoxCmd3D->Name = L"textBoxCmd3D";
			this->textBoxCmd3D->Size = System::Drawing::Size(120, 20);
			this->textBoxCmd3D->TabIndex = 8;
			this->textBoxCmd3D->Text = L"yet unused";
			// 
			// textBoxCmd3S
			// 
			this->textBoxCmd3S->Location = System::Drawing::Point(12, 96);
			this->textBoxCmd3S->Name = L"textBoxCmd3S";
			this->textBoxCmd3S->Size = System::Drawing::Size(120, 20);
			this->textBoxCmd3S->TabIndex = 6;
			// 
			// labelCmd3
			// 
			this->labelCmd3->AutoSize = true;
			this->labelCmd3->Location = System::Drawing::Point(143, 99);
			this->labelCmd3->Name = L"labelCmd3";
			this->labelCmd3->Size = System::Drawing::Size(16, 13);
			this->labelCmd3->TabIndex = 7;
			this->labelCmd3->Text = L"->";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->textBoxCmd3D);
			this->groupBox1->Controls->Add(this->textBoxCmd3S);
			this->groupBox1->Controls->Add(this->labelCmd3);
			this->groupBox1->Controls->Add(this->textBoxCmd2D);
			this->groupBox1->Controls->Add(this->textBoxCmd2S);
			this->groupBox1->Controls->Add(this->labelCmd2);
			this->groupBox1->Controls->Add(this->textBoxCmd1D);
			this->groupBox1->Controls->Add(this->textBoxCmd1S);
			this->groupBox1->Controls->Add(this->labelCmd1);
			this->groupBox1->Location = System::Drawing::Point(34, 91);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(300, 180);
			this->groupBox1->TabIndex = 4;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Command mappings";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(9, 137);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(209, 26);
			this->label1->TabIndex = 9;
			this->label1->Text = L"If the left of the two fields in a row is empty,\r\nthen the row is ignored.";
			// 
			// DemoToolsWiz2
			// 
			this->ClientSize = System::Drawing::Size(474, 335);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->labelCleanUpHelp);
			this->Controls->Add(this->labelDemoCleanUp);
			this->Controls->Add(this->buttonPrev);
			this->Controls->Add(this->buttonNext);
			this->Controls->Add(this->buttonCancel);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"DemoToolsWiz2";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"DemoTools Wizard - DemoCleanUp settings";
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	};

} // namespace tools {
} // namespace old {
} // namespace AfxCppCli {

