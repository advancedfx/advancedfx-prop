#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2011-01-05 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


#include "Compile.h"


using namespace Afx;
using namespace Afx::Expressions;


// BoolCompiler ////////////////////////////////////////////////////////////////

ICompiled * BoolCompiler::Compile (IStringValue * value)
{
	StringValueRef valueRef(value);
	BoolT rValue;
	bool match = false;

	for(int i=0; i<2; i++)
	{
		char * text = 0 == i ? "false" : "true";
		match = true;

		char const * valText = valueRef.getData();
		int textLen = strlen(valText);

		for(int j=0; match && j<textLen; j++)
		{
			match = text[j] == valText[j];
		}

		if(match)
		{
			rValue = 0 == i ? false : true;
			break;
		}
	}

	return match
		? new Compiled(new Bool(rValue))
		: new Compiled(new Error())
	;
}

IRef * BoolCompiler::Ref (void)
{
	return this;
}


// Compiled ////////////////////////////////////////////////////////////////////

Compiled::Compiled(IBool * value)
{
	if(value) value->Ref()->AddRef();

	m_Type = ICompiled::T_Bool;
	m_Value.Bool = value;
}

Compiled::Compiled(IEof * value)
{
	if(value) value->Ref()->AddRef();

	m_Type = ICompiled::T_Eof;
	m_Value.Eof = value;
}

Compiled::Compiled(IError * value)
{
	if(value) value->Ref()->AddRef();

	m_Type = ICompiled::T_Error;
	m_Value.Error = value;
}

Compiled::Compiled(INull * value)
{
	if(value) value->Ref()->AddRef();

	m_Type = ICompiled::T_Null;
	m_Value.Null = value;
}


Compiled::Compiled(IFloat * value)
{
	if(value) value->Ref()->AddRef();

	m_Type = ICompiled::T_Float;
	m_Value.Float = value;
}

Compiled::Compiled(IInt * value)
{
	if(value) value->Ref()->AddRef();

	m_Type = ICompiled::T_Int;
	m_Value.Int = value;
}


Compiled::Compiled(IString * value)
{
	if(value) value->Ref()->AddRef();

	m_Type = ICompiled::T_String;
	m_Value.String = value;
}


Compiled::Compiled(IVoid * value)
{
	if(value) value->Ref()->AddRef();

	m_Type = ICompiled::T_Void;
	m_Value.Void = value;
}


Compiled::~Compiled()
{
	switch(m_Type)
	{
	case ICompiled::T_Eof: if(m_Value.Eof) m_Value.Eof->Ref()->Release(); break;
	case ICompiled::T_Error: if(m_Value.Error) m_Value.Error->Ref()->Release(); break;
	case ICompiled::T_Null: if(m_Value.Null) m_Value.Null->Ref()->Release(); break;
	case ICompiled::T_Void: if(m_Value.Void) m_Value.Void->Ref()->Release(); break;
	case ICompiled::T_Bool: if(m_Value.Bool) m_Value.Bool->Ref()->Release(); break;
	case ICompiled::T_Int: if(m_Value.Int) m_Value.Int->Ref()->Release(); break;
	case ICompiled::T_Float: if(m_Value.Float) m_Value.Float->Ref()->Release(); break;
	case ICompiled::T_String: if(m_Value.String) m_Value.String->Ref()->Release(); break;
	}
}

IBool * Compiled::GetBool() { return ICompiled::T_Bool == m_Type ? m_Value.Bool : 0; }

IEof * Compiled::GetEof() { return ICompiled::T_Eof == m_Type ? m_Value.Eof : 0; }

IError * Compiled::GetError() { return ICompiled::T_Error == m_Type ? m_Value.Error : 0; }

IFloat * Compiled::GetFloat() { return ICompiled::T_Float == m_Type ? m_Value.Float : 0; }

IInt * Compiled::GetInt() { return ICompiled::T_Int == m_Type ? m_Value.Int : 0; }

INull * Compiled::GetNull() { return ICompiled::T_Null == m_Type ? m_Value.Null : 0; }

IString * Compiled::GetString() { return ICompiled::T_String == m_Type ? m_Value.String : 0; }

enum ICompiled::Type Compiled::GetType() { return m_Type; }

IVoid * Compiled::GetVoid() { return ICompiled::T_Void == m_Type ? m_Value.Void : 0; }

::Afx::IRef * Compiled::Ref() { return dynamic_cast<::Afx::IRef *>(this); }



// FloatCompiler ////////////////////////////////////////////////////////////////

ICompiled * FloatCompiler::Compile (IStringValue * value)
{
	StringValueRef valueRef(value);
	bool match = false;

	char const * startPtr = valueRef.getData();
	int textLen = strlen(startPtr);
	char * endPtr;
	FloatT rValue = strtod(startPtr, &endPtr);
	int skipped = (endPtr -startPtr) / sizeof(char);

	match = 0 < skipped && textLen == skipped;

	return match
		? new Compiled(new Float(rValue))
		: new Compiled(new Error())
	;
}

IRef * FloatCompiler::Ref (void)
{
	return this;
}


// IntCompiler ////////////////////////////////////////////////////////////////

ICompiled * IntCompiler::Compile (IStringValue * value)
{
	StringValueRef valueRef(value);
	bool match = false;

	char const * startPtr = valueRef.getData();
	int textLen = strlen(startPtr);
	char * endPtr;
	IntT rValue = strtol(startPtr, &endPtr, 0);
	int skipped = (endPtr -startPtr) / sizeof(char);

	match = 0 < skipped && textLen == skipped;

	return match
		? new Compiled(new Int(rValue))
		: new Compiled(new Error())
	;
}

IRef * IntCompiler::Ref (void)
{
	return this;
}



// StringCompiler //////////////////////////////////////////////////////////////

ICompiled * StringCompiler::Compile (IStringValue * value)
{
	return new Compiled(new String(value));
}

/*
bool StringTextCompiler::ReadString(Cursor * cursor, int & outLength, char * & outData)
{
	CursorRef curRef(cursor);
	bool escaped;
	bool done;
	char val;

	outData = 0;

	CursorBackup backup(curRef.get()->Backup());

	do
	{
		escaped = false;
		outLength = 0;
		int brackets = 0;

		curRef.get()->Restore(backup);

		while((val = curRef.get()->Get()), !curRef.get()->IsNull(val) && (escaped || 0 < brackets || !curRef.get()->IsPaClose(val)))
		{
			curRef.get()->Add();

			if(!escaped && curRef.get()->IsEscape(val))
			{
				escaped = true;
			}
			else {
				escaped = false;

				if(curRef.get()->IsPaOpen(val)) brackets++;
				else if(curRef.get()->IsPaClose(val)) brackets--;

				if(outData) outData[outLength] = val;
				outLength++;
			}
		}

		if(!outData && !escaped)
		{
			// not done yet, but we know the outLength now:
			outData = new char[1+outLength];
			outLength = 0;
			done = false;
		}
		else if(outData) {
			// ok and done.
			outData[outLength] = 0;
			done = true;
		}
		else {
			// error and done.
			done = true;
		}
	} while(!done);

	return 0 != outData;
}
*/

IRef * StringCompiler::Ref (void)
{
	return this;
}
