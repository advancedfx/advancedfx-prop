#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2011-03-10 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


#include "Types.h"


namespace Afx { namespace Expressions {


struct CursorBackup
{
	int Position;

	CursorBackup();
	CursorBackup(CursorBackup const & cursorBackup);

	void Copy(CursorBackup const & cursorBackup);
private:
};


class Cursor : public Ref
{
public:
	Cursor(IStringValue * text);

	/// <summary>Advance cursor position to the right.</summary>
	void Add();

	char AddGet();

	CursorBackup const & Backup() const;

	char Get() const;
	char Get(int ofs) const;
	char GetAbs(int pos) const;

	/// <summary>Get and Add</summary>
	/// <retruns>value from Get</returns>
	char GetAdd();

	int GetPos(void) const;

	bool IsAlnum() const;
	static bool IsAlnum (char const val);

	bool IsAlpha() const;
	static bool IsAlpha (char const val);

	bool IsControlChar() const;
	static bool IsControlChar(char val);

	bool IsDigit() const;
	static bool IsDigit (char const val);

	bool IsEscape() const;
	static bool IsEscape (char const val);

	bool IsNull() const;
	static bool IsNull (char const val);

	bool IsPaClose() const;
	static bool IsPaClose (char const val);

	bool IsPaOpen() const;
	static bool IsPaOpen (char const val);

	bool IsSpace() const;	
	static bool IsSpace (char const val);

	bool ReadBoolValue (BoolT & outValue);

	bool ReadFloatValue (FloatT & outValue);

	IStringValue * ReadIdentifier (void);
	
	bool ReadIntValue (IntT & outValue);

	bool ReadStringText(int & outLength, char * & outData);

	void Restore(CursorBackup const & backup);

	void Seek(int ofs);

	/// <retruns>Number of characters skipped.</returns>
	int SkipAlnum();

	/// <retruns>Number of characters skipped.</returns>
	int SkipAlpha();

	/// <retruns>Number of characters skipped.</returns>
	int SkipDigit();

	/// <summary>Advances the cursor until there is no more white space character under it.</summary>
	/// <retruns>Number of characters skipped.</returns>
	int SkipSpace();

	/// <summary>Recede cursor position to the left.</summary>
	void Sub();

	void SetPos(int value);

private:
	CursorBackup m_Backup;
	int m_Len;
	StringValueRef m_Text;
};

class CursorRef : public RefPtr<Cursor>
{
public:
	CursorRef(Cursor * val) : RefPtr(val) {}
};


} } // namespace Afx { namespace Expressions {
