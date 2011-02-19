#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-12-15 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


#include "Events.h"

#include "Functions.h"
#include "ParseArgs.h"

using namespace Afx;
using namespace Afx::Expressions;


// BoolEvent ///////////////////////////////////////////////////////////////////


class BoolEventSet : public Ref,
	public IVoid
{
public:
	BoolEventSet(BoolEvent * boolEvent, IBool * boolFn)
	: m_BoolEvent(boolEvent), m_Bool(boolFn)
	{
		boolFn->Ref()->AddRef();
		boolEvent->Ref()->AddRef();
	}

	virtual VoidT EvalVoid (void) {
		m_BoolEvent->SetEvent(m_Bool);
	}

	::Afx::IRef * Ref (void)
	{
		return dynamic_cast<::Afx::IRef *>(this);
	}

protected:
	virtual ~BoolEventSet()
	{
		m_Bool->Ref()->Release();
		m_BoolEvent->Ref()->Release();
	}

private:
	IBool * m_Bool;
	BoolEvent * m_BoolEvent;
};


BoolEvent::BoolEvent(ICompiler * compiler, BoolT initialValue)
: m_Compiler(compiler), m_Bool(new Bool(initialValue))
{
	m_Bool->Ref()->AddRef();
}

BoolEvent::~BoolEvent()
{
	m_Bool->Ref()->Release();
}

ICompiled * BoolEvent::Compile (ICompileNode * node)
{
	ICompiled * compiled = 0;

	ParseArgs * pa = new ParseArgs(m_Compiler.get(), node);
	pa->Ref()->AddRef();

	ICompiled::Type type = pa->ParseNextArgTE();

	switch(type)
	{
	case ICompiled::T_Eof:
		{
			compiled = new Compiled(this);
		}
		break;
	case ICompiled::T_Bool:
		if(pa->ParseEof())
		{
			compiled = new Compiled(new BoolEventSet(this, pa->GetArg(0)->GetBool()));
		}
		break;
	}

	pa->Ref()->Release();

	return compiled ? compiled : new Compiled(new Error());
}

BoolT BoolEvent::EvalBool (void) {
	return m_Bool->EvalBool();
}


::Afx::IRef * BoolEvent::Ref (void)
{
	return dynamic_cast<::Afx::IRef *>(this);
}

void BoolEvent::SetEvent(IBool * value)
{
	m_Bool->Ref()->Release();

	m_Bool = value;

	value->Ref()->AddRef();
}


// IntEvent ///////////////////////////////////////////////////////////////////


class IntEventSet : public Ref,
	public IVoid
{
public:
	IntEventSet(IntEvent * boolEvent, IInt * boolFn)
	: m_IntEvent(boolEvent), m_Int(boolFn)
	{
		boolFn->Ref()->AddRef();
		boolEvent->Ref()->AddRef();
	}

	virtual VoidT EvalVoid (void) {
		m_IntEvent->SetEvent(m_Int);
	}

	::Afx::IRef * Ref (void)
	{
		return dynamic_cast<::Afx::IRef *>(this);
	}

protected:
	virtual ~IntEventSet()
	{
		m_Int->Ref()->Release();
		m_IntEvent->Ref()->Release();
	}

private:
	IInt * m_Int;
	IntEvent * m_IntEvent;
};


IntEvent::IntEvent(ICompiler * compiler, IntT initialValue)
: m_Compiler(compiler), m_Int(new Int(initialValue))
{
	m_Int->Ref()->AddRef();
}

IntEvent::~IntEvent()
{
	m_Int->Ref()->Release();
}

ICompiled * IntEvent::Compile (ICompileNode * node)
{
	ICompiled * compiled = 0;

	ParseArgs * pa = new ParseArgs(m_Compiler.get(), node);
	pa->Ref()->AddRef();

	ICompiled::Type type = pa->ParseNextArgTE();

	switch(type)
	{
	case ICompiled::T_Eof:
		{
			compiled = new Compiled(this);
		}
		break;
	case ICompiled::T_Int:
		if(pa->ParseEof())
		{
			compiled = new Compiled(new IntEventSet(this, pa->GetArg(0)->GetInt()));
		}
		break;
	}

	pa->Ref()->Release();

	return compiled ? compiled : new Compiled(new Error());
}

IntT IntEvent::EvalInt (void) {
	return m_Int->EvalInt();
}


::Afx::IRef * IntEvent::Ref (void)
{
	return dynamic_cast<::Afx::IRef *>(this);
}

void IntEvent::SetEvent(IInt * value)
{
	m_Int->Ref()->Release();

	m_Int = value;

	value->Ref()->AddRef();
}


// VoidEvent ///////////////////////////////////////////////////////////////////


class VoidEventSet : public Ref,
	public IVoid
{
public:
	VoidEventSet(VoidEvent * voidEvent, IVoid * voidFn)
	: m_VoidEvent(voidEvent), m_Void(voidFn)
	{
		voidFn->Ref()->AddRef();
		voidEvent->Ref()->AddRef();
	}

	virtual VoidT EvalVoid (void) {
		m_VoidEvent->SetEvent(m_Void);
	}

	::Afx::IRef * Ref (void)
	{
		return dynamic_cast<::Afx::IRef *>(this);
	}

protected:
	virtual ~VoidEventSet()
	{
		m_Void->Ref()->Release();
		m_VoidEvent->Ref()->Release();
	}

private:
	IVoid * m_Void;
	VoidEvent * m_VoidEvent;

};


VoidEvent::VoidEvent(ICompiler * compiler)
: m_Compiler(compiler), m_Void(new Void())
{
	m_Void->Ref()->AddRef();
}

VoidEvent::~VoidEvent()
{
	m_Void->Ref()->Release();
}

ICompiled * VoidEvent::Compile (ICompileNode * node)
{
	ICompiled * compiled = 0;

	ParseArgs * pa = new ParseArgs(m_Compiler.get(), node);
	pa->Ref()->AddRef();

	ICompiled::Type type = pa->ParseNextArgTE();

	switch(type)
	{
	case ICompiled::T_Eof:
		{
			compiled = new Compiled(this);
		}
		break;
	case ICompiled::T_Void:
		if(pa->ParseEof())
		{
			compiled = new Compiled(new VoidEventSet(this, pa->GetArg(0)->GetVoid()));
		}
		break;
	}

	pa->Ref()->Release();

	return compiled ? compiled : new Compiled(new Error());
}

VoidT VoidEvent::EvalVoid (void) {
	m_Void->EvalVoid();
}


::Afx::IRef * VoidEvent::Ref (void)
{
	return dynamic_cast<::Afx::IRef *>(this);
}

void VoidEvent::SetEvent(IVoid * value)
{
	if(!value) value = new Void();

	m_Void->Ref()->Release();

	m_Void = value;

	value->Ref()->AddRef();
}

