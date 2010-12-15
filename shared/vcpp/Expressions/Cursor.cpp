#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-22 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


#include "Cursor.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

using namespace Afx;
using namespace Afx::Expressions;


// Cursor //////////////////////////////////////////////////////////////////////

Cursor::Cursor(IStringValue * text)
: m_Text(text)
{
	size_t len = strlen(m_Text.getData());

	m_Len = len;
}

void Cursor::Add()
{
	m_Backup.Position++;
}

char Cursor::AddGet()
{
	Add();
	return Get();
}

CursorBackup const & Cursor::Backup() const
{
	CursorBackup const & ret = m_Backup;

	return ret;
}

char Cursor::Get() const
{
	return Get(0);
}

char Cursor::Get(int ofs) const
{
	return GetAbs(m_Backup.Position +ofs);
}

char Cursor::GetAbs(int pos) const
{
	if(pos < 0 || m_Len <= pos) return 0;

	return m_Text.getData()[pos];
}

char Cursor::GetAdd()
{
	char val = Get();

	Add();

	return val;
}

int Cursor::GetPos(void) const
{
	return m_Backup.Position;
}

bool Cursor::IsAlnum() const
{
	return IsAlnum(Get());
}

bool Cursor::IsAlnum (char const val)
{
	return 0 != isalnum(val);
}

bool Cursor::IsAlpha() const
{
	return IsAlpha(Get());
}

bool Cursor::IsAlpha (char const val)
{
	return 0 != isalpha(val);
}

bool Cursor::IsControlChar() const
{
	return IsControlChar(Get());
}

bool Cursor::IsControlChar(char val)
{
	return
		Cursor::IsNull(val)
		|| Cursor::IsEscape(val)
		|| Cursor::IsSpace(val)
		|| Cursor::IsPaOpen(val)
		|| Cursor::IsPaClose(val)
	;
}


bool Cursor::IsDigit() const
{
	return IsDigit(Get());
}

bool Cursor::IsDigit (char const val)
{
	return 0 != isdigit(val);
}


bool Cursor::IsEscape() const
{
	return IsEscape(Get());
}

bool Cursor::IsEscape (char const val)
{
	return '\\' == val;
}

bool Cursor::IsNull() const
{
	return IsNull(Get());
}

bool Cursor::IsNull (char const val)
{
	return 0 == val;
}

bool Cursor::IsPaClose() const
{
	return IsPaClose(Get());
}

bool Cursor::IsPaClose (char const val)
{
	return ')' == val;
}

bool Cursor::IsPaOpen() const
{
	return IsPaOpen(Get());
}

bool Cursor::IsPaOpen (char const val)
{
	return '(' == val;
}


bool Cursor::IsSpace() const
{
	return IsSpace(Get());
}

bool Cursor::IsSpace (char const val)
{
	return 0 != isspace(val);
}


bool Cursor::ReadBoolValue (BoolT & outValue)
{
	for(int i=0; i<2; i++)
	{
		char * text = 0 == i ? "false" : "true";
		int textLen = strlen(text);
		bool match = true;

		for(int j=0; match && j<textLen; j++)
		{
			match = text[j] == Get(j);
		}

		match = match && IsControlChar(Get(textLen));

		if(match)
		{
			outValue = 0 == i ? false : true;
			m_Backup.Position += textLen;
			return true;
		}
	}

	return false;
}

bool Cursor::ReadIntValue (IntT & outValue)
{
	if(IsNull()) return false;

	char const * startPtr = m_Text.getData() +m_Backup.Position;
	char * endPtr;
	long value = strtol(startPtr, &endPtr, 0);
	int skipped = (endPtr -startPtr) / sizeof(char);

	if(0 < skipped && IsControlChar(Get(skipped)))
	{
		outValue = value;
		m_Backup.Position += skipped;
		return true;
	}

	return false;
}

bool Cursor::ReadFloatValue (FloatT & outValue)
{
	if(IsNull()) return false;

	char const * startPtr = m_Text.getData() +m_Backup.Position;
	char * endPtr;
	double value = strtod(startPtr, &endPtr);
	int skipped = (endPtr -startPtr) / sizeof(char);

	if(0 < skipped && IsControlChar(Get(skipped)))
	{
		outValue = value;
		m_Backup.Position += skipped;
		return true;
	}

	return false;
}

IStringValue * Cursor::ReadStringValue (void)
{
	int count = 0;

	while(!IsControlChar(Get(count))) count++;

	char * data = new char[1+count];
	for(int i=0; i<count; i++) data[i] = GetAdd();
	data[count] = 0;

	return StringValue::TakeOwnership(1+count, data);
}

void Cursor::Restore(CursorBackup const & backup)
{
	m_Backup.Copy(backup);
}

void Cursor::Seek(int ofs)
{
	m_Backup.Position += ofs;
}

int Cursor::SkipAlnum()
{
	int skipped = 0;

	while(IsAlnum(Get()))
	{
		Add();
		skipped++;
	}

	return skipped;
}

int Cursor::SkipAlpha()
{
	int skipped = 0;

	while(IsAlpha(Get()))
	{
		Add();
		skipped++;
	}

	return skipped;
}

int Cursor::SkipDigit()
{
	int skipped = 0;

	while(IsDigit(Get()))
	{
		Add();
		skipped++;
	}

	return skipped;
}

int Cursor::SkipSpace()
{
	int skipped = 0;

	while(IsSpace(Get()))
	{
		Add();
		skipped++;
	}

	return skipped;
}

void Cursor::Sub()
{
	m_Backup.Position--;
}


void Cursor::SetPos(int value)
{
	m_Backup.Position = value;
}



// CursorBackup ////////////////////////////////////////////////////////////////

CursorBackup::CursorBackup()
: Position(0)
{
}

CursorBackup::CursorBackup(CursorBackup const & cursorBackup)
{
	Copy(cursorBackup);
}

void CursorBackup::Copy(CursorBackup const & cursorBackup)
{
	Position = cursorBackup.Position;
}
