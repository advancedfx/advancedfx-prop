#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-02 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


// Parsing stage grammar (pseudo):
// Start = Input
//
// Empty = ""
// Type = "int" | "bool"
// BoolConstant = "false" | "true"
// Constant = BoolConstant | IntConstant
// InfOp2 = "&&" | "==" | ">" | ">=" | "in" | "<" | "<=" | "||"
// PreOp1 = "!"
// MoreArrayVals = Empty | WS* "," WS* Constant MoreArrayVals
// ArrayVals = Empty | Constant MoreArrayVals
// Array = "new" WS+ Type WS* "[" WS* "]" WS* "{" WS* ArrayVals WS* "}"
// Parenthesis = "(" WS* Expression WS* ")"
// InfixOp2 = Expression WS* InfOp2 WS* Expression
// PrefixOp1 = PreOp1 WS* Expression
// Function = PrefixOp1 | InfixOp2
// Expression = Parenthesis | Constant | Array | Function | Variable
// Input = WS* Expression WS*
//
// If multiple options are possible, the further left option is expanded / tried first.


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

enum CompiledType {
	CT_Error = 0,
	CT_BoolFn,
	CT_IntFn
};

struct __declspec(novtable) IError abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual char const * GetErrorString (void);
};

struct __declspec(novtable) IBoolFn abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual bool EvalBool (void) abstract = 0;
};

struct __declspec(novtable) IIntFn abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual int EvalInt (void) abstract = 0;
};


struct __declspec(novtable) ICompiled abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual CompiledType GetType (void) abstract = 0;

	virtual IBoolFn * GetBoolFn (void) abstract = 0;
	virtual IError * GetError (void) abstract = 0;
	virtual IIntFn * GetIntFn (void) abstract = 0;
};


class BoolVariable :  public Ref,
	public IBoolFn
{
public:
	BoolVariable();
	BoolVariable(bool value);

	bool Get() const;

	virtual bool EvalBool (void);
	virtual ::Afx::IRef * Ref (void);

	void Set(bool value);

protected:
	virtual ~BoolVariable();

private:
	bool m_Value;
};


class IntVariable :  public Ref,
	public IIntFn
{
public:
	IntVariable();
	IntVariable(int value);

	int Get() const;

	virtual int EvalInt (void);
	virtual ::Afx::IRef * Ref (void);

	void Set(int value);

protected:
	virtual ~IntVariable();

private:
	int m_Value;
};


struct __declspec(novtable) IBubble abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	/// <retruns>false in case registration failed, otherwise true</returns>
	virtual bool Add(IBoolFn * fn, char const * name) abstract = 0;

	/// <retruns>false in case registration failed, otherwise true</returns>
	virtual bool Add(IIntFn * fn, char const * name) abstract = 0;

	/// <summary>Compile</summary>
	/// <param name="code">code to compile</expression>
	/// <returns>The compiled code (errors are resolved to IError)</returns>
	virtual ICompiled * Compile(char const * code) abstract = 0;
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
	static IBoolFn * InConstSet (IBoolFn * fn1, int valCount, bool const * vals);
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
