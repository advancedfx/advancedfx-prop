#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-02 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


// HLAE Expressions are compiled from
// a Scheme-styled syntax

// Standard types (contract functions):
// Error
// Void
// Bool
// Int

// Primitive functions:
//
// IntConstant -> Int
// BoolConstant -> Bool

// Standard functions:
// and && : Bool Bool+ -> Bool
// or || : Bool Bool+ -> Bool
// not ! : Bool -> Bool
// less < : Bool Bool -> Bool
// less < : Int Int -> Bool
// equal == : Bool Bool -> Bool
// equal == : Int Int -> Bool
// greater > : Int Int -> Bool
// lessOrEqual <= Int Int -> Bool
// greaterOrEqual >= Int Int -> Bool
// in: Bool Bool* -> Bool
// in: Int Int* -> Bool
// max: Bool+ -> Bool
// max: Int+ -> Int
// min: Bool+ -> Bool
// min: Int+ -> Int

// 0. text
// 1. group parenthesis (tree)
// 2. resolve 

#include "Ref.h"

namespace Afx { namespace Expr {


struct __declspec(novtable) IError abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;
};

struct __declspec(novtable) IVoid abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual void EvalVoid (void) abstract = 0;
};

struct __declspec(novtable) IBool abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual bool EvalBool (void) abstract = 0;
};

struct __declspec(novtable) IInt abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual int EvalInt (void) abstract = 0;
};

struct __declspec(novtable) ICompiled abstract
{
	enum Type
	{
		T_None,
		T_Error,
		T_Void,
		T_Bool,
		T_Int
	};

	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual Type GetType (void) abstract = 0;

	virtual IError * GetError (void) abstract = 0;

	virtual IVoid * GetVoid (void) abstract = 0;

	virtual IBool * GetBool (void) abstract = 0;

	virtual IInt * GetInt (void) abstract = 0;
};


struct __declspec(novtable) ICompileArgs abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual ICompiled * GetArg (int index) abstract = 0;

	virtual int GetCount (void) abstract = 0;
};


struct __declspec(novtable) ICompileable abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual ICompiled * Compile (ICompileArgs * args) abstract = 0;
};


struct __declspec(novtable) IBubble abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	/// <summary>Adds a function to the bubble.</summary>
	virtual void Add(char const * name, ICompileable * compileable) abstract = 0;

	/// <summary>Compile</summary>
	/// <param name="code">code to compile</expression>
	/// <returns>The compiled code.</returns>
	virtual ICompiled * Compile(char const * code) abstract = 0;
};


struct __declspec(novtable) BubbleFactory abstract
{	
	/// <summary>Creates a new bubble with standard operators / functions.</summary>
	static IBubble * StandardBubble (void);
};


////////////////////////////////////////////////////////////////////////////////
//
// Implementation classes:


class BoolVariable :  public Ref,
	public IBool,
	public ICompileable
{
public:
	BoolVariable();
	BoolVariable(bool value);

	virtual ICompiled * Compile (ICompileArgs * args);

	bool Get() const;

	virtual bool EvalBool (void);

	virtual ::Afx::IRef * Ref (void);

	void Set(bool value);

protected:
	virtual ~BoolVariable();

private:
	bool m_Value;
};


class Compiled : public Ref,
	public ICompiled
{
public:
	Compiled(IError * value);
	Compiled(IVoid * value);
	Compiled(IBool * value);
	Compiled(IInt * value);

	virtual enum ICompiled::Type GetType();

	virtual IBool * GetBool();

	virtual IVoid * GetVoid();
	
	virtual IError * GetError();
	
	virtual IInt * GetInt();

	virtual ::Afx::IRef * Ref();

protected:
	virtual ~Compiled();

private:
	enum ICompiled::Type m_Type;
	union {
		IError * Error;
		IVoid * Void;
		IBool * Bool;
		IInt * Int;
	} m_Value;
};


class IntVariable :  public Ref,
	public IInt,
	public ICompileable
{
public:
	IntVariable();
	IntVariable(int value);

	virtual ICompiled * Compile (ICompileArgs * args);

	int Get() const;

	virtual int EvalInt (void);

	virtual ::Afx::IRef * Ref (void);

	void Set(int value);

protected:
	virtual ~IntVariable();

private:
	int m_Value;
};



} } // namespace Afx { namespace Expr {
