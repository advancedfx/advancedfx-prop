#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-12-15 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


#include "../Ref.h"


namespace Afx { namespace Expressions {


/// <summary>Eof function.</summary>
struct __declspec(novtable) IEof abstract
{
	virtual IRef * Ref (void) abstract = 0;
};

/// <summary>IEof standard implementation.</summary>
class Eof : public Ref,
	public IEof
{
public:
	virtual IRef * Ref (void);
};


/// <summary>Error function.</summary>
struct __declspec(novtable) IError abstract
{
	virtual IRef * Ref (void) abstract = 0;
};

class ErrorRef : public RefIPtr<IError>
{
public:
	ErrorRef(IError * val) : RefIPtr(val) {}
};


class Cursor;

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
	Error(ErrorCode errorCode, Cursor * cursor);

	virtual IRef * Ref (void);
};


/// <summary>Null function.</summary>
struct __declspec(novtable) INull abstract
{
	virtual IRef * Ref (void) abstract = 0;
};

class Null : public Ref,
	public INull
{
public:
	virtual IRef * Ref (void);
};


typedef void VoidT;

/// <summary>Void function.</summary>
struct __declspec(novtable) IVoid abstract
{
	virtual IRef * Ref (void) abstract = 0;

	virtual VoidT EvalVoid (void) abstract = 0;
};

class Void : public Ref,
	public IVoid
{
public:
	virtual VoidT EvalVoid (void);

	virtual IRef * Ref (void);
};

class VoidRef : public RefIPtr<IVoid>
{
public:
	VoidRef(IVoid * val) : RefIPtr(val) {}

	VoidT eval (void) const {
		m_Ref->EvalVoid();
	}
};


typedef bool BoolT;

/// <summary>Bool function.</summary>
struct __declspec(novtable) IBool abstract
{
	virtual IRef * Ref (void) abstract = 0;

	virtual BoolT EvalBool (void) abstract = 0;
};

class Bool : public Ref,
	public IBool
{
public:
	Bool(BoolT value);

	virtual BoolT EvalBool (void);

	virtual IRef * Ref (void);

private:
	BoolT m_Value;
};

class BoolRef : public RefIPtr<IBool>
{
public:
	BoolRef(IBool * val) : RefIPtr(val) {}

	BoolT eval (void) const {
		return m_Ref->EvalBool();
	}
};


typedef long IntT;

/// <summary>Int function.</summary>
struct __declspec(novtable) IInt abstract
{
	virtual IRef * Ref (void) abstract = 0;

	virtual IntT EvalInt (void) abstract = 0;
};

class Int : public Ref,
	public IInt
{
public:
	Int(IntT value);

	virtual IntT EvalInt (void);

	virtual IRef * Ref (void);

private:
	IntT m_Value;
};

class IntRef : public RefIPtr<IInt>
{
public:
	IntRef(IInt * val) : RefIPtr(val) {}

	IntT eval (void) const {
		return m_Ref->EvalInt();
	}
};


typedef double FloatT;

/// <summary>Float function.</summary>
struct __declspec(novtable) IFloat abstract
{
	virtual IRef * Ref (void) abstract = 0;

	virtual FloatT EvalFloat (void) abstract = 0;
};

class Float : public Ref,
	public IFloat
{
public:
	Float(FloatT value);

	virtual FloatT EvalFloat (void);

	virtual IRef * Ref (void);

private:
	FloatT m_Value;
};

class FloatRef : public RefIPtr<IFloat>
{
public:
	FloatRef(IFloat * val) : RefIPtr(val) {}

	FloatT eval (void) const {
		return m_Ref->EvalFloat();
	}
};


typedef char const * StringDataT;

/// <summary>A string value</summary>
struct __declspec(novtable) IStringValue abstract
{
	virtual IRef * Ref (void) abstract = 0;

	/// <remarks>
	/// GetLength is at least strlen of GetData, but may be longer i.e. in case of binary data
	/// containing \0 characters
	/// </remarks>
	virtual int GetLength (void) abstract = 0;

	/// <retruns>Binary data, that is guaranteed to be 0-terminated.</returns>
	virtual StringDataT GetData (void) abstract = 0;
};

class StringValue : public Ref,
	public IStringValue
{
public:
	/// <param name="zString">A zero terminated character string (C-style string).</param>
	static StringValue * CopyFrom(char const * cStr);

	/// <param name="length">number of chars in data array, minimum length must be 1</param>
	/// <param name="data">last char of data must be 0.</data>
	static StringValue * CopyFrom(int length, char const * data);

	/// <param name="length">number of chars in data array, minimum length must be 1</param>
	/// <param name="data">last char of data must be 0.</data>
	static StringValue * TakeOwnership(int length, char * data);

	virtual StringDataT GetData (void);

	virtual int GetLength (void);

	virtual IRef * Ref (void);
	
protected:
	virtual ~StringValue();

private:
	char * m_Data;
	int m_Length;

	StringValue(int length, char * data);
};

class StringValueRef : public RefIPtr<IStringValue>
{
public:
	StringValueRef(IStringValue * val) : RefIPtr(val) {}

	StringDataT getData (void) const {
		return m_Ref->GetData();
	}

	int getLength (void) const {
		return m_Ref->GetLength();
	}
};


/// <summary>String function.</summary>
struct __declspec(novtable) IString abstract
{
	virtual IRef * Ref (void) abstract = 0;

	/// <remarks>The result is a reference counted type and not a value type,
	///   so don't forget to apply reference counting!</remarks>
	virtual IStringValue * EvalString (void) abstract = 0;
};

class String : public Ref,
	public IString
{
public:
	String(IStringValue * value);

	virtual IStringValue * EvalString (void);

	virtual IRef * Ref (void);

protected:
	virtual ~String();

private:
	IStringValue * m_Value;
};

class StringRef : public RefIPtr<IString>
{
public:
	StringRef(IString * val) : RefIPtr(val) {}

	IStringValue * eval (void) const {
		return m_Ref->EvalString();
	}
};


} } // namespace Afx { namespace Expr {
