#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-11 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


#include "Cursor.h"

#include <ctype.h>
#include <string.h>


using namespace std;

using namespace Afx;


// Cursor //////////////////////////////////////////////////////////////////////

Cursor::Cursor(char const * text)
{
	size_t len = strlen(text);

	m_Len = len;
	m_Text = new char[1 +len];
	memcpy(m_Text, text, (1 +len) * sizeof(char));
}

Cursor::~Cursor()
{
	delete m_Text;
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

	return m_Text[pos];
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

bool Cursor::IsDigit() const
{
	return IsDigit(Get());
}

bool Cursor::IsDigit (char const val)
{
	return 0 != isdigit(val);
}

bool Cursor::IsNull() const
{
	return IsNull(Get());
}

bool Cursor::IsNull (char const val)
{
	return 0 == val;
}

bool Cursor::IsSpace() const
{
	return IsSpace(Get());
}

bool Cursor::IsSpace (char const val)
{
	return 0 != isspace(val);
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
