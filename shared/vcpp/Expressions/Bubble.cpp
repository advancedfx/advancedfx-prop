#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2011-01-03 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


#include "Bubble.h"

#include "Functions.h"

#include <ctype.h>
#include <string.h>

#include <exception>
#include <vector>


using namespace std;

using namespace Afx;
using namespace Afx::Expressions;


class BubbleEntry : public Ref,
{
public:
	BubbleEntry(ICallCompiler * compiler, IGetArgsCompiler * getArgsCompiler)
	: m_Compiler(compiler), m_GetArgsCompiler(getArgsCompiler)
	{
	}

	virtual ICompiled * Compile (IStringValue * value)
	{
		ICompiled * compiled = 0;

		//
#pragma error(TODO)

		// TODO: split into identifier and argument (call) string,
		// compile args string,
		// upon success call

		//

		

		return compiled ? compiled : new Compiled(new Error());
	}

	virtual IRef * Ref (void)
	{
		return this;
	}

private:
	RefIPtr<ICallCompiler> m_Compiler;
	RefIPtr<IGetArgsCompiler> m_GetArgsCompiler;
};


class BubbleCompilers : public Ref
{
public:
	BubbleCompilers()
	{
	}

	void Add (ICallCompiler * compiler)
	{
		compiler->Ref()->AddRef();

		m_Fns.push_back(compiler);
	}

	ICallCompiler * GetAt(int index)
	{
		return 0 <= index && index < m_Fns.size()
			? m_Fns.at(m_Fns.begin() +index)
			: 0
		;
	}

	int GetCount (void)
	{
		return m_Fns.size();
	}

	void ShutDown (void)
	{
		for(FnListT::iterator it = m_Fns.begin(); it != m_Fns.end(); )
		{
			(*it)->Ref()->Release();
			it = m_Fns.erase(it);
		}
	}

protected:
	virtual ~BubbleCompilers()
	{
		ShutDown();
	}

private:
	typedef vector<ICallCompiler *> FnListT;

	FnListT m_Fns;

};

class BubbleArgsCompiler : public Ref,
	public IArgsCompiler
{
public:
	BubbleArgsCompiler(BubbleCompilers * bubbleCompilers, IRef * dependency

};


// Bubble //////////////////////////////////////////////////////////////////////

class Bubble : public Ref,
	public IBubble
{
public:
	Bubble()
	: m_BubbleCompilers(new BubbleCompilers())
	{
	}

	virtual void Add(ICallCompiler * compiler)
	{
		m_BubbleCompilers.get()->Add(compiler);
	}

	virtual IRef * Ref (void) {
		return this;
	}

protected:
	virtual ~Bubble()
	{
		m_BubbleCompilers.get()->ShutDown();
	}

private:
	RefPtr<BubbleCompilers> m_BubbleCompilers;
};



// IBubble ///////////////////////////////////////////////////////////////

void IBubble::AddStandardFunctions (IBubble * bubble)
{
	bubble->Ref()->AddRef();

	StringValueRef strEmpty(StringValue::CopyFrom(""));

	bubble->Add(strEmpty.get(), new BoolCompiler());
	bubble->Add(strEmpty.get(), new IntCompiler());
	bubble->Add(strEmpty.get(), new FloatCompiler());

	bubble->Add(StringValue::CopyFrom("null"), FunctionCompilers::Null(bubble->Compiler()));
	bubble->Add(StringValue::CopyFrom("void"), FunctionCompilers::Void(bubble->Compiler()));

	bubble->Add(StringValue::CopyFrom("string"), new StringTextCompiler());
	bubble->Add(StringValue::CopyFrom("s"), new StringTextCompiler());

	bubble->Add(StringValue::CopyFrom("and"), FunctionCompilers::And(bubble->Compiler()));
	bubble->Add(StringValue::CopyFrom("&&"), FunctionCompilers::And(bubble->Compiler()));

	bubble->Add(StringValue::CopyFrom("or"), FunctionCompilers::Or(bubble->Compiler()));
	bubble->Add(StringValue::CopyFrom("||"), FunctionCompilers::Or(bubble->Compiler()));

	bubble->Add(StringValue::CopyFrom("not"), FunctionCompilers::Not(bubble->Compiler()));
	bubble->Add(StringValue::CopyFrom("!"), FunctionCompilers::Not(bubble->Compiler()));

	bubble->Add(StringValue::CopyFrom("less"), FunctionCompilers::Less(bubble->Compiler()));
	bubble->Add(StringValue::CopyFrom("<"), FunctionCompilers::Less(bubble->Compiler()));

	bubble->Add(StringValue::CopyFrom("lessOrEqual"), FunctionCompilers::LessOrEqual(bubble->Compiler()));
	bubble->Add(StringValue::CopyFrom("<="), FunctionCompilers::LessOrEqual(bubble->Compiler()));

	bubble->Add(StringValue::CopyFrom("equal"), FunctionCompilers::Equal(bubble->Compiler()));
	bubble->Add(StringValue::CopyFrom("=="), FunctionCompilers::Equal(bubble->Compiler()));

	bubble->Add(StringValue::CopyFrom("greater"), FunctionCompilers::Greater(bubble->Compiler()));
	bubble->Add(StringValue::CopyFrom(">"), FunctionCompilers::Greater(bubble->Compiler()));

	bubble->Add(StringValue::CopyFrom("greaterOrEqual"), FunctionCompilers::GreaterOrEqual(bubble->Compiler()));
	bubble->Add(StringValue::CopyFrom(">="), FunctionCompilers::GreaterOrEqual(bubble->Compiler()));

	bubble->Add(StringValue::CopyFrom("in"), FunctionCompilers::InBool(bubble->Compiler()));
	bubble->Add(StringValue::CopyFrom("in"), FunctionCompilers::InInt(bubble->Compiler()));

	bubble->Add(StringValue::CopyFrom("max"), FunctionCompilers::MaxBool(bubble->Compiler()));
	bubble->Add(StringValue::CopyFrom("max"), FunctionCompilers::MaxInt(bubble->Compiler()));

	bubble->Add(StringValue::CopyFrom("min"), FunctionCompilers::MinBool(bubble->Compiler()));
	bubble->Add(StringValue::CopyFrom("min"), FunctionCompilers::MinInt(bubble->Compiler()));

	bubble->Add(StringValue::CopyFrom("if"), FunctionCompilers::If(bubble->Compiler()));
	bubble->Add(StringValue::CopyFrom("?"), FunctionCompilers::If(bubble->Compiler()));

	bubble->Add(StringValue::CopyFrom("do"), FunctionCompilers::Do(bubble->Compiler()));
	bubble->Add(StringValue::CopyFrom("."), FunctionCompilers::Do(bubble->Compiler()));

	bubble->Add(StringValue::CopyFrom("sum"), FunctionCompilers::Sum(bubble->Compiler()));
	bubble->Add(StringValue::CopyFrom("+"), FunctionCompilers::Sum(bubble->Compiler()));

	bubble->Add(StringValue::CopyFrom("stringFromFile"), FunctionCompilers::StringFromFile(bubble->Compiler()));

	bubble->Add(StringValue::CopyFrom("ceval"), FunctionCompilers::CompileEval(bubble->Compiler()));

	bubble->Ref()->Release();
}


IBubble * IBubble::New ()
{
	return new Bubble();
}

