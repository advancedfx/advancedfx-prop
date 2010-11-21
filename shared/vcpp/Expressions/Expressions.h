#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-20 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


// Dear programmer:
//
// In order to easy changes later on stick with
// the implementation support classes wherever possible.


// Syntax and information:
//
// resources/doc/AfxGoldSrc_xpress.html


#include "Delegate.h"
#include "Functions.h"
#include "Parse.h"
#include "Events.h"


namespace Afx { namespace Expressions {



struct __declspec(novtable) IBubble abstract
{
	static void AddStandardFunctions (IBubble * bubble);

	static IBubble * New (bool standardConstFunctions);

	virtual ::Afx::IRef * Ref (void) abstract = 0;

	/// <summary>Adds a named function to the bubble.</summary>
	virtual void Add(char const * name, ICompileable * compileable) abstract = 0;

	/// <summary>Compile</summary>
	/// <param name="code">code to compile</expression>
	/// <returns>The compiled code.</returns>
	virtual ICompiled * Compile(char const * code) abstract = 0;

	virtual ICompiler * Compiler (void) abstract = 0;

	/// <summary>Hides all exposed functions matching the given name.</summary<
	virtual void Hide(char const * name) abstract = 0;
};

ICompileable * NewCompileableFnDef(ICompiler * compiler, ICompiler * & outDefCompiler);

} } // namespace Afx { namespace Expr {
