#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-02 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


// Test version:
//
// Currently precendence etc. won't work correctly (because I skipped the token stage).
// Several changes required, but this is for testing now.


// Currently the parsing is rather inefficient and can be speed-up later
// i.e. by using specialized parsing tools, that operate on trees generated
// from the grammar or similar tricks (currently it's more trial and error).


#include "expressions.h"

#include <ctype.h>
#include <string.h>

#include <exception>
#include <list>
#include <map>

using namespace std;

using namespace Afx;
using namespace Afx::Expr;


////////////////////////////////////////////////////////////////////////////////


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


////////////////////////////////////////////////////////////////////////////////




struct TokenParenthesis;
struct TokenSet;

struct Token
{
	enum Type_t {
		T_None,
		T_Parenthesis,
		T_Set,
		T_Bool,
		T_Int,
		T_Text
	} Type;

	union Value_t {
		bool Bool;
		int Int;
		TokenParenthesis * Parenthesis;
		TokenSet * Set;
		char * Text;
	} Value;

	Token() : Type(T_None) {}

	Token(TokenParenthesis * value) : Type(T_Parenthesis) { Value.Parenthesis = value; }

	Token(TokenSet * value) : Type(T_Set) { Value.Set = value; }
	
	Token(bool value) : Type(T_Bool) { Value.Bool = value; }
	
	Token(int value) : Type(T_Int) { Value.Int = value; }
	
	Token(char const * value) : Type(T_Text) {
		size_t len = strlen(value);

		Value.Text = new char[1+len];
		memcpy(Value.Text, value, len * sizeof(char));
		Value.Text[len] = 0;
	}

	~Token() {
		switch(Type)
		{
		case T_Text: delete Value.Text; break;
		}
	}
};

typedef std::list<Token *> TokenList;

class Tokens : public Ref
{
public:
	Tokens()
	{
	}

	TokenList & List (void)
	{
		TokenList & ret = m_TokenList;

		return ret;
	}

protected:
	~Tokens()
	{
		for(TokenList::iterator it = m_TokenList.begin(); it != m_TokenList.end(); it++)
		{
			delete (*it);
		}
	}

private:
	TokenList m_TokenList;
};




////////////////////////////////////////////////////////////////////////////////


class BubbleFn
{
public:
	BubbleFn(char const * name, ICompileable * compileable) {
		if(!name) m_Name = 0;
		else {
			size_t len = strlen(name);
			m_Name = new char[1+len];
			memcpy(m_Name, name, (1 +len) * sizeof(char));
			m_Name[len] = 0;
		}

		if(compileable) compileable->Ref()->AddRef();
		m_Compileable = compileable;
	}

	~BubbleFn() {
		delete m_Name;
		if(m_Compileable) m_Compileable->Ref()->Release();
	}

	ICompileable * GetCompileable (void) {
		return m_Compileable;
	}

	char const * GetName (void) {
		return m_Name;
	}


private:
	ICompileable * m_Compileable;
	char * m_Name;
};


class Bubble : public Ref,
	public IBubble
{
public:
	Bubble();

	virtual void Add(char const * name, ICompileable * compileable);

	virtual ICompiled * Compile(char const * code);

	virtual ::Afx::IRef * Ref (void);

protected:
	~Bubble();

private:
	typedef list<BubbleFn *> FunctionList;
	FunctionList m_Functions;
};



class Error : public Ref,
	public IError
{
public:
	Error() {}

	virtual ::Afx::IRef * Ref (void) { return dynamic_cast<::Afx::IRef *>(this); }

protected:


private:


};


struct __declspec(novtable) FnTools abstract
{
	static bool CheckArgs(ICompileArgs * args, ICompiled::Type type, int minCount)
	{
		bool bOk =
			0 != args
			&& 2 <= args->GetCount();
		;

		if(bOk)
		{
			int count = args->GetCount();

			for(int i=0; i<count && bOk; i++)
			{
				bOk = bOk && type == args->GetArg(i)->GetType();
			}
		}

		return bOk;
	}
};


class FnBoolsToBoolBase abstract : public Ref,
	public IBool
{
public:
	virtual bool EvalBool (void) abstract;

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

protected:
	typedef IBool * FnT;

	int m_Count;
	FnT * m_Fns;

	FnBoolsToBoolBase(ICompileArgs * args)
	{
		args->Ref()->AddRef();

		int count = args->GetCount();

		m_Count = count;
		m_Fns = new FnT[count];

		for(int i=0; i<count; i++)
		{
			FnT fn = args->GetArg(i)->GetBool();

			fn->Ref()->AddRef();

			m_Fns[i] = fn;
		}

		args->Ref()->Release();
	}

	virtual ~FnBoolsToBoolBase() {
		for(int i=0; i<m_Count; i++) m_Fns[i]->Ref()->Release();
	}
};


class FnIntsToBoolBase abstract : public Ref,
	public IBool
{
public:
	virtual bool EvalBool (void) abstract;

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

protected:
	typedef IInt * FnT;

	int m_Count;
	FnT * m_Fns;

	FnIntsToBoolBase(ICompileArgs * args)
	{
		args->Ref()->AddRef();

		int count = args->GetCount();

		m_Count = count;
		m_Fns = new FnT[count];

		for(int i=0; i<count; i++)
		{
			FnT fn = args->GetArg(i)->GetInt();

			fn->Ref()->AddRef();

			m_Fns[i] = fn;
		}

		args->Ref()->Release();
	}

	virtual ~FnIntsToBoolBase() {
		for(int i=0; i<m_Count; i++) m_Fns[i]->Ref()->Release();
	}
};


class FnIntsToIntBase abstract : public Ref,
	public IInt
{
public:
	virtual int EvalInt (void) abstract;

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

protected:
	typedef IInt * FnT;

	int m_Count;
	FnT * m_Fns;

	FnIntsToIntBase(ICompileArgs * args)
	{
		args->Ref()->AddRef();

		int count = args->GetCount();

		m_Count = count;
		m_Fns = new FnT[count];

		for(int i=0; i<count; i++)
		{
			FnT fn = args->GetArg(i)->GetInt();

			fn->Ref()->AddRef();

			m_Fns[i] = fn;
		}

		args->Ref()->Release();
	}

	virtual ~FnIntsToIntBase() {
		for(int i=0; i<m_Count; i++) m_Fns[i]->Ref()->Release();
	}
};


class FnAnd : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgs(args, ICompiled::T_Bool, 2)
			? new Compiled(new FnAnd(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		bool result = true;

		for(int i=0; i<m_Count; i++)
		{
			result = result && m_Fns[i]->EvalBool();
		}

		return result;
	}

private:
	FnAnd(ICompileArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnAndCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnAnd::Compile(args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnOr : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgs(args, ICompiled::T_Bool, 2)
			? new Compiled(new FnOr(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		bool result = false;

		for(int i=0; i<m_Count; i++)
		{
			result = result || m_Fns[i]->EvalBool();
		}

		return result;
	}

private:
	FnOr(ICompileArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnOrCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnOr::Compile(args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnNot : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgs(args, ICompiled::T_Bool, 1)
			? new Compiled(new FnNot(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return !m_Fns[0]->EvalBool();
	}

private:
	FnNot(ICompileArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnNotCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnNot::Compile(args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnLessBool : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgs(args, ICompiled::T_Bool, 2)
			? new Compiled(new FnLessBool(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return m_Fns[0]->EvalBool() < m_Fns[1]->EvalBool();
	}

private:
	FnLessBool(ICompileArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnLessBoolCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnLessBool::Compile(args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnLessInt : public FnIntsToBoolBase
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgs(args, ICompiled::T_Int, 2)
			? new Compiled(new FnLessInt(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return m_Fns[0]->EvalInt() < m_Fns[1]->EvalInt();
	}

private:
	FnLessInt(ICompileArgs * args) : FnIntsToBoolBase(args) {}
};


class FnLessIntCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnLessInt::Compile(args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};



////////////////////////////////////////////////////////////////////////////////


// BoolVariable ////////////////////////////////////////////////////////////////

BoolVariable::BoolVariable()
: m_Value(0)
{
}
BoolVariable::BoolVariable(bool value)
: m_Value(value)
{
}

BoolVariable::~BoolVariable()
{
}

ICompiled * BoolVariable::Compile (ICompileArgs * args)
{
	if(!args || 0 != args->GetCount())
		return new Compiled(new Error());

	return new Compiled(dynamic_cast<IBool *>(this));
}

bool BoolVariable::Get() const
{
	return m_Value;
}

bool BoolVariable::EvalBool (void)
{
	return m_Value;
}

::Afx::IRef * BoolVariable::Ref (void)
{
	return dynamic_cast<::Afx::IRef *>(this);
}

void BoolVariable::Set(bool value)
{
	m_Value = value;
}


// Bubble //////////////////////////////////////////////////////////////////////

Bubble::Bubble()
{
	//
	// Add standard functions:

	Add("and", new FnAndCompileable());
	Add("&&", new FnAndCompileable());

	Add("or", new FnOrCompileable());
	Add("||", new FnOrCompileable());

	Add("not", new FnNotCompileable());
	Add("!", new FnNotCompileable());

	Add("less", new FnLessBoolCompileable());
	Add("<", new FnLessBoolCompileable());

	Add("less", new FnLessIntCompileable());
	Add("<", new FnLessIntCompileable());


}

Bubble::~Bubble()
{
	for(FunctionList::iterator it = m_Functions.begin(); it != m_Functions.end(); it++)
	{
		BubbleFn * bubbleFn = *it;

		delete bubbleFn;
	}
}


void Bubble::Add(char const * name, ICompileable * compileable)
{
	m_Functions.push_back(new BubbleFn(name, compileable));
}

ICompiled * Bubble::Compile(const char * code)
{
	return new Compiled(new Error());
}

::Afx::IRef * Bubble::Ref (void)
{
	return dynamic_cast<::Afx::IRef * >(this);
}


// BubbleFactory ///////////////////////////////////////////////////////////////

IBubble * BubbleFactory::StandardBubble (void)
{
	return new Bubble();
}


// Compiled ////////////////////////////////////////////////////////////////////

Compiled::Compiled(IBool * value)
{
	if(value) value->Ref()->AddRef();

	m_Type = ICompiled::T_Bool;
	m_Value.Bool = value;
}


Compiled::Compiled(IError * value)
{
	if(value) value->Ref()->AddRef();

	m_Type = ICompiled::T_Error;
	m_Value.Error = value;
}

Compiled::Compiled(IInt * value)
{
	if(value) value->Ref()->AddRef();

	m_Type = ICompiled::T_Int;
	m_Value.Int = value;
}


Compiled::Compiled(IVoid * value)
{
	if(value) value->Ref()->AddRef();

	m_Type = ICompiled::T_Void;
	m_Value.Void = value;
}

Compiled::~Compiled()
{
	switch(m_Type)
	{
	case ICompiled::T_Error: if(m_Value.Error) m_Value.Error->Ref()->Release(); break;
	case ICompiled::T_Void: if(m_Value.Void) m_Value.Void->Ref()->Release(); break;
	case ICompiled::T_Bool: if(m_Value.Bool) m_Value.Bool->Ref()->Release(); break;
	case ICompiled::T_Int: if(m_Value.Int) m_Value.Int->Ref()->Release(); break;
	}
}

IBool * Compiled::GetBool() { return ICompiled::T_Bool == m_Type ? m_Value.Bool : 0; }

IError * Compiled::GetError() { return ICompiled::T_Error == m_Type ? m_Value.Error : 0; }

IInt * Compiled::GetInt() { return ICompiled::T_Int == m_Type ? m_Value.Int : 0; }

enum ICompiled::Type Compiled::GetType() { return m_Type; }

IVoid * Compiled::GetVoid() { return ICompiled::T_Void == m_Type ? m_Value.Void : 0; }

::Afx::IRef * Compiled::Ref() { return dynamic_cast<::Afx::IRef *>(this); }


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


// IntVariable /////////////////////////////////////////////////////////////////


IntVariable::IntVariable()
: m_Value(0)
{
}

IntVariable::IntVariable(int value)
: m_Value(value)
{
}

IntVariable::~IntVariable()
{
}

ICompiled * IntVariable::Compile (ICompileArgs * args)
{
	if(!args || 0 != args->GetCount())
		return new Compiled(new Error());

	return new Compiled(dynamic_cast<IInt *>(this));
}

int IntVariable::Get() const
{
	return m_Value;
}

int IntVariable::EvalInt (void)
{
	return m_Value;
}

::Afx::IRef * IntVariable::Ref (void)
{
	return dynamic_cast<::Afx::IRef *>(this);
}

void IntVariable::Set(int value)
{
	m_Value = value;
}

