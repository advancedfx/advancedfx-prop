#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-15 dominik.matrixstorm.com
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
// Null
// Void
// Bool
// Int
// Float
// String


// Standard functions:
//
// boolValue -> Bool
// integerValue -> Int
// floatValue -> Float
// "null" T* -> Null, where T is not Error
// "void" T* -> Void, where T is not Error
// "string" -> String
// "string" stringText -> String
// "s" = "string"
// "and" |  Bool Bool+ -> Bool // short-circuit evaluation
// "&&" = "and"
// "or" Bool Bool+ -> Bool // short-circuit evaluation
// "||" = "or"
// "not" Bool -> Bool
// "!" = "not"
// "less" T T -> Bool, where T = Void | Bool | Int | Float
// "<" = "less"
// "lessOrEqual" T T -> Bool, where T = Void | Bool | Int | Float
// "<=" = "lessOrEqual"
// "equal" T T -> Bool, where T = Void | Bool | Int | Float
// "==" = "equal"
// "greater" T T -> Bool, where T = Void | Bool | Int | Float
// ">" = "greater"
// "greaterOrEqual" T T -> Bool, where T = Void | Bool | Int | Float
// ">=" = "greaterOrEqual"
// "in" T T* -> Bool, where T = Bool | Int // short-circuit evaluation
// "max" Bool+ -> Bool
// "max" Int+ -> Int
// "min" Bool+ -> Bool
// "min" Int+ -> Int
// "if" |  : Bool T T -> T, where T = Void | Bool | Int | Float
// "?" = "if"
// "do" (Null | Void | Bool | Int | Float)* T -> T, where T = Void | Bool | Int | Float
// "." = "do"
// "sum" T+ -> T, where T = Int | Float
// "+" = "sum"
// "stringFromFile" String -> String
// "compile" String -> Error | Void  | Bool | Int | Float | String


#include "Ref.h"
#include "Cursor.h"


namespace Afx { namespace Expressions {

typedef bool BoolT;
typedef double FloatT;
typedef long IntT;
typedef char const * StringDataT;
typedef void VoidT;


struct __declspec(novtable) IError abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;
};

struct __declspec(novtable) INull abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;
};

/// <summary>Function that evaluates to VoidT</summary>
struct __declspec(novtable) IVoid abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual VoidT EvalVoid (void) abstract = 0;
};

/// <summary>Function that evaluates to BoolT</summary>
struct __declspec(novtable) IBool abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual BoolT EvalBool (void) abstract = 0;
};

/// <summary>Function that evaluates to IntT</summary>
struct __declspec(novtable) IInt abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual IntT EvalInt (void) abstract = 0;
};

/// <summary>Function that evaluates to FloatT</summary>
struct __declspec(novtable) IFloat abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual FloatT EvalFloat (void) abstract = 0;
};


/// <summary>A string</summary>
struct __declspec(novtable) IStringValue
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	/// <remarks>
	/// GetLength is at least strlen of GetData, but may be longer i.e. in case of binary data
	/// containing \0 characters
	/// </remarks>
	virtual int GetLength (void) abstract = 0;

	/// <retruns>Binary data, that is guaranteed to be 0-terminated.</returns>
	virtual StringDataT GetData (void) abstract = 0;
};


/// <summary>Function that evaluates to a IStringValue</summary>
struct __declspec(novtable) IString abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	/// <remarks>The result is a reference type and not a value type,
	///   so don't forget to apply reference counting!</remarks>
	virtual IStringValue * EvalString (void) abstract = 0;
};


struct __declspec(novtable) ICompiled abstract
{
	enum Type
	{
		T_None,
		T_Error,
		T_Null,
		T_Void,
		T_Bool,
		T_Int,
		T_Float,
		T_String
	};

	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual Type GetType (void) abstract = 0;

	virtual IError * GetError (void) abstract = 0;

	virtual INull * GetNull (void) abstract = 0;

	virtual IVoid * GetVoid (void) abstract = 0;

	virtual IBool * GetBool (void) abstract = 0;

	virtual IInt * GetInt (void) abstract = 0;

	virtual IFloat * GetFloat (void) abstract = 0;

	virtual IString * GetString (void) abstract = 0;
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

	/// <summary>
	/// Create a &quot;do&quot;-function that executes in a differnet compiler / bubble.
	/// This means you can use it to add a command function for cross-bubble access.
	/// </summary>
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


/// <summary>Support class for implementing ICompileable with an associated compiler.</summary>
class Compileable abstract : public Ref,
	public ICompileable
{
public:
	Compileable(ICompiler * compiler);

	virtual ICompiled * Compile (ICompileArgs * args) abstract = 0;

	virtual ::Afx::IRef * Ref (void);

protected:
	ICompiler * m_Compiler;

	virtual ~Compileable();

};


/// <summary>
/// Compiles to:<br />
/// (BoolGetter) -&gt; Bool
/// </summary>
class BoolGetter abstract :  public Compileable,
	public IBoolGetter
{
public:
	BoolGetter(ICompiler * compiler);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual ::Afx::IRef * Ref (void);

	virtual BoolT Get (void) abstract = 0;
};


/// <summary>
/// Compiles to:<br />
/// (BoolSetter Bool) -&gt; Void
/// </summary>
class BoolSetter abstract :  public Compileable,
	public IBoolSetter
{
public:
	BoolSetter(ICompiler * compiler);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual ::Afx::IRef * Ref (void);

	virtual void Set (BoolT value) abstract = 0;
};


/// <summary>
/// Compiles to (depends on compileAccess):<br />
/// (BoolGetter) -&gt; Bool<br />
/// (BoolSetter Bool) -&gt; Void
/// </summary>
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


/// <summary>Similar to BoolGetter.</summary>
class FloatGetter abstract :  public Compileable,
	public IFloatGetter
{
public:
	FloatGetter(ICompiler * compiler);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual FloatT Get (void) abstract = 0;

	virtual ::Afx::IRef * Ref (void);
};


/// <summary>Similar to BoolSetter.</summary>
class FloatSetter abstract :  public Compileable,
	public IFloatSetter
{
public:
	FloatSetter(ICompiler * compiler);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual ::Afx::IRef * Ref (void);

	virtual void Set (FloatT value) abstract = 0;
};


/// <summary>Similar to BoolProperty.</summary>
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


/// <summary>Similar to BoolGetter.</summary>
class IntGetter abstract :  public Compileable,
	public IIntGetter
{
public:
	IntGetter(ICompiler * compiler);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual IntT Get (void) abstract = 0;

	virtual ::Afx::IRef * Ref (void);

};


/// <summary>Similar to BoolSetter.</summary>
class IntSetter abstract :  public Compileable,
	public IIntSetter
{
public:
	IntSetter(ICompiler * compiler);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual ::Afx::IRef * Ref (void);

	virtual void Set (IntT value) abstract = 0;
};


/// <summary>Similar to BoolProperty.</summary>
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


/// <summary>
/// Compiles to:<br />
/// (VoidFunction) -&gt; Void
/// </summary>
class VoidFunction abstract :  public Compileable,
	public IVoid
{
public:
	VoidFunction(ICompiler * compiler);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual VoidT EvalVoid (void) abstract = 0;

	virtual ::Afx::IRef * Ref (void);
};


/// <summary>Implementation of BoolProperty as an variable.</summary>
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


/// <summary>ICompiled container implementation.</summary>
class Compiled : public Ref,
	public ICompiled
{
public:
	Compiled(IError * value);
	Compiled(INull * value);
	Compiled(IVoid * value);
	Compiled(IBool * value);
	Compiled(IInt * value);
	Compiled(IFloat * value);
	Compiled(IString * value);

	virtual enum ICompiled::Type GetType();
	
	virtual IError * GetError();

	virtual INull * GetNull();

	virtual IVoid * GetVoid();

	virtual IBool * GetBool();
	
	virtual IInt * GetInt();

	virtual IFloat * GetFloat();

	virtual IString * GetString();

	virtual ::Afx::IRef * Ref();

protected:
	virtual ~Compiled();

private:
	enum ICompiled::Type m_Type;
	union {
		IError * Error;
		INull * Null;
		IVoid * Void;
		IBool * Bool;
		IInt * Int;
		IFloat * Float;
		IString * String;
	} m_Value;
};


/// <summary>Implementation of FloatProperty as an variable.</summary>
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


/// <summary>Implementation of IntProperty as an variable.</summary>
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

/// <summary>
/// Compiles to:<br />
/// VoidEvent Null -&gt; Void - Set Null (no) event
/// VoidEvent Void -&gt; Void - Set Void event
/// </summary>
class VoidEvent : public Compileable
{
public:
	VoidEvent(ICompiler * compiler);

	void CallEvent();

	virtual ICompiled * Compile (ICompileArgs * args);

	bool HasEvent();

	void SetEvent(IVoid * value);

protected:
	virtual ~VoidEvent();

private:
	IVoid * m_Void;
};


} } // namespace Afx { namespace Expr {
