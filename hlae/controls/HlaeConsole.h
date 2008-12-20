#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace hlae {

	/// <summary>
	/// Summary for HlaeConsole
	/// </summary>
	public ref class HlaeConsole : public System::Windows::Forms::UserControl
	{
	public:
		HlaeConsole(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~HlaeConsole()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  groupBoxEnter;
	protected: 
	private: System::Windows::Forms::Label^  labelSendMode;
	private: System::Windows::Forms::Button^  buttonSend;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::RadioButton^  radioButtonGame;
	private: System::Windows::Forms::RadioButton^  radioButtonAuto;
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::GroupBox^  groupBoxEnter;
	protected: 
	private: System::Windows::Forms::Label^  labelSendMode;
	private: System::Windows::Forms::Button^  buttonSend;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::RadioButton^  radioButtonGame;
	private: System::Windows::Forms::RadioButton^  radioButtonAuto;
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::GroupBox^  groupBoxEnter;
	protected: 
	private: System::Windows::Forms::Label^  labelSendMode;
	private: System::Windows::Forms::Button^  buttonSend;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::RadioButton^  radioButtonGame;
	private: System::Windows::Forms::RadioButton^  radioButtonAuto;
	private: System::Windows::Forms::GroupBox^  groupBoxEnter;
	protected: 
	private: System::Windows::Forms::Label^  labelSendMode;
	private: System::Windows::Forms::Button^  buttonSend;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::RadioButton^  radioButtonGame;
	private: System::Windows::Forms::RadioButton^  radioButtonAuto;
	private: System::Windows::Forms::ListBox^  listBox1;
	private: System::Windows::Forms::GroupBox^  groupBoxEnter;
	protected: 
	private: System::Windows::Forms::Label^  labelSendMode;
	private: System::Windows::Forms::Button^  buttonSend;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::RadioButton^  radioButtonGame;
	private: System::Windows::Forms::RadioButton^  radioButtonAuto;
	private: System::Windows::Forms::GroupBox^  groupBoxEnter;
	protected: 
	private: System::Windows::Forms::Label^  labelSendMode;
	private: System::Windows::Forms::Button^  buttonSend;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::RadioButton^  radioButtonGame;
	private: System::Windows::Forms::RadioButton^  radioButtonAuto;
	private: System::Windows::Forms::GroupBox^  groupBoxEnter;
	protected: 
	private: System::Windows::Forms::Label^  labelSendMode;
	private: System::Windows::Forms::Button^  buttonSend;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::RadioButton^  radioButtonGame;
	private: System::Windows::Forms::RadioButton^  radioButtonAuto;
	private: System::Windows::Forms::GroupBox^  groupBoxEnter;
	protected: 
	private: System::Windows::Forms::Label^  labelSendMode;
	private: System::Windows::Forms::Button^  buttonSend;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::RadioButton^  radioButtonGame;
	private: System::Windows::Forms::RadioButton^  radioButtonAuto;
	private: System::Windows::Forms::ListView^  listViewConsole;
	private: System::Windows::Forms::ColumnHeader^  columnHeaderEvent;
	private: System::Windows::Forms::ColumnHeader^  columnHeaderContent;
	private: System::Windows::Forms::GroupBox^  groupBoxEnter;
	protected: 
	private: System::Windows::Forms::Label^  labelSendMode;
	private: System::Windows::Forms::Button^  buttonSend;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::RadioButton^  radioButtonGame;
	private: System::Windows::Forms::RadioButton^  radioButtonAuto;
	private: System::Windows::Forms::ListView^  listViewConsole;
	private: System::Windows::Forms::ColumnHeader^  columnHeaderEvent;
	private: System::Windows::Forms::ColumnHeader^  columnHeaderContent;
	private: System::Windows::Forms::GroupBox^  groupBoxEnter;
	protected: 
	private: System::Windows::Forms::Label^  labelSendMode;
	private: System::Windows::Forms::Button^  buttonSend;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::RadioButton^  radioButtonGame;
	private: System::Windows::Forms::RadioButton^  radioButtonAuto;
	private: System::Windows::Forms::ListView^  listViewConsole;
	private: System::Windows::Forms::ColumnHeader^  columnHeaderEvent;
	private: System::Windows::Forms::ColumnHeader^  columnHeaderContent;
	private: System::Windows::Forms::GroupBox^  groupBoxEnter;
	protected: 
	private: System::Windows::Forms::Label^  labelSendMode;
	private: System::Windows::Forms::Button^  buttonSend;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::RadioButton^  radioButtonGame;
	private: System::Windows::Forms::RadioButton^  radioButtonAuto;
	private: System::Windows::Forms::ListView^  listViewConsole;
	private: System::Windows::Forms::ColumnHeader^  columnHeaderEvent;
	private: System::Windows::Forms::ColumnHeader^  columnHeaderContent;
	private: System::Windows::Forms::GroupBox^  groupBoxEnter;
	protected: 
	private: System::Windows::Forms::Label^  labelSendMode;
	private: System::Windows::Forms::Button^  buttonSend;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::RadioButton^  radioButtonGame;
	private: System::Windows::Forms::RadioButton^  radioButtonAuto;
	private: System::Windows::Forms::ListView^  listViewConsole;
	private: System::Windows::Forms::GroupBox^  groupBoxEnter;
	protected: 
	private: System::Windows::Forms::Label^  labelSendMode;
	private: System::Windows::Forms::Button^  buttonSend;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::RadioButton^  radioButtonGame;
	private: System::Windows::Forms::RadioButton^  radioButtonAuto;
	private: System::Windows::Forms::GroupBox^  groupBoxEnter;
	protected: 
	private: System::Windows::Forms::Label^  labelSendMode;
	private: System::Windows::Forms::Button^  buttonSend;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::TextBox^  textBox1;
	protected: 
	private: System::Windows::Forms::Button^  buttonSend;
	private: System::Windows::Forms::TextBox^  textBox1;
	protected: 
	private: System::Windows::Forms::Button^  button1;

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
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->buttonSend = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// textBox1
			// 
			this->textBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox1->Location = System::Drawing::Point(6, 305);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(436, 20);
			this->textBox1->TabIndex = 0;
			// 
			// buttonSend
			// 
			this->buttonSend->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->buttonSend->Location = System::Drawing::Point(445, 303);
			this->buttonSend->Name = L"buttonSend";
			this->buttonSend->Size = System::Drawing::Size(93, 23);
			this->buttonSend->TabIndex = 1;
			this->buttonSend->Text = L"Send";
			this->buttonSend->UseVisualStyleBackColor = true;
			// 
			// HlaeConsole
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->buttonSend);
			this->Controls->Add(this->textBox1);
			this->Name = L"HlaeConsole";
			this->Size = System::Drawing::Size(541, 328);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	};
}
