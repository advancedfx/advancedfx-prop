#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-20 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


// Things could be speed up by directly resolving to machine code
// where applicable.


#include "Expressions.h"

#include <ctype.h>
#include <string.h>

#include <exception>
#include <list>

using namespace std;

using namespace Afx;
using namespace Afx::Expressions;


/*

// FnDef ////////////////////////////////////////////////////////////////////////

class FnDefCompileable : public Compileable
{
public:
	FnDefCompileable(ICompiler * compiler, ICompiled * compiled)
	: Compileable(compiler), m_Compiled(compiled)
	{
		compiled->Ref()->AddRef();
	}

	virtual ICompiled * Compile (ICompileArgs * args)
	{
		ICompiled * compiled = 0;

		ParseArgs * pa = new ParseArgs(m_Compiler, args);
		pa->Ref()->AddRef();

		if(pa->ParseEof())
			compiled = m_Compiled;

		pa->Ref()->Release();

		return compiled ? compiled : new Compiled(new Error());
	}

protected:
	virtual ~FnDefCompileable() {
		m_Compiled->Ref()->Release();
	}

private:
	ICompiled * m_Compiled;
};


class FnDef : public Compileable
{
public:
	/// <remarks>defBubble should not be the bubble hosting FnDef, otherwise
	/// you'll create a nice loop in the reference graph, which would prevent
	/// final Release </remarks>
	FnDef(ICompiler * compiler)
    : Compileable(compiler)
	{
		m_DefBubble = IBubble::New(false);

		m_DefBubble->Ref()->AddRef();

	}

	virtual ICompiled * Compile (ICompileArgs * args);

	void Define(IString * name, ICompiled * compiled)
	{
		name->Ref()->AddRef();
		compiled->Ref()->AddRef();

		IStringValue * nameVal = name->EvalString();
		nameVal->Ref()->AddRef();

		m_DefBubble->Hide(nameVal->GetData());

		m_DefBubble->Add(nameVal->GetData(), new FnDefCompileable(m_DefBubble->Compiler(), compiled));

		nameVal->Ref()->Release();
		compiled->Ref()->Release();
		name->Ref()->Release();
	}

	ICompiler * GetDefCompiler (void)
	{
		return m_DefBubble->Compiler();
	}

	void Undefine(IString * name)
	{
		name->Ref()->AddRef();

		IStringValue * nameVal = name->EvalString();
		nameVal->Ref()->AddRef();

		m_DefBubble->Hide(nameVal->GetData());

		nameVal->Ref()->Release();
		name->Ref()->Release();
	}

protected:
	virtual ~FnDef() {
		m_DefBubble->Ref()->Release();
	}

private:
	IBubble * m_DefBubble;
};


class FnDefDefineVoidC : public Ref,
	public IVoid
{
public:
	FnDefDefineVoidC(FnDef * fnDef, IString * name, IVoid * val)
	: m_FnDef(fnDef), m_Name(name), m_Value(val)
	{
		fnDef->AddRef();
		name->Ref()->AddRef();
		val->Ref()->AddRef();
	}

	virtual VoidT EvalVoid (void)
	{
		m_FnDef->Define(m_Name, new Compiled(m_Value));
	}

	virtual IRef * Ref (void) { return this; }

protected:
	virtual ~FnDefDefineVoidC()
	{
		m_Value->Ref()->Release();
		m_Name->Ref()->Release();
		m_FnDef->Release();
	}

private:
	FnDef * m_FnDef;
	IString * m_Name;
	IVoid * m_Value;
};


class FnDefUndefineC : public Ref,
	public IVoid
{
public:
	FnDefUndefineC(FnDef * fnDef, IString * name)
	: m_FnDef(fnDef), m_Name(name)
	{
		fnDef->AddRef();
		name->Ref()->AddRef();
	}

	virtual VoidT EvalVoid (void)
	{
		m_FnDef->Undefine(m_Name);
	}

	virtual IRef * Ref (void) { return this; }

protected:
	virtual ~FnDefUndefineC()
	{
		m_Name->Ref()->Release();
		m_FnDef->Release();
	}

private:
	FnDef * m_FnDef;
	IString * m_Name;
};


ICompiled * FnDef::Compile (ICompileArgs * args)
{
	ICompiled * compiled = 0;

	ParseArgs * pa = new ParseArgs(m_Compiler, args);
	pa->Ref()->AddRef();

	if(
		pa->ParseNextArgTC(ICompiled::T_String)
	)
	{
		ICompiled::Type type = pa->ParseNextArgTE();

		switch(type)
		{
		case ICompiled::T_Eof:
			compiled = new Compiled(new FnDefUndefineC(this, pa->GetArg(0)->GetString()));
			break;
		case ICompiled::T_Void:
			compiled = new Compiled(new FnDefDefineVoidC(this, pa->GetArg(0)->GetString(), pa->GetArg(1)->GetVoid()));
			break;
		default:
			break;
		}
	}


	pa->Ref()->Release();

	return compiled ? compiled : new Compiled(new Error());
}

ICompileable * Afx::Expressions::NewCompileableFnDef(ICompiler * compiler, ICompiler * & outDefCompiler)
{
	FnDef * fnDef = new FnDef(compiler);

	outDefCompiler = fnDef->GetDefCompiler();

	return fnDef;
}

*/
