#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-12-15 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


#include "Bubble.h"

#include "Functions.h"

#include <ctype.h>
#include <string.h>

#include <exception>
#include <list>


using namespace std;

using namespace Afx;
using namespace Afx::Expressions;


// BubbleFn ////////////////////////////////////////////////////////////////////

class BubbleFn : public Ref,
	public ICompiler
{
public:
	BubbleFn(IStringValue * stringValue, ICompiler * compiler)
	: m_Compiler(compiler), m_StringValue(stringValue)
	{
	}

	virtual ICompiled * Compile (Cursor * cursor)
	{
		ICompiled * compiled = 0;
		CursorRef curRef(cursor);

		if(1 < m_StringValue.get()->GetLength())
		{
			// named:
			StringValueRef strRef(curRef.get()->ReadStringValue());

			if(!strcmp(m_StringValue.getData(), strRef.getData()))
			{
				curRef.get()->SkipSpace();

				compiled = m_Compiler.get()->Compile(cursor);
			}
		}
		else
		{
			// unnamed:
			compiled = m_Compiler.get()->Compile(cursor);
		}

		return compiled ? compiled : new Compiled(new Error(Error::EC_ParseError, curRef.get()));
	}

	ICompiler * Compiler (void) const {
		return m_Compiler.get();
	}

	IStringValue * Name (void) const {
		return m_StringValue.get();
	}

	virtual IRef * Ref (void) {
		return this;
	}

private:
	RefIPtr<ICompiler> m_Compiler;
	StringValueRef m_StringValue;
};


// BubbleCompiler //////////////////////////////////////////////////////////////

class BubbleCompiler : public Ref,
	public ICompiler
{
public:
	BubbleCompiler()
	{
	}

	void Add(IStringValue * name, ICompiler * compiler)
	{
		BubbleFn * bubbleFn = new BubbleFn(name, compiler);
		bubbleFn->AddRef();

		m_Fns.push_back(bubbleFn);
	}

	virtual ICompiled * Compile(Cursor * cursor)
	{
		ICompiled * compiled = 0;

		CursorRef curRef(cursor);
		CursorBackup curBackup(curRef.get()->Backup());

		for(FnListT::iterator it = m_Fns.begin(); it != m_Fns.end() && !compiled; it++)
		{
			ICompiled * curCompiled = (*it)->Compile(curRef.get());

			if(curCompiled->GetError())
			{
				Ref::TouchRef(curCompiled->Ref());
				curRef.get()->Restore(curBackup);
			}
			else {
				compiled = curCompiled;
				break;
			}
		}

		return compiled ? compiled : new Compiled(new Error());
	}

	void Hide(IStringValue * name)
	{
		for(FnListT::iterator it = m_Fns.begin(); it != m_Fns.end(); )
		{
			if(!strcmp((*it)->Name()->GetData(), name->GetData()))
			{
				BubbleFn * bubbleFn = (*it);
				it = m_Fns.erase(it);
				bubbleFn->Release();
			}
			else it++;
		}
	}

	virtual IRef * Ref (void) {
		return this;
	}

	void ShutDown (void)
	{
		for(FnListT::iterator it = m_Fns.begin(); it != m_Fns.end(); )
		{
			(*it)->Release();
			it = m_Fns.erase(it);
		}
	}

protected:
	virtual ~BubbleCompiler()
	{
		ShutDown();
	}

private:
	typedef list<BubbleFn *> FnListT;

	FnListT m_Fns;

};


// Bubble //////////////////////////////////////////////////////////////////////

class Bubble : public Ref,
	public IBubble,
	public ICompiler
{
public:
	Bubble()
	: m_BubbleCompiler(new BubbleCompiler())
	{
	}

	virtual void Add(IStringValue * name, ICompiler * compiler)
	{
		m_BubbleCompiler.get()->Add(name, compiler);
	}

	virtual ICompiler * Compiler (void) {
		return m_BubbleCompiler.get();
	}

	virtual ICompiled * Compile(Cursor * cursor)
	{
		return m_BubbleCompiler.get()->Compile(cursor);
	}

	virtual ICompiled * Compile(IStringValue *  code)
	{
		return Compile(new Cursor(code));
	}

	virtual void Hide(IStringValue * name)
	{
		m_BubbleCompiler.get()->Hide(name);
	}

	virtual ICompiler * OuterCompiler (void) {
		return this;
	}

	virtual IRef * Ref (void) {
		return this;
	}

protected:
	virtual ~Bubble()
	{
		m_BubbleCompiler.get()->ShutDown();
	}

private:
	RefPtr<BubbleCompiler> m_BubbleCompiler;
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

