#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-11 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


namespace Afx {


struct CursorBackup
{
	int Position;

	CursorBackup();
	CursorBackup(CursorBackup const & cursorBackup);

	void Copy(CursorBackup const & cursorBackup);
private:
};


class Cursor
{
public:
	Cursor(char const * text);
	~Cursor();

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

	bool IsDigit() const;
	static bool IsDigit (char const val);

	bool IsNull() const;
	static bool IsNull (char const val);

	bool IsSpace() const;	
	static bool IsSpace (char const val);

	double ReadDouble (void);

	/// <param name="outSkipped">pointer, can be 0</param>
	double ReadDouble (int * outSkipped);

	long ReadLong (void);

	/// <param name="outSkipped">pointer, can be 0</param>
	long ReadLong (int * outSkipped);

	unsigned long ReadULong (void);

	/// <param name="outSkipped">pointer, can be 0</param>
	unsigned long ReadULong (int * outSkipped);
	
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
	char * m_Text;
};


} // namespace Afx {
