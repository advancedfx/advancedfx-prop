#pragma once

class CHlaeBcServer; // forward decleration

namespace hlae {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MainFOrm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
			
			// custom bellow this line

			MyCreate();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainForm()
		{
			MyDestroy();

			// custom above this line

			if (components)
			{
				delete components;
			}
		}

	protected:
		void MyCreate();
		void MyDestroy();

	private:
		CHlaeBcServer *hlaeBaseComServer;

	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;

	private: System::Windows::Forms::ToolStripMenuItem^  launchToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripMenuItem1;
	private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  toolsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  demoToolsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  helpToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  aboutToolStripMenuItem;




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
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->launchToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->demoToolsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->helpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->aboutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->fileToolStripMenuItem, 
				this->toolsToolStripMenuItem, this->helpToolStripMenuItem});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(292, 24);
			this->menuStrip1->TabIndex = 3;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->launchToolStripMenuItem, 
				this->toolStripMenuItem1, this->exitToolStripMenuItem});
			this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			this->fileToolStripMenuItem->Size = System::Drawing::Size(35, 20);
			this->fileToolStripMenuItem->Text = L"&File";
			// 
			// launchToolStripMenuItem
			// 
			this->launchToolStripMenuItem->Name = L"launchToolStripMenuItem";
			this->launchToolStripMenuItem->Size = System::Drawing::Size(119, 22);
			this->launchToolStripMenuItem->Text = L"&Launch";
			this->launchToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::launchToolStripMenuItem_Click);
			// 
			// toolStripMenuItem1
			// 
			this->toolStripMenuItem1->Name = L"toolStripMenuItem1";
			this->toolStripMenuItem1->Size = System::Drawing::Size(116, 6);
			// 
			// exitToolStripMenuItem
			// 
			this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			this->exitToolStripMenuItem->Size = System::Drawing::Size(119, 22);
			this->exitToolStripMenuItem->Text = L"&Exit";
			// 
			// toolsToolStripMenuItem
			// 
			this->toolsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->demoToolsToolStripMenuItem});
			this->toolsToolStripMenuItem->Name = L"toolsToolStripMenuItem";
			this->toolsToolStripMenuItem->Size = System::Drawing::Size(44, 20);
			this->toolsToolStripMenuItem->Text = L"&Tools";
			// 
			// demoToolsToolStripMenuItem
			// 
			this->demoToolsToolStripMenuItem->Name = L"demoToolsToolStripMenuItem";
			this->demoToolsToolStripMenuItem->Size = System::Drawing::Size(137, 22);
			this->demoToolsToolStripMenuItem->Text = L"&DemoTools";
			// 
			// helpToolStripMenuItem
			// 
			this->helpToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->aboutToolStripMenuItem});
			this->helpToolStripMenuItem->Name = L"helpToolStripMenuItem";
			this->helpToolStripMenuItem->Size = System::Drawing::Size(40, 20);
			this->helpToolStripMenuItem->Text = L"&Help";
			// 
			// aboutToolStripMenuItem
			// 
			this->aboutToolStripMenuItem->Name = L"aboutToolStripMenuItem";
			this->aboutToolStripMenuItem->Size = System::Drawing::Size(114, 22);
			this->aboutToolStripMenuItem->Text = L"&About";
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoScroll = true;
			this->ClientSize = System::Drawing::Size(292, 224);
			this->Controls->Add(this->menuStrip1);
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"MainForm";
			this->Text = L"Half-Life Advanced Effects";
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private: System::Void launchToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
};
}

