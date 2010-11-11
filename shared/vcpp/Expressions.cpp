#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-11 dominik.matrixstorm.com
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
	public ICompiler,
	public IBubble
{
public:
	Bubble();

	virtual void Add(char const * name, ICompileable * compileable);

	virtual ICompiled * Compile(char const * code);

	virtual ICompiler * Compiler (void);

	virtual ICompiled * Compile_Function (Cursor & cursor);

	virtual ::Afx::IRef * Ref (void);

protected:
	~Bubble();

private:
	typedef list<BubbleFn *> FunctionList;
	FunctionList m_Functions;

	ICompiled * Compile_Code(Cursor & cursor);

	ICompiled * Compile_Identifier(Cursor & cursor, bool inParenthesis);

	ICompiled * Compile_Parenthesis(Cursor & cursor);

	static bool IsIdentifierChar(char val);

	char * New_Identifier(Cursor & cursor);

	bool ToBool(char const * identifier, bool & outValue);

	bool Bubble::ToInt(char const * identifier, int & outValue);
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


class CompileArgs : public Ref,
	public ICompileArgs
{
public:
	CompileArgs(Cursor & cursor, bool inParenthesis)
	: m_Cursor(cursor), m_InParenthesis(inParenthesis)
	{
	}

	virtual ICompiled * CompileNextArg (ICompiler * compiler)
	{
		ICompiled * compiled = 0;

		compiler->Ref()->AddRef();

		if(HasNextArg())
		{
			compiled = compiler->Compile_Function(m_Cursor);
			if(!compiled->GetError())
			{
				m_Cursor.SkipSpace();
			}
		}

		compiler->Ref()->Release();

		return compiled ? compiled : new Compiled(new Error(Error::EC_ParseError, m_Cursor.GetPos()));
	}

	virtual bool HasNextArg (void)
	{
		return
			m_InParenthesis
			&& ')' != m_Cursor.Get()
		;
	}

	virtual ::Afx::IRef * Ref (void)
	{
		return dynamic_cast<::Afx::IRef *>(this);
	}

private:
	Cursor & m_Cursor;
	bool m_InParenthesis;
};


class ParseArgs : public Ref
{
public:
	ParseArgs(ICompiler * compiler, ICompileArgs * args)
	: m_Args(args), m_Compiler(compiler)
	{
		args->Ref()->AddRef();
		compiler->Ref()->AddRef();
	}

	ICompiled * GetArg(int index) {
		return m_Compileds[index];
	}

	int GetCount (void) {
		return m_Compileds.size();
	}

	bool HasNextArg (void)
	{
		return m_Args->HasNextArg();
	}

	ICompiled * ParseNextArg(void)
	{
		ICompiled * compiled = m_Args->CompileNextArg(m_Compiler);

		compiled->Ref()->AddRef();

		m_Compileds.push_back(compiled);

		return compiled;
	}

	ICompiled::Type ParseNextArgT (void)
	{
		return ParseNextArg()->GetType();
	}

	bool ParseNextArgTC (ICompiled::Type type)
	{
		return type == ParseNextArgT();
	}

	::Afx::IRef * Ref (void) {
		return dynamic_cast<::Afx::IRef *>(this);
	}

protected:
	~ParseArgs()
	{
		for(VectorT::iterator it = m_Compileds.begin(); it != m_Compileds.end(); it++)
		{
			(*it)->Ref()->Release();
		}

		m_Args->Ref()->Release();
		m_Compiler->Ref()->Release();
	}

private:
	typedef vector<ICompiled *> VectorT;

	VectorT m_Compileds;

	ICompileArgs * m_Args;
	ICompiler * m_Compiler;
	
};


class FnConstBool : public Ref,
	public IBool
{
public:
	static ICompiled * Compile (ICompiler * compiler, ICompileArgs * args, bool value)
	{
		ICompiled * compiled = 0;

		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		if(!pa->HasNextArg())
		{
			compiled = new Compiled(new FnConstBool(value));
		}

		pa->Ref()->Release();

		return compiled ? compiled : new Compiled(new Error());
	}

	FnConstBool(bool value) : m_Value(value) {}

	virtual bool EvalBool (void) { return m_Value; }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

private:
	bool m_Value;
};


class FnConstBoolCompileable : public Compileable
{
public:
	FnConstBoolCompileable(ICompiler * compiler, bool value)
	: Compileable(compiler), m_Value(value)
	{
	}

	virtual ICompiled * Compile (ICompileArgs * args)
	{
		return FnConstBool::Compile(m_Compiler, args, m_Value);
	}

	virtual ::Afx::IRef * Ref (void) { return dynamic_cast<::Afx::IRef *>(this); }	

private:
	bool m_Value;
};


class FnConstInt : public Ref,
	public IInt
{
public:
	static ICompiled * Compile (ICompiler * compiler, ICompileArgs * args, int value)
	{
		ICompiled * compiled = 0;

		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		if(!pa->HasNextArg())
		{
			compiled = new Compiled(new FnConstInt(value));
		}

		pa->Ref()->Release();

		return compiled ? compiled : new Compiled(new Error());
	}


	FnConstInt(int value) : m_Value(value) {}

	virtual int EvalInt (void) { return m_Value; }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

private:
	int m_Value;
};


class FnConstIntCompileable : public Compileable
{
public:
	FnConstIntCompileable(ICompiler * compiler, int value)
	: Compileable(compiler), m_Value(value)
	{
	}

	virtual ICompiled * Compile (ICompileArgs * args)
	{
		return FnConstInt::Compile(m_Compiler, args, m_Value);
	}

	virtual ::Afx::IRef * Ref (void) { return dynamic_cast<::Afx::IRef *>(this); }	

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

	FnBoolsToBoolBase(ParseArgs * args)
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

	FnIntsToBoolBase(ParseArgs * args)
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

	FnIntsToIntBase(ParseArgs * args)
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
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Bool)
			&& pa->ParseNextArgTC(ICompiled::T_Bool)
		;

		while(bOk && pa->HasNextArg())
			bOk = bOk && pa->ParseNextArgTC(ICompiled::T_Bool);

		ICompiled * compiled = bOk
			? new Compiled(new FnAnd(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

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
	FnAnd(ParseArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnAndCompileable : public Compileable
{
public:
	FnAndCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnAnd::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnOr : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Bool)
			&& pa->ParseNextArgTC(ICompiled::T_Bool)
		;

		while(bOk && pa->HasNextArg())
			bOk = bOk && pa->ParseNextArgTC(ICompiled::T_Bool);

		ICompiled * compiled = bOk
			? new Compiled(new FnOr(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

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
	FnOr(ParseArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnOrCompileable : public Compileable
{
public:
	FnOrCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnOr::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnNot : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Bool)
			&& !pa->HasNextArg()
		;

		ICompiled * compiled = bOk
			? new Compiled(new FnNot(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return !m_Fns[0]->EvalBool();
	}

private:
	FnNot(ParseArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnNotCompileable : public Compileable
{
public:
	FnNotCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnNot::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnLessBool : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Bool)
			&& pa->ParseNextArgTC(ICompiled::T_Bool)
			&& !pa->HasNextArg()
		;

		ICompiled * compiled = bOk
			? new Compiled(new FnLessBool(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return m_Fns[0]->EvalBool() < m_Fns[1]->EvalBool();
	}

private:
	FnLessBool(ParseArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnLessBoolCompileable : public Compileable
{
public:
	FnLessBoolCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnLessBool::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnLessInt : public FnIntsToBoolBase
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Int)
			&& pa->ParseNextArgTC(ICompiled::T_Int)
			&& !pa->HasNextArg()
		;

		ICompiled * compiled = bOk
			? new Compiled(new FnLessInt(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return m_Fns[0]->EvalInt() < m_Fns[1]->EvalInt();
	}

private:
	FnLessInt(ParseArgs * args) : FnIntsToBoolBase(args) {}
};


class FnLessIntCompileable : public Compileable
{
public:
	FnLessIntCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnLessInt::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnLessOrEqualBool : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Bool)
			&& pa->ParseNextArgTC(ICompiled::T_Bool)
			&& !pa->HasNextArg()
		;

		ICompiled * compiled = bOk
			? new Compiled(new FnLessOrEqualBool(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return m_Fns[0]->EvalBool() <= m_Fns[1]->EvalBool();
	}

private:
	FnLessOrEqualBool(ParseArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnLessOrEqualBoolCompileable : public Compileable
{
public:
	FnLessOrEqualBoolCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnLessOrEqualBool::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnLessOrEqualInt : public FnIntsToBoolBase
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Int)
			&& pa->ParseNextArgTC(ICompiled::T_Int)
			&& !pa->HasNextArg()
		;

		ICompiled * compiled = bOk
			? new Compiled(new FnLessOrEqualInt(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return m_Fns[0]->EvalInt() <= m_Fns[1]->EvalInt();
	}

private:
	FnLessOrEqualInt(ParseArgs * args) : FnIntsToBoolBase(args) {}
};


class FnLessOrEqualIntCompileable : public Compileable
{
public:
	FnLessOrEqualIntCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnLessOrEqualInt::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnEqualBool : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Bool)
			&& pa->ParseNextArgTC(ICompiled::T_Bool)
			&& !pa->HasNextArg()
		;

		ICompiled * compiled = bOk
			? new Compiled(new FnEqualBool(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return m_Fns[0]->EvalBool() == m_Fns[1]->EvalBool();
	}

private:
	FnEqualBool(ParseArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnEqualBoolCompileable : public Compileable
{
public:
	FnEqualBoolCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnEqualBool::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnEqualInt : public FnIntsToBoolBase
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Int)
			&& pa->ParseNextArgTC(ICompiled::T_Int)
			&& !pa->HasNextArg()
		;

		ICompiled * compiled = bOk
			? new Compiled(new FnEqualInt(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return m_Fns[0]->EvalInt() == m_Fns[1]->EvalInt();
	}

private:
	FnEqualInt(ParseArgs * args) : FnIntsToBoolBase(args) {}
};


class FnEqualIntCompileable : public Compileable
{
public:
	FnEqualIntCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnEqualInt::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnGreaterBool : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Bool)
			&& pa->ParseNextArgTC(ICompiled::T_Bool)
			&& !pa->HasNextArg()
		;

		ICompiled * compiled = bOk
			? new Compiled(new FnGreaterBool(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return m_Fns[0]->EvalBool() > m_Fns[1]->EvalBool();
	}

private:
	FnGreaterBool(ParseArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnGreaterBoolCompileable : public Compileable
{
public:
	FnGreaterBoolCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnGreaterBool::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnGreaterInt : public FnIntsToBoolBase
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Int)
			&& pa->ParseNextArgTC(ICompiled::T_Int)
			&& !pa->HasNextArg()
		;

		ICompiled * compiled = bOk
			? new Compiled(new FnGreaterInt(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return m_Fns[0]->EvalInt() > m_Fns[1]->EvalInt();
	}

private:
	FnGreaterInt(ParseArgs * args) : FnIntsToBoolBase(args) {}
};


class FnGreaterIntCompileable : public Compileable
{
public:
	FnGreaterIntCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnGreaterInt::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnGreaterOrEqualBool : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Bool)
			&& pa->ParseNextArgTC(ICompiled::T_Bool)
			&& !pa->HasNextArg()
		;

		ICompiled * compiled = bOk
			? new Compiled(new FnGreaterOrEqualBool(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return m_Fns[0]->EvalBool() >= m_Fns[1]->EvalBool();
	}

private:
	FnGreaterOrEqualBool(ParseArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnGreaterOrEqualBoolCompileable : public Compileable
{
public:
	FnGreaterOrEqualBoolCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnGreaterOrEqualBool::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnGreaterOrEqualInt : public FnIntsToBoolBase
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Int)
			&& pa->ParseNextArgTC(ICompiled::T_Int)
			&& !pa->HasNextArg()
		;

		ICompiled * compiled = bOk
			? new Compiled(new FnGreaterOrEqualInt(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

		return compiled;
	}

	virtual bool EvalBool (void) {

		return m_Fns[0]->EvalInt() >= m_Fns[1]->EvalInt();
	}

private:
	FnGreaterOrEqualInt(ParseArgs * args) : FnIntsToBoolBase(args) {}
};


class FnGreaterOrEqualIntCompileable : public Compileable
{
public:
	FnGreaterOrEqualIntCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnGreaterOrEqualInt::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnInBool : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Bool)
		;

		while(bOk && pa->HasNextArg()) bOk = pa->ParseNextArgTC(ICompiled::T_Bool);

		ICompiled * compiled = bOk
			? new Compiled(new FnInBool(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

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
	FnInBool(ParseArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnInBoolCompileable : public Compileable
{
public:
	FnInBoolCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnInBool::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnInInt : public FnIntsToBoolBase
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Int)
		;

		while(bOk && pa->HasNextArg()) bOk = pa->ParseNextArgTC(ICompiled::T_Int);

		ICompiled * compiled = bOk
			? new Compiled(new FnInInt(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

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
	FnInInt(ParseArgs * args) : FnIntsToBoolBase(args) {}
};


class FnInIntCompileable : public Compileable
{
public:
	FnInIntCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnInInt::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnMaxBool : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Bool)
		;

		while(bOk && pa->HasNextArg()) bOk = pa->ParseNextArgTC(ICompiled::T_Bool);

		ICompiled * compiled = bOk
			? new Compiled(new FnMaxBool(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

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
	FnMaxBool(ParseArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnMaxBoolCompileable : public Compileable
{
public:
	FnMaxBoolCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnMaxBool::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnMaxInt : public FnIntsToIntBase
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Int)
		;

		while(bOk && pa->HasNextArg()) bOk = pa->ParseNextArgTC(ICompiled::T_Int);

		ICompiled * compiled = bOk
			? new Compiled(new FnMaxInt(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

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
	FnMaxInt(ParseArgs * args) : FnIntsToIntBase(args) {}
};


class FnMaxIntCompileable : public Compileable
{
public:
	FnMaxIntCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnMaxInt::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnMinBool : public FnBoolsToBoolBase
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Bool)
		;

		while(bOk && pa->HasNextArg()) bOk = pa->ParseNextArgTC(ICompiled::T_Bool);

		ICompiled * compiled = bOk
			? new Compiled(new FnMinBool(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

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
	FnMinBool(ParseArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnMinBoolCompileable : public Compileable
{
public:
	FnMinBoolCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnMinBool::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnMinInt : public FnIntsToIntBase
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Int)
		;

		while(bOk && pa->HasNextArg()) bOk = pa->ParseNextArgTC(ICompiled::T_Int);

		ICompiled * compiled = bOk
			? new Compiled(new FnMinInt(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

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
	FnMinInt(ParseArgs * args) : FnIntsToIntBase(args) {}
};


class FnMinIntCompileable : public Compileable
{
public:
	FnMinIntCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnMinInt::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnIfBool : public Ref,
	public IBool
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Bool)
			&& pa->ParseNextArgTC(ICompiled::T_Bool)
			&& pa->ParseNextArgTC(ICompiled::T_Bool)
			&& !pa->HasNextArg()
		;

		ICompiled * compiled = bOk
			? new Compiled(new FnIfBool(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

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

	FnIfBool(ParseArgs * args) {

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


class FnIfBoolCompileable : public Compileable
{
public:
	FnIfBoolCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnIfBool::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnIfInt : public Ref,
	public IInt
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Int)
			&& pa->ParseNextArgTC(ICompiled::T_Int)
			&& pa->ParseNextArgTC(ICompiled::T_Int)
			&& !pa->HasNextArg()
		;

		ICompiled * compiled = bOk
			? new Compiled(new FnIfInt(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

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

	FnIfInt(ParseArgs * args) {

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


class FnIfIntCompileable : public Compileable
{
public:
	FnIfIntCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnIfInt::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	
};


class FnDo : public Ref,
	public IVoid,
	public IBool,
	public IInt
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk = true;

		ICompiled * compiled = 0;
		ICompiled::Type resultType = ICompiled::T_Void;

		while(bOk && pa->HasNextArg())
		{
			ICompiled::Type curType = pa->ParseNextArgT();

			switch(curType)
			{
			case ICompiled::T_Bool:
			case ICompiled::T_Int:
			case ICompiled::T_Void:
				resultType = curType;
				break;
			default:
				bOk = false;
				break;
			}
		}

		if(bOk)
		{
			switch(resultType)
			{
			case ICompiled::T_Bool:
				compiled = new Compiled(dynamic_cast<IBool *>(new FnDo(pa)));
				break;
			case ICompiled::T_Int:
				compiled = new Compiled(dynamic_cast<IInt *>(new FnDo(pa)));
				break;
			case ICompiled::T_Void:
				compiled = new Compiled(dynamic_cast<IVoid *>(new FnDo(pa)));
				break;
			default:
				break;
			}
		}

		pa->Ref()->Release();

		return compiled ? compiled : new Compiled(new Error());
	}

	virtual bool EvalBool (void)
	{
		for(int i=0; i<m_Count-1; i++) EvalX(i);

		return m_Fns[m_Count-1].Bool->EvalBool();
	}

	virtual int EvalInt (void)
	{
		for(int i=0; i<m_Count-1; i++) EvalX(i);

		return m_Fns[m_Count-1].Int->EvalInt();
	}

	virtual void EvalVoid (void)
	{
		for(int i=0; i<m_Count; i++) EvalX(i);
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	

protected:
	virtual ~FnDo()
	{
		for(int i=0; i<m_Count; i++)
		{
			switch(m_Types[i])
			{
			case ICompiled::T_Bool:
				m_Fns[i].Bool->Ref()->Release();
				break;
			case ICompiled::T_Int:
				m_Fns[i].Int->Ref()->Release();
				break;
			case ICompiled::T_Void:
				m_Fns[i].Void->Ref()->Release();
				break;
			default:
				throw exception();
				break;
			}
		}

		delete m_Types;
		delete m_Fns;
	}

private:
	typedef union {
		IVoid * Void;
		IBool * Bool;
		IInt * Int;
	} FnT;
	int m_Count;
	ICompiled::Type * m_Types;
	FnT * m_Fns;

	FnDo(ParseArgs * args) {

		args->Ref()->AddRef();

		m_Count = args->GetCount();

		m_Types = new ICompiled::Type[m_Count];
		m_Fns = new FnT[m_Count];

		for(int i=0; i<m_Count; i++)
		{
			ICompiled::Type curType = args->GetArg(i)->GetType();
			m_Types[i] = curType;

			switch(curType)
			{
			case ICompiled::T_Bool:
				m_Fns[i].Bool = args->GetArg(i)->GetBool();
				m_Fns[i].Bool->Ref()->AddRef();
				break;
			case ICompiled::T_Int:
				m_Fns[i].Int = args->GetArg(i)->GetInt();
				m_Fns[i].Int->Ref()->AddRef();
				break;
			case ICompiled::T_Void:
				m_Fns[i].Void = args->GetArg(i)->GetVoid();
				m_Fns[i].Void->Ref()->AddRef();
				break;
			default:
				throw exception();
				break;
			}
		}

		args->Ref()->Release();
	}

	void EvalX(int i)
	{
		switch(m_Types[i])
		{
		case ICompiled::T_Bool:
			m_Fns[i].Bool->EvalBool();
			break;
		case ICompiled::T_Int:
			m_Fns[i].Int->EvalInt();
			break;
		case ICompiled::T_Void:
			m_Fns[i].Void->EvalVoid();
		default:
			throw exception();
			break;
		}
	}
};

class FnDoCompileable : public Compileable
{
public:
	FnDoCompileable(ICompiler * compiler) : Compileable(compiler) {}

	virtual ICompiled * Compile (ICompileArgs * args) { return FnDo::Compile(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }		
};



// BoolGetterC /////////////////////////////////////////////////////////////////

class BoolGetterC : public Ref,
	public IBool
{
public:
	BoolGetterC(IBoolGetter * boolGetter)
	: m_BoolGetter(boolGetter)
	{
		boolGetter->Ref()->AddRef();
	}

	virtual bool EvalBool (void)
	{
		return m_BoolGetter->Get();
	}

	virtual ::Afx::IRef * Ref (void) { return dynamic_cast<::Afx::IRef *>(this); }

protected:
	virtual ~BoolGetterC()
	{
		m_BoolGetter->Ref()->Release();
	}

private:
	IBoolGetter * m_BoolGetter;
};


// BoolSetterC /////////////////////////////////////////////////////////////////

class BoolSetterC : public Ref,
	public IVoid
{
public:
	BoolSetterC(IBoolSetter * boolSetter, IBool * value)
	: m_BoolSetter(boolSetter), m_Value(value)
	{
		boolSetter->Ref()->AddRef();
		value->Ref()->AddRef();
	}

	virtual void EvalVoid (void)
	{
		m_BoolSetter->Set(m_Value->EvalBool());
	}

	virtual ::Afx::IRef * Ref (void) { return dynamic_cast<::Afx::IRef *>(this); }

protected:
	virtual ~BoolSetterC()
	{
		m_BoolSetter->Ref()->Release();
		m_Value->Ref()->Release();
	}

private:
	IBoolSetter * m_BoolSetter;
	IBool * m_Value;
};


// BoolGetter //////////////////////////////////////////////////////////////////

BoolGetter::BoolGetter(ICompiler * compiler)
: Compileable(compiler)
{
}

ICompiled * BoolGetter::Compile (ICompileArgs * args)
{
	ICompiled * compiled = 0;

	ParseArgs * pa = new ParseArgs(m_Compiler, args);
	pa->Ref()->AddRef();

	if(!pa->HasNextArg())
	{
		compiled = new Compiled(new BoolGetterC(this));
	}

	pa->Ref()->Release();

	return compiled ? compiled : new Compiled(new Error());
}


::Afx::IRef * BoolGetter::Ref (void)
{
	return dynamic_cast<::Afx::IRef *>(this);
}


// BoolProperty ////////////////////////////////////////////////////////////////


BoolProperty::BoolProperty(ICompiler * compiler, CompileAcces compileAccess)
: Compileable(compiler), m_CompileAccess(compileAccess)
{
}

ICompiled * BoolProperty::Compile (ICompileArgs * args)
{
	ICompiled * compiled = 0;

	ParseArgs * pa = new ParseArgs(m_Compiler, args);
	pa->Ref()->AddRef();

	if(
		(CA_Property == m_CompileAccess || CA_Getter == m_CompileAccess)
		&& !pa->HasNextArg()
	)
	{
		// Compile getter:
		compiled = new Compiled(new BoolGetterC(this));
	}
	else if(
		(CA_Property == m_CompileAccess || CA_Setter == m_CompileAccess)
		&& pa->ParseNextArgTC(ICompiled::T_Bool) && !pa->HasNextArg()
	)
	{
		// Compile setter:
		compiled = new Compiled(new BoolSetterC(this, pa->GetArg(0)->GetBool()));
	}

	pa->Ref()->Release();

	return compiled ? compiled : new Compiled(new Error());
}


::Afx::IRef * BoolProperty::Ref (void)
{
	return dynamic_cast<::Afx::IRef *>(this); 
}


// BoolSetter //////////////////////////////////////////////////////////////////

BoolSetter::BoolSetter(ICompiler * compiler)
: Compileable(compiler)
{
}

ICompiled * BoolSetter::Compile (ICompileArgs * args)
{
	ICompiled * compiled = 0;

	ParseArgs * pa = new ParseArgs(m_Compiler, args);
	pa->Ref()->AddRef();

	if(pa->ParseNextArgTC(ICompiled::T_Bool) && !pa->HasNextArg())
	{
		compiled = new Compiled(new BoolSetterC(this, pa->GetArg(0)->GetBool()));
	}

	pa->Ref()->Release();

	return compiled ? compiled : new Compiled(new Error());
}


::Afx::IRef * BoolSetter::Ref (void)
{
	return dynamic_cast<::Afx::IRef *>(this);
}


// BoolVariable ////////////////////////////////////////////////////////////////

BoolVariable::BoolVariable(ICompiler * compiler, CompileAcces compileAccess, bool value)
: BoolProperty(compiler, compileAccess), m_Value(value)
{
}

bool BoolVariable::Get() const
{
	return m_Value;
}

bool BoolVariable::Get (void)
{
	return m_Value;
}

void BoolVariable::Set (bool value)
{
	m_Value = value;
}


// Bubble //////////////////////////////////////////////////////////////////////

Bubble::Bubble()
{
	//
	// Add standard functions:

	Add("and", new FnAndCompileable(this));
	Add("&&", new FnAndCompileable(this));

	Add("or", new FnOrCompileable(this));
	Add("||", new FnOrCompileable(this));

	Add("not", new FnNotCompileable(this));
	Add("!", new FnNotCompileable(this));

	Add("less", new FnLessBoolCompileable(this));
	Add("<", new FnLessBoolCompileable(this));
	Add("less", new FnLessIntCompileable(this));
	Add("<", new FnLessIntCompileable(this));

	Add("lessOrEqual", new FnLessOrEqualBoolCompileable(this));
	Add("<=", new FnLessOrEqualBoolCompileable(this));
	Add("lessOrEqual", new FnLessOrEqualIntCompileable(this));
	Add("<=", new FnLessOrEqualIntCompileable(this));

	Add("equal", new FnEqualBoolCompileable(this));
	Add("==", new FnEqualBoolCompileable(this));
	Add("equal", new FnEqualIntCompileable(this));
	Add("==", new FnEqualIntCompileable(this));

	Add("greater", new FnGreaterBoolCompileable(this));
	Add(">", new FnGreaterBoolCompileable(this));
	Add("greater", new FnGreaterIntCompileable(this));
	Add(">", new FnGreaterIntCompileable(this));

	Add("greaterOrEqual", new FnGreaterOrEqualBoolCompileable(this));
	Add(">=", new FnGreaterOrEqualBoolCompileable(this));
	Add("greaterOrEqual", new FnGreaterOrEqualIntCompileable(this));
	Add(">=", new FnGreaterOrEqualIntCompileable(this));

	Add("in", new FnInBoolCompileable(this));
	Add("in", new FnInIntCompileable(this));

	Add("max", new FnMaxBoolCompileable(this));
	Add("max", new FnMaxIntCompileable(this));

	Add("min", new FnMinBoolCompileable(this));
	Add("min", new FnMinIntCompileable(this));

	Add("if", new FnIfBoolCompileable(this));
	Add("?", new FnIfBoolCompileable(this));
	Add("if", new FnIfIntCompileable(this));
	Add("?", new FnIfIntCompileable(this));

	Add("do", new FnDoCompileable(this));

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


ICompiler * Bubble::Compiler (void)
{
	return dynamic_cast<ICompiler *>(this);
}




ICompiled * Bubble::Compile_Code(Cursor & cursor)
{
	cursor.SkipSpace();

	ICompiled * compiled = Compile_Function(cursor);

	cursor.SkipSpace();

	if(!compiled->GetError())
	{
		// ok.

		if(!cursor.IsNull())
		{
			// error.

			compiled->Ref()->AddRef();
			compiled->Ref()->Release();

			compiled = new Compiled(new Error(Error::EC_ParseError, cursor.GetPos()));
		}
	}

	return compiled;
}

ICompiled * Bubble::Compile_Function(Cursor & cursor)
{
	if('(' == cursor.Get())
	{
		return Compile_Parenthesis(cursor);
	}

	return Compile_Identifier(cursor, false);
}

ICompiled * Bubble::Compile_Identifier(Cursor & cursor, bool inParenthesis)
{
	ICompiled * compiled = 0;

	if(inParenthesis) cursor.SkipSpace();

	char * id = New_Identifier(cursor);

	if(id)
	{
		if(inParenthesis) cursor.SkipSpace();

		if(!compiled)
		{
			// Bool?

			bool bVal;
			if(ToBool(id, bVal))
			{
				CursorBackup backup = cursor.Backup();

				compiled = FnConstBool::Compile(this, new CompileArgs(cursor, inParenthesis), bVal);
				if(compiled->GetError())
				{
					compiled->Ref()->AddRef();
					compiled->Ref()->Release();
					compiled = 0;
					cursor.Restore(backup);
				}
			}
		}

		if(!compiled)
		{
			// Int?

			int iVal;
			if(ToInt(id, iVal))
			{
				CursorBackup backup = cursor.Backup();

				compiled = FnConstInt::Compile(this, new CompileArgs(cursor, inParenthesis), iVal);
				if(compiled->GetError())
				{
					compiled->Ref()->AddRef();
					compiled->Ref()->Release();
					compiled = 0;
					cursor.Restore(backup);
				}
			}
		}

		if(!compiled)
		{
			// Some function?

			for(FunctionList::iterator it = m_Functions.begin(); it != m_Functions.end(); it++)
			{
				BubbleFn * bubbleFn = *it;

				if(!strcmp(id, bubbleFn->GetName()))
				{
					CursorBackup backup = cursor.Backup();

					compiled = bubbleFn->GetCompileable()->Compile(new CompileArgs(cursor, inParenthesis));

					if(!compiled->GetError())
						break;

					compiled->Ref()->AddRef();
					compiled->Ref()->Release();
					compiled = 0;

					cursor.Restore(backup);
				}
			}
		}
	}

	delete id;

	return compiled ? compiled : new Compiled(new Error(Error::EC_ParseError, cursor.GetPos()));
}


ICompiled * Bubble::Compile_Parenthesis(Cursor & cursor)
{
	ICompiled * compiled = 0;

	if('(' == cursor.Get())
	{
		cursor.Add();

		compiled = Compile_Identifier(cursor, true);

		if(!compiled->GetError())
		{
			if(')' == cursor.Get())
			{
				cursor.Add();
			}
			else
			{
				// error.
				compiled->Ref()->AddRef();
				compiled->Ref()->Release();

				compiled = 0;
			}
		}		
	}

	return compiled ? compiled : new Compiled(new Error(Error::EC_ParseError, cursor.GetPos()));
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


bool Bubble::ToBool(char const * identifier, bool & outValue)
{
	bool isBool = false;

	if(!strcmp("false", identifier))
	{
		isBool = true;
		outValue = false;
	}
	else if(!strcmp("true", identifier))
	{
		isBool = true;
		outValue = true;
	}

	return isBool;
}

bool Bubble::ToInt(char const * identifier, int & outValue)
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

	if(isInt) outValue = intVal;

	return isInt;
}



// Compileable /////////////////////////////////////////////////////////////////

Compileable::Compileable(ICompiler * compiler)
: m_Compiler(compiler)
{
	compiler->Ref()->AddRef();
}

Compileable::~Compileable()
{
	m_Compiler->Ref()->Release();
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


// IntGetterC /////////////////////////////////////////////////////////////////

class IntGetterC : public Ref,
	public IInt
{
public:
	IntGetterC(IIntGetter * intGetter)
	: m_IntGetter(intGetter)
	{
		intGetter->Ref()->AddRef();
	}

	virtual int EvalInt (void)
	{
		return m_IntGetter->Get();
	}

	virtual ::Afx::IRef * Ref (void) { return dynamic_cast<::Afx::IRef *>(this); }

protected:
	virtual ~IntGetterC()
	{
		m_IntGetter->Ref()->Release();
	}

private:
	IIntGetter * m_IntGetter;
};


// IntSetterC /////////////////////////////////////////////////////////////////

class IntSetterC : public Ref,
	public IVoid
{
public:
	IntSetterC(IIntSetter * intSetter, IInt * value)
	: m_IntSetter(intSetter), m_Value(value)
	{
		intSetter->Ref()->AddRef();
		value->Ref()->AddRef();
	}

	virtual void EvalVoid (void)
	{
		m_IntSetter->Set(m_Value->EvalInt());
	}

	virtual ::Afx::IRef * Ref (void) { return dynamic_cast<::Afx::IRef *>(this); }

protected:
	virtual ~IntSetterC()
	{
		m_IntSetter->Ref()->Release();
		m_Value->Ref()->Release();
	}

private:
	IIntSetter * m_IntSetter;
	IInt * m_Value;
};


// IntGetter //////////////////////////////////////////////////////////////////

IntGetter::IntGetter(ICompiler * compiler)
: Compileable(compiler)
{
}

ICompiled * IntGetter::Compile (ICompileArgs * args)
{
	ICompiled * compiled = 0;

	ParseArgs * pa = new ParseArgs(m_Compiler, args);
	pa->Ref()->AddRef();

	if(!pa->HasNextArg())
	{
		compiled = new Compiled(new IntGetterC(this));
	}

	pa->Ref()->Release();

	return compiled ? compiled : new Compiled(new Error());
}


::Afx::IRef * IntGetter::Ref (void)
{
	return dynamic_cast<::Afx::IRef *>(this);
}


// IntProperty ////////////////////////////////////////////////////////////////


IntProperty::IntProperty(ICompiler * compiler, CompileAcces compileAccess)
: Compileable(compiler), m_CompileAccess(compileAccess)
{
}


ICompiled * IntProperty::Compile (ICompileArgs * args)
{
	ICompiled * compiled = 0;

	ParseArgs * pa = new ParseArgs(m_Compiler, args);
	pa->Ref()->AddRef();

	if(
		(CA_Property == m_CompileAccess || CA_Getter == m_CompileAccess)
		&& !pa->HasNextArg()
	)
	{
		// Compile getter:
		compiled = new Compiled(new IntGetterC(this));
	}
	else if(
		(CA_Property == m_CompileAccess || CA_Setter == m_CompileAccess)
		&& pa->ParseNextArgTC(ICompiled::T_Int) && !pa->HasNextArg()
	)
	{
		// Compile setter:
		compiled = new Compiled(new IntSetterC(this, pa->GetArg(0)->GetInt()));
	}

	pa->Ref()->Release();

	return compiled ? compiled : new Compiled(new Error());
}


::Afx::IRef * IntProperty::Ref (void)
{
	return dynamic_cast<::Afx::IRef *>(this); 
}


// IntSetter //////////////////////////////////////////////////////////////////

IntSetter::IntSetter(ICompiler * compiler)
: Compileable(compiler)
{
}

ICompiled * IntSetter::Compile (ICompileArgs * args)
{
	ICompiled * compiled = 0;

	ParseArgs * pa = new ParseArgs(m_Compiler, args);
	pa->Ref()->AddRef();

	if(pa->ParseNextArgTC(ICompiled::T_Int) && !pa->HasNextArg())
	{
		compiled = new Compiled(new IntSetterC(this, pa->GetArg(0)->GetInt()));
	}

	pa->Ref()->Release();

	return compiled ? compiled : new Compiled(new Error());
}


::Afx::IRef * IntSetter::Ref (void)
{
	return dynamic_cast<::Afx::IRef *>(this);
}



// IntVariable ////////////////////////////////////////////////////////////////

IntVariable::IntVariable(ICompiler * compiler, CompileAcces compileAccess, int value)
: IntProperty(compiler, compileAccess), m_Value(value)
{
}

int IntVariable::Get() const
{
	return m_Value;
}

int IntVariable::Get (void)
{
	return m_Value;
}

void IntVariable::Set (int value)
{
	m_Value = value;
}


// VoidFunction ////////////////////////////////////////////////////////////////

VoidFunction::VoidFunction(ICompiler * compiler)
: Compileable(compiler)
{
}

ICompiled * VoidFunction::Compile (ICompileArgs * args)
{
	ICompiled * compiled = 0;

	ParseArgs * pa = new ParseArgs(m_Compiler, args);
	pa->Ref()->AddRef();

	if(!pa->HasNextArg())
	{
		compiled = new Compiled(dynamic_cast<IVoid *>(this));
	}

	pa->Ref()->Release();

	return compiled ? compiled : new Compiled(new Error());
}


::Afx::IRef * VoidFunction::Ref (void)
{
	return dynamic_cast<::Afx::IRef *>(this);
}



// Tools ///////////////////////////////////////////////////////////////

IBubble * Tools::StandardBubble (void)
{
	return new Bubble();
}

