#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-18 dominik.matrixstorm.com
//
// First changes
// 2010-11-18 dominik.matrixstorm.com

#include "Compile.h"
#include <vector>

using namespace std;


namespace Afx { namespace Expressions {


union Argument {
	IVoid * Void;
	IBool * Bool;
	IInt * Int;
	IFloat * Float;
	IString * String;
};

enum ArgumentT {
	A_Void,
	A_Bool,
	A_Int,
	A_Float,
	A_String
};

typedef Argument * Arguments;


class ArgumentsT : public Ref
{
public:
	static ArgumentsT * New ( void );
	static ArgumentsT * New ( int argCount, ArgumentT argumentT, ... );

	ArgumentT Get (int index) const;

	int Count (void) const;

private:
	typedef vector<ArgumentT> ArgsT;

	ArgsT m_Args;

	void Add(ArgumentT type);
};


class FunctionHost : public Ref
{
};

typedef VoidT (FunctionHost::*VoidFunction) (Arguments args);
typedef BoolT (FunctionHost::*BoolFunction) (Arguments args);
typedef IntT (FunctionHost::*IntFunction) (Arguments args);
typedef FloatT (FunctionHost::*FloatFunction) (Arguments args);
typedef IStringValue * (FunctionHost::*StringFunction) (Arguments args);


enum FunctionT {
	F_Void,
	F_Bool,
	F_Int,
	F_Float,
	F_String
};

union Function {
	VoidFunction Void;
	BoolFunction Bool;
	IntFunction Int;
	FloatFunction Float;
	StringFunction String;
};


class Delegate : public Compileable
{
public:
	static Delegate * New (ICompiler * compiler, FunctionHost * host, VoidFunction function, ArgumentsT * argumentsT);
	static Delegate * New (ICompiler * compiler, FunctionHost * host, BoolFunction function, ArgumentsT * argumentsT);
	static Delegate * New (ICompiler * compiler, FunctionHost * host, IntFunction function, ArgumentsT * argumentsT);
	static Delegate * New (ICompiler * compiler, FunctionHost * host, FloatFunction function, ArgumentsT * argumentsT);
	static Delegate * New (ICompiler * compiler, FunctionHost * host, StringFunction function, ArgumentsT * argumentsT);

	static ICompiled::Type Delegate::Translate(ArgumentT type);

	VoidT CallVoid(Arguments args);
	BoolT CallBool(Arguments args);
	IntT CallInt(Arguments args);
	FloatT CallFloat(Arguments args);
	IStringValue * CallString(Arguments args);

	void DeleteArgs(Arguments args);

	virtual ICompiled * Compile (ICompileArgs * args);

protected:
	virtual ~Delegate();

private:
	ArgumentsT * m_ArgumentsT;
	Function m_Function;
	FunctionT m_FunctionT;
	FunctionHost * m_Host;

	Delegate(ICompiler * compiler, FunctionHost * host, FunctionT functionT, Function function, ArgumentsT * argumentsT);
};


} } // namespace Afx { namespace Expr {
