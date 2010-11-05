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

#include <list>

using namespace std;

using namespace Afx;
using namespace Afx::Expr;


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


struct S_ParseElem
{
	::CursorBackup CursorBackup;
	int ErrorCode;

	S_ParseElem(Cursor const & cur);

	/// <summary>If there is an error.</summary>
	bool HasError() const;

	/// <summary>Indicate an error.</summary>
	void SetError();
};

struct S_ArrayVals;

struct S_Array : public S_ParseElem
{
	enum Type_t {
		T_None,
		T_BoolArray,
		T_IntArray
	} Type;

	S_ArrayVals * ArrayVals;

	S_Array(Cursor & cur);

	~S_Array();
};


struct S_Constant;

struct S_ArrayVals : public S_ParseElem
{
	enum Type_t {
		T_None,
		T_Empty,
		T_Constant,
		T_ConstantMore
	} Type;

	S_Constant * Constant;
	S_ArrayVals * More;

	S_ArrayVals (Cursor & cur, bool isFirst);
	~S_ArrayVals();
};


struct S_Constant : public S_ParseElem
{
	enum {
		T_None,
		T_BoolConstant,
		T_IntConstant
	} Type;
	union {
		bool BoolConstant;
		int IntConstant;
	} Value;

	S_Constant(Cursor & cur);

	~S_Constant();
};


struct S_Parenthesis;
struct S_Function;
struct S_Variable;

struct S_Expression : public S_ParseElem
{
	enum {
		T_None,
		T_Parenthesis,
		T_Constant,
		T_Array,
		T_Function,
		T_Variable
	} Type;
	union {
		S_Parenthesis * Parenthesis;
		S_Constant * Constant;
		S_Array * Array;
		S_Function * Function;
		S_Variable * Variable;
	} Value;

	S_Expression(Cursor & cur);

	~S_Expression();
};


struct S_InfixOp2;
struct S_PrefixOp1;

struct S_Function : public S_ParseElem
{
	enum {
		T_None,
		T_InfixOp2,
		T_PrefixOp1
	} Type;
	union {
		S_InfixOp2 * InfixOp2;
		S_PrefixOp1 * PrefixOp1;
	} Value;

	S_Function(Cursor & cur);

	~S_Function();
};


struct S_InfixOp2 : public S_ParseElem
{
	enum Type_t {
		T_None,
		T_And,
		T_Equal,
		T_Greater,
		T_GreaterOrEqual,
		T_In,
		T_Less,
		T_LessOrEqual,
		T_Or
	} Type;

	union {
		struct {
			S_Expression * Left;
			S_Expression * Right;
		} Expressions;
	} Value;

	S_InfixOp2(Cursor & cur);

	~S_InfixOp2();
};


struct S_Input : public S_ParseElem
{
	enum {
		T_None,
		T_Expression
	} Type;
	union {
		S_Expression * Expression;
	} Value;

	S_Input(Cursor & cur);

	~S_Input();
};


struct S_Parenthesis : public S_ParseElem
{
	enum {
		T_None,
		T_Expression
	} Type;
	union {
		S_Expression * Expression;
	} Value;

	S_Parenthesis(Cursor & cur);

	~S_Parenthesis();
};


struct S_PrefixOp1 : public S_ParseElem
{
	enum {
		T_None,
		T_Not
	} Type;

	union {
		S_Expression * Expression;
	} Value;

	S_PrefixOp1(Cursor & cur);

	~S_PrefixOp1();
};


struct S_Variable : public S_ParseElem
{
	enum {
		T_None,
		T_Name
	} Type;

	union {
		char * Name;
	} Value;

	S_Variable(Cursor & cur);

	~S_Variable();
};


////////////////////////////////////////////////////////////////////////////////

enum CompiledExType {
	CTE_Error = 0,
	CTE_BoolFn,
	CTE_IntFn,
	CTE_BoolArray,
	CTE_IntArray
};

struct __declspec(novtable) IBoolArray abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual int GetCount (void) abstract = 0;
	virtual bool const * GetValues (void) abstract = 0;
};

class BoolArray : Ref,
	public IBoolArray
{
public:
	BoolArray(int count, bool const * values)
	{
		m_Count = count;
		m_Values = new bool[count];

		for(int i=0; i<count; i++) m_Values[i] = values[i];
	}

	virtual int GetCount (void)
	{
		return m_Count;
	}

	virtual bool const * GetValues (void)
	{
		return m_Values;
	}

	virtual ::Afx::IRef * Ref (void) { return dynamic_cast<::Afx::IRef *>(this); }

protected:
	virtual ~BoolArray()
	{
		delete m_Values;
	}

private:
	int m_Count;
	bool * m_Values;

};

struct __declspec(novtable) IIntArray abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual int GetCount (void) abstract = 0;
	virtual int const * GetValues (void) abstract = 0;
};


class IntArray : Ref,
	public IIntArray
{
public:
	IntArray(int count, int const * values)
	{
		m_Count = count;
		m_Values = new int[count];

		for(int i=0; i<count; i++) m_Values[i] = values[i];
	}

	virtual int GetCount (void)
	{
		return m_Count;
	}

	virtual int const * GetValues (void)
	{
		return m_Values;
	}

	virtual ::Afx::IRef * Ref (void) { return dynamic_cast<::Afx::IRef *>(this); }

protected:
	virtual ~IntArray()
	{
		delete m_Values;
	}

private:
	int m_Count;
	int * m_Values;

};

struct __declspec(novtable) ICompiledEx abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual CompiledExType GetExType (void) abstract = 0;

	virtual IBoolArray * GetBoolArray (void) abstract = 0;
	virtual IBoolFn * GetBoolFn (void) abstract = 0;
	virtual IError * GetError (void) abstract = 0;
	virtual IIntArray * GetIntArray (void) abstract = 0;
	virtual IIntFn * GetIntFn (void) abstract = 0;
};

class BubbleFn;

class Bubble : public Ref,
	public IBubble
{
public:
	Bubble();

	virtual bool Add(IBoolFn * fn, char const * name);

	virtual bool Add(IIntFn * fn, char const * name);

	virtual ICompiled * Compile(char const * code);

	virtual ::Afx::IRef * Ref (void);

protected:
	~Bubble();

private:
	typedef list<BubbleFn *> FunctionList;

	FunctionList m_Functions;

	ICompiledEx * Compile_Array(S_Array & s);

	ICompiledEx * Compile_Constant(S_Constant & s);

	ICompiledEx * Compile_Expression(S_Expression & s);
	
	ICompiledEx * Compile_Function(S_Function & s);
	
	ICompiledEx * Compile_InfixOp2(S_InfixOp2 & s);

	ICompiledEx * Compile_Input(S_Input & s);
	
	ICompiledEx * Compile_Parenthesis(S_Parenthesis & s);
	
	ICompiledEx * Compile_PrefixOp1(S_PrefixOp1 & s);

	ICompiledEx * Compile_Variable(S_Variable & s);

	/// <summary>Searchs the list of named functions.</summary>
	/// <result>0 if not found, otherwise the first list entry found</result>
	BubbleFn * FindNamedFunction(char const * name);
};


class BubbleFn : public Ref
{
public:
	enum Type_t
	{
		T_None,
		T_Bool,
		T_Int
	};

	BubbleFn(IBoolFn * boolFn, char const * name) : m_Type(T_Bool) {
		if(boolFn) boolFn->Ref()->AddRef();
		m_Value.BoolFn = boolFn;

		MakeName(name);
	}

	BubbleFn(IIntFn * intFn, char const * name) : m_Type(T_Int) {
		if(intFn) intFn->Ref()->AddRef();
		m_Value.IntFn = intFn;

		MakeName(name);
	}

	char const * GetName (void) {
		return m_Name;
	}

	Type_t GetType (void) {
		return m_Type;
	}

	IBoolFn * GetBoolFn (void) {
		return T_Bool == m_Type ? m_Value.BoolFn : 0;
	}

	IIntFn * GetIntFn (void) {
		return T_Int == m_Type ? m_Value.IntFn : 0;
	}

protected:
	~BubbleFn()
	{
		switch(m_Type)
		{
		case T_Bool: m_Value.BoolFn->Ref()->Release(); break;
		case T_Int: m_Value.IntFn->Ref()->Release(); break;
		}
	}

private:
	char * m_Name;
	Type_t m_Type;
	union Value_t {
		IBoolFn * BoolFn;
		IIntFn * IntFn;
	} m_Value;

	void MakeName(char const * name)
	{
		if(!name) {
			m_Name = 0;
			return;
		}

		int lenName = strlen(name);
		m_Name = new char[1+lenName];
		memcpy(m_Name, name, (1+lenName)*sizeof(char));
		m_Name[lenName] = 0;
	}
};


class Error;

class Compiled : Ref,
	public ICompiled
{
public:
	Compiled(ICompiledEx * compiled);

	Compiled(IBoolFn * boolFn);

	Compiled(IError * error);

	Compiled(IIntFn * intFn);

	virtual IBoolFn * GetBoolFn();
	
	virtual IError * GetError();
	
	virtual IIntFn * GetIntFn();
	
	virtual enum CompiledType GetType();

	virtual ::Afx::IRef * Ref();

protected:
	virtual ~Compiled();

private:
	enum CompiledType m_Type;
	union {
		IBoolFn * BoolFn;
		IError * Error;
		IIntFn * IntFn;
	} m_Value;
};


class CompiledEx : Ref,
	public ICompiledEx
{
public:
	CompiledEx(ICompiledEx * compiled);

	CompiledEx(IBoolArray * boolArray);

	CompiledEx(IBoolFn * boolFn);

	CompiledEx(IError * error);

	CompiledEx(IIntArray * IntArray);

	CompiledEx(IIntFn * intFn);

	virtual IBoolArray * GetBoolArray();

	virtual IBoolFn * GetBoolFn();
	
	virtual IError * GetError();
	
	virtual IIntArray * GetIntArray();

	virtual IIntFn * GetIntFn();
	
	virtual enum CompiledExType GetExType();

	virtual ::Afx::IRef * Ref();

protected:
	virtual ~CompiledEx();

private:
	enum CompiledExType m_Type;
	union {
		IBoolArray * BoolArray;
		IBoolFn * BoolFn;
		IError * Error;
		IIntArray * IntArray;
		IIntFn * IntFn;
	} m_Value;
};



class Error : Ref,
	public IError
{
public:
	Error() {}

	virtual char const * GetErrorString (void)
	{
		return "Error";
	}

	virtual ::Afx::IRef * Ref (void) { return dynamic_cast<::Afx::IRef *>(this); }

protected:


private:


};


class FnAnd : Ref,
	public IBoolFn
{
public:
	FnAnd(IBoolFn * fn1, IBoolFn * fn2) {
		fn1->Ref()->AddRef();
		fn2->Ref()->AddRef();

		m_fn1 = fn1;
		m_fn2 = fn2;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) { return m_fn1->EvalBool() && m_fn2->EvalBool(); }

protected:
	virtual ~FnAnd() {
		m_fn1->Ref()->Release();
		m_fn2->Ref()->Release();
	}

private:
	IBoolFn * m_fn1;
	IBoolFn * m_fn2;
};


class FnConstBool : Ref,
	public IBoolFn
{
public:
	FnConstBool(bool val) {
		m_val = val;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) { return m_val; }

protected:
	virtual ~FnConstBool() {
	}

private:
	bool m_val;
};


class FnConstInt : Ref,
	public IIntFn
{
public:
	FnConstInt(int val) {
		m_val = val;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual int EvalInt (void) { return m_val; }

protected:
	virtual ~FnConstInt() {
	}

private:
	int m_val;
};


class FnEqualBool : Ref,
	public IBoolFn
{
public:
	FnEqualBool(IBoolFn * fn1, IBoolFn * fn2) {
		fn1->Ref()->AddRef();
		fn2->Ref()->AddRef();

		m_fn1 = fn1;
		m_fn2 = fn2;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) { return m_fn1->EvalBool() == m_fn2->EvalBool(); }

protected:
	virtual ~FnEqualBool() {
		m_fn1->Ref()->Release();
		m_fn2->Ref()->Release();
	}

private:
	IBoolFn * m_fn1;
	IBoolFn * m_fn2;
};


class FnEqualInt : Ref,
	public IBoolFn
{
public:
	FnEqualInt(IIntFn * fn1, IIntFn * fn2) {
		fn1->Ref()->AddRef();
		fn2->Ref()->AddRef();

		m_fn1 = fn1;
		m_fn2 = fn2;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) { return m_fn1->EvalInt() == m_fn2->EvalInt(); }

protected:
	virtual ~FnEqualInt() {
		m_fn1->Ref()->Release();
		m_fn2->Ref()->Release();
	}

private:
	IIntFn * m_fn1;
	IIntFn * m_fn2;
};


class FnGreater : Ref,
	public IBoolFn
{
public:
	FnGreater(IIntFn * fn1, IIntFn * fn2) {
		fn1->Ref()->AddRef();
		fn2->Ref()->AddRef();

		m_fn1 = fn1;
		m_fn2 = fn2;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) { return m_fn1->EvalInt() > m_fn2->EvalInt(); }

protected:
	virtual ~FnGreater() {
		m_fn1->Ref()->Release();
		m_fn2->Ref()->Release();
	}

private:
	IIntFn * m_fn1;
	IIntFn * m_fn2;
};


class FnGreaterOrEqual : Ref,
	public IBoolFn
{
public:
	FnGreaterOrEqual(IIntFn * fn1, IIntFn * fn2) {
		fn1->Ref()->AddRef();
		fn2->Ref()->AddRef();

		m_fn1 = fn1;
		m_fn2 = fn2;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) { return m_fn1->EvalInt() >= m_fn2->EvalInt(); }

protected:
	virtual ~FnGreaterOrEqual() {
		m_fn1->Ref()->Release();
		m_fn2->Ref()->Release();
	}

private:
	IIntFn * m_fn1;
	IIntFn * m_fn2;
};


class FnInConstBoolSet : Ref,
	public IBoolFn
{
public:
	FnInConstBoolSet(IBoolFn * fn1, int valCount, bool const * vals) {
		fn1->Ref()->AddRef();

		m_fn1 = fn1;
		m_valCount = valCount;
		m_vals = new bool[valCount];

		for(int i=0; i<valCount; m_vals[i++] = *(vals++));
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) {
		bool val = m_fn1->EvalBool();

		for(int i=0; i<m_valCount; i++)
		{
			if(val == m_vals[i]) return true;
		}

		return false;
	}

protected:
	virtual ~FnInConstBoolSet() {
		m_fn1->Ref()->Release();

		delete m_vals;
	}

private:
	IBoolFn * m_fn1;
	int m_valCount;
	bool * m_vals;
};


class FnInConstIntSet : Ref,
	public IBoolFn
{
public:
	FnInConstIntSet(IIntFn * fn1, int valCount, int const * vals) {
		fn1->Ref()->AddRef();

		m_fn1 = fn1;
		m_valCount = valCount;
		m_vals = new int[valCount];

		for(int i=0; i<valCount; m_vals[i++] = *(vals++));
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) {
		int val = m_fn1->EvalInt();

		for(int i=0; i<m_valCount; i++)
		{
			if(val == m_vals[i]) return true;
		}

		return false;
	}

protected:
	virtual ~FnInConstIntSet() {
		m_fn1->Ref()->Release();

		delete m_vals;
	}

private:
	IIntFn * m_fn1;
	int m_valCount;
	int * m_vals;
};


class FnLess : Ref,
	public IBoolFn
{
public:
	FnLess(IIntFn * fn1, IIntFn * fn2) {
		fn1->Ref()->AddRef();
		fn2->Ref()->AddRef();

		m_fn1 = fn1;
		m_fn2 = fn2;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) { return m_fn1->EvalInt() < m_fn2->EvalInt(); }

protected:
	virtual ~FnLess() {
		m_fn1->Ref()->Release();
		m_fn2->Ref()->Release();
	}

private:
	IIntFn * m_fn1;
	IIntFn * m_fn2;
};


class FnLessOrEqual : Ref,
	public IBoolFn
{
public:
	FnLessOrEqual(IIntFn * fn1, IIntFn * fn2) {
		fn1->Ref()->AddRef();
		fn2->Ref()->AddRef();

		m_fn1 = fn1;
		m_fn2 = fn2;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) { return m_fn1->EvalInt() <= m_fn2->EvalInt(); }

protected:
	virtual ~FnLessOrEqual() {
		m_fn1->Ref()->Release();
		m_fn2->Ref()->Release();
	}

private:
	IIntFn * m_fn1;
	IIntFn * m_fn2;
};


class FnNot : Ref,
	public IBoolFn
{
public:
	FnNot(IBoolFn * fn1) {
		fn1->Ref()->AddRef();

		m_fn1 = fn1;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) { return !m_fn1->EvalBool(); }

protected:
	virtual ~FnNot() {
		m_fn1->Ref()->Release();
	}

private:
	IBoolFn * m_fn1;
};


class FnOr : Ref,
	public IBoolFn
{
public:
	FnOr(IBoolFn * fn1, IBoolFn * fn2) {
		fn1->Ref()->AddRef();
		fn2->Ref()->AddRef();

		m_fn1 = fn1;
		m_fn2 = fn2;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) { return m_fn1->EvalBool() || m_fn2->EvalBool(); }

protected:
	virtual ~FnOr() {
		m_fn1->Ref()->Release();
		m_fn2->Ref()->Release();
	}

private:
	IBoolFn * m_fn1;
	IBoolFn * m_fn2;
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
}

Bubble::~Bubble()
{
	for(FunctionList::iterator it = m_Functions.begin(); it != m_Functions.end(); it++)
	{
		(*it)->Release();
	}
}

bool Bubble::Add(IBoolFn * fn, char const * name)
{
	BubbleFn * bFn = new BubbleFn(fn, name);
	bFn->AddRef();

	m_Functions.push_back(bFn);

	return true;
}

bool Bubble::Add(IIntFn * fn, char const * name)
{
	BubbleFn * bFn = new BubbleFn(fn, name);
	bFn->AddRef();

	m_Functions.push_back(bFn);

	return true;
}


ICompiled * Bubble::Compile(const char * code)
{
	Cursor cursor(code);
	S_Input input(cursor);
	
	return new Compiled(Compile_Input(input));
}


ICompiledEx * Bubble::Compile_Constant(S_Constant & s)
{
	if(!s.HasError())
	{
		switch(s.Type)
		{
		case S_Constant::T_BoolConstant:
			return new CompiledEx(FnFactory::Const(s.Value.BoolConstant));
		case S_Constant::T_IntConstant:
			return new CompiledEx(FnFactory::Const(s.Value.IntConstant));
		}
	}

	return new CompiledEx(new Error());
}

ICompiledEx * Bubble::Compile_Array(S_Array & s)
{
	bool error = s.HasError();
	int count = 0;

	if(!error
		&& (s.T_BoolArray == s.Type || s.T_IntArray == s.Type)
	) {
		S_Array::Type_t arrayType = s.Type;

		for(S_ArrayVals * cur = s.ArrayVals; ; cur = cur->More)
		{
			error = cur->HasError();
			if(error) break;

			if(S_ArrayVals::T_Empty == cur->Type) break; // done.

			error = !(S_ArrayVals::T_ConstantMore == cur->Type)
				|| cur->Constant->HasError()
				|| S_Array::T_BoolArray == arrayType && S_Constant::T_BoolConstant != cur->Constant->Type
				|| S_Array::T_IntArray == arrayType && S_Constant::T_IntConstant != cur->Constant->Type
			;
			if(error) break;

			count++;
		}
	}
	else error = true;

	if(!error)
	{
		S_Array::Type_t arrayType = s.Type;
		bool * bools = S_Array::T_BoolArray == arrayType ? new bool[count] : 0;
		int * ints = S_Array::T_IntArray == arrayType ? new int[count] : 0;

		int idx = 0;

		for(S_ArrayVals * cur = s.ArrayVals; S_ArrayVals::T_Empty != cur->Type; cur = cur->More)
		{
			if(S_Array::T_BoolArray == arrayType)
				bools[idx++] = cur->Constant->Value.BoolConstant;
			else
				ints[idx++] = cur->Constant->Value.IntConstant;
		}

		ICompiledEx * compiled = S_Array::T_BoolArray == arrayType
			? new CompiledEx(new BoolArray(count, bools))
			: new CompiledEx(new IntArray(count, ints))
		;

		return compiled;
	}

	return new CompiledEx(new Error());
}

ICompiledEx * Bubble::Compile_Expression(S_Expression & s)
{
	if(!s.HasError())
	{
		switch(s.Type)
		{
		case S_Expression::T_Parenthesis:
			return Compile_Parenthesis(*s.Value.Parenthesis);
		case S_Expression::T_Constant:
			return Compile_Constant(*s.Value.Constant);
		case S_Expression::T_Array:
			return Compile_Array(*s.Value.Array);
		case S_Expression::T_Function:
			return Compile_Function(*s.Value.Function);
		case S_Expression::T_Variable:
			return Compile_Variable(*s.Value.Variable);
		}
	}

	return new CompiledEx(new Error());
}

ICompiledEx * Bubble::Compile_Function(S_Function & s)
{
	if(!s.HasError())
	{
		switch(s.Type)
		{
		case S_Function::T_InfixOp2:
			return Compile_InfixOp2(*s.Value.InfixOp2);
		case S_Function::T_PrefixOp1:
			return Compile_PrefixOp1(*s.Value.PrefixOp1);
		}
	}

	return new CompiledEx(new Error());
}


ICompiledEx * Bubble::Compile_InfixOp2(S_InfixOp2 & s)
{
	if(!s.HasError())
	{
		ICompiledEx * exL = Compile_Expression(*s.Value.Expressions.Left);
		ICompiledEx * exR = Compile_Expression(*s.Value.Expressions.Right);
		ICompiledEx * compiled = 0;

		exL->Ref()->AddRef();
		exR->Ref()->AddRef();

		switch(s.Type)
		{
		case S_InfixOp2::T_And:
			if(0 != exL->GetBoolFn() && 0 != exR->GetBoolFn())
				compiled = new CompiledEx(FnFactory::And(exL->GetBoolFn(), exR->GetBoolFn()));
			break;
		case S_InfixOp2::T_Equal:
			if(0 != exL->GetBoolFn() && 0 != exR->GetBoolFn())
				compiled = new CompiledEx(FnFactory::Equal(exL->GetBoolFn(), exR->GetBoolFn()));
			else if(0 != exL->GetIntFn() && 0 != exR->GetIntFn())
				compiled = new CompiledEx(FnFactory::Equal(exL->GetIntFn(), exR->GetIntFn()));
			break;
		case S_InfixOp2::T_Greater:
			if(0 != exL->GetIntFn() && 0 != exR->GetIntFn())
				compiled = new CompiledEx(FnFactory::Greater(exL->GetIntFn(), exR->GetIntFn()));
			break;
		case S_InfixOp2::T_GreaterOrEqual:
			if(0 != exL->GetIntFn() && 0 != exR->GetIntFn())
				compiled = new CompiledEx(FnFactory::GreaterOrEqual(exL->GetIntFn(), exR->GetIntFn()));
			break;
		case S_InfixOp2::T_In:
			if(0 != exL->GetBoolFn() && 0 != exR->GetBoolArray())
			{
				IBoolArray * aR = exR->GetBoolArray();
				compiled = new CompiledEx(FnFactory::InConstSet(exL->GetBoolFn(), aR->GetCount(), aR->GetValues()));
			}
			else if(0 != exL->GetIntFn() && 0 != exR->GetIntArray())
			{
				IIntArray * aR = exR->GetIntArray();
				compiled = new CompiledEx(FnFactory::InConstSet(exL->GetIntFn(), aR->GetCount(), aR->GetValues()));
			}
			break;
		case S_InfixOp2::T_Less:
			if(0 != exL->GetIntFn() && 0 != exR->GetIntFn())
				compiled = new CompiledEx(FnFactory::Less(exL->GetIntFn(), exR->GetIntFn()));
			break;
		case S_InfixOp2::T_LessOrEqual:
			if(0 != exL->GetIntFn() && 0 != exR->GetIntFn())
				compiled = new CompiledEx(FnFactory::Less(exL->GetIntFn(), exR->GetIntFn()));
			break;
		case S_InfixOp2::T_Or:
			if(0 != exL->GetBoolFn() && 0 != exR->GetBoolFn())
				compiled = new CompiledEx(FnFactory::Or(exL->GetBoolFn(), exR->GetBoolFn()));
			break;
		}

		exR->Ref()->Release();
		exL->Ref()->Release();

		if(0 != compiled)
			return compiled;
	}

	return new CompiledEx(new Error());
}


ICompiledEx * Bubble::Compile_Input(S_Input & s)
{
	if(!s.HasError() && S_Input::T_Expression == s.Type)
		return Compile_Expression(*s.Value.Expression);

	return new CompiledEx(new Error());
}


ICompiledEx * Bubble::Compile_Parenthesis(S_Parenthesis & s)
{
	if(!s.HasError())
	{
		switch(s.Type)
		{
		case S_Parenthesis::T_Expression:
			return Compile_Expression(*s.Value.Expression);
		}
	}

	return new CompiledEx(new Error());
}

ICompiledEx * Bubble::Compile_PrefixOp1(S_PrefixOp1 & s)
{
	if(!s.HasError() && s.T_None != s.Type)
	{
		ICompiledEx * expr  = Compile_Expression(*s.Value.Expression);
		ICompiledEx * compiled = 0;

		expr->Ref()->AddRef();

		switch(s.Type)
		{
		case s.T_Not:
			if(0 != expr->GetBoolFn())
				compiled = new CompiledEx(FnFactory::Not(expr->GetBoolFn()));
			break;
		}

		expr->Ref()->Release();

		if(0 != compiled)
			return compiled;
	}

	return new CompiledEx(new Error());
}


ICompiledEx * Bubble::Compile_Variable(S_Variable & s)
{
	if(!s.HasError() && s.T_Name == s.Type)
	{
		BubbleFn * bFn = FindNamedFunction(s.Value.Name);

		bFn->AddRef();

		ICompiledEx * compiled = 0;

		if(0 != bFn)
		{
			if(BubbleFn::T_Bool == bFn->GetType())
				compiled = new CompiledEx(bFn->GetBoolFn());
			else if(BubbleFn::T_Int == bFn->GetType())
				compiled = new CompiledEx(bFn->GetIntFn());
		}

		bFn->Release();

		if(0 != compiled)
			return compiled;
	}

	return new CompiledEx(new Error());
}


BubbleFn * Bubble::FindNamedFunction(char const * name)
{
	for(FunctionList::iterator it = m_Functions.begin(); it != m_Functions.end(); it++)
	{
		if(0 == strcmp(name, (*it)->GetName()))
		{
			return (*it);
		}
	}

	return 0;
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

Compiled::Compiled(ICompiledEx * compiled)
{
	compiled->Ref()->AddRef();

	switch(compiled->GetExType())
	{
	case CTE_BoolFn:
		m_Type = CT_BoolFn;
		m_Value.BoolFn = compiled->GetBoolFn();
		if(m_Value.BoolFn) m_Value.BoolFn->Ref()->AddRef();
		break;
	case CTE_Error:
		m_Type = CT_Error;
		m_Value.Error = compiled->GetError();
		if(m_Value.Error) m_Value.Error->Ref()->AddRef();
		break;
	case CTE_IntFn:
		m_Type = CT_IntFn;
		m_Value.IntFn = compiled->GetIntFn();
		if(m_Value.IntFn) m_Value.IntFn->Ref()->AddRef();
		break;
	default:
		m_Type = CT_Error;
		m_Value.Error = new Error();
		m_Value.Error->Ref()->AddRef();
	}

	compiled->Ref()->Release();
}

Compiled::Compiled(IBoolFn * boolFn)
{
	if(boolFn) boolFn->Ref()->AddRef();

	m_Type = CT_BoolFn;
	m_Value.BoolFn = boolFn;
}

Compiled::Compiled(IError * error)
{
	if(error) error->Ref()->AddRef();

	m_Type = CT_Error;
	m_Value.Error = error;
}

Compiled::Compiled(IIntFn * intFn)
{
	if(intFn) intFn->Ref()->AddRef();

	m_Type = CT_IntFn;
	m_Value.IntFn = intFn;
}


Compiled::~Compiled()
{
	if(CT_BoolFn == m_Type && m_Value.BoolFn) m_Value.BoolFn->Ref()->Release();
	if(CT_Error == m_Type && m_Value.Error) m_Value.Error->Ref()->Release();
	if(CT_IntFn == m_Type && m_Value.IntFn) m_Value.IntFn->Ref()->Release();
}


IBoolFn * Compiled::GetBoolFn() { return CT_BoolFn == m_Type ? m_Value.BoolFn : 0; }

IError * Compiled::GetError() { return CT_Error == m_Type ? m_Value.Error : 0; }

IIntFn * Compiled::GetIntFn() { return CT_IntFn == m_Type ? m_Value.IntFn : 0; }

enum CompiledType Compiled::GetType() { return m_Type; }

::Afx::IRef * Compiled::Ref() { return dynamic_cast<::Afx::IRef *>(this); }



// CompiledEx ////////////////////////////////////////////////////////////////////

CompiledEx::CompiledEx(ICompiledEx * compiled)
{
	compiled->Ref()->AddRef();

	switch(compiled->GetExType())
	{
	case CTE_BoolArray:
		m_Type = CTE_BoolArray;
		m_Value.BoolArray = compiled->GetBoolArray();
		if(m_Value.BoolArray) m_Value.BoolArray->Ref()->AddRef();
		break;
	case CTE_BoolFn:
		m_Type = CTE_BoolFn;
		m_Value.BoolFn = compiled->GetBoolFn();
		if(m_Value.BoolFn) m_Value.BoolFn->Ref()->AddRef();
		break;
	case CTE_Error:
		m_Type = CTE_Error;
		m_Value.Error = compiled->GetError();
		if(m_Value.Error) m_Value.Error->Ref()->AddRef();
		break;
	case CTE_IntArray:
		m_Type = CTE_IntArray;
		m_Value.IntArray = compiled->GetIntArray();
		if(m_Value.IntArray) m_Value.IntArray->Ref()->AddRef();
		break;
	case CTE_IntFn:
		m_Type = CTE_IntFn;
		m_Value.IntFn = compiled->GetIntFn();
		if(m_Value.IntFn) m_Value.IntFn->Ref()->AddRef();
		break;
	default:
		m_Type = CTE_Error;
		m_Value.Error = new Error();
		m_Value.Error->Ref()->AddRef();
	}

	compiled->Ref()->Release();
}

CompiledEx::CompiledEx(IBoolArray * boolArray)
{
	if(boolArray) boolArray->Ref()->AddRef();

	m_Type = CTE_BoolArray;
	m_Value.BoolArray = boolArray;
}

CompiledEx::CompiledEx(IBoolFn * boolFn)
{
	if(boolFn) boolFn->Ref()->AddRef();

	m_Type = CTE_BoolFn;
	m_Value.BoolFn = boolFn;
}

CompiledEx::CompiledEx(IError * error)
{
	if(error) error->Ref()->AddRef();

	m_Type = CTE_Error;
	m_Value.Error = error;
}


CompiledEx::CompiledEx(IIntArray * intArray)
{
	if(intArray) intArray->Ref()->AddRef();

	m_Type = CTE_IntArray;
	m_Value.IntArray = intArray;
}


CompiledEx::CompiledEx(IIntFn * intFn)
{
	if(intFn) intFn->Ref()->AddRef();

	m_Type = CTE_IntFn;
	m_Value.IntFn = intFn;
}


CompiledEx::~CompiledEx()
{
	if(CTE_BoolArray == m_Type && m_Value.BoolArray) m_Value.BoolArray->Ref()->Release();
	if(CTE_BoolFn == m_Type && m_Value.BoolFn) m_Value.BoolFn->Ref()->Release();
	if(CTE_Error == m_Type && m_Value.Error) m_Value.Error->Ref()->Release();
	if(CTE_IntArray == m_Type && m_Value.IntArray) m_Value.IntArray->Ref()->Release();
	if(CTE_IntFn == m_Type && m_Value.IntFn) m_Value.IntFn->Ref()->Release();
}

IBoolArray * CompiledEx::GetBoolArray() { return CTE_BoolArray == m_Type ? m_Value.BoolArray : 0; }

IBoolFn * CompiledEx::GetBoolFn() { return CTE_BoolFn == m_Type ? m_Value.BoolFn : 0; }

IError * CompiledEx::GetError() { return CTE_Error == m_Type ? m_Value.Error : 0; }

IIntArray * CompiledEx::GetIntArray() { return CTE_IntArray == m_Type ? m_Value.IntArray : 0; }

IIntFn * CompiledEx::GetIntFn() { return CTE_IntFn == m_Type ? m_Value.IntFn : 0; }

enum CompiledExType CompiledEx::GetExType() { return m_Type; }

::Afx::IRef * CompiledEx::Ref() { return dynamic_cast<::Afx::IRef *>(this); }




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


// FnFactory ///////////////////////////////////////////////////////////////////

IBoolFn * FnFactory::And(IBoolFn * fn1, IBoolFn * fn2)
{
	return new FnAnd(fn1, fn2);
}

IBoolFn * FnFactory::Const(bool val)
{
	return new FnConstBool(val);
}

IIntFn  * FnFactory::Const(int val)
{
	return new FnConstInt(val);
}

IBoolFn * FnFactory::Equal (IBoolFn * fn1, IBoolFn * fn2)
{
	return new FnEqualBool(fn1 ,fn2);
}

IBoolFn * FnFactory::Equal (IIntFn * fn1, IIntFn * fn2)
{
	return new FnEqualInt(fn1, fn2);
}

IBoolFn * FnFactory::Greater (IIntFn * fn1, IIntFn * fn2)
{
	return new FnGreater(fn1, fn2);
}

IBoolFn * FnFactory::GreaterOrEqual (IIntFn * fn1, IIntFn * fn2)
{
	return new FnGreaterOrEqual(fn1, fn2);
}

IBoolFn * FnFactory::InConstSet (IBoolFn * fn1, int valCount, bool const * vals)
{
	return new FnInConstBoolSet(fn1, valCount, vals);
}

IBoolFn * FnFactory::InConstSet (IIntFn * fn1, int valCount, int const * vals)
{
	return new FnInConstIntSet(fn1, valCount, vals);
}

IBoolFn * FnFactory::Less (IIntFn * fn1, IIntFn * fn2)
{
	return new FnLess(fn1, fn2);
}

IBoolFn * FnFactory::Not(IBoolFn * fn1)
{
	return new FnNot(fn1);
}

IBoolFn * FnFactory::Or(IBoolFn * fn1, IBoolFn * fn2)
{
	return new FnOr(fn1, fn2);
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




// S_Array /////////////////////////////////////////////////////////////////////

S_Array::S_Array(Cursor & cur) :
S_ParseElem(cur), Type(T_None)
{
	char const * textNew = "new";
	int lenTextNew = strlen(textNew);
	bool matches = true;
	bool isBool = true;
	bool isInt = true;

	for(int i=0; i<lenTextNew && (matches = matches && textNew[i] == cur.Get()); i++)
	{
		cur.Add();
	}

	matches =
		matches && cur.IsSpace();

	if(matches)
	{
		cur.Add();
		cur.SkipSpace();

		char const * textBool = "bool";
		char const * textInt = "int";
		int lenTextBool = strlen(textBool);
		int lenTextInt = strlen(textInt);

		for(int i=0; i<lenTextBool && (isBool = isBool && textBool[i] == cur.Get(i)); i++);
		for(int i=0; i<lenTextInt && (isInt = isInt && textInt[i] == cur.Get(i)); i++);

		if(isBool)
		{
			cur.Seek(lenTextBool);
		}
		else if(isInt)
		{
			cur.Seek(lenTextInt);
		}
		else
			matches = false;
	}

	matches = matches
		&& (cur.SkipSpace(), '[' == cur.Get())
		&& (cur.Add(), cur.SkipSpace(), ']' == cur.Get())
		&& (cur.Add(), cur.SkipSpace(), '{' == cur.Get())
	;


	if(matches)
	{
		cur.Add();
		cur.SkipSpace();

		Type = isInt ? T_IntArray : T_BoolArray;

		ArrayVals = new S_ArrayVals(cur, true);

		matches = !ArrayVals->HasError()
			&& (cur.SkipSpace(), '}' == cur.Get())
		;
		
		if(matches) cur.Add();
	}

	if(!matches)
		SetError();
}

S_Array::~S_Array()
{
	switch(Type)
	{
	case T_BoolArray:
	case T_IntArray:
		delete ArrayVals;
		break;
	}
}


// S_ArrayVals /////////////////////////////////////////////////////////////////

S_ArrayVals::S_ArrayVals (Cursor & cur, bool isFirst)
	: S_ParseElem(cur), Type(T_None)
{
	if(!isFirst)
	{
		cur.SkipSpace();
		if(',' == cur.Get())
		{
			cur.Add();
			cur.SkipSpace();
		}
		else
		{
			Type = T_Empty;
		}
	}

	if(T_Empty != Type)
	{
		Type = T_Constant;
		Constant = new S_Constant(cur);

		if(Constant->HasError())
		{
			if(isFirst)
			{
				delete Constant;
				Type = T_Empty;
			}
			else
				SetError();
		}
		else
		{
			Type = T_ConstantMore;
			More = new S_ArrayVals(cur, false);
			if(More->HasError()) SetError();
		}
	}

	if(T_Empty == Type) cur.Restore(CursorBackup);
}

S_ArrayVals::~S_ArrayVals()
{
	switch(Type)
	{
	case T_ConstantMore:
		delete More;
	case T_Constant:
		delete Constant;
		break;
	}
}


// S_Constant //////////////////////////////////////////////////////////////////

S_Constant::S_Constant(Cursor & cur)
: S_ParseElem(cur), Type(T_None)
{
	int count = 0;

	if('-' == cur.Get())
	{
		count++;
		cur.Add();
	}
	count += cur.SkipAlnum();
	cur.Restore(CursorBackup);

	if(T_None == Type && 0 < count)
	{
		// false?

		char const * textFalse = "false";
		int lenFalse = strlen(textFalse);
		bool isFalse = lenFalse == count;
		for(int i=0; i<lenFalse && isFalse; i++) isFalse = isFalse && textFalse[i] == cur.Get(i);
		if(isFalse)
		{
			Type = T_BoolConstant;
			Value.BoolConstant = false;

			cur.Seek(lenFalse);
		}
	}
	if(T_None == Type && 0 < count)
	{
		// true?

		char const * textTrue = "true";
		int lenTrue = strlen(textTrue);
		bool isTrue = lenTrue == count;
		for(int i=0; i<lenTrue && isTrue; i++) isTrue = isTrue && textTrue[i] == cur.Get(i);
		if(isTrue)
		{
			Type = T_BoolConstant;
			Value.BoolConstant = true;

			cur.Seek(lenTrue);
		}
	}
	if(T_None == Type && 0 < count)
	{
		// int?
		
		char * text = new char[1+count];
		for(int i=0; i<count; i++) { text[i] = cur.Get(i); }
		text[count] = 0;

		int ival = atoi(text);

		bool isInt = true;

		if(0 == ival)
		{
			// we need to distinguish if it's not an int or if it's zero (0):
			// (-0 is not allowed for now)
			for(int i = 0; i<count && isInt; i++)
			{
				isInt = isInt && '0' == text[i];
			}
		}

		if(isInt)
		{
			Type = T_IntConstant;
			Value.IntConstant = ival;

			cur.Seek(count);
		}


		delete text;

	}
	if(T_None == Type)
	{
		SetError();

	}
}

S_Constant::~S_Constant()
{
}


// S_Expression ////////////////////////////////////////////////////////////////

S_Expression::S_Expression(Cursor & cur)
: S_ParseElem(cur), Type(T_None)
{
	if(T_None == Type)
	{
		// Parenthesis?
		Type = T_Parenthesis;
		Value.Parenthesis = new S_Parenthesis(cur);
		if(Value.Parenthesis->HasError())
		{
			delete Value.Parenthesis;
			Type = T_None;
			cur.Restore(CursorBackup);
		}
	}
	if(T_None == Type)
	{
		// Constant?
		Type = T_Constant;
		Value.Constant = new S_Constant(cur);
		if(Value.Constant->HasError())
		{
			delete Value.Constant;
			Type = T_None;
			cur.Restore(CursorBackup);
		}
	}
	if(T_None == Type)
	{
		// Array?
		Type = T_Array;
		Value.Array = new S_Array(cur);
		if(Value.Array->HasError())
		{
			delete Value.Array;
			Type = T_None;
			cur.Restore(CursorBackup);
		}
	}
	if(T_None == Type)
	{
		// Function?
		Type = T_Function;
		Value.Function = new S_Function(cur);
		if(Value.Function->HasError())
		{
			delete Value.Function;
			Type = T_None;
			cur.Restore(CursorBackup);
		}
	}
	if(T_None == Type)
	{
		// Variable?
		Type = T_Variable;
		Value.Variable = new S_Variable(cur);
		if(Value.Variable->HasError())
		{
			delete Value.Variable;
			Type = T_None;
			cur.Restore(CursorBackup);
		}
	}
	if(T_None == Type)
	{
		SetError();
	}
}

S_Expression::~S_Expression()
{
	switch(Type)
	{
	case T_Parenthesis: delete Value.Parenthesis; break;
	case T_Constant: delete Value.Constant; break;
	case T_Array: delete Value.Array; break;
	case T_Function: delete Value.Function; break;
	case T_Variable: delete Value.Variable; break;
	}
}


// S_Function //////////////////////////////////////////////////////////////////

S_Function::S_Function(Cursor & cur)
: S_ParseElem(cur), Type(T_None)
{
	if(T_None == Type)
	{
		// PrefixOp1?
		Type = T_PrefixOp1;
		Value.PrefixOp1 = new S_PrefixOp1(cur);
		if(Value.PrefixOp1->HasError())
		{
			delete Value.PrefixOp1;
			Type = T_None;
			cur.Restore(CursorBackup);
		}
	}
	if(T_None == Type)
	{
		// InfixOp2?
		Type = T_InfixOp2;
		Value.InfixOp2 = new S_InfixOp2(cur);
		if(Value.InfixOp2->HasError())
		{
			delete Value.InfixOp2;
			Type = T_None;
			cur.Restore(CursorBackup);
		}
	}
	if(T_None == Type)
		SetError();
}

S_Function::~S_Function()
{
	switch(Type)
	{
	case T_InfixOp2: delete Value.InfixOp2; break;
	case T_PrefixOp1: delete Value.PrefixOp1; break;
	}
}


// S_InfixOp2 //////////////////////////////////////////////////////////////////

S_InfixOp2::S_InfixOp2(Cursor & cur)
: S_ParseElem(cur), Type(T_None)
{
	Value.Expressions.Left = 0;
	Value.Expressions.Right = 0;

	Value.Expressions.Left = new S_Expression(cur);

	if(Value.Expressions.Left->HasError())
		SetError();
	else
	{
		cur.SkipSpace();

		Type_t t_Op = T_None;

		if('&' == cur.Get(0) && '&' == cur.Get(1))
		{
			t_Op = T_And;
			cur.Seek(2);	
		}
		else if('=' == cur.Get(0) && '=' == cur.Get(1))
		{
			t_Op = T_Equal;
			cur.Seek(2);	
		}
		else if('>' == cur.Get(0))
		{
			t_Op = T_Greater;
			cur.Seek(2);	
		}
		else if('>' == cur.Get(0) && '=' == cur.Get(1))
		{
			t_Op = T_GreaterOrEqual;
			cur.Seek(2);	
		}
		else if('i' == cur.Get(0) && 'n' == cur.Get(1) && !Cursor::IsAlnum(cur.Get(2)))
		{
			t_Op = T_In;
			cur.Seek(2);	
		}
		else if('<' == cur.Get(0))
		{
			t_Op = T_Less;
			cur.Seek(2);	
		}
		else if('<' == cur.Get(0) && '=' == cur.Get(1))
		{
			t_Op = T_LessOrEqual;
			cur.Seek(2);	
		}
		else if('|' == cur.Get(0) && '|' == cur.Get(1))
		{
			t_Op = T_Or;
			cur.Seek(2);	
		}

		if(T_None == t_Op)
			SetError();
		else
		{
			Type = t_Op;

			Value.Expressions.Right = new S_Expression(cur);

			if(Value.Expressions.Right->HasError())
				SetError();
		}

	}
}

S_InfixOp2::~S_InfixOp2()
{
	delete Value.Expressions.Left;
	delete Value.Expressions.Right;
}


// S_Input /////////////////////////////////////////////////////////////////////

S_Input::S_Input(Cursor & cur)
: S_ParseElem(cur), Type(T_None)
{
	cur.SkipSpace();

	Type = T_Expression;
	Value.Expression = new S_Expression(cur);

	if(Value.Expression->HasError())
	{
		SetError();
	}
	else
	{
		cur.SkipSpace();
		if(!cur.IsNull())
			SetError();
	}
}

S_Input::~S_Input()
{
	switch(Type)
	{
	case T_Expression: delete Value.Expression; break;
	}
}


// S_Parenthesis ///////////////////////////////////////////////////////////////

S_Parenthesis::S_Parenthesis(Cursor & cur)
: S_ParseElem(cur), Type(T_None)
{
	if('(' != cur.Get())
		SetError();
	else
	{
		cur.Add();

		cur.SkipSpace();

		Type = T_Expression;
		Value.Expression = new S_Expression(cur);

		if(Value.Expression->HasError())
			SetError();
		else
		{
			cur.SkipSpace();

			if(')' != cur.Get())
				SetError();
			else
				cur.Add();
		}
	}
}

S_Parenthesis::~S_Parenthesis()
{
	switch(Type)
	{
	case T_Expression: delete Value.Expression; break;
	}
}


// S_ParseElem /////////////////////////////////////////////////////////////////

S_ParseElem::S_ParseElem(Cursor const & cur)
: CursorBackup(cur.Backup()), ErrorCode(0)
{
}

bool S_ParseElem::HasError() const
{
	return 0 != ErrorCode;
}

void S_ParseElem::SetError()
{
	ErrorCode = 1;
}


// S_PrefixOp1 /////////////////////////////////////////////////////////////////

S_PrefixOp1::S_PrefixOp1(Cursor & cur)
: S_ParseElem(cur), Type(T_None)
{
	if('!' == cur.Get())
	{
		cur.Add();
		cur.SkipSpace();

		Type = T_Not;
		Value.Expression = new S_Expression(cur);

		if(Value.Expression->HasError())
			SetError();
	}
	else
		SetError();
}

S_PrefixOp1::~S_PrefixOp1()
{
	switch(Type)
	{
	case T_Not: delete Value.Expression; break;
	}
}


// S_Variable //////////////////////////////////////////////////////////////////

S_Variable::S_Variable(Cursor & cur)
: S_ParseElem(cur), Type(T_None)
{
	int count = cur.SkipAlpha();
	if(0 < count) count += cur.SkipAlnum();
	cur.Restore(CursorBackup);

	if(0 < count)
	{
		Type = T_Name;

		Value.Name = new char[1+count];
		for(int i=0; i<count; i++) { Value.Name[i] = cur.Get(i); }
		Value.Name[count] = 0;
	}
	
	if(T_None == Type)
		SetError();
}

S_Variable::~S_Variable()
{
	switch(Type)
	{
	case T_Name: delete Value.Name; break;
	}
}

