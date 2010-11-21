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


#include "Functions.h"

#include "Parse.h"

#include <ctype.h>
#include <string.h>

#include <exception>
#include <list>
#include <vector>


using namespace std;

using namespace Afx;
using namespace Afx::Expressions;




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


// FnStringFromFile ////////////////////////////////////////////////////////////

class FnStringFromFile : public Ref,
	public IString
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
			compiled = new Compiled(new FnStringFromFile(pa->GetArg(0)->GetString()));
		}


		pa->Ref()->Release();

		return compiled ? compiled : new Compiled(new Error());
	}

	virtual IStringValue * EvalString (void)
	{
		IStringValue * retVal = 0;
		IStringValue * strVal = m_String->EvalString();
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

			myChars[numChars] = 0; // re-terminate (just in case ;)
			retVal = new StringValue((int)numChars, myChars);
		}

		if(file) fclose(file);

		strVal->Ref()->Release();

		if(!retVal)
		{
			char * strEmpty = new char[1];
			strEmpty[0] = 0;

			retVal = new StringValue(1, strEmpty);
		}

		return retVal;
	}

	virtual IRef * Ref (void) {
		return this;
	}

protected:
	virtual ~FnStringFromFile()
	{
		m_String->Ref()->Release();
	}

private:
	IString * m_String;

	FnStringFromFile(IString * string)
	: m_String(string)
	{
		string->Ref()->AddRef();
	}
};


// FnCompile ///////////////////////////////////////////////////////////////////

class FnCompile : public Ref,
	public IBool
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
			compiled = new Compiled(new FnCompile(compiler, pa->GetArg(0)->GetString()));
		}

		pa->Ref()->Release();

		return compiled ? compiled : new Compiled(new Error());
	}

	virtual BoolT EvalBool (void)
	{
		IStringValue * strVal = m_String->EvalString();
		strVal->Ref()->AddRef();

		Cursor cur(strVal->GetData());

		ICompiled * compiled = m_Compiler->Compile_Function(cur);
		compiled->Ref()->AddRef();

		bool bOk = false;
		if(compiled->GetVoid())
		{
			compiled->GetVoid()->EvalVoid();
			bOk = true;
		}

		compiled->Ref()->Release();
		strVal->Ref()->Release();

		return bOk;
	}

	virtual IRef * Ref (void) {
		return this;
	}


protected:
	virtual ~FnCompile()
	{
		m_Compiler->Ref()->Release();
		m_String->Ref()->Release();
	}

private:
	ICompiler * m_Compiler;
	IString * m_String;

	FnCompile(ICompiler * compiler, IString * string)
	: m_Compiler(compiler), m_String(string)
	{
		m_Compiler->Ref()->AddRef();
		m_String->Ref()->AddRef();
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


// Functions ///////////////////////////////////////////////////////////////////

IBool * Functions::Bool (BoolT value) {
	return new FnConstBool(value);
}

IFloat * Functions::Float (IntT value) {
	return new FnConstFloat(value);
}

IInt * Functions::Int (IntT value) {
	return new FnConstInt(value);
}

INull * Functions::Null (void) {
	return new FnNull();
}

IString * Functions::String (IStringValue * value) {
	return new FnString(value);
}

IVoid * Functions::Void (void) {
	return new FnVoid();
}


// FunctionsC //////////////////////////////////////////////////////////////////


ICompileable * FunctionsC::And (ICompiler * compiler) {
	return new StaticCompileable(compiler, &FnAnd::Compile);
}

ICompiled * FunctionsC::Bool (ICompiler * compiler, ICompileArgs * args, BoolT value) {
	return FnConstBool::Compile(compiler, args, value);
}

ICompileable * FunctionsC::CompileEval (ICompiler * compiler){
	return new StaticCompileable(compiler, &FnCompile::Compile);
}

ICompileable * FunctionsC::Do (ICompiler * compiler){
	return new StaticCompileable(compiler, &FnDo::Compile);
}

ICompileable * FunctionsC::Equal (ICompiler * compiler){
	return new StaticCompileable(compiler, &FnCompare::CompileEqual);
}

ICompiled * FunctionsC::Float (ICompiler * compiler, ICompileArgs * args, FloatT value) {
	return FnConstFloat::Compile(compiler, args, value);
}

ICompileable * FunctionsC::Greater (ICompiler * compiler){
	return new StaticCompileable(compiler, &FnCompare::CompileGreater);
}

ICompileable * FunctionsC::GreaterOrEqual (ICompiler * compiler){
	return new StaticCompileable(compiler, &FnCompare::CompileGreaterOrEqual);
}

ICompileable * FunctionsC::If (ICompiler * compiler){
	return new StaticCompileable(compiler, &FnIf::Compile);
}

ICompileable * FunctionsC::InBool (ICompiler * compiler){
	return new StaticCompileable(compiler, &FnInBool::Compile);
}

ICompileable * FunctionsC::InInt (ICompiler * compiler){
	return new StaticCompileable(compiler, &FnInInt::Compile);
}

ICompiled * FunctionsC::Int (ICompiler * compiler, ICompileArgs * args, IntT value) {
	return FnConstInt::Compile(compiler, args, value);
}

ICompileable * FunctionsC::Less (ICompiler * compiler){
	return new StaticCompileable(compiler, &FnCompare::CompileLess);
}

ICompileable * FunctionsC::LessOrEqual (ICompiler * compiler){
	return new StaticCompileable(compiler, &FnCompare::CompileLessOrEqual);
}

ICompileable * FunctionsC::MaxBool (ICompiler * compiler){
	return new StaticCompileable(compiler, &FnMaxBool::Compile);
}

ICompileable * FunctionsC::MaxInt (ICompiler * compiler){
	return new StaticCompileable(compiler, &FnMaxInt::Compile);
}

ICompileable * FunctionsC::MinBool (ICompiler * compiler){
	return new StaticCompileable(compiler, &FnMinBool::Compile);
}

ICompileable * FunctionsC::MinInt (ICompiler * compiler){
	return new StaticCompileable(compiler, &FnMinInt::Compile);
}

ICompileable * FunctionsC::Not (ICompiler * compiler){
	return new StaticCompileable(compiler, &FnNot::Compile);
}

ICompileable * FunctionsC::Null (ICompiler * compiler){
	return new StaticCompileable(compiler, &FnNull::Compile);
}

ICompileable * FunctionsC::Or (ICompiler * compiler){
	return new StaticCompileable(compiler, &FnOr::Compile);
}

ICompileable * FunctionsC::String (ICompiler * compiler) {
	return new StringCompileable();
}

ICompileable * FunctionsC::StringFromFile (ICompiler * compiler){
	return new StaticCompileable(compiler, &FnStringFromFile::Compile);
}

ICompileable * FunctionsC::Sum (ICompiler * compiler){
	return new StaticCompileable(compiler, &FnSum::Compile);
}

ICompileable * FunctionsC::Void (ICompiler * compiler){
	return new StaticCompileable(compiler, &FnVoid::Compile);
}

