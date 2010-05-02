#pragma once

#include <controls/HlaeConsole.h>
#include <forms/calculator.h>
#include <system/debug.h>
#include <system/debug_file.h>
#include <system/globals.h>
#include <system/remoting.h>
#include <tools/customloader/CustomLoader.h>
#include <tools/skymanager/skymanager.h>
#include <system/updater.h>


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

			//
			// Setup HLAE Console:

			this->splitContainerSecondary->Panel2->SuspendLayout();
			this->splitContainerSecondary->SuspendLayout();
			this->splitContainerPrimary->Panel2->SuspendLayout();
			this->splitContainerPrimary->SuspendLayout();
			this->SuspendLayout();

			this->hlaeConsole = gcnew HlaeConsole();
			this->splitContainerSecondary->Panel2->Controls->Add(this->hlaeConsole);
			this->hlaeConsole->Dock = System::Windows::Forms::DockStyle::Fill;
			this->hlaeConsole->Location = System::Drawing::Point(0, 0);
			this->hlaeConsole->Name = L"hlaeConsole";

			this->splitContainerSecondary->Panel2->ResumeLayout(false);
			this->splitContainerSecondary->ResumeLayout(false);
			this->splitContainerPrimary->Panel2->ResumeLayout(false);
			this->splitContainerPrimary->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

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
		UpdaterNotificationTarget ^ m_UpdaterCheckedNotification;
		HlaeRemoting ^remotingSystem;
		CGlobals ^Globals;
		FileDebugListener ^debugFile;
		HlaeConsole ^ hlaeConsole;
		System::Windows::Forms::ToolStripStatusLabel^  statusLabelHide;
		System::Void MainForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
		void UpdaterChecked(System::Object ^sender, IUpdaterCheckResult ^checkResult);


	private: System::Windows::Forms::StatusStrip^  statusStrip1;
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  launchToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripMenuItem1;
	private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  viewToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  dockGameToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  viewConsoleToolStripMenuItem;

	private: System::Windows::Forms::ToolStripMenuItem^  toolsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  demoToolsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  calculatorsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  fileSizeToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  helpToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  advancedfxorgToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  menuUpdates;




	private: System::Windows::Forms::ToolStripMenuItem^  debugToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  flushLogFileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  neverDropMessagesToolStripMenuItem;

	private: System::Windows::Forms::SplitContainer^  splitContainerPrimary;
	private: System::Windows::Forms::SplitContainer^  splitContainerSecondary;
	private: System::Windows::Forms::Panel^  panelGame;


private: System::Windows::Forms::ToolStripMenuItem^  skyManagerToolStripMenuItem;
private: System::Windows::Forms::ToolStripSeparator^  toolStripMenuItem2;
private: System::Windows::Forms::ToolStripMenuItem^  developerToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  customLoaderToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  menuAutoCheck;

private: System::Windows::Forms::ToolStripSeparator^  toolStripMenuItem3;
private: System::Windows::Forms::ToolStripMenuItem^  checkNowToolStripMenuItem;
private: System::Windows::Forms::ToolStripStatusLabel^  statusLabelUpdate;
private: System::Windows::Forms::ToolStripMenuItem^  statusBarToolStripMenuItem;






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
			this->viewToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->dockGameToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->viewConsoleToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->statusBarToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->demoToolsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->calculatorsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->fileSizeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->skyManagerToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem2 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->developerToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->customLoaderToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->helpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->advancedfxorgToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem3 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->menuUpdates = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuAutoCheck = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->checkNowToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->debugToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->flushLogFileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->neverDropMessagesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->splitContainerPrimary = (gcnew System::Windows::Forms::SplitContainer());
			this->splitContainerSecondary = (gcnew System::Windows::Forms::SplitContainer());
			this->panelGame = (gcnew System::Windows::Forms::Panel());
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->statusLabelUpdate = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->statusLabelHide = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->menuStrip1->SuspendLayout();
			this->splitContainerPrimary->Panel2->SuspendLayout();
			this->splitContainerPrimary->SuspendLayout();
			this->splitContainerSecondary->Panel1->SuspendLayout();
			this->splitContainerSecondary->SuspendLayout();
			this->statusStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {this->fileToolStripMenuItem, 
				this->viewToolStripMenuItem, this->toolsToolStripMenuItem, this->helpToolStripMenuItem, this->debugToolStripMenuItem});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(492, 24);
			this->menuStrip1->TabIndex = 3;
			this->menuStrip1->Text = L"menuStripMain";
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
			this->viewToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->dockGameToolStripMenuItem, 
				this->viewConsoleToolStripMenuItem, this->statusBarToolStripMenuItem});
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
			this->dockGameToolStripMenuItem->Visible = false;
			// 
			// viewConsoleToolStripMenuItem
			// 
			this->viewConsoleToolStripMenuItem->CheckOnClick = true;
			this->viewConsoleToolStripMenuItem->Name = L"viewConsoleToolStripMenuItem";
			this->viewConsoleToolStripMenuItem->Size = System::Drawing::Size(138, 22);
			this->viewConsoleToolStripMenuItem->Text = L"&Console";
			this->viewConsoleToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::externalConsoleToolStripMenuItem_Click);
			// 
			// statusBarToolStripMenuItem
			// 
			this->statusBarToolStripMenuItem->Name = L"statusBarToolStripMenuItem";
			this->statusBarToolStripMenuItem->Size = System::Drawing::Size(138, 22);
			this->statusBarToolStripMenuItem->Text = L"&Status Bar";
			this->statusBarToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::statusBarToolStripMenuItem_Click);
			// 
			// toolsToolStripMenuItem
			// 
			this->toolsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {this->demoToolsToolStripMenuItem, 
				this->calculatorsToolStripMenuItem, this->skyManagerToolStripMenuItem, this->toolStripMenuItem2, this->developerToolStripMenuItem});
			this->toolsToolStripMenuItem->Name = L"toolsToolStripMenuItem";
			this->toolsToolStripMenuItem->Size = System::Drawing::Size(44, 20);
			this->toolsToolStripMenuItem->Text = L"&Tools";
			// 
			// demoToolsToolStripMenuItem
			// 
			this->demoToolsToolStripMenuItem->Name = L"demoToolsToolStripMenuItem";
			this->demoToolsToolStripMenuItem->Size = System::Drawing::Size(147, 22);
			this->demoToolsToolStripMenuItem->Text = L"&DemoTools";
			this->demoToolsToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::demoToolsToolStripMenuItem_Click);
			// 
			// calculatorsToolStripMenuItem
			// 
			this->calculatorsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->fileSizeToolStripMenuItem});
			this->calculatorsToolStripMenuItem->Name = L"calculatorsToolStripMenuItem";
			this->calculatorsToolStripMenuItem->Size = System::Drawing::Size(147, 22);
			this->calculatorsToolStripMenuItem->Text = L"&Calculators";
			// 
			// fileSizeToolStripMenuItem
			// 
			this->fileSizeToolStripMenuItem->Name = L"fileSizeToolStripMenuItem";
			this->fileSizeToolStripMenuItem->Size = System::Drawing::Size(123, 22);
			this->fileSizeToolStripMenuItem->Text = L"&File Size";
			this->fileSizeToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::fileSizeToolStripMenuItem_Click);
			// 
			// skyManagerToolStripMenuItem
			// 
			this->skyManagerToolStripMenuItem->Name = L"skyManagerToolStripMenuItem";
			this->skyManagerToolStripMenuItem->Size = System::Drawing::Size(147, 22);
			this->skyManagerToolStripMenuItem->Text = L"Sky Manager";
			this->skyManagerToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::skyManagerToolStripMenuItem_Click);
			// 
			// toolStripMenuItem2
			// 
			this->toolStripMenuItem2->Name = L"toolStripMenuItem2";
			this->toolStripMenuItem2->Size = System::Drawing::Size(144, 6);
			// 
			// developerToolStripMenuItem
			// 
			this->developerToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->customLoaderToolStripMenuItem});
			this->developerToolStripMenuItem->Name = L"developerToolStripMenuItem";
			this->developerToolStripMenuItem->Size = System::Drawing::Size(147, 22);
			this->developerToolStripMenuItem->Text = L"Developer";
			// 
			// customLoaderToolStripMenuItem
			// 
			this->customLoaderToolStripMenuItem->Name = L"customLoaderToolStripMenuItem";
			this->customLoaderToolStripMenuItem->Size = System::Drawing::Size(154, 22);
			this->customLoaderToolStripMenuItem->Text = L"CustomLoader";
			this->customLoaderToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::customLoaderToolStripMenuItem_Click);
			// 
			// helpToolStripMenuItem
			// 
			this->helpToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->advancedfxorgToolStripMenuItem, 
				this->toolStripMenuItem3, this->menuUpdates});
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
			// toolStripMenuItem3
			// 
			this->toolStripMenuItem3->Name = L"toolStripMenuItem3";
			this->toolStripMenuItem3->Size = System::Drawing::Size(171, 6);
			// 
			// menuUpdates
			// 
			this->menuUpdates->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->menuAutoCheck, 
				this->checkNowToolStripMenuItem});
			this->menuUpdates->Name = L"menuUpdates";
			this->menuUpdates->Size = System::Drawing::Size(174, 22);
			this->menuUpdates->Text = L"Check for &Updates";
			// 
			// menuAutoCheck
			// 
			this->menuAutoCheck->Name = L"menuAutoCheck";
			this->menuAutoCheck->Size = System::Drawing::Size(140, 22);
			this->menuAutoCheck->Text = L"&Auto Check";
			this->menuAutoCheck->Click += gcnew System::EventHandler(this, &MainForm::autoCheckToolStripMenuItem_Click);
			// 
			// checkNowToolStripMenuItem
			// 
			this->checkNowToolStripMenuItem->Name = L"checkNowToolStripMenuItem";
			this->checkNowToolStripMenuItem->Size = System::Drawing::Size(140, 22);
			this->checkNowToolStripMenuItem->Text = L"&Check now";
			this->checkNowToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::checkNowToolStripMenuItem_Click);
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
			// splitContainerPrimary
			// 
			this->splitContainerPrimary->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainerPrimary->Location = System::Drawing::Point(0, 24);
			this->splitContainerPrimary->Name = L"splitContainerPrimary";
			// 
			// splitContainerPrimary.Panel1
			// 
			this->splitContainerPrimary->Panel1->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->splitContainerPrimary->Panel1Collapsed = true;
			// 
			// splitContainerPrimary.Panel2
			// 
			this->splitContainerPrimary->Panel2->Controls->Add(this->splitContainerSecondary);
			this->splitContainerPrimary->Size = System::Drawing::Size(492, 349);
			this->splitContainerPrimary->SplitterDistance = 183;
			this->splitContainerPrimary->TabIndex = 6;
			// 
			// splitContainerSecondary
			// 
			this->splitContainerSecondary->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainerSecondary->Location = System::Drawing::Point(0, 0);
			this->splitContainerSecondary->Name = L"splitContainerSecondary";
			this->splitContainerSecondary->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainerSecondary.Panel1
			// 
			this->splitContainerSecondary->Panel1->Controls->Add(this->panelGame);
			this->splitContainerSecondary->Panel2Collapsed = true;
			this->splitContainerSecondary->Size = System::Drawing::Size(492, 349);
			this->splitContainerSecondary->SplitterDistance = 248;
			this->splitContainerSecondary->TabIndex = 0;
			// 
			// panelGame
			// 
			this->panelGame->BackColor = System::Drawing::Color::Gray;
			this->panelGame->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->panelGame->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panelGame->Location = System::Drawing::Point(0, 0);
			this->panelGame->Name = L"panelGame";
			this->panelGame->Size = System::Drawing::Size(492, 349);
			this->panelGame->TabIndex = 0;
			// 
			// statusStrip1
			// 
			this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->statusLabelUpdate, 
				this->statusLabelHide});
			this->statusStrip1->Location = System::Drawing::Point(0, 351);
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Size = System::Drawing::Size(492, 22);
			this->statusStrip1->TabIndex = 0;
			this->statusStrip1->Text = L"hello";
			this->statusStrip1->Visible = false;
			this->statusStrip1->VisibleChanged += gcnew System::EventHandler(this, &MainForm::statusStrip1_VisibleChanged);
			// 
			// statusLabelUpdate
			// 
			this->statusLabelUpdate->Name = L"statusLabelUpdate";
			this->statusLabelUpdate->Size = System::Drawing::Size(446, 17);
			this->statusLabelUpdate->Spring = true;
			this->statusLabelUpdate->Text = L"Update status unknown";
			this->statusLabelUpdate->Click += gcnew System::EventHandler(this, &MainForm::statusLabelUpdate_Click);
			// 
			// statusLabelHide
			// 
			this->statusLabelHide->BorderSides = static_cast<System::Windows::Forms::ToolStripStatusLabelBorderSides>((((System::Windows::Forms::ToolStripStatusLabelBorderSides::Left | System::Windows::Forms::ToolStripStatusLabelBorderSides::Top) 
				| System::Windows::Forms::ToolStripStatusLabelBorderSides::Right) 
				| System::Windows::Forms::ToolStripStatusLabelBorderSides::Bottom));
			this->statusLabelHide->Name = L"statusLabelHide";
			this->statusLabelHide->Size = System::Drawing::Size(31, 17);
			this->statusLabelHide->Text = L"hide";
			this->statusLabelHide->Click += gcnew System::EventHandler(this, &MainForm::statusLabelHide_Click);
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(492, 373);
			this->Controls->Add(this->splitContainerPrimary);
			this->Controls->Add(this->menuStrip1);
			this->Controls->Add(this->statusStrip1);
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"MainForm";
			this->Text = L"Half-Life Advanced Effects";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MainForm::MainForm_FormClosing);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->splitContainerPrimary->Panel2->ResumeLayout(false);
			this->splitContainerPrimary->ResumeLayout(false);
			this->splitContainerSecondary->Panel1->ResumeLayout(false);
			this->splitContainerSecondary->ResumeLayout(false);
			this->statusStrip1->ResumeLayout(false);
			this->statusStrip1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

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
private: System::Void externalConsoleToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->splitContainerSecondary->Panel2Collapsed = !(this->viewConsoleToolStripMenuItem->Checked);
		 }

	private: System::Void skyManagerToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				 skymanager ^sm = gcnew skymanager(Globals);
				 sm->Show(this);
			 }
private: System::Void customLoaderToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 RunCustomLoader(this);
		 }


private: System::Void statusBarToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->statusStrip1->Visible = !this->statusBarToolStripMenuItem->Checked;
		 }
private: System::Void statusStrip1_VisibleChanged(System::Object^  sender, System::EventArgs^  e) {
			 this->statusBarToolStripMenuItem->Checked = this->statusStrip1->Visible;
		 }
private: System::Void checkNowToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			this->statusStrip1->Visible = true;
			this->statusLabelUpdate->IsLink = false;
			this->statusLabelUpdate->Text = "Checking for updates ...";
			this->statusLabelUpdate->ForeColor = Color::FromKnownColor(KnownColor::ControlText);
			this->statusLabelUpdate->BackColor = Color::FromKnownColor(KnownColor::Control);

			Updater::Singelton->StartCheck();
		 }
private: System::Void statusLabelUpdate_Click(System::Object^  sender, System::EventArgs^  e) {
			 if(statusLabelUpdate->IsLink) {
				 System::Diagnostics::Process::Start(
					 statusLabelUpdate->Tag->ToString()
				);
			 }
		 }
private: System::Void autoCheckToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->menuAutoCheck->Checked = !(0 < HlaeConfig::Config->Settings->UpdateCheck);
			 HlaeConfig::Config->Settings->UpdateCheck = this->menuAutoCheck->Checked ? 1 : -1;
			 HlaeConfig::BackUp();
		 }
private: System::Void statusLabelHide_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->statusStrip1->Visible = false;
		 }
}; // MainForm
} // namespace hlae

