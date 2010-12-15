#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-12-15 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


#include "Types.h"
#include "Cursor.h"

#include <stdexcept>

using namespace std;

using namespace Afx;
using namespace Afx::Expressions;


// Bool ////////////////////////////////////////////////////////////////////////

Bool::Bool(BoolT value)
: m_Value(value)
{
}

BoolT Bool::EvalBool (void)
{
	return m_Value;
}

IRef * Bool::Ref (void)
{
	return this;
}


// Eof /////////////////////////////////////////////////////////////////////////

IRef * Eof::Ref (void)
{
	return dynamic_cast<IRef *>(this);
}


// Error ///////////////////////////////////////////////////////////////////////

Error::Error()
{
}

Error::Error(ErrorCode errorCode, Cursor * cursor)
{
	CursorRef curRef(cursor);
/*
	ostringstream errText("Error #");

	errText << (int)errorCode;

	errText << " at ";

	errText << (int)cur.GetPos();

	errText << " (...\"";

	for(int i=-16; i<0; i++)
	{
		char val = cur.Get(i);

		if(!Cursor::IsNull(val)) errText << val;
	}

	errText << "\" [> \"";

	if(!cur.IsNull()) errText << cur.Get();

	errText << "\" <] \"";

	for(int i=1; i<16; i++)
	{
		char val = cur.Get(i);

		if(!Cursor::IsNull(val)) errText << val;
	}


	errText << "\"...).";

	MessageBox(0, errText.str().c_str(), "Afx::Expression::Error(ErrorCode, Cursor &)", MB_OK|MB_ICONINFORMATION);
*/
}

IRef * Error::Ref (void) {
	return dynamic_cast<IRef *>(this);
}



// Float ////////////////////////////////////////////////////////////////////////

Float::Float(FloatT value)
: m_Value(value)
{
}

FloatT Float::EvalFloat (void)
{
	return m_Value;
}

IRef * Float::Ref (void)
{
	return this;
}



// Int ////////////////////////////////////////////////////////////////////////

Int::Int(IntT value)
: m_Value(value)
{
}

IntT Int::EvalInt (void)
{
	return m_Value;
}

IRef * Int::Ref (void)
{
	return this;
}


// Null ////////////////////////////////////////////////////////////////////////

IRef * Null::Ref (void)
{
	return this;
}


// String //////////////////////////////////////////////////////////////////////

String::String(IStringValue * value)
: m_Value(value)
{
	value->Ref()->AddRef();
}

String::~String()
{
	m_Value->Ref()->Release();
}

IStringValue * String::EvalString (void)
{
	return m_Value;
}

IRef * String::Ref (void)
{
	return this;
}


// StringValue /////////////////////////////////////////////////////////////////

StringValue * StringValue::CopyFrom(char const * cStr)
{
	return CopyFrom(1 +strlen(cStr), cStr);
}

StringValue * StringValue::CopyFrom(int length, char const * data)
{
	if(length < 1 || 0 != data[length]) throw new exception();

	char * ownData = new char[length];

	memcpy(ownData, data, length * sizeof(char));

	return new StringValue(length, ownData);
}

StringValue * StringValue::TakeOwnership(int length, char * data)
{
	if(length < 1 || 0 != data[length]) throw new exception();

	return new StringValue(length, data);
}
	
StringValue::StringValue(int length, char * data)
: m_Length(length), m_Data(data)
{
}
	
StringValue::~StringValue()
{
	delete m_Data;
}

StringDataT StringValue::GetData (void)
{
	return m_Data;
}

int StringValue::GetLength (void)
{
	return m_Length;
}

IRef * StringValue::Ref (void)
{
	return this;
}


// Void ////////////////////////////////////////////////////////////////////////

VoidT Void::EvalVoid (void)
{
}

IRef * Void::Ref (void)
{
	return this;
}
