#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-10-24 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com

#include "expressions.h"


using namespace Afx;
using namespace Afx::Expr;


class FnAnd : Ref,
	public IBoolFn
{
public:
	FnAnd(IBoolFn * fn1, IBoolFn * fn2) {
		fn1->Ref()->AddRef();
		fn2->Ref()->AddRef();

		m_fn1 = fn1;
		m_fn2 = fn2;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) { return m_fn1->EvalBool() && m_fn2->EvalBool(); }

protected:
	virtual ~FnAnd() {
		m_fn1->Ref()->Release();
		m_fn2->Ref()->Release();
	}

private:
	IBoolFn * m_fn1;
	IBoolFn * m_fn2;
};


class FnConstBool : Ref,
	public IBoolFn
{
public:
	FnConstBool(bool val) {
		m_val = val;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) { return m_val; }

protected:
	virtual ~FnConstBool() {
	}

private:
	bool m_val;
};


class FnConstInt : Ref,
	public IIntFn
{
public:
	FnConstInt(int val) {
		m_val = val;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual int EvalInt (void) { return m_val; }

protected:
	virtual ~FnConstInt() {
	}

private:
	int m_val;
};


class FnEqualBool : Ref,
	public IBoolFn
{
public:
	FnEqualBool(IBoolFn * fn1, IBoolFn * fn2) {
		fn1->Ref()->AddRef();
		fn2->Ref()->AddRef();

		m_fn1 = fn1;
		m_fn2 = fn2;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) { return m_fn1->EvalBool() == m_fn2->EvalBool(); }

protected:
	virtual ~FnEqualBool() {
		m_fn1->Ref()->Release();
		m_fn2->Ref()->Release();
	}

private:
	IBoolFn * m_fn1;
	IBoolFn * m_fn2;
};


class FnEqualInt : Ref,
	public IBoolFn
{
public:
	FnEqualInt(IIntFn * fn1, IIntFn * fn2) {
		fn1->Ref()->AddRef();
		fn2->Ref()->AddRef();

		m_fn1 = fn1;
		m_fn2 = fn2;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) { return m_fn1->EvalInt() == m_fn2->EvalInt(); }

protected:
	virtual ~FnEqualInt() {
		m_fn1->Ref()->Release();
		m_fn2->Ref()->Release();
	}

private:
	IIntFn * m_fn1;
	IIntFn * m_fn2;
};


class FnGreater : Ref,
	public IBoolFn
{
public:
	FnGreater(IIntFn * fn1, IIntFn * fn2) {
		fn1->Ref()->AddRef();
		fn2->Ref()->AddRef();

		m_fn1 = fn1;
		m_fn2 = fn2;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) { return m_fn1->EvalInt() > m_fn2->EvalInt(); }

protected:
	virtual ~FnGreater() {
		m_fn1->Ref()->Release();
		m_fn2->Ref()->Release();
	}

private:
	IIntFn * m_fn1;
	IIntFn * m_fn2;
};


class FnGreaterOrEqual : Ref,
	public IBoolFn
{
public:
	FnGreaterOrEqual(IIntFn * fn1, IIntFn * fn2) {
		fn1->Ref()->AddRef();
		fn2->Ref()->AddRef();

		m_fn1 = fn1;
		m_fn2 = fn2;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) { return m_fn1->EvalInt() >= m_fn2->EvalInt(); }

protected:
	virtual ~FnGreaterOrEqual() {
		m_fn1->Ref()->Release();
		m_fn2->Ref()->Release();
	}

private:
	IIntFn * m_fn1;
	IIntFn * m_fn2;
};


class FnInConstSet : Ref,
	public IBoolFn
{
public:
	FnInConstSet(IIntFn * fn1, int valCount, int const * vals) {
		fn1->Ref()->AddRef();

		m_fn1 = fn1;
		m_valCount = valCount;
		m_vals = new int[valCount];

		for(int i=0; i<valCount; m_vals[i++] = *(vals++));
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) {
		int val = m_fn1->EvalInt();

		for(int i=0; i<m_valCount; i++)
		{
			if(val == m_vals[i]) return true;
		}

		return false;
	}

protected:
	virtual ~FnInConstSet() {
		m_fn1->Ref()->Release();

		delete m_vals;
	}

private:
	IIntFn * m_fn1;
	int m_valCount;
	int * m_vals;
};


class FnLess : Ref,
	public IBoolFn
{
public:
	FnLess(IIntFn * fn1, IIntFn * fn2) {
		fn1->Ref()->AddRef();
		fn2->Ref()->AddRef();

		m_fn1 = fn1;
		m_fn2 = fn2;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) { return m_fn1->EvalInt() < m_fn2->EvalInt(); }

protected:
	virtual ~FnLess() {
		m_fn1->Ref()->Release();
		m_fn2->Ref()->Release();
	}

private:
	IIntFn * m_fn1;
	IIntFn * m_fn2;
};


class FnLessOrEqual : Ref,
	public IBoolFn
{
public:
	FnLessOrEqual(IIntFn * fn1, IIntFn * fn2) {
		fn1->Ref()->AddRef();
		fn2->Ref()->AddRef();

		m_fn1 = fn1;
		m_fn2 = fn2;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) { return m_fn1->EvalInt() <= m_fn2->EvalInt(); }

protected:
	virtual ~FnLessOrEqual() {
		m_fn1->Ref()->Release();
		m_fn2->Ref()->Release();
	}

private:
	IIntFn * m_fn1;
	IIntFn * m_fn2;
};


class FnNot : Ref,
	public IBoolFn
{
public:
	FnNot(IBoolFn * fn1) {
		fn1->Ref()->AddRef();

		m_fn1 = fn1;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) { return !m_fn1->EvalBool(); }

protected:
	virtual ~FnNot() {
		m_fn1->Ref()->Release();
	}

private:
	IBoolFn * m_fn1;
};


class FnOr : Ref,
	public IBoolFn
{
public:
	FnOr(IBoolFn * fn1, IBoolFn * fn2) {
		fn1->Ref()->AddRef();
		fn2->Ref()->AddRef();

		m_fn1 = fn1;
		m_fn2 = fn2;
	}

	virtual ::Afx::IRef * Ref() { return dynamic_cast<::Afx::IRef *>(this); }

	virtual bool EvalBool (void) { return m_fn1->EvalBool() || m_fn2->EvalBool(); }

protected:
	virtual ~FnOr() {
		m_fn1->Ref()->Release();
		m_fn2->Ref()->Release();
	}

private:
	IBoolFn * m_fn1;
	IBoolFn * m_fn2;
};


// FnFactory ///////////////////////////////////////////////////////////////////

IBoolFn * FnFactory::And(IBoolFn * fn1, IBoolFn * fn2)
{
	return new FnAnd(fn1, fn2);
}

IBoolFn * FnFactory::Const(bool val)
{
	return new FnConstBool(val);
}

IIntFn  * FnFactory::Const(int val)
{
	return new FnConstInt(val);
}

IBoolFn * FnFactory::Equal (IBoolFn * fn1, IBoolFn * fn2)
{
	return new FnEqualBool(fn1 ,fn2);
}

IBoolFn * FnFactory::Equal (IIntFn * fn1, IIntFn * fn2)
{
	return new FnEqualInt(fn1, fn2);
}

IBoolFn * FnFactory::Greater (IIntFn * fn1, IIntFn * fn2)
{
	return new FnGreater(fn1, fn2);
}

IBoolFn * FnFactory::GreaterOrEqual (IIntFn * fn1, IIntFn * fn2)
{
	return new FnGreaterOrEqual(fn1, fn2);
}

IBoolFn * FnFactory::InConstSet (IIntFn * fn1, int valCount, int const * vals)
{
	return new FnInConstSet(fn1, valCount, vals);
}

IBoolFn * FnFactory::Less (IIntFn * fn1, IIntFn * fn2)
{
	return new FnLess(fn1, fn2);
}

IBoolFn * FnFactory::Not(IBoolFn * fn1)
{
	return new FnNot(fn1);
}

IBoolFn * FnFactory::Or(IBoolFn * fn1, IBoolFn * fn2)
{
	return new FnOr(fn1, fn2);
}
