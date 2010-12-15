#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-12-15 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


#include "Types.h"
#include "Compile.h"


namespace Afx { namespace Expressions {


struct __declspec(novtable) FunctionCompilers abstract
{
	static ICompiler * And (ICompiler * compiler);

	static ICompiler * CompileEval (ICompiler * compiler);

	/// <summary>
	/// Create a &quot;do&quot;-function that executes in a differnet compiler / bubble.
	/// This means you can use it to add a command function for cross-bubble access.
	/// </summary>
	static ICompiler * Do (ICompiler * compiler);

	static ICompiler * Equal (ICompiler * compiler);

	static ICompiler * Greater (ICompiler * compiler);

	static ICompiler * GreaterOrEqual (ICompiler * compiler);

	static ICompiler * If (ICompiler * compiler);

	static ICompiler * InBool (ICompiler * compiler);

	static ICompiler * InInt (ICompiler * compiler);

	static ICompiler * Less (ICompiler * compiler);

	static ICompiler * LessOrEqual (ICompiler * compiler);

	static ICompiler * MaxBool (ICompiler * compiler);

	static ICompiler * MaxInt (ICompiler * compiler);

	static ICompiler * MinBool (ICompiler * compiler);

	static ICompiler * MinInt (ICompiler * compiler);

	static ICompiler * Not (ICompiler * compiler);

	static ICompiler * Null (ICompiler * compiler);

	static ICompiler * Or (ICompiler * compiler);

	static ICompiler * StringFromFile (ICompiler * compiler);

	static ICompiler * Sum (ICompiler * compiler);

	static ICompiler * Void (ICompiler * compiler);
};


} } // namespace Afx { namespace Expr {
