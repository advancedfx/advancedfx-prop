#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-16 dominik.matrixstorm.com
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

#if 0
#include <sstream>
#define AFX_XPRESS_MDEBUG
#endif

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

	bool ToBool(Cursor & cur, BoolT & outValue);

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
	Error(ErrorCode errorCode, Cursor & cur) {

#ifdef AFX_XPRESS_MDEBUG
		ostringstream errText("Error #");

		errText << (int)errorCode;

		errText << " at ";

		errText << (int)cur.GetPos();

		errText << " (...\"";

		for(int i=-16; i<0; i++)
		{
			char val = cur.Get(i);

			if(!Cursor::IsNull(val)) errText << val;
		}

		errText << "\" [> \"";

		if(!cur.IsNull()) errText << cur.Get();

		errText << "\" <] \"";

		for(int i=1; i<16; i++)
		{
			char val = cur.Get(i);

			if(!Cursor::IsNull(val)) errText << val;
		}


		errText << "\"...).";

		MessageBox(0, errText.str().c_str(), "Afx::Expression::Error(ErrorCode, Cursor &)", MB_OK|MB_ICONINFORMATION);
#endif

	}

	virtual ::Afx::IRef * Ref (void) { return dynamic_cast<::Afx::IRef *>(this); }

protected:


private:


};


// FnEof ///////////////////////////////////////////////////////////////////////

class FnEof : public Ref,
	public IEof
{
public:
	virtual ::Afx::IRef * Ref (void) {
		return dynamic_cast<::Afx::IRef *>(this);
	}
};


class CompileArgs : public Ref,
	public ICompileArgs
{
public:
	CompileArgs(Cursor & cursor, bool inParenthesis)
	: m_Cursor(cursor), m_Eof(false), m_MoreArgs(inParenthesis)
	{
	}

	virtual ICompiled * CompileNextArg (ICompiler * compiler)
	{
		ICompiled * compiled = 0;

		compiler->Ref()->AddRef();

		if(m_MoreArgs)
		{
			if(')' == m_Cursor.Get())
			{
				m_MoreArgs = false;
			}
			else
			{
				compiled = compiler->Compile_Function(m_Cursor);
				if(!compiled->GetError())
				{
					m_Cursor.SkipSpace();
				}
			}
		}

		if(!compiled && !m_Eof)
		{
			compiled =  new Compiled(new FnEof());
		}

		compiler->Ref()->Release();

		return compiled ? compiled : new Compiled(new Error(Error::EC_ParseError, m_Cursor));
	}

	virtual ::Afx::IRef * Ref (void)
	{
		return dynamic_cast<::Afx::IRef *>(this);
	}

private:
	Cursor & m_Cursor;
	bool m_Eof;
	bool m_MoreArgs;
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

	bool ParseEof (void)
	{
		ICompiled * nextArg = ParseNextArg_Internal();

		nextArg->Ref()->AddRef();

		bool isEof = 0 != nextArg->GetEof();

		nextArg->Ref()->Release();

		return isEof;
	}

	/// <summary>Parses the next argument, references it in the arg list and returns it.</summary>
	/// <remarks>Null is skipped.</remarks>
	ICompiled * ParseNextArg(void)
	{
		ICompiled * compiled = ParseNextArg_Internal();

		compiled->Ref()->AddRef();
		m_Compileds.push_back(compiled);

		return compiled;
	}

	/// <summary>Parses the next argument, references it in the arg list and returns it's type.</summary>
	/// <remarks>Null is skipped.</remarks>
	ICompiled::Type ParseNextArgT (void)
	{
		return ParseNextArg()->GetType();
	}

	/// <summary>Parses the next argument, references it in the arg list only if it's not Eof and returns it's type.</summary>
	/// <remarks>Null is skipped.</remarks>
	ICompiled::Type ParseNextArgTE (void)
	{
		ICompiled * compiled = ParseNextArg_Internal();

		compiled->Ref()->AddRef();

		ICompiled::Type type = compiled->GetType();

		if(ICompiled::T_Eof == type)
		{
			compiled->Ref()->Release();
		}
		else
		{
			m_Compileds.push_back(compiled);
		}

		return type;
	}

	/// <summary>Parses the next argument, references it in the arg list and returns if it matches a given type.</summary>
	/// <remarks>Null is skipped.</remarks>
	bool ParseNextArgTC (ICompiled::Type type)
	{
		return type == ParseNextArgT();
	}

	/// <summary>Parses the next argument, references it in the arg list only if it's not Eof and returns if it matches a given type.</summary>
	/// <remarks>Null is skipped.</remarks>
	bool ParseNextArgTCE (ICompiled::Type type)
	{
		return type == ParseNextArgTE();
	}

	/// <summary>Parses the next argument, references it in the arg list only if it's not Eof.</summary>
	/// <remarks>Null is skipped.</remarks>
	/// <param name="type">type to match</param>
	/// <param name="outMatchedOrEof">if type is matched or Eof</param>
	/// <returns>if not Eof</returns>
	bool ParseNextArgTCEX(ICompiled::Type type, bool & outMatchedOrEof)
	{
		ICompiled::Type curType = ParseNextArgTE();

		outMatchedOrEof = type == curType || ICompiled::T_Eof == curType;

		return ICompiled::T_Eof != curType;
	}


	::Afx::IRef * Ref (void) {
		return dynamic_cast<::Afx::IRef *>(this);
	}

	void SetArg(int index, ICompiled * value) {
		m_Compileds[index] = value;
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

	ICompiled * ParseNextArg_Internal(void)
	{
		ICompiled * compiled;
		bool bSkip;
		
		do {
			compiled = m_Args->CompileNextArg(m_Compiler);

			bSkip = 0 != compiled->GetNull();

			if(bSkip)
			{
				Ref::TouchRef(compiled->Ref());
			}

		} while(bSkip);

		return compiled;
	}
	
};




// FnString ////////////////////////////////////////////////////////////////////

class FnString : public Ref,
	public IString
{
public:
	FnString(IStringValue * stringValue)
	: m_StringValue(stringValue)
	{
		stringValue->Ref()->AddRef();
	}

	virtual ::Afx::IRef * Ref (void) {
		return dynamic_cast<::Afx::IRef *>(this);
	}

	virtual IStringValue * EvalString (void) {
		return m_StringValue;
	}

protected:
	~FnString()
	{
		m_StringValue->Ref()->Release();
	}

private:
	IStringValue * m_StringValue;
};



// StringValue /////////////////////////////////////////////////////////////////

class StringValue : public Ref,
	public IStringValue
{
public:
	/// <returns>0 on fail, otherwise StringValue object</returns>
	static StringValue * TryRead(Cursor & cur)
	{
		char * data = ReadString(cur);

		if(data)
		{
			int length = strlen(data);

			return new StringValue(length, data);
		}

		return 0;
	}

	StringValue(int length, char * data) : m_Length(length), m_Data(data)
	{
	}

	virtual StringDataT GetData (void) {
		return m_Data;
	}

	virtual int GetLength (void) {
		return m_Length;
	}

	virtual ::Afx::IRef * Ref (void) {
		return dynamic_cast<::Afx::IRef *>(this);
	}

protected:
	virtual ~StringValue()
	{
		delete m_Data;
	}

private:
	int m_Length;
	char * m_Data;

	static char * ReadString(Cursor & cur)
	{
		char * data = 0;
		int length;
		bool escaped;
		bool done;
		char val;

		CursorBackup backup(cur.Backup());

		do {
			escaped = false;
			length = 0;
			int brackets = 0;

			cur.Restore(backup);

			while((val = cur.Get()), !Cursor::IsNull(val) && (escaped || 0 < brackets || val != ')'))
			{
				cur.Add();

				if(!escaped && '\\' == val)
				{
					escaped = true;
				}
				else {
					escaped = false;
					
					switch(val)
					{
					case '(':
						brackets++;
						break;
					case ')':
						brackets--;
						break;
					default:
						break;
					};

					if(data) data[length] = val;
					length++;
				}
			}

			if(!data && !escaped)
			{
				// not done yet, but we know the length now:
				data = new char[1+length];
				length = 0;
				done = false;
			}
			else if(data) {
				// ok and done.
				data[length] = 0;
				done = true;
			}
			else {
				// error and done.
				done = true;
			}
		} while(!done);

		return data;
	}
};


// StringCompileable //////////////////////////////////////////////////////////////

class StringCompileable : public Ref,
	public ICompiler,
	public ICompileable
{
public:
	virtual ICompiled * Compile_Function (Cursor & cursor) {
		StringValue * stringValue = StringValue::TryRead(cursor);

		return stringValue ? new Compiled(new FnString(stringValue)) : new Compiled(new Error(Error::EC_ParseError, cursor));
	}

	virtual ICompiled * Compile (ICompileArgs * args)
	{
		args->Ref()->AddRef();

		ICompiled * compiled = args->CompileNextArg(this);

		args->Ref()->Release();

		return compiled;
	}

	virtual ::Afx::IRef * Ref (void) {
		return dynamic_cast<::Afx::IRef *>(this);
	}
};


////////////////////////////////////////////////////////////////////////////////


class FnCompileable : public Compileable
{
public:
	typedef ICompiled * (* CompileFn) (ICompiler * compiler, ICompileArgs * args);

	FnCompileable(ICompiler * compiler, CompileFn compileFn)
	: Compileable(compiler), m_CompileFn(compileFn)
	{}

	virtual ICompiled * Compile (ICompileArgs * args) { return (*m_CompileFn)(m_Compiler, args); }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

private:
	CompileFn m_CompileFn;
};



class FnConstBool : public Ref,
	public IBool
{
public:
	static ICompiled * Compile (ICompiler * compiler, ICompileArgs * args, BoolT value)
	{
		ICompiled * compiled = 0;

		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		if(pa->ParseEof())
		{
			compiled = new Compiled(new FnConstBool(value));
		}

		pa->Ref()->Release();

		return compiled ? compiled : new Compiled(new Error());
	}

	FnConstBool(BoolT value) : m_Value(value) {}

	virtual BoolT EvalBool (void) { return m_Value; }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

private:
	BoolT m_Value;
};


class FnConstBoolCompileable : public Compileable
{
public:
	FnConstBoolCompileable(ICompiler * compiler, BoolT value)
	: Compileable(compiler), m_Value(value)
	{
	}

	virtual ICompiled * Compile (ICompileArgs * args)
	{
		return FnConstBool::Compile(m_Compiler, args, m_Value);
	}

	virtual ::Afx::IRef * Ref (void) { return dynamic_cast<::Afx::IRef *>(this); }	

private:
	BoolT m_Value;
};


class FnConstInt : public Ref,
	public IInt
{
public:
	static ICompiled * Compile (ICompiler * compiler, ICompileArgs * args, IntT value)
	{
		ICompiled * compiled = 0;

		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		if(pa->ParseEof())
		{
			compiled = new Compiled(new FnConstInt(value));
		}

		pa->Ref()->Release();

		return compiled ? compiled : new Compiled(new Error());
	}


	FnConstInt(IntT value) : m_Value(value) {}

	virtual IntT EvalInt (void) { return m_Value; }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

private:
	IntT m_Value;
};


class FnConstIntCompileable : public Compileable
{
public:
	FnConstIntCompileable(ICompiler * compiler, IntT value)
	: Compileable(compiler), m_Value(value)
	{
	}

	virtual ICompiled * Compile (ICompileArgs * args)
	{
		return FnConstInt::Compile(m_Compiler, args, m_Value);
	}

	virtual ::Afx::IRef * Ref (void) { return dynamic_cast<::Afx::IRef *>(this); }	

private:
	IntT m_Value;
};


class FnConstFloat : public Ref,
	public IFloat
{
public:
	static ICompiled * Compile (ICompiler * compiler, ICompileArgs * args, FloatT value)
	{
		ICompiled * compiled = 0;

		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		if(pa->ParseEof())
		{
			compiled = new Compiled(new FnConstFloat(value));
		}

		pa->Ref()->Release();

		return compiled ? compiled : new Compiled(new Error());
	}


	FnConstFloat(FloatT value) : m_Value(value) {}

	virtual FloatT EvalFloat (void) { return m_Value; }

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

private:
	FloatT m_Value;
};


class FnConstFloatCompileable : public Compileable
{
public:
	FnConstFloatCompileable(ICompiler * compiler, FloatT value)
	: Compileable(compiler), m_Value(value)
	{
	}

	virtual ICompiled * Compile (ICompileArgs * args)
	{
		return FnConstFloat::Compile(m_Compiler, args, m_Value);
	}

	virtual ::Afx::IRef * Ref (void) { return dynamic_cast<::Afx::IRef *>(this); }	

private:
	FloatT m_Value;
};



class FnBoolsToBoolBase abstract : public Ref,
	public IBool
{
public:
	virtual BoolT EvalBool (void) abstract;

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
	virtual BoolT EvalBool (void) abstract;

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
	virtual IntT EvalInt (void) abstract;

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

		while(bOk && pa->ParseNextArgTCEX(ICompiled::T_Bool, bOk));

		ICompiled * compiled = bOk
			? new Compiled(new FnAnd(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

		return compiled;
	}

	virtual BoolT EvalBool (void) {

		BoolT result = true;

		for(int i=0; i<m_Count && result; i++)
		{
			result = result && m_Fns[i]->EvalBool();
		}

		return result;
	}

private:
	FnAnd(ParseArgs * args) : FnBoolsToBoolBase(args) {}
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

		while(bOk && pa->ParseNextArgTCEX(ICompiled::T_Bool, bOk));

		ICompiled * compiled = bOk
			? new Compiled(new FnOr(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

		return compiled;
	}

	virtual BoolT EvalBool (void) {

		BoolT result = false;

		for(int i=0; i<m_Count  && !result; i++)
		{
			result = result || m_Fns[i]->EvalBool();
		}

		return result;
	}

private:
	FnOr(ParseArgs * args) : FnBoolsToBoolBase(args) {}
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
			&& pa->ParseEof()
		;

		ICompiled * compiled = bOk
			? new Compiled(new FnNot(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

		return compiled;
	}

	virtual BoolT EvalBool (void) {

		return !m_Fns[0]->EvalBool();
	}

private:
	FnNot(ParseArgs * args) : FnBoolsToBoolBase(args) {}
};


class FnCompare : public Ref,
	public IBool
{
public:
	enum Operation
	{
		O_Less,
		O_LessOrEqual,
		O_Equal,
		O_GreaterOrEqual,
		O_Greater
	};

	static ICompiled * CompileLess(ICompiler * compiler, ICompileArgs * args) { return Compile(compiler, args, O_Less); }
	static ICompiled * CompileLessOrEqual(ICompiler * compiler, ICompileArgs * args) { return Compile(compiler, args, O_LessOrEqual); }
	static ICompiled * CompileEqual(ICompiler * compiler, ICompileArgs * args) { return Compile(compiler, args, O_Equal); }
	static ICompiled * CompileGreaterOrEqual(ICompiler * compiler, ICompileArgs * args) { return Compile(compiler, args, O_GreaterOrEqual); }
	static ICompiled * CompileGreater(ICompiler * compiler, ICompileArgs * args) { return Compile(compiler, args, O_Greater); }

	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args, Operation op)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		ICompiled::Type resultType = ICompiled::T_None;
		ICompiled * compiled = 0;

		bool bOk =
			(resultType = pa->ParseNextArgT()) == pa->ParseNextArgT()
			&& pa->ParseEof()
		;

		if(bOk)
		{
			switch(resultType)
			{
			case ICompiled::T_Bool:
			case ICompiled::T_Int:
			case ICompiled::T_Float:
				break;
			default:
				bOk = false;
				break;
			}
		}

		if(bOk)
		{
			compiled = new Compiled(new FnCompare(pa, resultType, op));
		}

		pa->Ref()->Release();

		return compiled ? compiled :  new Compiled(new Error());
	}

	virtual BoolT EvalBool (void)
	{
		return (*this.*m_CompareFn)();
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

protected:
	virtual ~FnCompare()
	{
		switch(m_ResultType)
		{
		case ICompiled::T_Bool:
			m_AL.Bool->Ref()->Release();
			m_AR.Bool->Ref()->Release();
			break;
		case ICompiled::T_Int:
			m_AL.Int->Ref()->Release();
			m_AR.Int->Ref()->Release();
			break;
		case ICompiled::T_Float:
			m_AL.Float->Ref()->Release();
			m_AR.Float->Ref()->Release();
			break;
		default:
			throw new exception();
			break;
		}
	}

private:
	union FnT
	{
		IBool * Bool;
		IInt * Int;
		IFloat * Float;
	};
	FnT m_AL;
	FnT m_AR;
	ICompiled::Type m_ResultType;
	bool (FnCompare::*m_CompareFn) (void);

	FnCompare(ParseArgs * args, ICompiled::Type resultType, Operation op)
	{
		args->Ref()->AddRef();

		m_ResultType = resultType;

		switch(resultType)
		{
		case ICompiled::T_Bool:
			m_AL.Bool = args->GetArg(0)->GetBool();
			m_AL.Bool->Ref()->AddRef();
			m_AR.Bool = args->GetArg(1)->GetBool();
			m_AR.Bool->Ref()->AddRef();
			SetOpBool(op);
			break;
		case ICompiled::T_Int:
			m_AL.Int = args->GetArg(0)->GetInt();
			m_AL.Int->Ref()->AddRef();
			m_AR.Int = args->GetArg(1)->GetInt();
			m_AR.Int->Ref()->AddRef();
			SetOpInt(op);
			break;
		case ICompiled::T_Float:
			m_AL.Float = args->GetArg(0)->GetFloat();
			m_AL.Float->Ref()->AddRef();
			m_AR.Float = args->GetArg(1)->GetFloat();
			m_AR.Float->Ref()->AddRef();
			SetOpFloat(op);
			break;
		default:
			throw new exception();
			break;
		}

		args->Ref()->Release();
	}

	void SetOpBool(Operation op)
	{
		switch(op)
		{
		case O_Less:
			m_CompareFn = &FnCompare::LessBool;
			break;
		case O_LessOrEqual:
			m_CompareFn = &FnCompare::LessOrEqualBool;
			break;
		case O_Equal:
			m_CompareFn = &FnCompare::EqualBool;
			break;
		case O_GreaterOrEqual:
			m_CompareFn = &FnCompare::GreaterOrEqualBool;
			break;
		case O_Greater:
			m_CompareFn = &FnCompare::GreaterBool;
			break;
		default:
			throw new exception();
			break;
		}
	}

	void SetOpInt(Operation op)
	{
		switch(op)
		{
		case O_Less:
			m_CompareFn = &FnCompare::LessInt;
			break;
		case O_LessOrEqual:
			m_CompareFn = &FnCompare::LessOrEqualInt;
			break;
		case O_Equal:
			m_CompareFn = &FnCompare::EqualInt;
			break;
		case O_GreaterOrEqual:
			m_CompareFn = &FnCompare::GreaterOrEqualInt;
			break;
		case O_Greater:
			m_CompareFn = &FnCompare::GreaterInt;
			break;
		default:
			throw new exception();
			break;
		}
	}

	void SetOpFloat(Operation op)
	{
		switch(op)
		{
		case O_Less:
			m_CompareFn = &FnCompare::LessFloat;
			break;
		case O_LessOrEqual:
			m_CompareFn = &FnCompare::LessOrEqualFloat;
			break;
		case O_Equal:
			m_CompareFn = &FnCompare::EqualFloat;
			break;
		case O_GreaterOrEqual:
			m_CompareFn = &FnCompare::GreaterOrEqualFloat;
			break;
		case O_Greater:
			m_CompareFn = &FnCompare::GreaterFloat;
			break;
		default:
			throw new exception();
			break;
		}
	}

	bool LessBool (void) { return m_AL.Bool->EvalBool() < m_AR.Bool->EvalBool(); }
	bool LessInt (void) { return m_AL.Int->EvalInt() < m_AR.Int->EvalInt(); }
	bool LessFloat (void) { return m_AL.Float->EvalFloat() < m_AR.Float->EvalFloat(); }
	
	bool LessOrEqualBool (void) { return m_AL.Bool->EvalBool() <= m_AR.Bool->EvalBool(); }
	bool LessOrEqualInt (void) { return m_AL.Int->EvalInt() <= m_AR.Int->EvalInt(); }
	bool LessOrEqualFloat (void) { return m_AL.Float->EvalFloat() <= m_AR.Float->EvalFloat(); }

	bool EqualBool (void) { return m_AL.Bool->EvalBool() == m_AR.Bool->EvalBool(); }
	bool EqualInt (void) { return m_AL.Int->EvalInt() == m_AR.Int->EvalInt(); }
	bool EqualFloat (void) { return m_AL.Float->EvalFloat() == m_AR.Float->EvalFloat(); }

	bool GreaterOrEqualBool (void) { return m_AL.Bool->EvalBool() >= m_AR.Bool->EvalBool(); }
	bool GreaterOrEqualInt (void) { return m_AL.Int->EvalInt() >= m_AR.Int->EvalInt(); }
	bool GreaterOrEqualFloat (void) { return m_AL.Float->EvalFloat() >= m_AR.Float->EvalFloat(); }

	bool GreaterBool (void) { return m_AL.Bool->EvalBool() > m_AR.Bool->EvalBool(); }
	bool GreaterInt (void) { return m_AL.Int->EvalInt() > m_AR.Int->EvalInt(); }
	bool GreaterFloat (void) { return m_AL.Float->EvalFloat() > m_AR.Float->EvalFloat(); }
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

		while(bOk && pa->ParseNextArgTCEX(ICompiled::T_Bool, bOk));

		ICompiled * compiled = bOk
			? new Compiled(new FnInBool(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

		return compiled;
	}

	virtual BoolT EvalBool (void) {

		BoolT findVal = m_Fns[0]->EvalBool();

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

		while(bOk && pa->ParseNextArgTCEX(ICompiled::T_Int, bOk));

		ICompiled * compiled = bOk
			? new Compiled(new FnInInt(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

		return compiled;
	}

	virtual BoolT EvalBool (void) {

		IntT findVal = m_Fns[0]->EvalInt();

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

		while(bOk && pa->ParseNextArgTCEX(ICompiled::T_Bool, bOk));

		ICompiled * compiled = bOk
			? new Compiled(new FnMaxBool(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

		return compiled;
	}

	virtual BoolT EvalBool (void) {
		
		BoolT winVal = false;
		bool noVal = true;

		for(int i=0; i<m_Count; i++)
		{
			BoolT curVal = m_Fns[i]->EvalBool();

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

		while(bOk && pa->ParseNextArgTCEX(ICompiled::T_Int, bOk));

		ICompiled * compiled = bOk
			? new Compiled(new FnMaxInt(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

		return compiled;
	}

	virtual IntT EvalInt (void) {
		
		IntT winVal = 0;
		bool noVal = true;

		for(int i=0; i<m_Count; i++)
		{
			IntT curVal = m_Fns[i]->EvalInt();

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

		while(bOk && pa->ParseNextArgTCEX(ICompiled::T_Bool, bOk));

		ICompiled * compiled = bOk
			? new Compiled(new FnMinBool(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

		return compiled;
	}

	virtual BoolT EvalBool (void) {
		
		BoolT winVal = false;
		bool noVal = true;

		for(int i=0; i<m_Count; i++)
		{
			BoolT curVal = m_Fns[i]->EvalBool();

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

		while(bOk && pa->ParseNextArgTCEX(ICompiled::T_Int, bOk));

		ICompiled * compiled = bOk
			? new Compiled(new FnMinInt(pa))
			: new Compiled(new Error())
		;

		pa->Ref()->Release();

		return compiled;
	}

	virtual IntT EvalInt (void) {
		
		IntT winVal = 0;
		bool noVal = true;

		for(int i=0; i<m_Count; i++)
		{
			IntT curVal = m_Fns[i]->EvalInt();

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


class FnIf : public Ref,
	public IFloat,
	public IBool,
	public IInt,
	public IVoid
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		ICompiled::Type resultType = ICompiled::T_None;
		ICompiled * compiled = 0;

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_Bool)
			&& (resultType = pa->ParseNextArgT()) == pa->ParseNextArgT()
			&& pa->ParseEof()
		;

		if(bOk)
		{
			switch(resultType)
			{
			case ICompiled::T_Void:
				compiled = new Compiled(dynamic_cast<IVoid *>(new FnIf(pa, resultType)));
				break;
			case ICompiled::T_Bool:
				compiled = new Compiled(dynamic_cast<IBool *>(new FnIf(pa, resultType)));
				break;
			case ICompiled::T_Int:
				compiled = new Compiled(dynamic_cast<IInt *>(new FnIf(pa, resultType)));
				break;
			case ICompiled::T_Float:
				compiled = new Compiled(dynamic_cast<IFloat *>(new FnIf(pa, resultType)));
				break;
			default:
				bOk = false;
				break;
			}
		}

		pa->Ref()->Release();

		return compiled ? compiled : new Compiled(new Error());
	}

	virtual VoidT EvalVoid (void) {
		
		if(m_If->EvalBool()) m_IfTrue.Void->EvalVoid();
		else m_IfFalse.Void->EvalVoid();
	}

	virtual BoolT EvalBool (void) {
		
		return 	m_If->EvalBool()
			? m_IfTrue.Bool->EvalBool()
			: m_IfFalse.Bool->EvalBool()
		;
	}

	virtual IntT EvalInt (void) {
		
		return 	m_If->EvalBool()
			? m_IfTrue.Int->EvalInt()
			: m_IfFalse.Int->EvalInt()
		;
	}

	virtual FloatT EvalFloat (void) {
		
		return 	m_If->EvalBool()
			? m_IfTrue.Float->EvalFloat()
			: m_IfFalse.Float->EvalFloat()
		;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	

protected:
	virtual ~FnIf()
	{
		m_If->Ref()->Release();

		switch(m_ResultType)
		{
		case ICompiled::T_Void:
			m_IfTrue.Void->Ref()->Release();
			m_IfFalse.Void->Ref()->Release();
			break;
		case ICompiled::T_Bool:
			m_IfTrue.Bool->Ref()->Release();
			m_IfFalse.Bool->Ref()->Release();
			break;
		case ICompiled::T_Int:
			m_IfTrue.Int->Ref()->Release();
			m_IfFalse.Int->Ref()->Release();
			break;
		case ICompiled::T_Float:
			m_IfTrue.Float->Ref()->Release();
			m_IfFalse.Float->Ref()->Release();
			break;
		default:
			throw new exception();
			break;
		}
	}

private:
	union ResultT
	{
		IVoid * Void;
		IBool * Bool;
		IInt * Int;
		IFloat  * Float;
	};

	IBool * m_If;
	ResultT m_IfTrue;
	ResultT m_IfFalse;
	ICompiled::Type m_ResultType;

	FnIf(ParseArgs * args, ICompiled::Type resultType)
	{
		args->Ref()->AddRef();

		m_ResultType = resultType;

		m_If = args->GetArg(0)->GetBool();
		m_If->Ref()->AddRef();

		switch(resultType)
		{
		case ICompiled::T_Void:
			m_IfTrue.Void = args->GetArg(1)->GetVoid();
			m_IfTrue.Void->Ref()->AddRef();
			m_IfFalse.Void = args->GetArg(2)->GetVoid();
			m_IfFalse.Void->Ref()->AddRef();
			break;
		case ICompiled::T_Bool:
			m_IfTrue.Bool = args->GetArg(1)->GetBool();
			m_IfTrue.Bool->Ref()->AddRef();
			m_IfFalse.Bool = args->GetArg(2)->GetBool();
			m_IfFalse.Bool->Ref()->AddRef();
			break;
		case ICompiled::T_Int:
			m_IfTrue.Int = args->GetArg(1)->GetInt();
			m_IfTrue.Int->Ref()->AddRef();
			m_IfFalse.Int = args->GetArg(2)->GetInt();
			m_IfFalse.Int->Ref()->AddRef();
			break;
		case ICompiled::T_Float:
			m_IfTrue.Float = args->GetArg(1)->GetFloat();
			m_IfTrue.Float->Ref()->AddRef();
			m_IfFalse.Float = args->GetArg(2)->GetFloat();
			m_IfFalse.Float->Ref()->AddRef();
			break;
		default:
			throw new exception();
			break;
		}

		args->Ref()->Release();
	}
};


class FnDo : public Ref,
	public IVoid,
	public IBool,
	public IInt,
	public IFloat
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();


		ICompiled * compiled = 0;
		ICompiled::Type resultType = ICompiled::T_None;
		
		bool bOk = true;
		bool bEof = false;

		do
		{
			ICompiled::Type curType = pa->ParseNextArgTE();

			switch(curType)
			{
			case ICompiled::T_Eof:
				bEof = true;
				break;
			case ICompiled::T_Void:
			case ICompiled::T_Bool:
			case ICompiled::T_Int:
			case ICompiled::T_Float:
				resultType = curType;
				break;
			default:
				bOk = false;
				break;
			}
		} while(bOk && !bEof);

		if(bOk)
		{
			switch(resultType)
			{
			case ICompiled::T_Void:
				compiled = new Compiled(dynamic_cast<IVoid *>(new FnDo(pa)));
				break;
			case ICompiled::T_Bool:
				compiled = new Compiled(dynamic_cast<IBool *>(new FnDo(pa)));
				break;
			case ICompiled::T_Int:
				compiled = new Compiled(dynamic_cast<IInt *>(new FnDo(pa)));
				break;
			case ICompiled::T_Float:
				compiled = new Compiled(dynamic_cast<IFloat *>(new FnDo(pa)));
				break;
			default:
				break;
			}
		}

		pa->Ref()->Release();

		return compiled ? compiled : new Compiled(new Error());
	}

	virtual VoidT EvalVoid (void)
	{
		for(int i=0; i<m_Count; i++) EvalX(i);
	}

	virtual BoolT EvalBool (void)
	{
		for(int i=0; i<m_Count-1; i++) EvalX(i);

		return m_Fns[m_Count-1].Bool->EvalBool();
	}

	virtual FloatT EvalFloat (void)
	{
		for(int i=0; i<m_Count-1; i++) EvalX(i);

		return m_Fns[m_Count-1].Float->EvalFloat();
	}

	virtual IntT EvalInt (void)
	{
		for(int i=0; i<m_Count-1; i++) EvalX(i);

		return m_Fns[m_Count-1].Int->EvalInt();
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	

protected:
	virtual ~FnDo()
	{
		for(int i=0; i<m_Count; i++)
		{
			switch(m_Types[i])
			{
			case ICompiled::T_Void:
				m_Fns[i].Void->Ref()->Release();
				break;
			case ICompiled::T_Bool:
				m_Fns[i].Bool->Ref()->Release();
				break;
			case ICompiled::T_Int:
				m_Fns[i].Int->Ref()->Release();
				break;
			case ICompiled::T_Float:
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
		IFloat * Float;
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
			case ICompiled::T_Void:
				m_Fns[i].Void = args->GetArg(i)->GetVoid();
				m_Fns[i].Void->Ref()->AddRef();
				break;
			case ICompiled::T_Bool:
				m_Fns[i].Bool = args->GetArg(i)->GetBool();
				m_Fns[i].Bool->Ref()->AddRef();
				break;
			case ICompiled::T_Int:
				m_Fns[i].Int = args->GetArg(i)->GetInt();
				m_Fns[i].Int->Ref()->AddRef();
				break;
			case ICompiled::T_Float:
				m_Fns[i].Float = args->GetArg(i)->GetFloat();
				m_Fns[i].Float->Ref()->AddRef();
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
		case ICompiled::T_Void:
			m_Fns[i].Void->EvalVoid();
			break;
		case ICompiled::T_Bool:
			m_Fns[i].Bool->EvalBool();
			break;
		case ICompiled::T_Int:
			m_Fns[i].Int->EvalInt();
			break;
		case ICompiled::T_Float:
			m_Fns[i].Float->EvalFloat();
			break;
		default:
			throw exception();
			break;
		}
	}
};


class FnSum : public Ref,
	public IInt,
	public IFloat
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		ICompiled::Type resultType = ICompiled::T_None;
		ICompiled * compiled = 0;

		bool bOk = true;
		bool bEof = false;

		do
		{
			ICompiled::Type curType = pa->ParseNextArgTE();

			switch(curType)
			{
			case ICompiled::T_Eof:
				bEof = true;
				break;
			case ICompiled::T_Int:
			case ICompiled::T_Float:
				bOk = curType == resultType ||  ICompiled::T_None == resultType; // must match first type (if any):
				resultType = curType;
				break;
			default:
				bOk = false;
				break;
			}			
		}
		while (bOk && !bEof);

		if(bOk)
		{
			switch(resultType)
			{
			case ICompiled::T_Int:
				compiled = new Compiled(dynamic_cast<IInt *>(new FnSum(pa, resultType)));
				break;
			case ICompiled::T_Float:
				compiled = new Compiled(dynamic_cast<IFloat *>(new FnSum(pa, resultType)));
				break;
			default:
				break;
			}
		}

		pa->Ref()->Release();

		return compiled ? compiled : new Compiled(new Error());
	}

	virtual IntT EvalInt (void) {
		IntT sum = 0;
		for(int i=0; i < m_Count; i++) sum += m_Fns[i].Int->EvalInt();
		return sum;
	}

	virtual FloatT EvalFloat (void) {
		FloatT sum = 0;
		for(int i=0; i < m_Count; i++) sum += m_Fns[i].Float->EvalFloat();
		return sum;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }	

protected:
	virtual ~FnSum()
	{
		for(int i=0; i < m_Count; i++)
		{
			switch(m_Type)
			{
			case ICompiled::T_Int:
				m_Fns[i].Int->Ref()->Release();
				break;
			case ICompiled::T_Float:
				m_Fns[i].Float->Ref()->Release();
				break;
			default:
				throw new exception();
				break;
			}
		}

		delete m_Fns;
	}

private:
	union FnT
	{
		IInt * Int;
		IFloat  * Float;
	};
	int m_Count;
	FnT * m_Fns;
	ICompiled::Type m_Type;

	FnSum(ParseArgs * args, ICompiled::Type type)
	{
		args->Ref()->AddRef();

		m_Count = args->GetCount();
		m_Fns = new FnT[args->GetCount()];
		m_Type = type;

		for(int i=0; i < args->GetCount(); i++)
		{
			switch(type)
			{
			case ICompiled::T_Int:
				m_Fns[i].Int = args->GetArg(i)->GetInt();
				m_Fns[i].Int->Ref()->AddRef();
				break;
			case ICompiled::T_Float:
				m_Fns[i].Float = args->GetArg(i)->GetFloat();
				m_Fns[i].Float->Ref()->AddRef();
				break;
			default:
				throw new exception();
				break;
			}
		}

		args->Ref()->Release();
	}
};


class FnStringFromFile
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_String)
			&& pa->ParseEof()
		;

		ICompiled * compiled = 0;

		if(bOk)
		{
			IString * str = pa->GetArg(0)->GetString();

			IStringValue * strVal = str->EvalString();
			strVal->Ref()->AddRef();

			size_t fsize;
			FILE * file;
			if(!fopen_s(&file, strVal->GetData(), "rt")
				&& 0 ==fseek(file, 0, SEEK_END)
				&& ((fsize = ftell(file)), 0 == fseek(file, 0, SEEK_SET))
			)
			{
				size_t numChars = fsize / sizeof(char);

				char * myChars = new char[1+numChars];

				memset(myChars, 0, sizeof(char)*(1+numChars));
				fread(myChars, sizeof(char), numChars, file);

				// (actual number of chars read may be less than numChars due to text translation)

				if(feof(file))
				{
					myChars[numChars] = 0; // re-terminate (just in case ;)
					compiled = new Compiled(new FnString(new StringValue((int)numChars, myChars)));
				}				
				else
					delete myChars;
			}

			if(file) fclose(file);


			strVal->Ref()->Release();
		}

		pa->Ref()->Release();

		return compiled ? compiled : new Compiled(new Error());
	}
};



class FnCompile
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk =
			pa->ParseNextArgTC(ICompiled::T_String)
			&& pa->ParseEof()
		;

		ICompiled * compiled = 0;

		if(bOk)
		{
			IString * str = pa->GetArg(0)->GetString();

			IStringValue * strVal = str->EvalString();
			strVal->Ref()->AddRef();

			Cursor cur(strVal->GetData());

			compiled = compiler->Compile_Function(cur);

			strVal->Ref()->Release();
		}

		pa->Ref()->Release();

		return compiled ? compiled : new Compiled(new Error());
	}
};




// FnNull //////////////////////////////////////////////////////////////////////

class FnNull : public Ref,
	public INull
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		compiler->Ref()->AddRef();
		args->Ref()->AddRef();

		ICompiled * error = 0;
		bool bContinue = true;

		do
		{
			ICompiled * compiled = args->CompileNextArg(compiler);

			compiled->Ref()->AddRef();

			if(compiled->GetError())
				error = new Compiled(compiled->GetError());
			else
				bContinue = !compiled->GetEof();

			compiled->Ref()->Release();
		}
		while(!error && bContinue);

		compiler->Ref()->Release();
		args->Ref()->Release();

		return !error ? new Compiled(new FnNull()) : error;
	}

	virtual ::Afx::IRef * Ref (void) {
		return dynamic_cast<::Afx::IRef *>(this);
	}
};


// FnVoid //////////////////////////////////////////////////////////////////////

class FnVoid : public Ref,
	public IVoid
{
public:
	static ICompiled * Compile(ICompiler * compiler, ICompileArgs * args)
	{
		ParseArgs * pa = new ParseArgs(compiler, args);
		pa->Ref()->AddRef();

		bool bOk = pa->ParseEof();

		pa->Ref()->Release();

		return bOk ? new Compiled(new FnVoid()) : new Compiled(new Error());
	}

	virtual VoidT EvalVoid (void) {
	}

	virtual ::Afx::IRef * Ref (void) {
		return dynamic_cast<::Afx::IRef *>(this);
	}
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

	virtual BoolT EvalBool (void)
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

	virtual VoidT EvalVoid (void)
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

	if(pa->ParseEof())
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

	bool bOk;
	if(pa->ParseNextArgTCEX(ICompiled::T_Bool, bOk) && bOk)
	{
		if(
			(CA_Property == m_CompileAccess || CA_Setter == m_CompileAccess)
			&& pa->ParseEof()
		)
		{
			// Compile setter:
			compiled = new Compiled(new BoolSetterC(this, pa->GetArg(0)->GetBool()));
		}
	}
	else if(bOk)
	{
		if(
			(CA_Property == m_CompileAccess || CA_Getter == m_CompileAccess)
			&& pa->ParseEof()
		)
		{
			// Compile getter:
			compiled = new Compiled(new BoolGetterC(this));
		}
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

	if(pa->ParseNextArgTC(ICompiled::T_Bool) && pa->ParseEof())
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

BoolVariable::BoolVariable(ICompiler * compiler, CompileAcces compileAccess, BoolT value)
: BoolProperty(compiler, compileAccess), m_Value(value)
{
}

BoolT BoolVariable::Get() const
{
	return m_Value;
}

BoolT BoolVariable::Get (void)
{
	return m_Value;
}

void BoolVariable::Set (BoolT value)
{
	m_Value = value;
}


// Bubble //////////////////////////////////////////////////////////////////////

Bubble::Bubble()
{
	//
	// Add standard functions:

	// todo map to FnCompileable :-)

	Add("null", new FnCompileable(this, &FnNull::Compile));
	Add("void", new FnCompileable(this, &FnVoid::Compile)); // actually this is a subset of do, but meh.

	Add("string", new StringCompileable());
	Add("s", new StringCompileable());

	Add("and", new FnCompileable(this, &FnAnd::Compile));
	Add("&&", new FnCompileable(this, &FnAnd::Compile));

	Add("or", new FnCompileable(this, &FnOr::Compile));
	Add("||", new FnCompileable(this, &FnOr::Compile));

	Add("not", new FnCompileable(this, &FnNot::Compile));
	Add("!", new FnCompileable(this, &FnNot::Compile));

	Add("less", new FnCompileable(this, &FnCompare::CompileLess));
	Add("<", new FnCompileable(this, &FnCompare::CompileLess));

	Add("lessOrEqual", new FnCompileable(this, &FnCompare::CompileLessOrEqual));
	Add("<=", new FnCompileable(this, &FnCompare::CompileLessOrEqual));

	Add("equal", new FnCompileable(this, &FnCompare::CompileEqual));
	Add("==", new FnCompileable(this, &FnCompare::CompileEqual));

	Add("greater", new FnCompileable(this, &FnCompare::CompileGreater));
	Add(">", new FnCompileable(this, &FnCompare::CompileGreater));

	Add("greaterOrEqual", new FnCompileable(this, &FnCompare::CompileGreaterOrEqual));
	Add(">=", new FnCompileable(this, &FnCompare::CompileGreaterOrEqual));

	Add("in", new FnCompileable(this, &FnInBool::Compile));
	Add("in", new FnCompileable(this, &FnInInt::Compile));

	Add("max", new FnCompileable(this, &FnMaxBool::Compile));
	Add("max", new FnCompileable(this, &FnMaxInt::Compile));

	Add("min", new FnCompileable(this, &FnMinBool::Compile));
	Add("min", new FnCompileable(this, &FnMinInt::Compile));

	Add("if", new FnCompileable(this, &FnIf::Compile));
	Add("?", new FnCompileable(this, &FnIf::Compile));

	Add("do", new FnCompileable(this, &FnDo::Compile));
	Add(".", new FnCompileable(this, &FnDo::Compile));

	Add("sum", new FnCompileable(this, &FnSum::Compile));
	Add("+", new FnCompileable(this, &FnSum::Compile));

	Add("stringFromFile", new FnCompileable(this, &FnStringFromFile::Compile));

	Add("compile", new FnCompileable(this, &FnCompile::Compile));
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

	if(!compiled)
	{
		// Bool?

		CursorBackup backup = cursor.Backup();

		BoolT bVal;
		if(ToBool(cursor, bVal))
		{

			compiled = FnConstBool::Compile(this, new CompileArgs(cursor, inParenthesis), bVal);
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
		// Int?

		CursorBackup backup = cursor.Backup();

		int skipped;
		IntT iVal = cursor.ReadLong(&skipped);

		if(0 < skipped && !IsIdentifierChar(cursor.Get()))
		{
			compiled = FnConstInt::Compile(this, new CompileArgs(cursor, inParenthesis), iVal);
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

::Afx::IRef * Compileable::Ref (void)
{
	return dynamic_cast<::Afx::IRef *>(this);
}


// Compiled ////////////////////////////////////////////////////////////////////

Compiled::Compiled(IBool * value)
{
	if(value) value->Ref()->AddRef();

	m_Type = ICompiled::T_Bool;
	m_Value.Bool = value;
}

Compiled::Compiled(IEof * value)
{
	if(value) value->Ref()->AddRef();

	m_Type = ICompiled::T_Eof;
	m_Value.Eof = value;
}

Compiled::Compiled(IError * value)
{
	if(value) value->Ref()->AddRef();

	m_Type = ICompiled::T_Error;
	m_Value.Error = value;
}

Compiled::Compiled(INull * value)
{
	if(value) value->Ref()->AddRef();

	m_Type = ICompiled::T_Null;
	m_Value.Null = value;
}


Compiled::Compiled(IFloat * value)
{
	if(value) value->Ref()->AddRef();

	m_Type = ICompiled::T_Float;
	m_Value.Float = value;
}

Compiled::Compiled(IInt * value)
{
	if(value) value->Ref()->AddRef();

	m_Type = ICompiled::T_Int;
	m_Value.Int = value;
}


Compiled::Compiled(IString * value)
{
	if(value) value->Ref()->AddRef();

	m_Type = ICompiled::T_String;
	m_Value.String = value;
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
	case ICompiled::T_Eof: if(m_Value.Eof) m_Value.Eof->Ref()->Release(); break;
	case ICompiled::T_Error: if(m_Value.Error) m_Value.Error->Ref()->Release(); break;
	case ICompiled::T_Null: if(m_Value.Null) m_Value.Null->Ref()->Release(); break;
	case ICompiled::T_Void: if(m_Value.Void) m_Value.Void->Ref()->Release(); break;
	case ICompiled::T_Bool: if(m_Value.Bool) m_Value.Bool->Ref()->Release(); break;
	case ICompiled::T_Int: if(m_Value.Int) m_Value.Int->Ref()->Release(); break;
	case ICompiled::T_Float: if(m_Value.Float) m_Value.Float->Ref()->Release(); break;
	case ICompiled::T_String: if(m_Value.String) m_Value.String->Ref()->Release(); break;
	}
}

IBool * Compiled::GetBool() { return ICompiled::T_Bool == m_Type ? m_Value.Bool : 0; }

IEof * Compiled::GetEof() { return ICompiled::T_Eof == m_Type ? m_Value.Eof : 0; }

IError * Compiled::GetError() { return ICompiled::T_Error == m_Type ? m_Value.Error : 0; }

IFloat * Compiled::GetFloat() { return ICompiled::T_Float == m_Type ? m_Value.Float : 0; }

IInt * Compiled::GetInt() { return ICompiled::T_Int == m_Type ? m_Value.Int : 0; }

INull * Compiled::GetNull() { return ICompiled::T_Null == m_Type ? m_Value.Null : 0; }

IString * Compiled::GetString() { return ICompiled::T_String == m_Type ? m_Value.String : 0; }

enum ICompiled::Type Compiled::GetType() { return m_Type; }

IVoid * Compiled::GetVoid() { return ICompiled::T_Void == m_Type ? m_Value.Void : 0; }

::Afx::IRef * Compiled::Ref() { return dynamic_cast<::Afx::IRef *>(this); }



// FloatGetterC /////////////////////////////////////////////////////////////////

class FloatGetterC : public Ref,
	public IFloat
{
public:
	FloatGetterC(IFloatGetter * intGetter)
	: m_FloatGetter(intGetter)
	{
		intGetter->Ref()->AddRef();
	}

	virtual FloatT EvalFloat (void)
	{
		return m_FloatGetter->Get();
	}

	virtual ::Afx::IRef * Ref (void) { return dynamic_cast<::Afx::IRef *>(this); }

protected:
	virtual ~FloatGetterC()
	{
		m_FloatGetter->Ref()->Release();
	}

private:
	IFloatGetter * m_FloatGetter;
};


// FloatSetterC /////////////////////////////////////////////////////////////////

class FloatSetterC : public Ref,
	public IVoid
{
public:
	FloatSetterC(IFloatSetter * intSetter, IFloat * value)
	: m_FloatSetter(intSetter), m_Value(value)
	{
		intSetter->Ref()->AddRef();
		value->Ref()->AddRef();
	}

	virtual VoidT EvalVoid (void)
	{
		m_FloatSetter->Set(m_Value->EvalFloat());
	}

	virtual ::Afx::IRef * Ref (void) { return dynamic_cast<::Afx::IRef *>(this); }

protected:
	virtual ~FloatSetterC()
	{
		m_FloatSetter->Ref()->Release();
		m_Value->Ref()->Release();
	}

private:
	IFloatSetter * m_FloatSetter;
	IFloat * m_Value;
};


// FloatGetter //////////////////////////////////////////////////////////////////

FloatGetter::FloatGetter(ICompiler * compiler)
: Compileable(compiler)
{
}

ICompiled * FloatGetter::Compile (ICompileArgs * args)
{
	ICompiled * compiled = 0;

	ParseArgs * pa = new ParseArgs(m_Compiler, args);
	pa->Ref()->AddRef();

	if(pa->ParseEof())
	{
		compiled = new Compiled(new FloatGetterC(this));
	}

	pa->Ref()->Release();

	return compiled ? compiled : new Compiled(new Error());
}


::Afx::IRef * FloatGetter::Ref (void)
{
	return dynamic_cast<::Afx::IRef *>(this);
}

// FloatProperty ////////////////////////////////////////////////////////////////


FloatProperty::FloatProperty(ICompiler * compiler, CompileAcces compileAccess)
: Compileable(compiler), m_CompileAccess(compileAccess)
{
}


ICompiled * FloatProperty::Compile (ICompileArgs * args)
{
	ICompiled * compiled = 0;

	ParseArgs * pa = new ParseArgs(m_Compiler, args);
	pa->Ref()->AddRef();

	bool bOk;
	if(pa->ParseNextArgTCEX(ICompiled::T_Float, bOk) && bOk)
	{
		if(
			(CA_Property == m_CompileAccess || CA_Setter == m_CompileAccess)
			&& pa->ParseEof()
		)
		{
			// Compile setter:
			compiled = new Compiled(new FloatSetterC(this, pa->GetArg(0)->GetFloat()));
		}
	}
	else if(bOk)
	{
		if(
			(CA_Property == m_CompileAccess || CA_Getter == m_CompileAccess)
			&& pa->ParseEof()
		)
		{
			// Compile getter:
			compiled = new Compiled(new FloatGetterC(this));
		}
	}

	pa->Ref()->Release();

	return compiled ? compiled : new Compiled(new Error());
}

::Afx::IRef * FloatProperty::Ref (void)
{
	return dynamic_cast<::Afx::IRef *>(this);
}


// FloatSetter //////////////////////////////////////////////////////////////////

FloatSetter::FloatSetter(ICompiler * compiler)
: Compileable(compiler)
{
}

ICompiled * FloatSetter::Compile (ICompileArgs * args)
{
	ICompiled * compiled = 0;

	ParseArgs * pa = new ParseArgs(m_Compiler, args);
	pa->Ref()->AddRef();

	if(pa->ParseNextArgTC(ICompiled::T_Float) && pa->ParseEof())
	{
		compiled = new Compiled(new FloatSetterC(this, pa->GetArg(0)->GetFloat()));
	}

	pa->Ref()->Release();

	return compiled ? compiled : new Compiled(new Error());
}

::Afx::IRef * FloatSetter::Ref (void)
{
	return dynamic_cast<::Afx::IRef *>(this);
}


// FloatVariable ////////////////////////////////////////////////////////////////

FloatVariable::FloatVariable(ICompiler * compiler, CompileAcces compileAccess, int value)
: FloatProperty(compiler, compileAccess), m_Value(value)
{
}

FloatT FloatVariable::Get() const
{
	return m_Value;
}

FloatT FloatVariable::Get (void)
{
	return m_Value;
}

void FloatVariable::Set (FloatT value)
{
	m_Value = value;
}



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

	virtual IntT EvalInt (void)
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

	virtual VoidT EvalVoid (void)
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

	if(pa->ParseEof())
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

	bool bOk;
	if(pa->ParseNextArgTCEX(ICompiled::T_Int, bOk) && bOk)
	{
		if(
			(CA_Property == m_CompileAccess || CA_Setter == m_CompileAccess)
			&& pa->ParseEof()
		)
		{
			// Compile setter:
			compiled = new Compiled(new IntSetterC(this, pa->GetArg(0)->GetInt()));
		}
	}
	else if(bOk)
	{
		if(
			(CA_Property == m_CompileAccess || CA_Getter == m_CompileAccess)
			&& pa->ParseEof()
		)
		{
			// Compile getter:
			compiled = new Compiled(new IntGetterC(this));
		}
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

	if(pa->ParseNextArgTC(ICompiled::T_Int) && pa->ParseEof())
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

IntT IntVariable::Get() const
{
	return m_Value;
}

IntT IntVariable::Get (void)
{
	return m_Value;
}

void IntVariable::Set (IntT value)
{
	m_Value = value;
}




// Tools ///////////////////////////////////////////////////////////////

IBubble * Tools::StandardBubble (void)
{
	return new Bubble();
}

ICompileable * Tools::FnDoCompileable(ICompiler * compiler)
{
	return new ::FnCompileable(compiler, &FnDo::Compile);
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

	if(pa->ParseEof())
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


// VoidEvent ///////////////////////////////////////////////////////////////////

class VoidEventSetNull : public VoidFunction
{
public:
	VoidEventSetNull(ICompiler * compiler, VoidEvent * voidEvent)
	: VoidFunction(compiler), m_VoidEvent(voidEvent)
	{
		voidEvent->Ref()->AddRef();
	}

	virtual VoidT EvalVoid (void) {
		m_VoidEvent->SetEvent(0);
	}

protected:
	virtual ~VoidEventSetNull()
	{
		m_VoidEvent->Ref()->Release();
	}

private:
	VoidEvent * m_VoidEvent;

};


class VoidEventSetVoid : public VoidFunction
{
public:
	VoidEventSetVoid(ICompiler * compiler, VoidEvent * voidEvent, IVoid * voidFn)
	: VoidFunction(compiler), m_VoidEvent(voidEvent), m_Void(voidFn)
	{
		voidFn->Ref()->AddRef();
		voidEvent->Ref()->AddRef();
	}

	virtual VoidT EvalVoid (void) {
		m_VoidEvent->SetEvent(m_Void);
	}

protected:
	virtual ~VoidEventSetVoid()
	{
		m_Void->Ref()->Release();
		m_VoidEvent->Ref()->Release();
	}

private:
	IVoid * m_Void;
	VoidEvent * m_VoidEvent;

};


VoidEvent::VoidEvent(ICompiler * compiler)
: Compileable(compiler), m_Void(0)
{
}

VoidEvent::~VoidEvent()
{
	SetEvent(0);
}

void VoidEvent::CallEvent()
{
	if(m_Void) m_Void->EvalVoid();
}

ICompiled * VoidEvent::Compile (ICompileArgs * args)
{
	ICompiled * compiled = 0;

	ParseArgs * pa = new ParseArgs(m_Compiler, args);
	pa->Ref()->AddRef();

	bool bOk;
	if(pa->ParseNextArgTCEX(ICompiled::T_Void, bOk))
	{
		if(bOk && pa->ParseEof()) compiled = new Compiled(new VoidEventSetVoid(m_Compiler, this, pa->GetArg(0)->GetVoid()));
	}
	else if(bOk && pa->ParseEof())
	{
		compiled = new Compiled(new VoidEventSetNull(m_Compiler, this));
	}

	pa->Ref()->Release();

	return compiled ? compiled : new Compiled(new Error());
}

bool VoidEvent::HasEvent()
{
	return 0 != m_Void;
}

void VoidEvent::SetEvent(IVoid * value)
{
	if(m_Void) m_Void->Ref()->Release();

	m_Void = value;

	if(value) value->Ref()->AddRef();
}

