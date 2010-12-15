#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-12-15 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


#include "Compile.h"


namespace Afx { namespace Expressions {



struct __declspec(novtable) IBubble abstract
{
	static void AddStandardFunctions (IBubble * bubble);

	static IBubble * New ();

	virtual IRef * Ref (void) abstract = 0;

	virtual ICompiler * Compiler (void) abstract = 0;

	/// <summary>Adds a named compiler to the bubble.</summary>
	virtual void Add(IStringValue * name, ICompiler * compiler) abstract = 0;

	/// <summary>Compile</summary>
	/// <param name="code">code to compile</expression>
	/// <returns>The compiled code.</returns>
	virtual ICompiled * Compile(IStringValue * code) abstract = 0;

	/// <summary>Hides all exposed function compilers matching the given name.</summary<
	virtual void Hide(IStringValue * name) abstract = 0;
};


} } // namespace Afx { namespace Expr {

