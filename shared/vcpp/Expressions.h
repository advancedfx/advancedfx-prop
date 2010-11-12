#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-11 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


// Dear programmer:
//
// In order to easy changes later on stick with
// the implementation support classes wherever possible.


// Expression syntax (EBNF):
//
// empty = ;
// arguments = empty | WS, WS*, function, arguments ;
// parenthesis = "(", WS*, identifier, arguments, WS*, ")" ;
// function = parenthesis | identifier ;
// code = WS*, function, WS* ;


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
// do .: -> Void
// do .: (Void | Bool | Int | Float)* Void -> Void
// do .: (Void | Bool | Int | Float)* Bool -> Bool
// do .: (Void | Bool | Int | Float)* Int -> Int


#include "Ref.h"
#include "Cursor.h"


namespace Afx { namespace Expressions {

typedef bool BoolT;
typedef double FloatT;
typedef long IntT;
typedef void VoidT;

struct __declspec(novtable) IError abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;
};

struct __declspec(novtable) IVoid abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual VoidT EvalVoid (void) abstract = 0;
};

struct __declspec(novtable) IBool abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual BoolT EvalBool (void) abstract = 0;
};

struct __declspec(novtable) IInt abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual IntT EvalInt (void) abstract = 0;
};

struct __declspec(novtable) IFloat abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual FloatT EvalFloat (void) abstract = 0;
};

struct __declspec(novtable) ICompiled abstract
{
	enum Type
	{
		T_None,
		T_Error,
		T_Void,
		T_Bool,
		T_Int,
		T_Float
	};

	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual Type GetType (void) abstract = 0;

	virtual IError * GetError (void) abstract = 0;

	virtual IVoid * GetVoid (void) abstract = 0;

	virtual IBool * GetBool (void) abstract = 0;

	virtual IInt * GetInt (void) abstract = 0;

	virtual IFloat * GetFloat (void) abstract = 0;
};


struct __declspec(novtable) ICompiler abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual ICompiled * Compile_Function (Cursor & cursor) abstract = 0;
};

struct __declspec(novtable) ICompileArgs abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual bool HasNextArg (void) abstract = 0;

	virtual ICompiled * CompileNextArg (ICompiler * compiler) abstract = 0;
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

	virtual ICompiler * Compiler (void) abstract = 0;
};


struct __declspec(novtable) Tools abstract
{	
	/// <summary>Creates a new bubble with standard operators / functions.</summary>
	static IBubble * StandardBubble (void);

	/// <summary>This can be used to add commands that execute in a differnet compiler / bubble.</summary>
	static ICompileable * FnDoCompileable(ICompiler * compiler);
};


////////////////////////////////////////////////////////////////////////////////
//
// Implementation support classes:

struct __declspec(novtable) IBoolGetter abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual BoolT Get (void) abstract = 0;
};

struct __declspec(novtable) IBoolSetter abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual void Set (BoolT value) abstract = 0;
};

struct __declspec(novtable) IFloatGetter abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual FloatT Get (void) abstract = 0;
};

struct __declspec(novtable) IFloatSetter abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual void Set (FloatT value) abstract = 0;
};

struct __declspec(novtable) IIntGetter abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual IntT Get (void) abstract = 0;
};

struct __declspec(novtable) IIntSetter abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual void Set (IntT value) abstract = 0;
};


class Compileable abstract : public Ref,
	public ICompileable
{
public:
	Compileable(ICompiler * compiler);

	virtual ICompiled * Compile (ICompileArgs * args) abstract = 0;

protected:
	ICompiler * m_Compiler;

	virtual ~Compileable();

};


/// <summary>Compiles as: -&gt; Bool</summary>
class BoolGetter abstract :  public Compileable,
	public IBoolGetter
{
public:
	BoolGetter(ICompiler * compiler);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual BoolT Get (void) abstract = 0;

	virtual ::Afx::IRef * Ref (void);
};


/// <summary>Compiles as: Bool -&gt; Void</summary>
class BoolSetter abstract :  public Compileable,
	public IBoolSetter
{
public:
	BoolSetter(ICompiler * compiler);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual void Set (BoolT value) abstract = 0;

	virtual ::Afx::IRef * Ref (void);
};


class BoolProperty abstract :  public Compileable,
	public IBoolGetter,
	public IBoolSetter
{
public:
	enum CompileAcces {
		CA_Property,
		CA_Getter,
		CA_Setter
	};

	BoolProperty(ICompiler * compiler, CompileAcces compileAccess);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual BoolT Get (void) abstract = 0;

	virtual ::Afx::IRef * Ref (void);

	virtual void Set (BoolT value) abstract = 0;

private:
	CompileAcces m_CompileAccess;
};



/// <summary>Compiles as: -&gt; Float</summary>
class FloatGetter abstract :  public Compileable,
	public IFloatGetter
{
public:
	FloatGetter(ICompiler * compiler);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual FloatT Get (void) abstract = 0;

	virtual ::Afx::IRef * Ref (void);
};


/// <summary>Compiles as: Float -&gt; Void</summary>
class FloatSetter abstract :  public Compileable,
	public IFloatSetter
{
public:
	FloatSetter(ICompiler * compiler);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual void Set (FloatT value) abstract = 0;

	virtual ::Afx::IRef * Ref (void);
};


class FloatProperty abstract :  public Compileable,
	public IFloatGetter,
	public IFloatSetter
{
public:
	enum CompileAcces {
		CA_Property,
		CA_Getter,
		CA_Setter
	};

	FloatProperty(ICompiler * compiler, CompileAcces compileAccess);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual FloatT Get (void) abstract = 0;

	virtual ::Afx::IRef * Ref (void);

	virtual void Set (FloatT value) abstract = 0;

private:
	CompileAcces m_CompileAccess;
};


/// <summary>Compiles as: -&gt; Int</summary>
class IntGetter abstract :  public Compileable,
	public IIntGetter
{
public:
	IntGetter(ICompiler * compiler);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual IntT Get (void) abstract = 0;

	virtual ::Afx::IRef * Ref (void);
};


/// <summary>Compiles as: Int -&gt; Void</summary>
class IntSetter abstract :  public Compileable,
	public IIntSetter
{
public:
	IntSetter(ICompiler * compiler);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual void Set (IntT value) abstract = 0;

	virtual ::Afx::IRef * Ref (void);
};


class IntProperty abstract :  public Compileable,
	public IIntGetter,
	public IIntSetter
{
public:
	enum CompileAcces {
		CA_Property,
		CA_Getter,
		CA_Setter
	};

	IntProperty(ICompiler * compiler, CompileAcces compileAccess);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual IntT Get (void) abstract = 0;

	virtual ::Afx::IRef * Ref (void);

	virtual void Set (IntT value) abstract = 0;

private:
	CompileAcces m_CompileAccess;
};


/// <summary>Compiles as: -&gt; Void</summary>
class VoidFunction abstract :  public Compileable,
	public IVoid
{
public:
	VoidFunction(ICompiler * compiler);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual VoidT EvalVoid (void) abstract = 0;

	virtual ::Afx::IRef * Ref (void);
};


class BoolVariable :  public BoolProperty
{
public:
	BoolVariable(ICompiler * compiler, CompileAcces compileAccess, bool value);

	BoolT Get() const;
	virtual BoolT Get (void);

	virtual void Set (BoolT value);

private:
	BoolT m_Value;
};


class Compiled : public Ref,
	public ICompiled
{
public:
	Compiled(IError * value);
	Compiled(IVoid * value);
	Compiled(IBool * value);
	Compiled(IInt * value);
	Compiled(IFloat * value);

	virtual enum ICompiled::Type GetType();

	virtual IBool * GetBool();

	virtual IVoid * GetVoid();
	
	virtual IError * GetError();
	
	virtual IInt * GetInt();

	virtual IFloat * GetFloat();

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
		IFloat * Float;
	} m_Value;
};

class FloatVariable :  public FloatProperty
{
public:
	FloatVariable(ICompiler * compiler, CompileAcces compileAccess, int value);

	FloatT Get() const;
	virtual FloatT Get (void);

	virtual void Set (FloatT value);

private:
	FloatT m_Value;
};



class IntVariable :  public IntProperty
{
public:
	IntVariable(ICompiler * compiler, CompileAcces compileAccess, int value);

	IntT Get() const;
	virtual IntT Get (void);

	virtual void Set (IntT value);

private:
	IntT m_Value;
};


} } // namespace Afx { namespace Expr {
