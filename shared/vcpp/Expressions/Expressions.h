#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-16 dominik.matrixstorm.com
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
#include "Parse.h"


namespace Afx { namespace Expressions {



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
/// BoolEvent -&gt; Bool - Get event
/// BoolEvent Bool -&gt; Void - Set event
/// </summary>
class BoolEvent : public Compileable,
	public IBool
{
public:
	BoolEvent(ICompiler * compiler, BoolT initialValue);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual BoolT EvalBool (void);

	void SetEvent(IBool * value);

	virtual ::Afx::IRef * Ref (void);

protected:
	virtual ~BoolEvent();

private:
	IBool * m_Bool;
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


/// <summary>
/// Compiles to:<br />
/// IntEvent -&gt; Int - Get event
/// IntEvent Int -&gt; Void - Set event
/// </summary>
class IntEvent : public Compileable,
	public IInt
{
public:
	IntEvent(ICompiler * compiler, IntT initialValue);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual IntT EvalInt (void);

	void SetEvent(IInt * value);

	virtual ::Afx::IRef * Ref (void);

protected:
	virtual ~IntEvent();

private:
	IInt * m_Int;
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
/// VoidEvent -&gt; Void - Get event
/// VoidEvent Void -&gt; Void - Set event
/// </summary>
class VoidEvent : public Compileable,
	public IVoid
{
public:
	VoidEvent(ICompiler * compiler);

	virtual ICompiled * Compile (ICompileArgs * args);

	virtual VoidT EvalVoid (void);

	void SetEvent(IVoid * value);

	virtual ::Afx::IRef * Ref (void);

protected:
	virtual ~VoidEvent();

private:
	IVoid * m_Void;
};


} } // namespace Afx { namespace Expr {
