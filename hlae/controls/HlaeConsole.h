#pragma once

#include <system/debug.h>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace hlae::debug;

namespace hlae {

namespace debug {
	ref class HlaeConsoleListener : public DebugListener
	{
	public:
		HlaeConsoleListener(DebugMaster ^debugMaster, DebugListener::OnSpewMessageDelegate ^onSpewMessage)
		:  DebugListener( true )
		{
			// firste provide new SpewMessage delegate:
			OnSpewMessage = onSpewMessage;

			// and after that attach, so we don't miss out on any messages due to the speMessage hook
			// not being set:
			Attach(debugMaster);
		}
	};
} // namespace debug

	/// <summary>
	/// Summary for HlaeConsole
	/// </summary>
public ref class HlaeConsole : public System::Windows::Forms::UserControl
	{
	public:
		HlaeConsole(DebugMaster ^debugMaster)
		{
			InitializeComponent();

			DebugListener::OnSpewMessageDelegate ^test =  gcnew DebugListener::OnSpewMessageDelegate( this, &HlaeConsole::MySpewMessage );

			hlaeConsoleListener = gcnew HlaeConsoleListener( debugMaster, test);
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~HlaeConsole()
		{
			delete hlaeConsoleListener;

			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  textBoxLog;

	private:
		HlaeConsoleListener ^hlaeConsoleListener;
		DebugMessageState MySpewMessage( DebugMaster ^debugMaster, DebugMessage ^debugMessage )
		{
			//	return DebugMessageState::DMS_FAILED;

			System::String ^tmsg= "Unknown: " ;

			switch( debugMessage->type )
			{
			case DebugMessageType::DMT_ERROR:
				tmsg= "Error: " ;
				break;
			case DebugMessageType::DMT_WARNING:
				tmsg= "Warning" ;
				break;
			case DebugMessageType::DMT_INFO:
				tmsg=  "Info: " ;
				break;
			case DebugMessageType::DMT_VERBOSE:
				tmsg= "Verbose: " ;
				break;
			case DebugMessageType::DMT_DEBUG:
				tmsg= "Debug: " ;
				break;
			}

			bool bPosted=false;

			array<System::String ^> ^lines = this->textBoxLog->Lines;
			int len = lines->Length;
			if(len!=100) System::Array::Resize(lines,100);
			for(int i=99;i>0;i--)
			{
				lines[i]=lines[i-1];
			}
			lines[0] = System::String::Concat(tmsg, debugMessage->string);
			this->textBoxLog->Lines = lines;

			bPosted = true;

			return bPosted ? DebugMessageState::DMS_POSTED : DebugMessageState::DMS_FAILED ;
		}

	private: System::Windows::Forms::Button^  buttonSend;
	private: System::Windows::Forms::TextBox^  textBoxCommand;

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
			this->buttonSend = (gcnew System::Windows::Forms::Button());
			this->textBoxCommand = (gcnew System::Windows::Forms::TextBox());
			this->textBoxLog = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// buttonSend
			// 
			this->buttonSend->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->buttonSend->Enabled = false;
			this->buttonSend->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->buttonSend->Location = System::Drawing::Point(377, 198);
			this->buttonSend->Name = L"buttonSend";
			this->buttonSend->Size = System::Drawing::Size(74, 20);
			this->buttonSend->TabIndex = 1;
			this->buttonSend->Text = L"Send";
			// 
			// textBoxCommand
			// 
			this->textBoxCommand->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBoxCommand->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->textBoxCommand->Enabled = false;
			this->textBoxCommand->Location = System::Drawing::Point(0, 198);
			this->textBoxCommand->Name = L"textBoxCommand";
			this->textBoxCommand->Size = System::Drawing::Size(371, 20);
			this->textBoxCommand->TabIndex = 0;
			// 
			// textBoxLog
			// 
			this->textBoxLog->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBoxLog->BackColor = System::Drawing::SystemColors::Window;
			this->textBoxLog->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->textBoxLog->Location = System::Drawing::Point(0, 0);
			this->textBoxLog->Multiline = true;
			this->textBoxLog->Name = L"textBoxLog";
			this->textBoxLog->ReadOnly = true;
			this->textBoxLog->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->textBoxLog->Size = System::Drawing::Size(454, 192);
			this->textBoxLog->TabIndex = 2;
			this->textBoxLog->WordWrap = false;
			// 
			// HlaeConsole
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->textBoxLog);
			this->Controls->Add(this->buttonSend);
			this->Controls->Add(this->textBoxCommand);
			this->Name = L"HlaeConsole";
			this->Size = System::Drawing::Size(454, 225);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	};

} // namespace hlae