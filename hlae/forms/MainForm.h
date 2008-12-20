#pragma once

#include <system/debug.h>
#include <system/debug_file.h>
#include <system/globals.h>
#include <system/remoting.h>
#include <forms/calculator.h>

class CHlaeBcServer; // forward decleration

namespace hlae {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	using namespace hlae::debug;
	using namespace hlae::globals;
	using namespace hlae::remoting;

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
		MainForm(CGlobals ^Globals, FileDebugListener ^debugFile)
		{
			this->Globals = Globals;
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
		HlaeRemoting ^remotingSystem;
		CGlobals ^Globals;
		FileDebugListener ^debugFile;






	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  launchToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripMenuItem1;
	private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  viewToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  dockGameToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  externalConsoleToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  toolsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  demoToolsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  helpToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  advancedfxorgToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  debugToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  flushLogFileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  neverDropMessagesToolStripMenuItem;
	private: System::Windows::Forms::ToolStripContainer^  toolStripContainer1;

	private: System::Windows::Forms::ToolStripPanel^  BottomToolStripPanel;
	private: System::Windows::Forms::ToolStripPanel^  TopToolStripPanel;
	private: System::Windows::Forms::ToolStripPanel^  RightToolStripPanel;
	private: System::Windows::Forms::ToolStripPanel^  LeftToolStripPanel;
	private: System::Windows::Forms::ToolStripContentPanel^  ContentPanel;
	private: System::Windows::Forms::ToolStripMenuItem^  checkForUpdateToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripMenuItem2;
	private: System::Windows::Forms::ToolStripMenuItem^  calculatorsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  fileSizeToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripMenuItem3;
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
			this->BottomToolStripPanel = (gcnew System::Windows::Forms::ToolStripPanel());
			this->TopToolStripPanel = (gcnew System::Windows::Forms::ToolStripPanel());
			this->RightToolStripPanel = (gcnew System::Windows::Forms::ToolStripPanel());
			this->LeftToolStripPanel = (gcnew System::Windows::Forms::ToolStripPanel());
			this->ContentPanel = (gcnew System::Windows::Forms::ToolStripContentPanel());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->launchToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->viewToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->dockGameToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->externalConsoleToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->demoToolsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->calculatorsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->fileSizeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->helpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->advancedfxorgToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem2 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->checkForUpdateToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem3 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->aboutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->debugToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->flushLogFileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->neverDropMessagesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripContainer1 = (gcnew System::Windows::Forms::ToolStripContainer());
			this->menuStrip1->SuspendLayout();
			this->toolStripContainer1->TopToolStripPanel->SuspendLayout();
			this->toolStripContainer1->SuspendLayout();
			this->SuspendLayout();
			// 
			// BottomToolStripPanel
			// 
			this->BottomToolStripPanel->Location = System::Drawing::Point(0, 0);
			this->BottomToolStripPanel->Name = L"BottomToolStripPanel";
			this->BottomToolStripPanel->Orientation = System::Windows::Forms::Orientation::Horizontal;
			this->BottomToolStripPanel->RowMargin = System::Windows::Forms::Padding(3, 0, 0, 0);
			this->BottomToolStripPanel->Size = System::Drawing::Size(0, 0);
			// 
			// TopToolStripPanel
			// 
			this->TopToolStripPanel->Location = System::Drawing::Point(0, 0);
			this->TopToolStripPanel->Name = L"TopToolStripPanel";
			this->TopToolStripPanel->Orientation = System::Windows::Forms::Orientation::Horizontal;
			this->TopToolStripPanel->RowMargin = System::Windows::Forms::Padding(3, 0, 0, 0);
			this->TopToolStripPanel->Size = System::Drawing::Size(0, 0);
			// 
			// RightToolStripPanel
			// 
			this->RightToolStripPanel->Location = System::Drawing::Point(0, 0);
			this->RightToolStripPanel->Name = L"RightToolStripPanel";
			this->RightToolStripPanel->Orientation = System::Windows::Forms::Orientation::Horizontal;
			this->RightToolStripPanel->RowMargin = System::Windows::Forms::Padding(3, 0, 0, 0);
			this->RightToolStripPanel->Size = System::Drawing::Size(0, 0);
			// 
			// LeftToolStripPanel
			// 
			this->LeftToolStripPanel->Location = System::Drawing::Point(0, 0);
			this->LeftToolStripPanel->Name = L"LeftToolStripPanel";
			this->LeftToolStripPanel->Orientation = System::Windows::Forms::Orientation::Horizontal;
			this->LeftToolStripPanel->RowMargin = System::Windows::Forms::Padding(3, 0, 0, 0);
			this->LeftToolStripPanel->Size = System::Drawing::Size(0, 0);
			// 
			// ContentPanel
			// 
			this->ContentPanel->Size = System::Drawing::Size(292, 174);
			// 
			// menuStrip1
			// 
			this->menuStrip1->Dock = System::Windows::Forms::DockStyle::None;
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {this->fileToolStripMenuItem, 
				this->viewToolStripMenuItem, this->toolsToolStripMenuItem, this->helpToolStripMenuItem, this->debugToolStripMenuItem});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(492, 24);
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
			// viewToolStripMenuItem
			// 
			this->viewToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->dockGameToolStripMenuItem, 
				this->externalConsoleToolStripMenuItem});
			this->viewToolStripMenuItem->Name = L"viewToolStripMenuItem";
			this->viewToolStripMenuItem->Size = System::Drawing::Size(41, 20);
			this->viewToolStripMenuItem->Text = L"&View";
			// 
			// dockGameToolStripMenuItem
			// 
			this->dockGameToolStripMenuItem->Checked = true;
			this->dockGameToolStripMenuItem->CheckState = System::Windows::Forms::CheckState::Checked;
			this->dockGameToolStripMenuItem->Name = L"dockGameToolStripMenuItem";
			this->dockGameToolStripMenuItem->Size = System::Drawing::Size(138, 22);
			this->dockGameToolStripMenuItem->Text = L"&Dock Game";
			// 
			// externalConsoleToolStripMenuItem
			// 
			this->externalConsoleToolStripMenuItem->Checked = true;
			this->externalConsoleToolStripMenuItem->CheckState = System::Windows::Forms::CheckState::Checked;
			this->externalConsoleToolStripMenuItem->Name = L"externalConsoleToolStripMenuItem";
			this->externalConsoleToolStripMenuItem->Size = System::Drawing::Size(138, 22);
			this->externalConsoleToolStripMenuItem->Text = L"&Console";
			// 
			// toolsToolStripMenuItem
			// 
			this->toolsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->demoToolsToolStripMenuItem, 
				this->calculatorsToolStripMenuItem});
			this->toolsToolStripMenuItem->Name = L"toolsToolStripMenuItem";
			this->toolsToolStripMenuItem->Size = System::Drawing::Size(44, 20);
			this->toolsToolStripMenuItem->Text = L"&Tools";
			// 
			// demoToolsToolStripMenuItem
			// 
			this->demoToolsToolStripMenuItem->Name = L"demoToolsToolStripMenuItem";
			this->demoToolsToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->demoToolsToolStripMenuItem->Text = L"&DemoTools";
			this->demoToolsToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::demoToolsToolStripMenuItem_Click);
			// 
			// calculatorsToolStripMenuItem
			// 
			this->calculatorsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->fileSizeToolStripMenuItem});
			this->calculatorsToolStripMenuItem->Name = L"calculatorsToolStripMenuItem";
			this->calculatorsToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->calculatorsToolStripMenuItem->Text = L"&Calculators";
			// 
			// fileSizeToolStripMenuItem
			// 
			this->fileSizeToolStripMenuItem->Name = L"fileSizeToolStripMenuItem";
			this->fileSizeToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->fileSizeToolStripMenuItem->Text = L"&File Size";
			this->fileSizeToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::fileSizeToolStripMenuItem_Click);
			// 
			// helpToolStripMenuItem
			// 
			this->helpToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {this->advancedfxorgToolStripMenuItem, 
				this->toolStripMenuItem2, this->checkForUpdateToolStripMenuItem, this->toolStripMenuItem3, this->aboutToolStripMenuItem});
			this->helpToolStripMenuItem->Name = L"helpToolStripMenuItem";
			this->helpToolStripMenuItem->Size = System::Drawing::Size(40, 20);
			this->helpToolStripMenuItem->Text = L"&Help";
			// 
			// advancedfxorgToolStripMenuItem
			// 
			this->advancedfxorgToolStripMenuItem->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Underline));
			this->advancedfxorgToolStripMenuItem->Name = L"advancedfxorgToolStripMenuItem";
			this->advancedfxorgToolStripMenuItem->Size = System::Drawing::Size(174, 22);
			this->advancedfxorgToolStripMenuItem->Text = L"advancedfx.org";
			this->advancedfxorgToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::advancedfxorgToolStripMenuItem_Click);
			// 
			// toolStripMenuItem2
			// 
			this->toolStripMenuItem2->Name = L"toolStripMenuItem2";
			this->toolStripMenuItem2->Size = System::Drawing::Size(171, 6);
			// 
			// checkForUpdateToolStripMenuItem
			// 
			this->checkForUpdateToolStripMenuItem->Name = L"checkForUpdateToolStripMenuItem";
			this->checkForUpdateToolStripMenuItem->Size = System::Drawing::Size(174, 22);
			this->checkForUpdateToolStripMenuItem->Text = L"Check for &Updates";
			// 
			// toolStripMenuItem3
			// 
			this->toolStripMenuItem3->Name = L"toolStripMenuItem3";
			this->toolStripMenuItem3->Size = System::Drawing::Size(171, 6);
			// 
			// aboutToolStripMenuItem
			// 
			this->aboutToolStripMenuItem->Name = L"aboutToolStripMenuItem";
			this->aboutToolStripMenuItem->Size = System::Drawing::Size(174, 22);
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
			// neverDropMessagesToolStripMenuItem
			// 
			this->neverDropMessagesToolStripMenuItem->CheckOnClick = true;
			this->neverDropMessagesToolStripMenuItem->Name = L"neverDropMessagesToolStripMenuItem";
			this->neverDropMessagesToolStripMenuItem->Size = System::Drawing::Size(189, 22);
			this->neverDropMessagesToolStripMenuItem->Text = L"Never drop messages";
			this->neverDropMessagesToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::neverDropMessagesToolStripMenuItem_Click);
			// 
			// toolStripContainer1
			// 
			// 
			// toolStripContainer1.ContentPanel
			// 
			this->toolStripContainer1->ContentPanel->Size = System::Drawing::Size(492, 349);
			this->toolStripContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->toolStripContainer1->Location = System::Drawing::Point(0, 0);
			this->toolStripContainer1->Name = L"toolStripContainer1";
			this->toolStripContainer1->RightToolStripPanelVisible = false;
			this->toolStripContainer1->Size = System::Drawing::Size(492, 373);
			this->toolStripContainer1->TabIndex = 4;
			this->toolStripContainer1->Text = L"toolStripContainer1";
			// 
			// toolStripContainer1.TopToolStripPanel
			// 
			this->toolStripContainer1->TopToolStripPanel->Controls->Add(this->menuStrip1);
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(492, 373);
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
	
	private: System::Void advancedfxorgToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
			 {
				 System::Diagnostics::Process::Start("http://advancedfx.org/");
			 }
private: System::Void fileSizeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 calcu::Form1 ^calcu = gcnew calcu::Form1();
			 calcu->Show(this);
		 }
};

} // namespace hlae

