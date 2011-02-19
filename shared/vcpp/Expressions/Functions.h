#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2011-01-05 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


#include "Types.h"
#include "Compile.h"


namespace Afx { namespace Expressions {


struct __declspec(novtable) FunctionCompilers abstract
{
	static ICompiler * And (ICompiler  * argCompiler);
/*
	static ICompiler * CompileEval (ICompiler * compiler);
*/

	/// <summary>
	/// Create a &quot;do&quot;-function that executes in a differnet compiler / bubble.
	/// This means you can use it to add a command function for cross-bubble access.
	/// </summary>
	static ICompiler * Do (ICompiler * compiler);

	static ICompiler * Equal (ICompiler  * argCompiler);

	static ICompiler * Greater (ICompiler  * argCompiler);

	static ICompiler * GreaterOrEqual (ICompiler  * argCompiler);

	static ICompiler * If (ICompiler  * argCompiler);

	static ICompiler * InBool (ICompiler  * argCompiler);

	static ICompiler * InInt (ICompiler  * argCompiler);

	static ICompiler * Less (ICompiler  * argCompiler);

	static ICompiler * LessOrEqual (ICompiler  * argCompiler);

	static ICompiler * MaxBool (ICompiler  * argCompiler);

	static ICompiler * MaxInt (ICompiler  * argCompiler);

	static ICompiler * MinBool (ICompiler  * argCompiler);

	static ICompiler * MinInt (ICompiler  * argCompiler);

	static ICompiler * Not (ICompiler  * argCompiler);

	static ICompiler * Null (ICompiler  * argCompiler);

	static ICompiler * Or (ICompiler  * argCompiler);

	static ICompiler * StringFromFile (ICompiler  * argCompiler);

	static ICompiler * Sum (ICompiler  * argCompiler);

	static ICompiler * Void (ICompiler  * argCompiler);
};


} } // namespace Afx { namespace Expr {
