#pragma once

//  Copyright (c) advamcedfx.org
//
//  Last changes:
//	2009-12-20 by dominik.matrixstorm.com
//
//  First changes:
//	2009-06-30 by dominik.matrixstorm.com


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace hlae {


// HlaeConsole /////////////////////////////////////////////////////////////////

ref class HlaeConsole;

public ref class HlaeConsole : public System::Windows::Forms::UserControl
{
public:
	HlaeConsole();

	void Clear();

	void CopyToClipboard();

	String ^GetSelection();

	bool HasSelection();

	void SelectAll(bool bAutoUnselect);

	void SelectAll();

	void Unselect();

	void Write(System::Char aChar, Color aColor);

	void Write(System::Char aChar);

	void Write(String ^aString, Color aColor);

	void Write(String ^aString);

	void WriteLn(System::Char aChar, Color aColor);

	void WriteLn(System::Char aChar);

	void WriteLn(String ^aString, Color aColor);

	void WriteLn(String ^aString);

	void WriteLn();

protected:
	~HlaeConsole();

private:
	static Color const m_BackGroundColor = Color::Black;
	static Color const m_ForeGroundColor = Color::White;
	static Color const m_SelectingColor = Color::Yellow;
	static Color const m_BgSelectedColor = Color::Silver;
	static Color const m_DecorColor = Color::Cyan;


	array<System::Char, 2> ^m_Chars;
	array<Color, 2> ^m_Colors;
	int m_Cols;
	int m_CurCol;
	int m_CurRow;
	Control ^m_Display;
	System::Drawing::Font ^m_Font;
	int m_FontHeight;
	int m_MinimumHeight;
	int m_MinimumWidth;
	Rectangle m_MouseSelection;
	Rectangle m_MouseSelectionOld;
	bool m_RotateBuffer;
	int m_RotRow;
	int m_Rows;
	HScrollBar ^m_ScrollBarH;
	VScrollBar ^m_ScrollBarV;
	int m_SelBeginRow;
	int m_SelEndRow;
	bool m_Selecting;


	void AdjustScrollBars();

	int BufferRowToRow(int iBufferRow);

	Rectangle CalcVirtualRect();

	void ClearPhysRow(int iPhysRow);

	void ConsoleWindow_Resize(Object^ , EventArgs^);

	void ConsoleWindow_Scroll(Object ^, System::Windows::Forms::ScrollEventArgs ^);


	void Display_MouseDown(Object ^, System::Windows::Forms::MouseEventArgs ^e);

	void Display_MouseMove(Object ^, System::Windows::Forms::MouseEventArgs ^e);

	void Display_MouseUp(Object ^, System::Windows::Forms::MouseEventArgs ^e);


	void Display_Paint(Object ^, System::Windows::Forms::PaintEventArgs ^e);

	void DoClear();

	String ^DoGetSelection();

	void DoUnselect();

	void DoSelectAll(bool bAutoUnselect);

	void DoWriteChar(System::Char aChar, Color aColor);

	void DoWriteString(String ^aString, Color aColor);

	String ^GetRowString(int iRow, bool bWithLineBreak);

	void InitConsoleWindow(int iCols, int iRows);

	bool IsSelectedRow(int iBufferRow);

	void LineFeed();

	/// <returns> width of every char in a line + last entry is total width of the line</returns>
	array<float> ^LineStringWidths(String ^aLineString, System::Drawing::Graphics ^g);

	void PaintLine( int iRow, int iOfsX, int iOfsY, System::Drawing::Graphics ^g );

	int RowToBufferRow(int iRow);

	void SelectByVirtualRect(Rectangle virtRect, bool bAutoUnselect);

	void SelectByRows(int iBegin, int iEnd);

	void InvalidateDisplay();

private:
	/// <summary>
	/// Required designer variable.
	/// </summary>
	System::ComponentModel::IContainer^  components;

private: System::Windows::Forms::ContextMenuStrip^  contextMenuStrip;
private: System::Windows::Forms::ToolStripMenuItem^  copyToolStripMenuItem;
private: System::Windows::Forms::ToolStripSeparator^  toolStripMenuItem1;
private: System::Windows::Forms::ToolStripMenuItem^  clearToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  selectAllToolStripMenuItem;
private: System::Windows::Forms::ToolStripSeparator^  toolStripMenuItem2;




#pragma region Windows Form Designer generated code
	/// <summary>
	/// Required method for Designer support - do not modify
	/// the contents of this method with the code editor.
	/// </summary>
	void InitializeComponent(void)
	{
		this->components = (gcnew System::ComponentModel::Container());
		this->contextMenuStrip = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
		this->copyToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->toolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripSeparator());
		this->selectAllToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->toolStripMenuItem2 = (gcnew System::Windows::Forms::ToolStripSeparator());
		this->clearToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->contextMenuStrip->SuspendLayout();
		this->SuspendLayout();
		// 
		// contextMenuStrip
		// 
		this->contextMenuStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {this->copyToolStripMenuItem, 
			this->toolStripMenuItem1, this->selectAllToolStripMenuItem, this->toolStripMenuItem2, this->clearToolStripMenuItem});
		this->contextMenuStrip->Name = L"contextMenuStrip";
		this->contextMenuStrip->Size = System::Drawing::Size(171, 82);
		// 
		// copyToolStripMenuItem
		// 
		this->copyToolStripMenuItem->Name = L"copyToolStripMenuItem";
		this->copyToolStripMenuItem->ShortcutKeyDisplayString = L"Strg+C";
		this->copyToolStripMenuItem->Size = System::Drawing::Size(170, 22);
		this->copyToolStripMenuItem->Text = L"&Copy";
		this->copyToolStripMenuItem->Click += gcnew System::EventHandler(this, &HlaeConsole::copyToolStripMenuItem_Click);
		// 
		// toolStripMenuItem1
		// 
		this->toolStripMenuItem1->Name = L"toolStripMenuItem1";
		this->toolStripMenuItem1->Size = System::Drawing::Size(167, 6);
		// 
		// selectAllToolStripMenuItem
		// 
		this->selectAllToolStripMenuItem->Name = L"selectAllToolStripMenuItem";
		this->selectAllToolStripMenuItem->ShortcutKeyDisplayString = L"Strg+A";
		this->selectAllToolStripMenuItem->Size = System::Drawing::Size(170, 22);
		this->selectAllToolStripMenuItem->Text = L"Select &All";
		this->selectAllToolStripMenuItem->Click += gcnew System::EventHandler(this, &HlaeConsole::selectAllToolStripMenuItem_Click);
		// 
		// toolStripMenuItem2
		// 
		this->toolStripMenuItem2->Name = L"toolStripMenuItem2";
		this->toolStripMenuItem2->Size = System::Drawing::Size(167, 6);
		// 
		// clearToolStripMenuItem
		// 
		this->clearToolStripMenuItem->Name = L"clearToolStripMenuItem";
		this->clearToolStripMenuItem->Size = System::Drawing::Size(170, 22);
		this->clearToolStripMenuItem->Text = L"Cl&ear";
		this->clearToolStripMenuItem->Click += gcnew System::EventHandler(this, &HlaeConsole::clearToolStripMenuItem_Click);
		// 
		// HlaeConsole
		// 
		this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
		this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		this->ContextMenuStrip = this->contextMenuStrip;
		this->MinimumSize = System::Drawing::Size(50, 50);
		this->Name = L"HlaeConsole";
		this->Size = System::Drawing::Size(326, 243);
		this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &HlaeConsole::HlaeConsole_KeyDown);
		this->contextMenuStrip->ResumeLayout(false);
		this->ResumeLayout(false);

	}
#pragma endregion


private: System::Void copyToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->CopyToClipboard();
		 }

private:
	System::Void HlaeConsole_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e)
	{
		if(e->Control)
		{
			if(e->KeyCode == Keys::C) CopyToClipboard();
			else if(e->KeyCode == Keys::A) SelectAll();
		}
	}

private: System::Void selectAllToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->SelectAll();
		 }
private: System::Void clearToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->Clear();
		 }
private: System::Void buttonSubmit_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private:
	System::Void textBoxCommand_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e)
	{
		switch(e->KeyCode)
		{
		case Keys::Enter:
			e->Handled = true;
			//this->m_CommandHistory->StoreCommand(this->textBoxCommand->Text);
			//this->DoGameConsoleCommand();
			//this->textBoxCommand->Text = "";
			break;
		case Keys::Up:
			e->Handled = true;
			//this->textBoxCommand->Text = this->m_CommandHistory->OlderCommand();
			break;
		case Keys::Down:
			e->Handled = true;
			//this->textBoxCommand->Text = this->m_CommandHistory->NewerCommand();
			break;
		}

	}
};


// HlaeConsoleColors ///////////////////////////////////////////////////////////


ref class HlaeConsoleColors {
public:
	Color m_BgDecor;
	Color m_BgSelected;
	Color m_BgStandard;
	Color m_Debug;
	Color m_Error;
	Color m_GameMesssage;
	Color m_Info;
	Color m_Selecting;
	Color m_Standard;
	Color m_Unknown;
	Color m_Verbose;
	Color m_Warning;

	HlaeConsoleColors();
};


// HlaeConsoleHistory //////////////////////////////////////////////////////////

ref class HlaeConsoleHistory
{
public:
	HlaeConsoleHistory(unsigned int maxHistory);

	String ^GetCurrentCommand();

	String ^NewerCommand();

	void StoreCommand(String ^aCommand);

	String ^OlderCommand();

private:
	unsigned int m_MaxHistory;
	System::Collections::Generic::LinkedList<String ^> m_Commands;
	System::Collections::Generic::LinkedListNode<String ^> ^m_CurNode;

	void MoveToLast();
};


} // namespace hlae