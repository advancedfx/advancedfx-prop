#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-12-15 dominik.matrixstorm.com
//
// First changes
// 2010-11-18 dominik.matrixstorm.com

#include "Delegate.h"

#include "Parse.h"

#include <cstdarg>
#include <exception>


using namespace std;
using namespace Afx;
using namespace Afx::Expressions;


// DelegateC ///////////////////////////////////////////////////////////////////

class DelegateC : public Ref,
	public IVoid,
	public IBool,
	public IInt,
	public IFloat,
	public IString
{
public:
	DelegateC(Delegate * aDelegate, Arguments args)
	: m_Delegate(aDelegate), m_Args(args)
	{
		aDelegate->AddRef();
	}

	virtual VoidT DelegateC::EvalVoid (void) {
		m_Delegate->CallVoid(m_Args);
	}

	virtual BoolT DelegateC::EvalBool (void) {
		return m_Delegate->CallBool(m_Args);
	}

	virtual IntT DelegateC::EvalInt (void) {
		return m_Delegate->CallInt(m_Args);
	}

	virtual FloatT DelegateC::EvalFloat (void) {
		return m_Delegate->CallFloat(m_Args);
	}

	virtual IStringValue * DelegateC::EvalString (void) {
		return m_Delegate->CallString(m_Args);
	}

	virtual ::Afx::IRef * Ref (void) {
		return this;
	}

protected:
	virtual ~DelegateC() {
		m_Delegate->DeleteArgs(m_Args);
		m_Delegate->Release();
	}

private:
	Arguments  m_Args;
	Delegate * m_Delegate;
};


// ArgumentsT ////////////////////////////////////////////////////////


void ArgumentsT::Add(ArgumentT type)
{
	m_Args.push_back(type);
}

ArgumentT ArgumentsT::Get (int index) const
{
	return m_Args[index];
}

int ArgumentsT::Count (void) const
{
	return m_Args.size();
}


ArgumentsT * ArgumentsT::New ( void )
{
	return new ArgumentsT();
}

ArgumentsT * ArgumentsT::New ( int argCount, ArgumentT argumentT, ... )
{
  ArgumentsT * args = new ArgumentsT();
  va_list vl;

  args->Add(argumentT);

  va_start(vl, argumentT);
  for(int i=1; i<argCount; i++)
  {
	  ArgumentT curT = va_arg(vl, ArgumentT);
	  args->Add(curT);
  }
  va_end(vl);

  return args;
}




// Delegate ////////////////////////////////////////////////////////////////////

Delegate::Delegate(ICompiler * compiler, FunctionHost * host, FunctionT functionT, Function function, ArgumentsT * argumentsT)
: m_Compiler(compiler), m_Host(host), m_FunctionT(functionT), m_Function(function), m_ArgumentsT(argumentsT)
{
}

VoidT Delegate::CallVoid(Arguments args)
{
	(m_Host.get()->*m_Function.Void)(args);
}

BoolT Delegate::CallBool(Arguments args)
{
	return (m_Host.get()->*m_Function.Bool)(args);
}


IntT Delegate::CallInt(Arguments args)
{
	return (m_Host.get()->*m_Function.Int)(args);
}

FloatT Delegate::CallFloat(Arguments args)
{
	return (m_Host.get()->*m_Function.Float)(args);
}

IStringValue * Delegate::CallString(Arguments args)
{
	return (m_Host.get()->*m_Function.String)(args);
}

void Delegate::DeleteArgs(Arguments args)
{
	int count = m_ArgumentsT.get()->Count();
	for(int i=0; i <count; i++)
	{
		switch(m_ArgumentsT.get()->Get(i))
		{
		case A_Void:
			args[i].Void->Ref()->Release();
			break;
		case A_Bool:
			args[i].Bool->Ref()->Release();
			break;
		case A_Int:
			args[i].Int->Ref()->Release();
			break;
		case A_Float:
			args[i].Float->Ref()->Release();
			break;
		case A_String:
			args[i].String->Ref()->Release();
			break;
		default:
			throw new exception();
			break;
		}
	}
}

ICompiled * Delegate::Compile (Cursor * cursor)
{
	ICompiled * compiled = 0;

	ParseArgs * pa = new ParseArgs(new ArgumentCompiler(m_Compiler.get(), cursor));
	pa->Ref()->AddRef();

	bool bOk = true;

	for(int i=0; bOk && i < m_ArgumentsT.get()->Count(); i++)
	{
		bOk = pa->ParseNextArgTC( Translate(m_ArgumentsT.get()->Get(i)) );
	}

	if(bOk && pa->ParseEof())
	{
		int count = m_ArgumentsT.get()->Count();
		Arguments args = new Argument[count];
		for(int i=0; i < count; i++)
		{
			switch(m_ArgumentsT.get()->Get(i))
			{
			case A_Void:
				args[i].Void = pa->GetArg(i)->GetVoid();
				args[i].Void->Ref()->AddRef();
				break;
			case A_Bool:
				args[i].Bool = pa->GetArg(i)->GetBool();
				args[i].Bool->Ref()->AddRef();
				break;
			case A_Int:
				args[i].Int = pa->GetArg(i)->GetInt();
				args[i].Int->Ref()->AddRef();
				break;
			case A_Float:
				args[i].Float = pa->GetArg(i)->GetFloat();
				args[i].Float->Ref()->AddRef();
				break;
			case A_String:
				args[i].String = pa->GetArg(i)->GetString();
				args[i].String->Ref()->AddRef();
				break;
			default:
				throw new exception();
				break;
			}
		}

		DelegateC * delegateC = new DelegateC(this, args);

		switch(m_FunctionT)
		{
		case F_Void:
			compiled = new Compiled(dynamic_cast<IVoid *>(delegateC));
			break;
		case F_Bool:
			compiled = new Compiled(dynamic_cast<IBool *>(delegateC));
			break;
		case F_Int:
			compiled = new Compiled(dynamic_cast<IInt *>(delegateC));
			break;
		case F_Float:
			compiled = new Compiled(dynamic_cast<IFloat *>(delegateC));
			break;
		case F_String:
			compiled = new Compiled(dynamic_cast<IString *>(delegateC));
			break;
		default:
			throw new exception();
			break;
		}
	}

	pa->Ref()->Release();

	return compiled ? compiled : new Compiled(new Error());
}


Delegate * Delegate::New(ICompiler * compiler, FunctionHost * host, VoidFunction function, ArgumentsT * argumentsT)
{
	Function f;
	f.Void = function;

	return new Delegate(compiler, host, F_Void, f, argumentsT);
}

Delegate * Delegate::New(ICompiler * compiler, FunctionHost * host, BoolFunction function, ArgumentsT * argumentsT)
{
	Function f;
	f.Bool = function;

	return new Delegate(compiler, host, F_Bool, f, argumentsT);
}

Delegate * Delegate::New(ICompiler * compiler, FunctionHost * host, IntFunction function, ArgumentsT * argumentsT)
{
	Function f;
	f.Int = function;

	return new Delegate(compiler, host, F_Int, f, argumentsT);
}

Delegate * Delegate::New(ICompiler * compiler, FunctionHost * host, FloatFunction function, ArgumentsT * argumentsT)
{
	Function f;
	f.Float = function;

	return new Delegate(compiler, host, F_Float, f, argumentsT);
}

Delegate * Delegate::New(ICompiler * compiler, FunctionHost * host, StringFunction function, ArgumentsT * argumentsT)
{
	Function f;
	f.String = function;

	return new Delegate(compiler, host, F_String, f, argumentsT);
}

IRef * Delegate::Ref (void) {
	return this;
}

ICompiled::Type Delegate::Translate(ArgumentT type)
{
	switch(type)
	{
	case A_Void:
		return ICompiled::T_Void;
	case A_Bool:
		return ICompiled::T_Bool;
	case A_Int:
		return ICompiled::T_Int;
	case A_Float:
		return ICompiled::T_Float;
	case A_String:
		return ICompiled::T_String;
	default:
		throw new logic_error("type");
		break;
	}

	return ICompiled::T_None;
}


/*
IRef * FunctionRef(FunctionT functionT, Function function)
{
	switch(functionT)
	{
	case F_Void:
		return dynamic_cast<IRef *>(function.Void);
	case F_Bool:
		return dynamic_cast<IRef *>(function.Bool);
	case F_Int:
		return dynamic_cast<IRef *>(function.Int);
	case F_Float:
		return dynamic_cast<IRef *>(function.Float);
	case F_String:
		return dynamic_cast<IRef *>(function.String);
	default:
		throw new exception();
		break;
	}
}
*/

