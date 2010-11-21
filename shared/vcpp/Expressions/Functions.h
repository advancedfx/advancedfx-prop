#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-21 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


#include "Types.h"
#include "Compile.h"


namespace Afx { namespace Expressions {


struct __declspec(novtable) Functions abstract
{	
	static IBool * Bool (BoolT value);

	static IFloat * Float (IntT value);

	static IInt * Int (IntT value);

	static INull * Null (void);

	static IString * String (IStringValue * value);

	static IVoid * Void (void);
};

struct __declspec(novtable) FunctionsC abstract
{
	static ICompileable * And (ICompiler * compiler);

	static ICompiled * Bool (ICompiler * compiler, ICompileArgs * args, BoolT value);

	static ICompileable * CompileEval (ICompiler * compiler);

	/// <summary>
	/// Create a &quot;do&quot;-function that executes in a differnet compiler / bubble.
	/// This means you can use it to add a command function for cross-bubble access.
	/// </summary>
	static ICompileable * Do (ICompiler * compiler);

	static ICompileable * Equal (ICompiler * compiler);

	static ICompiled * Float (ICompiler * compiler, ICompileArgs * args, FloatT value);

	static ICompileable * Greater (ICompiler * compiler);

	static ICompileable * GreaterOrEqual (ICompiler * compiler);

	static ICompileable * If (ICompiler * compiler);

	static ICompileable * InBool (ICompiler * compiler);

	static ICompileable * InInt (ICompiler * compiler);

	static ICompiled * Int (ICompiler * compiler, ICompileArgs * args, IntT value);

	static ICompileable * Less (ICompiler * compiler);

	static ICompileable * LessOrEqual (ICompiler * compiler);

	static ICompileable * MaxBool (ICompiler * compiler);

	static ICompileable * MaxInt (ICompiler * compiler);

	static ICompileable * MinBool (ICompiler * compiler);

	static ICompileable * MinInt (ICompiler * compiler);

	static ICompileable * Not (ICompiler * compiler);

	static ICompileable * Null (ICompiler * compiler);

	static ICompileable * Or (ICompiler * compiler);

	static ICompileable * String (ICompiler * compiler);

	static ICompileable * StringFromFile (ICompiler * compiler);

	static ICompileable * Sum (ICompiler * compiler);

	static ICompileable * Void (ICompiler * compiler);
};


} } // namespace Afx { namespace Expr {
