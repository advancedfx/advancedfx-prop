#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-16 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


// Standard types:
//
// Eof
// Error
// Null
// Void
// Bool
// Int
// Float
// String


#include "../Ref.h"
#include "Cursor.h"


namespace Afx { namespace Expressions {


typedef bool BoolT;
typedef double FloatT;
typedef long IntT;
typedef char const * StringDataT;
typedef void VoidT;


struct __declspec(novtable) IEof abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;
};


/// <summary>IEof standard implementation.</summary>
class Eof : public Ref,
	public IEof
{
public:
	virtual ::Afx::IRef * Ref (void);
};


struct __declspec(novtable) IError abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;
};


/// <summary>IError standard implementation.</summary>
class Error : public Ref,
	public IError
{
public:	
	enum ErrorCode {
		EC_None,
		EC_Error,
		EC_ParseError
	};

	Error();
	Error(ErrorCode errorCode, Cursor & cur);

	virtual ::Afx::IRef * Ref (void);
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


class VoidRef
{
public:
	VoidRef(IVoid * ref)
	: m_Ref(ref)
	{
		ref->Ref()->AddRef();
	}

	VoidT eval (void) const {
		m_Ref->EvalVoid();
	}

	IVoid * get (void) const {
		return m_Ref;
	}

	~VoidRef()
	{
		m_Ref->Ref()->Release();
	}

private:
	IVoid * m_Ref;
};


/// <summary>Function that evaluates to BoolT</summary>
struct __declspec(novtable) IBool abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual BoolT EvalBool (void) abstract = 0;
};

class BoolRef
{
public:
	BoolRef(IBool * ref)
	: m_Ref(ref)
	{
		ref->Ref()->AddRef();
	}

	BoolT eval (void) const {
		return m_Ref->EvalBool();
	}

	IBool * get (void) const {
		return m_Ref;
	}

	~BoolRef()
	{
		m_Ref->Ref()->Release();
	}

private:
	IBool * m_Ref;
};


/// <summary>Function that evaluates to IntT</summary>
struct __declspec(novtable) IInt abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual IntT EvalInt (void) abstract = 0;
};

class IntRef
{
public:
	IntRef(IInt * ref)
	: m_Ref(ref)
	{
		ref->Ref()->AddRef();
	}

	IntT eval (void) const {
		return m_Ref->EvalInt();
	}

	IInt * get (void) const {
		return m_Ref;
	}

	~IntRef()
	{
		m_Ref->Ref()->Release();
	}

private:
	IInt * m_Ref;
};



/// <summary>Function that evaluates to FloatT</summary>
struct __declspec(novtable) IFloat abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual FloatT EvalFloat (void) abstract = 0;
};

class FloatRef
{
public:
	FloatRef(IFloat * ref)
	: m_Ref(ref)
	{
		ref->Ref()->AddRef();
	}

	FloatT eval (void) const {
		return m_Ref->EvalFloat();
	}

	IFloat * get (void) const {
		return m_Ref;
	}

	~FloatRef()
	{
		m_Ref->Ref()->Release();
	}

private:
	IFloat * m_Ref;
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

class StringValueRef
{
public:
	StringValueRef(IStringValue * ref)
	: m_Ref(ref)
	{
		ref->Ref()->AddRef();
	}

	IStringValue * get (void) const {
		return m_Ref;
	}

	StringDataT getData (void) const {
		return m_Ref->GetData();
	}

	int getLength (void) const {
		return m_Ref->GetLength();
	}

	~StringValueRef()
	{
		m_Ref->Ref()->Release();
	}

private:
	IStringValue * m_Ref;
};


/// <summary>Function that evaluates to a IStringValue</summary>
struct __declspec(novtable) IString abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	/// <remarks>The result is a reference type and not a value type,
	///   so don't forget to apply reference counting!</remarks>
	virtual IStringValue * EvalString (void) abstract = 0;
};

class StringRef
{
public:
	StringRef(IString * ref)
	: m_Ref(ref)
	{
		ref->Ref()->AddRef();
	}

	IStringValue * eval (void) const {
		return m_Ref->EvalString();
	}

	IString * get (void) const {
		return m_Ref;
	}

	~StringRef()
	{
		m_Ref->Ref()->Release();
	}

private:
	IString * m_Ref;
};


} } // namespace Afx { namespace Expr {
