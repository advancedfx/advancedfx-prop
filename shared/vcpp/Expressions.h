#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-10-24 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com

#include "Ref.h"

namespace Afx { namespace Expr {

/*
typedef unsigned __int8 FaceGuid[16];

struct __declspec(novtable) IFunction abstract
{
	//virtual void AddRef (void) abstract = 0;
	//virtual void Release (void) abstract = 0;
	// virtual IRef * Ref() abstract = 0;

	/// <summary>Evaluate the function</summary>
	/// <param name="function">0 or an function</param>
	/// <returns>0 or an function. A function must released (Release)</returns>
	virtual IFunction * Eval(IFunction * function) abstract = 0;

	/// <summary>Run-time interface access</summary>
	/// <retruns>0 if requested face is not supported, otherwise the face requested</returns>
	virtual void * GetFace(FaceGuid const guid);
};
*/

struct __declspec(novtable) IBoolFn abstract
{
	virtual ::Afx::IRef * Ref() abstract = 0;

	virtual bool EvalBool (void) abstract = 0;
};

struct __declspec(novtable) IIntFn abstract
{
	virtual ::Afx::IRef * Ref() abstract = 0;

	virtual int EvalInt (void) abstract = 0;
};


struct __declspec(novtable) IBubble abstract
{
	/// <summary>Compile an bool-function</summary>
	/// <param name="expression">code to compile</expression>
	/// <returns>0 in case compiling failed, otherwise the compiled function</returns>
	virtual IBoolFn * CompileBool(char const * expression) abstract = 0;

	/// <summary>Compile an int-function</summary>
	/// <param name="expression">code to compile</expression>
	/// <returns>0 in case compiling failed, otherwise the compiled function</returns>
	virtual IIntFn * CompileInt(char const * expression) abstract = 0;

	/// <retruns>false in case registration failed, otherwise true</returns>
	virtual bool Add(IBoolFn * fn, char const * name) abstract = 0;

	/// <retruns>false in case registration failed, otherwise true</returns>
	virtual bool Add(IIntFn * fn, char const * name) abstract = 0;
};

struct __declspec(novtable) FnFactory
{
	static IBoolFn * And (IBoolFn * fn1, IBoolFn * fn2);
	static IBoolFn * Const (bool val);
	static IIntFn * Const (int val);
	static IBoolFn * Equal (IBoolFn * fn1, IBoolFn * fn2);
	static IBoolFn * Equal (IIntFn * fn1, IIntFn * fn2);
	static IBoolFn * Greater (IIntFn * fn1, IIntFn * fn2);
	static IBoolFn * GreaterOrEqual (IIntFn * fn1, IIntFn * fn2);
	static IBoolFn * InConstSet (IIntFn * fn1, int valCount, int const * vals);
	static IBoolFn * Less (IIntFn * fn1, IIntFn * fn2);
	static IBoolFn * LessOrEqual (IIntFn * fn1, IIntFn * fn2);
	static IBoolFn * Not (IBoolFn * fn1);
	static IBoolFn * Or (IBoolFn * fn1, IBoolFn * fn2);
};

struct __declspec(novtable) BubbleFactory
{	
	/// <summary>Creates a new bubble with standard operators / functions.</summary>
	static IBubble * StandardBubble (void);
};


} } // namespace Afx { namespace Expr {
