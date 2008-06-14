#pragma once

#include <system/debug.h>
#include <system/debug_file.h>

class CHlaeBcServer; // forward decleration

namespace hlae {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	using namespace hlae::debug;

	/// <summary>
	/// Summary for MainFOrm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(DebugMaster ^debugMaster, FileDebugListener ^debugFile)
		{
			this->debugMaster = debugMaster;
			this->debugFile = debugFile;

			InitializeComponent();
			
			this->Icon = System::Drawing::Icon::ExtractAssociatedIcon(System::Windows::Forms::Application::ExecutablePath );

			#ifdef _DEBUG
				// Enable Debug Menu
				this->menuStrip1->SuspendLayout();

				this->debugToolStripMenuItem->Visible = true;

				this->menuStrip1->ResumeLayout(false);
				this->menuStrip1->PerformLayout();
			#endif

			MyCreate();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainForm()
		{
			MyDestroy();

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
		DebugMaster ^debugMaster;
		FileDebugListener ^debugFile;

	private: System::Windows::Forms::MenuStrip^  menuStrip1;





	private: System::Windows::Forms::ToolStripMenuItem^  toolsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  demoToolsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  helpToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  aboutToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  launchToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripMenuItem1;
	private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
	private: System::Windows::Forms::ToolStripContainer^  toolStripContainer1;
	private: System::Windows::Forms::ToolStripMenuItem^  debugToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  flushLogFileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  neverDropMessagesToolStripMenuItem;





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
			this->debugToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->flushLogFileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripContainer1 = (gcnew System::Windows::Forms::ToolStripContainer());
			this->neverDropMessagesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuStrip1->SuspendLayout();
			this->toolStripContainer1->TopToolStripPanel->SuspendLayout();
			this->toolStripContainer1->SuspendLayout();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->Dock = System::Windows::Forms::DockStyle::None;
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->fileToolStripMenuItem, 
				this->toolsToolStripMenuItem, this->helpToolStripMenuItem, this->debugToolStripMenuItem});
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
			this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::exitToolStripMenuItem_Click);
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
			this->demoToolsToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::demoToolsToolStripMenuItem_Click);
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
			this->aboutToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->aboutToolStripMenuItem->Text = L"&About";
			// 
			// debugToolStripMenuItem
			// 
			this->debugToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->flushLogFileToolStripMenuItem, 
				this->neverDropMessagesToolStripMenuItem});
			this->debugToolStripMenuItem->Name = L"debugToolStripMenuItem";
			this->debugToolStripMenuItem->Size = System::Drawing::Size(50, 20);
			this->debugToolStripMenuItem->Text = L"Debug";
			this->debugToolStripMenuItem->Visible = false;
			// 
			// flushLogFileToolStripMenuItem
			// 
			this->flushLogFileToolStripMenuItem->Name = L"flushLogFileToolStripMenuItem";
			this->flushLogFileToolStripMenuItem->Size = System::Drawing::Size(189, 22);
			this->flushLogFileToolStripMenuItem->Text = L"Flush log file";
			this->flushLogFileToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::flushLogFileToolStripMenuItem_Click);
			// 
			// toolStripContainer1
			// 
			this->toolStripContainer1->BottomToolStripPanelVisible = false;
			// 
			// toolStripContainer1.ContentPanel
			// 
			this->toolStripContainer1->ContentPanel->Size = System::Drawing::Size(292, 200);
			this->toolStripContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->toolStripContainer1->LeftToolStripPanelVisible = false;
			this->toolStripContainer1->Location = System::Drawing::Point(0, 0);
			this->toolStripContainer1->Name = L"toolStripContainer1";
			this->toolStripContainer1->RightToolStripPanelVisible = false;
			this->toolStripContainer1->Size = System::Drawing::Size(292, 224);
			this->toolStripContainer1->TabIndex = 4;
			this->toolStripContainer1->Text = L"toolStripContainer1";
			// 
			// toolStripContainer1.TopToolStripPanel
			// 
			this->toolStripContainer1->TopToolStripPanel->Controls->Add(this->menuStrip1);
			// 
			// neverDropMessagesToolStripMenuItem
			// 
			this->neverDropMessagesToolStripMenuItem->CheckOnClick = true;
			this->neverDropMessagesToolStripMenuItem->Name = L"neverDropMessagesToolStripMenuItem";
			this->neverDropMessagesToolStripMenuItem->Size = System::Drawing::Size(189, 22);
			this->neverDropMessagesToolStripMenuItem->Text = L"Never drop messages";
			this->neverDropMessagesToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::neverDropMessagesToolStripMenuItem_Click);
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(292, 224);
			this->Controls->Add(this->toolStripContainer1);
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"MainForm";
			this->Text = L"Half-Life Advanced Effects";
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->toolStripContainer1->TopToolStripPanel->ResumeLayout(false);
			this->toolStripContainer1->TopToolStripPanel->PerformLayout();
			this->toolStripContainer1->ResumeLayout(false);
			this->toolStripContainer1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

	private:
		System::Void launchToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void demoToolsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);

	private: System::Void exitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
			 {	this->Close(); }


	private: System::Void flushLogFileToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
			 {	if( debugFile ) debugFile->Flush(); }
	private: System::Void neverDropMessagesToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
};
}

