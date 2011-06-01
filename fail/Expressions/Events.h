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

	
/// <summary>
/// Compiles to:<br />
/// BoolEvent -&gt; Bool - Get event
/// BoolEvent Bool -&gt; Void - Set event
/// </summary>
class BoolEvent : public Ref,
	public IBool,
	public ICompiler
{
public:
	BoolEvent(ICompiler * compiler, BoolT initialValue);

	virtual ICompiled * Compile (ICompileNode * node);

	virtual BoolT EvalBool (void);

	void SetEvent(IBool * value);

	virtual ::Afx::IRef * Ref (void);

protected:
	virtual ~BoolEvent();

private:
	RefIPtr<ICompiler> m_Compiler;
	IBool * m_Bool;
};


/// <summary>
/// Compiles to:<br />
/// IntEvent -&gt; Int - Get event
/// IntEvent Int -&gt; Void - Set event
/// </summary>
class IntEvent : public Ref,
	public IInt,
	public ICompiler
{
public:
	IntEvent(ICompiler * compiler, IntT initialValue);

	virtual ICompiled * Compile (ICompileNode * node);

	virtual IntT EvalInt (void);

	void SetEvent(IInt * value);

	virtual ::Afx::IRef * Ref (void);

protected:
	virtual ~IntEvent();

private:
	RefIPtr<ICompiler> m_Compiler;
	IInt * m_Int;
};


/// <summary>
/// Compiles to:<br />
/// VoidEvent -&gt; Void - Get event
/// VoidEvent Void -&gt; Void - Set event
/// </summary>
class VoidEvent : public Ref,
	public IVoid,
	public ICompiler
{
public:
	VoidEvent(ICompiler * compiler);

	virtual ICompiled * Compile (ICompileNode * node);

	virtual VoidT EvalVoid (void);

	void SetEvent(IVoid * value);

	virtual ::Afx::IRef * Ref (void);

protected:
	virtual ~VoidEvent();

private:
	RefIPtr<ICompiler> m_Compiler;
	IVoid * m_Void;
};


} } // namespace Afx { namespace Expr {
