#include "stdafx.h"

//  Copyright (c) advamcedfx.org
//
//  Last changes:
//	2009-12-20 by dominik.matrixstorm.com
//
//  First changes:
//	2009-06-30 by dominik.matrixstorm.com

#include "HlaeConsole.h"

using namespace hlae;


// HlaeConsole /////////////////////////////////////////////////////////////////

HlaeConsole::HlaeConsole() {
	InitializeComponent();

	InitConsoleWindow(80, 100);

	this->WriteLn("Hello World.");
	this->WriteLn("Built: " __DATE__, Drawing::Color::Yellow);
	this->WriteLn(">U|");

	// make sure the window handle is created (by referencing it):
	if(System::IntPtr::Zero == this->Handle)
		throw "Window handle not present";

}


HlaeConsole::~HlaeConsole()
{
	if (components)
	{
		delete components;
	}
}


void HlaeConsole::AdjustScrollBars()
{
	this->m_ScrollBarH->Minimum = 0;
	this->m_ScrollBarV->Minimum = 0;

	int iDWidth = this->m_MinimumWidth - this->m_Display->ClientSize.Width;
	int iDHeight = this->m_MinimumHeight - this->m_Display->ClientSize.Height;

	if(iDWidth<0) iDWidth = 0;
	if(iDHeight<0) iDHeight = 0;

	this->m_ScrollBarH->Maximum = iDWidth;
	this->m_ScrollBarV->Maximum = iDHeight;

	this->m_ScrollBarH->LargeChange = this->m_ScrollBarH->Maximum / m_FontHeight;
	this->m_ScrollBarH->SmallChange = 1;
	this->m_ScrollBarV->LargeChange = this->m_ScrollBarV->Maximum / m_FontHeight;
	this->m_ScrollBarV->SmallChange = 1;

	// cheat for max value:

	this->m_ScrollBarH->Maximum += this->m_ScrollBarH->LargeChange;
	this->m_ScrollBarV->Maximum += this->m_ScrollBarV->LargeChange;
}

int HlaeConsole::BufferRowToRow(int iBufferRow)
{
	int iRow =  m_RotateBuffer ? -m_RotRow +iBufferRow : iBufferRow;
	iRow = iRow % m_Rows;
	if(iRow < 0) iRow += m_Rows;

	return iRow;
}


Rectangle HlaeConsole::CalcVirtualRect()
{
	Rectangle rect;

	rect.Width = this->m_Display->Width;
	rect.Height = this->m_Display->Height;

	rect.X = this->m_ScrollBarH->Value;
	rect.Y = this->m_ScrollBarV->Value;

	return rect;
}


void HlaeConsole::Clear()
{
	DoClear();

	this->InvalidateDisplay();
}


void HlaeConsole::ClearPhysRow(int iPhysRow)
{
	if(iPhysRow<0 || iPhysRow >= m_Rows)
		return;

	for(int j=0; j<m_Cols+1; j++)
	{
		m_Chars[iPhysRow,j] = System::Char('\0');
		m_Colors[iPhysRow,j] = m_BackGroundColor;			
	}
}


void HlaeConsole::ConsoleWindow_Resize(Object^ , EventArgs^)
{
	this->AdjustScrollBars();
	this->InvalidateDisplay();
}


void HlaeConsole::ConsoleWindow_Scroll(Object ^, System::Windows::Forms::ScrollEventArgs ^)
{
	this->InvalidateDisplay();
}


void HlaeConsole::CopyToClipboard()
{
	String ^str;
	
	if(this->HasSelection())
		str = this->GetSelection();
	else {
		this->SelectAll(false);
		str = this->GetSelection();
		this->Unselect();
	}

	String ^trimchars = " \n\r";
	str = str->Trim(trimchars->ToCharArray());
	if(str->Length <= 0)
		Clipboard::Clear();
	else
		Clipboard::SetText(str, TextDataFormat::Text);
}


void HlaeConsole::Display_MouseDown(Object ^, System::Windows::Forms::MouseEventArgs ^e)
{
	if(e->Button != ::MouseButtons::Left)
	{
		m_Selecting = false;
		return;
	}

	m_MouseSelectionOld = m_MouseSelection;

	this->m_MouseSelection.X = e->X;
	this->m_MouseSelection.Y = e->Y;
	this->m_MouseSelection.Width = 1;
	this->m_MouseSelection.Height = 1;

	Rectangle vRect = this->CalcVirtualRect();

	this->m_MouseSelection.X += vRect.X;
	this->m_MouseSelection.Y += vRect.Y;

	this->m_Selecting = true;
}


void HlaeConsole::Display_MouseMove(Object ^, System::Windows::Forms::MouseEventArgs ^e)
{
	if(!this->m_Selecting)
		return;
	
	Rectangle vRect = this->CalcVirtualRect();

	this->m_MouseSelection.Width  = vRect.X + e->X - this->m_MouseSelection.X +1;
	this->m_MouseSelection.Height = vRect.Y + e->Y - this->m_MouseSelection.Y +1;

	Rectangle iRect = m_MouseSelection;
	Rectangle iRect2 = m_MouseSelectionOld;

	iRect.X -= vRect.X;
	iRect.Y -= vRect.Y;

	iRect2.X -= vRect.X;
	iRect2.Y -= vRect.Y;

	this->m_Display->Invalidate(iRect2);
	this->m_Display->Invalidate(iRect);

	m_MouseSelectionOld = m_MouseSelection;
}


void HlaeConsole::Display_MouseUp(Object ^, System::Windows::Forms::MouseEventArgs ^e)
{
	if(!this->m_Selecting)
		return;

	this->m_Selecting = false;

	if(e->Button == ::MouseButtons::Left)
	{
		Rectangle vRect = this->CalcVirtualRect();

		this->m_MouseSelection.Width  = vRect.X + e->X - this->m_MouseSelection.X +1;
		this->m_MouseSelection.Height = vRect.Y + e->Y - this->m_MouseSelection.Y +1;

		SelectByVirtualRect(m_MouseSelection, true);
	}

	this->m_Display->Refresh();

}



void HlaeConsole::Display_Paint(Object ^, System::Windows::Forms::PaintEventArgs ^e)
{
	Rectangle vRect = CalcVirtualRect();

	vRect.X += e->ClipRectangle.X;
	vRect.Y += e->ClipRectangle.Y;
	vRect.Height = e->ClipRectangle.Height;
	vRect.Width = e->ClipRectangle.Width;

	int iFirstRow = vRect.Y / m_FontHeight;
	if(iFirstRow >= m_Rows) iFirstRow = m_Rows -1;
	if(iFirstRow < 0) iFirstRow = 0;

	int iRowCnt = vRect.Height / m_FontHeight;
	if(vRect.Height % m_FontHeight) iRowCnt++;
	if(iRowCnt<1) iRowCnt = 1;
	if(iFirstRow+iRowCnt > m_Rows) iRowCnt = m_Rows -iFirstRow;

	int iOfsX = -vRect.X +e->ClipRectangle.X;
	int iOfsY = -vRect.Y +(m_FontHeight*iFirstRow) +e->ClipRectangle.Y;

	// and paint em:
	for(int i=0; i<iRowCnt; i++)
	{
		PaintLine(iFirstRow +i, iOfsX, iOfsY, e->Graphics);

		iOfsY += m_FontHeight;
	}

	// Draw selecting rect:
	if(m_Selecting)
	{
		Rectangle ttr = m_MouseSelection;
		ttr.X += -vRect.X + e->ClipRectangle.X;
		ttr.Y += -vRect.Y + e->ClipRectangle.Y;
		ttr.Width--;
		ttr.Height--;
		e->Graphics->DrawRectangle(gcnew Pen(m_SelectingColor), ttr);
	}
}

void HlaeConsole::DoClear()
{
	for(int i=0; i<m_Rows; i++)
		ClearPhysRow(i);

	m_RotateBuffer = false;
	m_RotRow = 0;
	m_CurRow = 0;

	this->DoUnselect();
}

String ^HlaeConsole::DoGetSelection()
{
	System::Text::StringBuilder ^sb = gcnew System::Text::StringBuilder("");

	if(0 <= m_SelBeginRow && 0 <= m_SelEndRow)
	{
		int iCurRow = m_SelBeginRow;
		while(true)
		{
			sb->Append(this->GetRowString(iCurRow,true));

			if(iCurRow == m_SelEndRow)
				break;
			
			iCurRow = (iCurRow+1) % m_Rows;
		}
	}

	return sb->ToString();
}


void HlaeConsole::DoSelectAll(bool bAutoUnselect)
{
	if(bAutoUnselect
		&& RowToBufferRow(0) == m_SelBeginRow
		&& RowToBufferRow(m_Rows-1) == m_SelEndRow
	)
		Unselect();
	else {
		this->SelectByRows(0,m_Rows-1);
	}

}

void HlaeConsole::DoUnselect()
{
	m_SelBeginRow = -1;
	m_SelEndRow = -1;
}


void HlaeConsole::DoWriteChar(System::Char aChar, Color aColor)
{
	if(0 == m_CurCol) ClearPhysRow(m_CurRow);

	if('\n' == aChar)
	{
		m_Chars[m_CurRow, m_CurCol] = aChar;
		m_Colors[m_CurRow, m_CurCol] = aColor;
		m_CurCol++;

		this->LineFeed();
	} else {
		if(m_CurCol >= m_Cols)
		{
			this->LineFeed();
		}
	
		m_Chars[m_CurRow, m_CurCol] = aChar;
		m_Colors[m_CurRow, m_CurCol] = aColor;
		m_CurCol++;
	}
};

void HlaeConsole::DoWriteString(String ^aString, Color aColor)
{
	for(int i=0; i<aString->Length; i++)
	{
		this->DoWriteChar(aString[i], aColor);
	}
}


String ^HlaeConsole::GetRowString(int iRow, bool bWithLineBreak)
{
	System::Text::StringBuilder ^sb = gcnew System::Text::StringBuilder();

	for(int i=0; i<m_Cols; i++)
	{
		System::Char curChar = m_Chars[iRow, i];

		if('\0' == curChar || (!bWithLineBreak && '\n' == curChar))
			break;

		sb->Append(curChar);
	}

	return sb->ToString();
}


String ^HlaeConsole::GetSelection() {
	return this->DoGetSelection();
}


bool HlaeConsole::HasSelection() {
	return (0 <= this->m_SelBeginRow) && (0 <= this->m_SelEndRow);
}


void HlaeConsole::InitConsoleWindow(int iCols, int iRows)
{
	m_Chars = gcnew array<System::Char, 2>(iRows, iCols+1); // 1 extra field for explicit newlines
	m_Colors = gcnew array<Color, 2>(iRows, iCols+1); // .
	m_Cols = iCols;
	m_CurCol = 0;
	m_CurRow = 0;
	m_Font = gcnew Drawing::Font(Drawing::FontFamily::GenericMonospace, 10.0f);
	m_FontHeight  = (int)System::Math::Ceiling(m_Font->GetHeight()); 
	m_MinimumHeight = iRows * m_FontHeight;
	m_MinimumWidth = 0;
	m_MouseSelection = Rectangle(0,0,0,0);
	m_MouseSelectionOld = m_MouseSelection;
	m_RotateBuffer = false;
	m_RotRow = 0;
	m_Rows = iRows;
	m_SelBeginRow = -1;
	m_SelEndRow = -1;
	m_Selecting = false;

	// init console arrays:
	DoClear();

	//
	// visual properties:

	this->SuspendLayout();

	this->AutoSize = false;
	this->BackColor = m_BackGroundColor;
	this->ForeColor = m_ForeGroundColor;

	this->m_Display = gcnew Control();
	this->m_Display->Cursor = Cursors::IBeam;
	this->m_Display->Dock = DockStyle::Fill;
	this->m_Display->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &HlaeConsole::Display_MouseDown);
	this->m_Display->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &HlaeConsole::Display_MouseMove);
	this->m_Display->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &HlaeConsole::Display_MouseUp);
	this->m_Display->Paint += gcnew System::Windows::Forms::PaintEventHandler( this, &HlaeConsole::Display_Paint );
	this->Controls->Add(m_Display);

	this->m_ScrollBarH = gcnew HScrollBar();
	this->m_ScrollBarH->Dock = DockStyle::Bottom;
	this->m_ScrollBarH->Minimum = 0;
	this->m_ScrollBarH->Maximum = m_Cols-1;
	this->m_ScrollBarH->Scroll += gcnew System::Windows::Forms::ScrollEventHandler(this, &HlaeConsole::ConsoleWindow_Scroll);
	this->Controls->Add(m_ScrollBarH);

	this->m_ScrollBarV = gcnew VScrollBar();
	this->m_ScrollBarV->Dock = DockStyle::Right;
	this->m_ScrollBarV->Minimum = 0;
	this->m_ScrollBarV->Maximum = m_Rows-1;
	this->m_ScrollBarV->Scroll += gcnew System::Windows::Forms::ScrollEventHandler(this, &HlaeConsole::ConsoleWindow_Scroll);
	this->Controls->Add(m_ScrollBarV);

	this->Resize += gcnew System::EventHandler(this, &HlaeConsole::ConsoleWindow_Resize);

	this->ResumeLayout(false);
}


void HlaeConsole::InvalidateDisplay()
{
	this->m_Display->Invalidate(this->m_Display->ClientRectangle);
}


bool HlaeConsole::IsSelectedRow(int iBufferRow)
{
	if(m_SelBeginRow<0 || m_SelEndRow <0)
		return false;

	int iVRowBegin = BufferRowToRow(m_SelBeginRow);
	int iVRowEnd = BufferRowToRow(m_SelEndRow);
	int iVRow = BufferRowToRow(iBufferRow);

	return ((iVRowBegin <= iVRow) && (iVRow <= iVRowEnd));

}

void HlaeConsole::LineFeed()
{
	m_CurRow++;
	if(m_CurRow == m_Rows) m_RotateBuffer = true;				
	m_CurRow = m_CurRow % m_Rows;
	m_CurCol = 0;
	if(m_RotateBuffer) m_RotRow = m_CurRow;
	if(m_RotRow <0) m_RotRow += m_Rows;

	// update selction:
	if(0 <= m_SelBeginRow && 0<= m_SelEndRow)
	{
		bool bWasOne = m_SelBeginRow == m_SelEndRow;

		if(m_CurRow == m_SelBeginRow)
		{
			if(bWasOne)
			{
				m_SelBeginRow = -1;
				m_SelEndRow = -1;
			} else {
				m_SelBeginRow = (m_SelBeginRow -1) % m_Rows;
			}
		}
	}
}

/// <returns> width of every char in a line + last entry is total width of the line</returns>
array<float> ^HlaeConsole::LineStringWidths(String ^aLineString, System::Drawing::Graphics ^g)
{

	StringFormat ^stringFormat = gcnew StringFormat();
	array<float> ^stringWidths = gcnew array<float>(aLineString->Length+1);

	stringWidths[aLineString->Length] = g->MeasureString(aLineString, m_Font).Width;
	RectangleF tRect = RectangleF(0.0f, 0.0f, stringWidths[aLineString->Length], (float)m_FontHeight);
	for(int i=0; i<aLineString->Length;)
	{
		int h=aLineString->Length - i;
		if(h>30) h=30;

		array<System::Drawing::CharacterRange> ^ranges = gcnew array<System::Drawing::CharacterRange>(h);
		for(int j=0; j<h; j++)
			ranges[j] = CharacterRange(i+j,1);

		stringFormat->SetMeasurableCharacterRanges(ranges);
		array<System::Drawing::Region^>^ stringRegions = g->MeasureCharacterRanges(aLineString, m_Font, tRect, stringFormat );

		for(int j=0; j<h; j++)
		{
			RectangleF bounds;
			bounds = stringRegions[j]->GetBounds(g);
			stringWidths[i+j] = bounds.Width;
		}

		i += h;
	}

	return stringWidths;
}

void HlaeConsole::PaintLine( int iRow, int iOfsX, int iOfsY, System::Drawing::Graphics ^g )
{
	int iBufferRow = RowToBufferRow(iRow);

	int iLineHeight = m_FontHeight;
	int iLineWidth = this->m_MinimumWidth;

	//
	// Do String measuring:

	String ^lineString = this->GetRowString(iBufferRow, false);
	array<float> ^ lineStringWidths = this->LineStringWidths(lineString, g);

	int iTextWidth = (int)System::Math::Ceiling(lineStringWidths[lineString->Length]);

	// increase scroll size if neccessary:
	if(iLineWidth < iTextWidth)
	{
		this->m_MinimumWidth = iTextWidth;
		iLineWidth = iTextWidth;
		
	}

	//
	// Drawing

	// Draw selected:
	if(IsSelectedRow(iBufferRow))
	{
		g->FillRectangle(gcnew SolidBrush(m_BgSelectedColor), RectangleF((float)g->VisibleClipBounds.Left,(float)iOfsY,(float)g->VisibleClipBounds.Width,(float)iLineHeight));
	}

	// draw decorations (line number, ...);
	g->DrawLine(gcnew Pen(m_DecorColor), 0.0f, (float)iOfsY +(float)iLineHeight, (float)g->VisibleClipBounds.Width,  (float)iOfsY +(float)iLineHeight);

	//
	// Now draw the line text:
	float fPos = 0;
	for(int i=0; i<lineString->Length; i++)
	{
		System::Drawing::Brush ^textBrush = gcnew SolidBrush(m_Colors[iBufferRow, i]);

		System::Char curChar = m_Chars[iBufferRow, i];
		System::String ^aSymbol = curChar.ToString();

		float fNewWidth = lineStringWidths[i];
		g->DrawString(aSymbol, m_Font, textBrush, fPos +(float)iOfsX, (float)iOfsY);

		fPos += fNewWidth;
	}

}

int HlaeConsole::RowToBufferRow(int iRow)
{
	int iBufferRow = m_RotateBuffer ? m_RotRow +iRow : iRow;
	iBufferRow = iBufferRow % m_Rows;
	if(iBufferRow < 0) iBufferRow += m_Rows;

	return iBufferRow;
}

void HlaeConsole::SelectAll() {
	SelectAll(true);
}


void HlaeConsole::SelectAll(bool bAutoUnselect) {
	DoSelectAll(bAutoUnselect);

	this->InvalidateDisplay();
}


void HlaeConsole::SelectByVirtualRect(Rectangle virtRect, bool bAutoUnselect)
{
	int iBeginRow = (int)Math::Floor((float)virtRect.Y / (float)m_FontHeight);
	int iEndRow = (int)Math::Floor((float)(virtRect.Y +virtRect.Height -1) / (float)m_FontHeight);

	if(bAutoUnselect && iEndRow == iBeginRow && 0<=m_SelBeginRow && 0<=m_SelEndRow)
	{
		DoUnselect();
	}
	else if((0<=iBeginRow) && (iBeginRow<=m_Rows) && (iBeginRow <= iEndRow) && (iEndRow<=m_Rows))
	{
		m_SelBeginRow = RowToBufferRow(iBeginRow);
		m_SelEndRow = RowToBufferRow(iEndRow);
	} else {
		DoUnselect();
	}
}

void HlaeConsole::SelectByRows(int iBegin, int iEnd)
{
	if(m_Rows <= iBegin) iBegin = m_Rows-1;
	if(m_Rows <= iEnd) iEnd = m_Rows-1;
	if(iBegin<0) iBegin=0;
	if(iEnd<iBegin) iEnd=iBegin;

	m_SelBeginRow = RowToBufferRow(iBegin);
	m_SelEndRow = RowToBufferRow(iEnd);
}


void HlaeConsole::Unselect()
{
	DoUnselect();

	this->InvalidateDisplay();
}

void HlaeConsole::Write(System::Char aChar, Color aColor)
{
	this->DoWriteChar(aChar, aColor);
	this->InvalidateDisplay();
}

void HlaeConsole::Write(System::Char aChar)
{
	this->DoWriteChar(aChar, m_ForeGroundColor);
	this->InvalidateDisplay();
}

void HlaeConsole::Write(String ^aString, Color aColor)
{
	this->DoWriteString(aString, aColor);
	this->InvalidateDisplay();
}

void HlaeConsole::Write(String ^aString)
{
	this->DoWriteString(aString, m_ForeGroundColor);
	this->InvalidateDisplay();
}

void HlaeConsole::WriteLn(System::Char aChar, Color aColor)
{
	this->DoWriteChar(aChar, aColor);
	this->DoWriteChar('\n', aColor);
	this->InvalidateDisplay();
}

void HlaeConsole::WriteLn(System::Char aChar)
{
	Color aColor = m_ForeGroundColor;
	this->DoWriteChar(aChar, aColor);
	this->DoWriteChar('\n', aColor);
	this->InvalidateDisplay();
}

void HlaeConsole::WriteLn(String ^aString, Color aColor)
{
	this->DoWriteString(aString, aColor);
	this->DoWriteChar('\n', aColor);
	this->InvalidateDisplay();
}

void HlaeConsole::WriteLn(String ^aString)
{
	Color aColor = m_ForeGroundColor;
	this->DoWriteString(aString, aColor);
	this->DoWriteChar('\n', aColor);
	this->InvalidateDisplay();
}

void HlaeConsole::WriteLn()
{
	this->DoWriteChar('\n', m_ForeGroundColor);
	this->InvalidateDisplay();
}



// HlaeConsoleColors ///////////////////////////////////////////////////////////

HlaeConsoleColors::HlaeConsoleColors() {
	this->m_BgDecor = Color::FromArgb(255,24,32,64);
	this->m_BgSelected = Color::DarkGray;
	this->m_BgStandard = Color::Black;
	this->m_Debug = Color::Violet;
	this->m_Error = Color::Red;
	this->m_GameMesssage = Color::Orange;
	this->m_Info = Color::LightGreen;
	this->m_Selecting = Color::Pink;
	this->m_Standard = Color::White;
	this->m_Unknown = Color::Pink;
	this->m_Verbose = Color::LightBlue;
	this->m_Warning = Color::Yellow;
}


// HlaeConsoleHistory //////////////////////////////////////////////////////////

HlaeConsoleHistory::HlaeConsoleHistory(unsigned int maxHistory) {
	this->m_CurNode = nullptr;
	this->m_MaxHistory = maxHistory;
}


String ^HlaeConsoleHistory::GetCurrentCommand() {
	if(nullptr == this->m_CurNode)
		return "";

	return this->m_CurNode->Value;
}


void HlaeConsoleHistory::MoveToLast() {
	if(0 < m_Commands.Count)
		m_CurNode = m_Commands.Last;
}


String ^HlaeConsoleHistory::NewerCommand() {
	if(nullptr == m_CurNode)
		MoveToLast();
	else
	{
		System::Collections::Generic::LinkedListNode<String ^> ^node = this->m_CurNode->Next;
		if(nullptr != node)
			this->m_CurNode = node;
	}

	return GetCurrentCommand();
}


String ^HlaeConsoleHistory::OlderCommand() {
	if(nullptr == m_CurNode)
		MoveToLast();
	else
	{
		System::Collections::Generic::LinkedListNode<String ^> ^node = this->m_CurNode->Previous;
		if(nullptr != node)
			this->m_CurNode = node;
	}

	return GetCurrentCommand();
}


void HlaeConsoleHistory::StoreCommand(String ^aCommand) {
	m_Commands.AddLast(aCommand);

	while(m_MaxHistory < m_Commands.Count)
		m_Commands.RemoveFirst();
	
	this->m_CurNode = nullptr;
}
