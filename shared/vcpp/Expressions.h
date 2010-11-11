#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-10 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


// HLAE Expressions are compiled from
// a Scheme-styled syntax:

// Standard types (contract functions):
// Error
// Void
// Bool
// Int

// Primitive functions:
//
// BoolConstant: "false" | "true" -> Bool
// IntConstant: integer -> Int

// Standard functions:
// and && : Bool Bool+ -> Bool
// or || : Bool Bool+ -> Bool
// not ! : Bool -> Bool
// less < : Bool Bool -> Bool
// less < : Int Int -> Bool
// lessOrEqual <= Bool Bool -> Bool
// lessOrEqual <= Int Int -> Bool
// equal == : Bool Bool -> Bool
// equal == : Int Int -> Bool
// greater > : Bool Bool -> Bool
// greater > : Int Int -> Bool
// greaterOrEqual >= Bool Bool -> Bool
// greaterOrEqual >= Int Int -> Bool
// in: Bool Bool* -> Bool
// in: Int Int* -> Bool
// max: Bool+ -> Bool
// max: Int+ -> Int
// min: Bool+ -> Bool
// min: Int+ -> Int
// if ?: Bool Bool Bool -> Bool
// if ?: Bool Int Int -> Int
// do: -> Void
// do: (Void | Bool | Int)* Void -> Void
// do: (Void | Bool | Int)* Bool -> Bool
// do: (Void | Bool | Int)* Int -> Int

// 0. text
// 1. group parenthesis (tree)
// 2. resolve 

// empty = ;
// arguments = empty | WS function, arguments ;
// parenthesis = "(", WS*, identifier, WS*, arguments, WS* ")" ;
// function = parenthesis | identifier
// code = WS* function WS*


#include "Ref.h"

namespace Afx { namespace Expressions {


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

/*
struct __declspec(novtable) ICompiler abstract
{
	virtual ICompiled * Compile (Cursor & cur) abstract = 0;
};
*/


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


struct __declspec(novtable) Tools abstract
{	
	/// <summary>Creates a new bubble with standard operators / functions.</summary>
	static IBubble * StandardBubble (void);
};


////////////////////////////////////////////////////////////////////////////////
//
// Implementation support classes:

struct __declspec(novtable) IBoolGetter abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual bool Get (void) abstract = 0;
};

struct __declspec(novtable) IBoolSetter abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual void Set (bool value) abstract = 0;
};

struct __declspec(novtable) IIntGetter abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual int Get (void) abstract = 0;
};

struct __declspec(novtable) IIntSetter abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual void Set (int value) abstract = 0;
};


/// <summary>Compiles as: -&gt; Bool</summary>
class BoolGetter abstract :  public Ref,
	public IBoolGetter
{
public:
	virtual ICompiled * Compile (ICompileArgs * args);

	virtual bool Get (void) abstract = 0;

	virtual ::Afx::IRef * Ref (void);
};


/// <summary>Compiles as: Bool -&gt; Void</summary>
class BoolSetter abstract :  public Ref,
	public ICompileable,
	public IBoolSetter
{
public:
	virtual ICompiled * Compile (ICompileArgs * args);

	virtual void Set (bool value) abstract = 0;

	virtual ::Afx::IRef * Ref (void);
};


class BoolProperty abstract :  public Ref,
	public ICompileable,
	public IBoolGetter,
	public IBoolSetter
{
public:
	enum CompileAcces {
		CA_Property,
		CA_Getter,
		CA_Setter
	};

	BoolProperty(CompileAcces compileAccess);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual bool Get (void) abstract = 0;

	virtual ::Afx::IRef * Ref (void);

	virtual void Set (bool value) abstract = 0;

private:
	CompileAcces m_CompileAccess;
};


/// <summary>Compiles as: -&gt; Int</summary>
class IntGetter abstract :  public Ref,
	public ICompileable,
	public IIntGetter
{
public:
	virtual ICompiled * Compile (ICompileArgs * args);

	virtual int Get (void) abstract = 0;

	virtual ::Afx::IRef * Ref (void);
};


/// <summary>Compiles as: Int -&gt; Void</summary>
class IntSetter abstract :  public Ref,
	public ICompileable,
	public IIntSetter
{
public:
	virtual ICompiled * Compile (ICompileArgs * args);

	virtual void Set (int value) abstract = 0;

	virtual ::Afx::IRef * Ref (void);
};


class IntProperty abstract :  public Ref,
	public ICompileable,
	public IIntGetter,
	public IIntSetter
{
public:
	enum CompileAcces {
		CA_Property,
		CA_Getter,
		CA_Setter
	};

	IntProperty(CompileAcces compileAccess);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual int Get (void) abstract = 0;

	virtual ::Afx::IRef * Ref (void);

	virtual void Set (int value) abstract = 0;

private:
	CompileAcces m_CompileAccess;
};


/// <summary>Compiles as: -&gt; Void</summary>
class VoidFunction abstract :  public Ref,
	public IVoid,
	public ICompileable
{
public:
	virtual ICompiled * Compile (ICompileArgs * args);

	virtual void EvalVoid (void) abstract = 0;

	virtual ::Afx::IRef * Ref (void);
};


////////////////////////////////////////////////////////////////////////////////
//
// Implementation classes:


class BoolVariable :  public BoolProperty
{
public:
	BoolVariable(CompileAcces compileAccess, bool value);

	bool Get() const;
	virtual bool Get (void);

	virtual void Set (bool value);

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


class IntVariable :  public IntProperty
{
public:
	IntVariable(CompileAcces compileAccess, int value);

	int Get() const;
	virtual int Get (void);

	virtual void Set (int value);

private:
	int m_Value;
};



} } // namespace Afx { namespace Expr {
