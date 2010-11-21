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
#include <vector>

using namespace std;

using namespace Afx;
using namespace Afx::Expressions;


// BubbleFn ////////////////////////////////////////////////////////////////////

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


// Bubble //////////////////////////////////////////////////////////////////////



class Bubble : public Ref,
	public IBubble,
	public ICompiler
{
public:
	Bubble(bool standardConstFunctions);

	virtual void Add(char const * name, ICompileable * compileable);

	virtual void Hide(char const * name);

	virtual ICompiled * Compile(char const * code);

	virtual ICompiled * Compile_Function (Cursor & cursor);

	virtual ICompiler * Compiler (void);

	virtual ::Afx::IRef * Ref (void);

protected:
	virtual ~Bubble();

private:
	typedef list<BubbleFn *> FunctionList;

	FunctionList m_Functions;

	bool m_StandardConstFunctions;

	ICompiled * Compile_Code(Cursor & cursor);

	ICompiled * Compile_Identifier(Cursor & cursor, bool inParenthesis);

	ICompiled * Compile_Parenthesis(Cursor & cursor);

	static bool IsIdentifierChar(char val);

	char * New_Identifier(Cursor & cursor);

	bool ToBool(Cursor & cur, BoolT & outValue);

};


// Bubble //////////////////////////////////////////////////////////////////////


Bubble::Bubble(bool standardConstFunctions)
: m_StandardConstFunctions(standardConstFunctions)
{
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

void Bubble::Hide(char const * name)
{
	for(FunctionList::iterator it = m_Functions.begin(); it != m_Functions.end(); )
	{
		BubbleFn * bubbleFn = *it;

		if(!strcmp(name, bubbleFn->GetName()))
		{
			delete bubbleFn;

			it = m_Functions.erase(it);
		}
		else it++;
	}
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

			compiled = new Compiled(new Error(Error::EC_ParseError, cursor));
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

	if(m_StandardConstFunctions && !compiled)
	{
		// Bool?

		CursorBackup backup = cursor.Backup();

		BoolT bVal;
		if(ToBool(cursor, bVal))
		{

			compiled = FunctionsC::Bool(this, new CompileArgs(cursor, inParenthesis), bVal);
			if(compiled->GetError())
			{
				Ref::TouchRef(compiled->Ref());
				compiled = 0;
			}
		}

		if(!compiled) cursor.Restore(backup);
	}

	if(m_StandardConstFunctions && !compiled)
	{
		// Int?

		CursorBackup backup = cursor.Backup();

		int skipped;
		IntT iVal = cursor.ReadLong(&skipped);

		if(0 < skipped && !IsIdentifierChar(cursor.Get()))
		{
			compiled = FunctionsC::Int(this, new CompileArgs(cursor, inParenthesis), iVal);
			if(compiled->GetError())
			{
				Ref::TouchRef(compiled->Ref());
				compiled = 0;
			}
		}

		if(!compiled) cursor.Restore(backup);
	}

	if(m_StandardConstFunctions && !compiled)
	{
		// Float?

		CursorBackup backup = cursor.Backup();

		int skipped;
		FloatT fVal = cursor.ReadDouble(&skipped);

		if(0 < skipped && !IsIdentifierChar(cursor.Get()))
		{
			compiled = FunctionsC::Float(this, new CompileArgs(cursor, inParenthesis), fVal);
			if(compiled->GetError())
			{
				Ref::TouchRef(compiled->Ref());
				compiled = 0;
			}
		}

		if(!compiled) cursor.Restore(backup);
	}

	if(!compiled)
	{
		// some function?

		char * id = New_Identifier(cursor);

		if(id)
		{
			if(inParenthesis) cursor.SkipSpace();


			for(FunctionList::iterator it = m_Functions.begin(); it != m_Functions.end(); it++)
			{
				BubbleFn * bubbleFn = *it;

				if(!strcmp(id, bubbleFn->GetName()))
				{
					CursorBackup backup = cursor.Backup();

					compiled = bubbleFn->GetCompileable()->Compile(new CompileArgs(cursor, inParenthesis));

					if(!compiled->GetError())
						break;

					Ref::TouchRef(compiled->Ref());
					compiled = 0;

					cursor.Restore(backup);
				}
			}
		}

		delete id;
	}

	return compiled ? compiled : new Compiled(new Error(Error::EC_ParseError, cursor));
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
				Ref::TouchRef(compiled->Ref());
				compiled = 0;
			}
		}		
	}

	return compiled ? compiled : new Compiled(new Error(Error::EC_ParseError, cursor));
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

bool Bubble::ToBool(Cursor & cur, BoolT & outValue)
{
	bool isBool = false;
	BoolT bVal;
	int lenBool;

	if(!isBool)
	{
		char const * textBool = "false";
		lenBool = strlen(textBool);
		bVal = false;
		
		isBool = true;
		for(int i=0; i<lenBool && isBool; i++) isBool = isBool && textBool[i] == cur.Get(i);
		isBool = isBool && !IsIdentifierChar(cur.Get(lenBool));
	}

	if(!isBool)
	{
		char const * textBool = "true";
		lenBool = strlen(textBool);
		bVal = true;
		
		isBool = true;
		for(int i=0; i<lenBool && isBool; i++) isBool = isBool && textBool[i] == cur.Get(i);
		isBool = isBool && !IsIdentifierChar(cur.Get(lenBool));
	}

	if(isBool)
	{
		outValue = bVal;
		cur.Seek(lenBool);
	}

	return isBool;
}


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


// IBubble ///////////////////////////////////////////////////////////////

void IBubble::AddStandardFunctions (IBubble * bubble)
{
	bubble->Ref()->AddRef();

	bubble->Add("null", FunctionsC::Null(bubble->Compiler()));
	bubble->Add("void", FunctionsC::Void(bubble->Compiler()));

	bubble->Add("string", FunctionsC::String(bubble->Compiler()));
	bubble->Add("s", FunctionsC::String(bubble->Compiler()));

	bubble->Add("and", FunctionsC::And(bubble->Compiler()));
	bubble->Add("&&", FunctionsC::And(bubble->Compiler()));

	bubble->Add("or", FunctionsC::Or(bubble->Compiler()));
	bubble->Add("||", FunctionsC::Or(bubble->Compiler()));

	bubble->Add("not", FunctionsC::Not(bubble->Compiler()));
	bubble->Add("!", FunctionsC::Not(bubble->Compiler()));

	bubble->Add("less", FunctionsC::Less(bubble->Compiler()));
	bubble->Add("<", FunctionsC::Less(bubble->Compiler()));

	bubble->Add("lessOrEqual", FunctionsC::LessOrEqual(bubble->Compiler()));
	bubble->Add("<=", FunctionsC::LessOrEqual(bubble->Compiler()));

	bubble->Add("equal", FunctionsC::Equal(bubble->Compiler()));
	bubble->Add("==", FunctionsC::Equal(bubble->Compiler()));

	bubble->Add("greater", FunctionsC::Greater(bubble->Compiler()));
	bubble->Add(">", FunctionsC::Greater(bubble->Compiler()));

	bubble->Add("greaterOrEqual", FunctionsC::GreaterOrEqual(bubble->Compiler()));
	bubble->Add(">=", FunctionsC::GreaterOrEqual(bubble->Compiler()));

	bubble->Add("in", FunctionsC::InBool(bubble->Compiler()));
	bubble->Add("in", FunctionsC::InInt(bubble->Compiler()));

	bubble->Add("max", FunctionsC::MaxBool(bubble->Compiler()));
	bubble->Add("max", FunctionsC::MaxInt(bubble->Compiler()));

	bubble->Add("min", FunctionsC::MinBool(bubble->Compiler()));
	bubble->Add("min", FunctionsC::MinInt(bubble->Compiler()));

	bubble->Add("if", FunctionsC::If(bubble->Compiler()));
	bubble->Add("?", FunctionsC::If(bubble->Compiler()));

	bubble->Add("do", FunctionsC::Do(bubble->Compiler()));
	bubble->Add(".", FunctionsC::Do(bubble->Compiler()));

	bubble->Add("sum", FunctionsC::Sum(bubble->Compiler()));
	bubble->Add("+", FunctionsC::Sum(bubble->Compiler()));

	bubble->Add("stringFromFile", FunctionsC::StringFromFile(bubble->Compiler()));

	bubble->Add("ceval", FunctionsC::CompileEval(bubble->Compiler()));

	bubble->Ref()->Release();
}


IBubble * IBubble::New (bool standardConstFunctions)
{
	return new Bubble(standardConstFunctions);
}

