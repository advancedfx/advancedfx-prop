#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-10 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


// Things could be speed up by directly resolving to machine code
// where applicable.


#include "expressions.h"

#include <ctype.h>
#include <string.h>

#include <exception>
#include <list>
#include <vector>

using namespace std;

using namespace Afx;
using namespace Afx::Expressions;


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

class CompileArgs : public Ref,
	public ICompileArgs
{
public:
	CompileArgs()
	{
	}

	void Add(ICompiled * value)
	{
#ifdef _DEBUG
		if(!value) throw new invalid_argument("value must not be null");
#endif

		value->Ref()->AddRef();

		m_Compileds.push_back(value);
	}

	virtual ICompiled * GetArg (int index) {
		size_t index_szt = (size_t)index;

		if(index < 0 || m_Compileds.size() <= index_szt)
			return 0;

		return m_Compileds[index_szt];
	}

	virtual int GetCount (void) {
		return m_Compileds.size();
	}

	virtual ::Afx::IRef * Ref (void) {
		return dynamic_cast<::Afx::IRef *>(this);
	}

protected:
	~CompileArgs()
	{
		for(CompiledList::iterator it = m_Compileds.begin(); it != m_Compileds.end(); it++)
		{
			(*it)->Ref()->Release();
		}
	}

private:
	typedef vector<ICompiled *> CompiledList;

	CompiledList m_Compileds;
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

	ICompiled * Compile_Bool(char const * identifier, ICompileArgs * args, int errorPos);

	ICompiled * Compile_Code(Cursor & cursor);

	ICompiled * Compile_Function(Cursor & cursor);

	ICompiled * Compile_Identifier(Cursor & cursor);

	ICompiled * Compile_Identifier(char const * identifier, ICompileArgs * args, int errorPos);

	ICompiled * Compile_Int(char const * identifier, ICompileArgs * args, int errorPos);

	ICompiled * Compile_Parenthesis(Cursor & cursor);

	static bool IsIdentifierChar(char val);

	char * New_Identifier(Cursor & cursor);
};



class Error : public Ref,
	public IError
{
public:	
	enum ErrorCode {
		EC_None,
		EC_Error,
		EC_ParseError
	};

	Error() {}
	Error(ErrorCode errorCode, int errorPosition) {}

	virtual ::Afx::IRef * Ref (void) { return dynamic_cast<::Afx::IRef *>(this); }

protected:


private:


};


struct __declspec(novtable) FnTools abstract
{
	static bool CheckArgs(ICompileArgs * args, ICompiled::Type type)
	{
		if(args) args->Ref()->AddRef();

		bool result =
			0 != args
			&& 1 == args->GetCount()
			&& type == args->GetArg(0)->GetType()
		;

		if(args) args->Ref()->Release();

		return result;
	}

	static bool CheckArgs(ICompileArgs * args, ICompiled::Type type, ICompiled::Type type2)
	{
		if(args) args->Ref()->AddRef();

		bool result =
			0 != args
			&& 2 == args->GetCount()
			&& type == args->GetArg(0)->GetType()
			&& type2 == args->GetArg(1)->GetType()
		;

		if(args) args->Ref()->Release();

		return result;
	}

	static bool CheckArgs(ICompileArgs * args, ICompiled::Type type, ICompiled::Type type2, ICompiled::Type type3)
	{
		if(args) args->Ref()->AddRef();

		bool result =
			0 != args
			&& 3 == args->GetCount()
			&& type == args->GetArg(0)->GetType()
			&& type2 == args->GetArg(1)->GetType()
			&& type3 == args->GetArg(2)->GetType()
		;

		if(args) args->Ref()->Release();

		return result;
	}

	static bool CheckArgsMin(ICompileArgs * args, ICompiled::Type type, int minCount)
	{
		if(args) args->Ref()->AddRef();

		bool result =
			0 != args
			&& minCount <= args->GetCount();
		;

		if(result)
		{
			int count = args->GetCount();

			for(int i=0; i<count && result; i++)
			{
				result = result && type == args->GetArg(i)->GetType();
			}
		}

		if(args) args->Ref()->Release();

		return result;
	}

};


class FnConstBool : public Ref,
	public IBool
{
public:
	FnConstBool(bool value) : m_Value(value) {}

	virtual bool EvalBool (void) { return m_Value; }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

private:
	bool m_Value;
};


class FnConstInt : public Ref,
	public IInt
{
public:
	FnConstInt(int value) : m_Value(value) {}

	virtual int EvalInt (void) { return m_Value; }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

private:
	int m_Value;
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

		ICompiled * compiled = FnTools::CheckArgsMin(args, ICompiled::T_Bool, 2)
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

		ICompiled * compiled = FnTools::CheckArgsMin(args, ICompiled::T_Bool, 2)
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

		ICompiled * compiled = FnTools::CheckArgs(args, ICompiled::T_Bool)
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

		ICompiled * compiled = FnTools::CheckArgs(args, ICompiled::T_Bool, ICompiled::T_Bool)
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

		ICompiled * compiled = FnTools::CheckArgs(args, ICompiled::T_Int, ICompiled::T_Int)
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


class FnLessOrEqualBool : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgs(args, ICompiled::T_Bool, ICompiled::T_Bool)
			? new Compiled(new FnLessOrEqualBool(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return m_Fns[0]->EvalBool() <= m_Fns[1]->EvalBool();
	}

private:
	FnLessOrEqualBool(ICompileArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnLessOrEqualBoolCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnLessOrEqualBool::Compile(args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnLessOrEqualInt : public FnIntsToBoolBase
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgs(args, ICompiled::T_Int, ICompiled::T_Int)
			? new Compiled(new FnLessOrEqualInt(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return m_Fns[0]->EvalInt() <= m_Fns[1]->EvalInt();
	}

private:
	FnLessOrEqualInt(ICompileArgs * args) : FnIntsToBoolBase(args) {}
};


class FnLessOrEqualIntCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnLessOrEqualInt::Compile(args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnEqualBool : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgs(args, ICompiled::T_Bool, ICompiled::T_Bool)
			? new Compiled(new FnEqualBool(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return m_Fns[0]->EvalBool() == m_Fns[1]->EvalBool();
	}

private:
	FnEqualBool(ICompileArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnEqualBoolCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnEqualBool::Compile(args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnEqualInt : public FnIntsToBoolBase
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgs(args, ICompiled::T_Int, ICompiled::T_Int)
			? new Compiled(new FnEqualInt(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return m_Fns[0]->EvalInt() == m_Fns[1]->EvalInt();
	}

private:
	FnEqualInt(ICompileArgs * args) : FnIntsToBoolBase(args) {}
};


class FnEqualIntCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnEqualInt::Compile(args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnGreaterBool : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgs(args, ICompiled::T_Bool, ICompiled::T_Bool)
			? new Compiled(new FnGreaterBool(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return m_Fns[0]->EvalBool() > m_Fns[1]->EvalBool();
	}

private:
	FnGreaterBool(ICompileArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnGreaterBoolCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnGreaterBool::Compile(args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnGreaterInt : public FnIntsToBoolBase
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgs(args, ICompiled::T_Int, ICompiled::T_Int)
			? new Compiled(new FnGreaterInt(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return m_Fns[0]->EvalInt() > m_Fns[1]->EvalInt();
	}

private:
	FnGreaterInt(ICompileArgs * args) : FnIntsToBoolBase(args) {}
};


class FnGreaterIntCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnGreaterInt::Compile(args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnGreaterOrEqualBool : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgs(args, ICompiled::T_Bool, ICompiled::T_Bool)
			? new Compiled(new FnGreaterOrEqualBool(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return m_Fns[0]->EvalBool() >= m_Fns[1]->EvalBool();
	}

private:
	FnGreaterOrEqualBool(ICompileArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnGreaterOrEqualBoolCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnGreaterOrEqualBool::Compile(args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnGreaterOrEqualInt : public FnIntsToBoolBase
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgs(args, ICompiled::T_Int, ICompiled::T_Int)
			? new Compiled(new FnGreaterOrEqualInt(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return m_Fns[0]->EvalInt() >= m_Fns[1]->EvalInt();
	}

private:
	FnGreaterOrEqualInt(ICompileArgs * args) : FnIntsToBoolBase(args) {}
};


class FnGreaterOrEqualIntCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnGreaterOrEqualInt::Compile(args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnInBool : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgsMin(args, ICompiled::T_Bool, 1)
			? new Compiled(new FnInBool(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		bool findVal = m_Fns[0]->EvalBool();

		for(int i=1; i<m_Count; i++)
		{
			if(findVal == m_Fns[i]->EvalBool())
				return true;
		}

		return false;
	}

private:
	FnInBool(ICompileArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnInBoolCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnInBool::Compile(args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnInInt : public FnIntsToBoolBase
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgsMin(args, ICompiled::T_Int, 1)
			? new Compiled(new FnInInt(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		int findVal = m_Fns[0]->EvalInt();

		for(int i=1; i<m_Count; i++)
		{
			if(findVal == m_Fns[i]->EvalInt())
				return true;
		}

		return false;
	}

private:
	FnInInt(ICompileArgs * args) : FnIntsToBoolBase(args) {}
};


class FnInIntCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnInInt::Compile(args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnMaxBool : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgsMin(args, ICompiled::T_Bool, 1)
			? new Compiled(new FnMaxBool(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {
		
		bool winVal = false;
		bool noVal = true;

		for(int i=0; i<m_Count; i++)
		{
			bool curVal = m_Fns[i]->EvalBool();

			if(noVal)
			{
				winVal = curVal;
				noVal = false;
			}
			else if(winVal < curVal) winVal = curVal;
		}

		return winVal;
	}

private:
	FnMaxBool(ICompileArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnMaxBoolCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnMaxBool::Compile(args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnMaxInt : public FnIntsToIntBase
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgsMin(args, ICompiled::T_Int, 1)
			? new Compiled(new FnMaxInt(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual int EvalInt (void) {
		
		int winVal = 0;
		bool noVal = true;

		for(int i=0; i<m_Count; i++)
		{
			int curVal = m_Fns[i]->EvalInt();

			if(noVal)
			{
				winVal = curVal;
				noVal = false;
			}
			else if(winVal < curVal) winVal = curVal;
		}

		return winVal;
	}

private:
	FnMaxInt(ICompileArgs * args) : FnIntsToIntBase(args) {}
};


class FnMaxIntCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnMaxInt::Compile(args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnMinBool : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgsMin(args, ICompiled::T_Bool, 1)
			? new Compiled(new FnMinBool(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {
		
		bool winVal = false;
		bool noVal = true;

		for(int i=0; i<m_Count; i++)
		{
			bool curVal = m_Fns[i]->EvalBool();

			if(noVal)
			{
				winVal = curVal;
				noVal = false;
			}
			else if(winVal > curVal) winVal = curVal;
		}

		return winVal;
	}

private:
	FnMinBool(ICompileArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnMinBoolCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnMinBool::Compile(args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnMinInt : public FnIntsToIntBase
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgsMin(args, ICompiled::T_Int, 1)
			? new Compiled(new FnMinInt(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual int EvalInt (void) {
		
		int winVal = 0;
		bool noVal = true;

		for(int i=0; i<m_Count; i++)
		{
			int curVal = m_Fns[i]->EvalInt();

			if(noVal)
			{
				winVal = curVal;
				noVal = false;
			}
			else if(winVal > curVal) winVal = curVal;
		}

		return winVal;
	}

private:
	FnMinInt(ICompileArgs * args) : FnIntsToIntBase(args) {}
};


class FnMinIntCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnMinInt::Compile(args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnIfBool : public Ref,
	public IBool
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgs(args, ICompiled::T_Bool, ICompiled::T_Bool, ICompiled::T_Bool)
			? new Compiled(new FnIfBool(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {
		
		return 	m_If->EvalBool()
			? m_IfTrue->EvalBool()
			: m_IfFalse->EvalBool()
		;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	

protected:
	virtual ~FnIfBool()
	{
		m_If->Ref()->Release();
		m_IfTrue->Ref()->Release();
		m_IfFalse->Ref()->Release();
	}

private:
	IBool * m_If;
	IBool * m_IfTrue;
	IBool * m_IfFalse;

	FnIfBool(ICompileArgs * args) {

		args->Ref()->AddRef();

		m_If = args->GetArg(0)->GetBool();
		m_If->Ref()->AddRef();

		m_IfTrue = args->GetArg(1)->GetBool();
		m_IfTrue->Ref()->AddRef();

		m_IfFalse = args->GetArg(2)->GetBool();
		m_IfFalse->Ref()->AddRef();

		args->Ref()->Release();
	}
};


class FnIfBoolCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnIfBool::Compile(args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnIfInt : public Ref,
	public IInt
{
public:
	static ICompiled * Compile(ICompileArgs * args)
	{
		if(args) args->Ref()->AddRef();

		ICompiled * compiled = FnTools::CheckArgs(args, ICompiled::T_Bool, ICompiled::T_Int, ICompiled::T_Int)
			? new Compiled(new FnIfInt(args))
			: new Compiled(new Error())
		;

		if(args) args->Ref()->Release();

		return compiled;
	}

	virtual int EvalInt (void) {
		
		return 	m_If->EvalBool()
			? m_IfTrue->EvalInt()
			: m_IfFalse->EvalInt()
		;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

protected:
	virtual ~FnIfInt()
	{
		m_If->Ref()->Release();
		m_IfTrue->Ref()->Release();
		m_IfFalse->Ref()->Release();
	}

private:
	IBool * m_If;
	IInt * m_IfTrue;
	IInt * m_IfFalse;

	FnIfInt(ICompileArgs * args) {

		args->Ref()->AddRef();

		m_If = args->GetArg(0)->GetBool();
		m_If->Ref()->AddRef();

		m_IfTrue = args->GetArg(1)->GetInt();
		m_IfTrue->Ref()->AddRef();

		m_IfFalse = args->GetArg(2)->GetInt();
		m_IfFalse->Ref()->AddRef();

		args->Ref()->Release();
	}
};


class FnIfIntCompileable : public Ref,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args) { return FnIfInt::Compile(args); }

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

	Add("lessOrEqual", new FnLessOrEqualBoolCompileable());
	Add("<=", new FnLessOrEqualBoolCompileable());
	Add("lessOrEqual", new FnLessOrEqualIntCompileable());
	Add("<=", new FnLessOrEqualIntCompileable());

	Add("equal", new FnEqualBoolCompileable());
	Add("==", new FnEqualBoolCompileable());
	Add("equal", new FnEqualIntCompileable());
	Add("==", new FnEqualIntCompileable());

	Add("greater", new FnGreaterBoolCompileable());
	Add(">", new FnGreaterBoolCompileable());
	Add("greater", new FnGreaterIntCompileable());
	Add(">", new FnGreaterIntCompileable());

	Add("greaterOrEqual", new FnGreaterOrEqualBoolCompileable());
	Add(">=", new FnGreaterOrEqualBoolCompileable());
	Add("greaterOrEqual", new FnGreaterOrEqualIntCompileable());
	Add(">=", new FnGreaterOrEqualIntCompileable());

	Add("in", new FnInBoolCompileable());
	Add("in", new FnInIntCompileable());

	Add("max", new FnMaxBoolCompileable());
	Add("max", new FnMaxIntCompileable());

	Add("min", new FnMinBoolCompileable());
	Add("min", new FnMinIntCompileable());

	Add("if", new FnIfBoolCompileable());
	Add("?", new FnIfBoolCompileable());
	Add("if", new FnIfIntCompileable());
	Add("?", new FnIfIntCompileable());
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
	Cursor cursor(code);

	return Compile_Code(cursor);
}

ICompiled * Bubble::Compile_Bool(char const * identifier, ICompileArgs * args, int errorPos)
{
	if(0 == args->GetCount())
	{
		bool isBool = false;
		bool boolVal;

		if(!strcmp("false", identifier))
		{
			isBool = true;
			boolVal = false;
		}
		else if(!strcmp("true", identifier))
		{
			isBool = true;
			boolVal = true;
		}

		if(isBool) return new Compiled(new FnConstBool(boolVal));
	}

	return new Compiled(new Error(Error::EC_ParseError, errorPos));
}

ICompiled * Bubble::Compile_Code(Cursor & cursor)
{
	cursor.SkipSpace();

	ICompiled * compiled = Compile_Function(cursor);

	cursor.SkipSpace();

	if(!cursor.IsNull())
	{
		compiled->Ref()->AddRef();
		compiled->Ref()->Release();

		compiled = new Compiled(new Error(Error::EC_ParseError, cursor.GetPos()));
	}

	return compiled;
}

ICompiled * Bubble::Compile_Function(Cursor & cursor)
{
	if('(' == cursor.Get())
	{
		return Compile_Parenthesis(cursor);
	}

	return Compile_Identifier(cursor);
}

ICompiled * Bubble::Compile_Identifier(Cursor & cursor)
{
	ICompiled * compiled = 0;

	char * id = New_Identifier(cursor);

	if(id) compiled = Compile_Identifier(id, new CompileArgs(), cursor.GetPos());

	delete id;

	return compiled ? compiled : new Compiled(new Error(Error::EC_ParseError, cursor.GetPos()));
}

ICompiled * Bubble::Compile_Identifier(char const * identifier, ICompileArgs * args, int errorPos)
{
	ICompiled * compiled = 0;

	args->Ref()->AddRef();

	if(!compiled)
	{
		// Bool?

		compiled = Compile_Bool(identifier, args, errorPos);
		if(compiled->GetError())
		{
			compiled->Ref()->AddRef();
			compiled->Ref()->Release();
			compiled = 0;
		}
	}

	if(!compiled)
	{
		// Int?

		compiled = Compile_Int(identifier, args, errorPos);
		if(compiled->GetError())
		{
			compiled->Ref()->AddRef();
			compiled->Ref()->Release();
			compiled = 0;
		}
	}

	if(!compiled)
	{
		// Some function?

		for(FunctionList::iterator it = m_Functions.begin(); it != m_Functions.end(); it++)
		{
			BubbleFn * bubbleFn = *it;

			if(!strcmp(identifier, bubbleFn->GetName()))
			{
				compiled = bubbleFn->GetCompileable()->Compile(args);

				if(!compiled->GetError())
					break;

				compiled->Ref()->AddRef();
				compiled->Ref()->Release();
				compiled = 0;
			}
		}
	}

	args->Ref()->Release();

	return compiled ? compiled : new Compiled(new Error(Error::EC_ParseError, errorPos));
}

ICompiled * Bubble::Compile_Int(char const * identifier, ICompileArgs * args, int errorPos)
{
	if(0 == args->GetCount())
	{
		bool isInt = false;
		int intVal = atoi(identifier);

		if(0 == intVal)
		{
			size_t len = strlen(identifier);

			if(0 < len)
			{
				// string is not empty.
				isInt = true;

				for(size_t i=0; i<len; i++)
				{
					isInt = isInt && (
						'0' == identifier[i]
						|| (0 == i && 2 <= len && '-' == identifier[i])
					);
				}
			}
		}
		else isInt = true;

		if(isInt) return new Compiled(new FnConstInt(intVal));
	}

	return new Compiled(new Error(Error::EC_ParseError, errorPos));
}

ICompiled * Bubble::Compile_Parenthesis(Cursor & cursor)
{
	if('(' == cursor.Get())
	{
		cursor.Add();

		cursor.SkipSpace();

		ICompiled * compiled = 0;
		char * id = New_Identifier(cursor);

		if(id)
		{
			CompileArgs * args = new CompileArgs();
			args->AddRef();

			for(
				bool hasMore;
				(hasMore = 0 < cursor.SkipSpace() && ')' != cursor.Get()) && !compiled;
			)
			{
				ICompiled * curCompiled = Compile_Function(cursor);

				if(curCompiled->GetError())
					// Error, first error is compile result:
					compiled = curCompiled;
				else
					// Ok, add compiled argument:
					args->Add(curCompiled);

			}

			if(!compiled)
			{
				if(')' == cursor.Get())
				{
					// Try to compile it:
					compiled = Compile_Identifier(id, args, cursor.GetPos());

					if(!compiled->GetError()) cursor.Add();
				}
			}

			args->Release();
		}

		delete id;

		if(compiled) return compiled;
	}

	return new Compiled(new Error(Error::EC_ParseError, cursor.GetPos()));
}

bool Bubble::IsIdentifierChar(char val)
{
	return
		!Cursor::IsNull(val)
		&& !Cursor::IsSpace(val)
		&& '(' != val
		&& ')' != val
	;
}

char * Bubble::New_Identifier(Cursor & cursor)
{
	int count = 0;

	while(IsIdentifierChar(cursor.Get(count))) count++;

	if(0 < count)
	{
		char * ret = new char[1+count];
		for(int i=0; i<count; i++) ret[i] = cursor.GetAdd();
		ret[count] = 0;

		return ret;
	}

	return 0;
}


::Afx::IRef * Bubble::Ref (void)
{
	return dynamic_cast<::Afx::IRef * >(this);
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


// Tools ///////////////////////////////////////////////////////////////

IBubble * Tools::StandardBubble (void)
{
	return new Bubble();
}

