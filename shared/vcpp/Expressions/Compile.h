#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-16 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


#include "Types.h"
#include "Cursor.h"


namespace Afx { namespace Expressions {


struct __declspec(novtable) ICompiled abstract
{
	enum Type
	{
		T_None,
		T_Eof,
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

	virtual IEof * GetEof (void) abstract = 0;

	virtual IError * GetError (void) abstract = 0;

	virtual INull * GetNull (void) abstract = 0;

	virtual IVoid * GetVoid (void) abstract = 0;

	virtual IBool * GetBool (void) abstract = 0;

	virtual IInt * GetInt (void) abstract = 0;

	virtual IFloat * GetFloat (void) abstract = 0;

	virtual IString * GetString (void) abstract = 0;
};


/// <summary>ICompiled standard implementation.</summary>
class Compiled : public Ref,
	public ICompiled
{
public:
	Compiled(IEof * value);
	Compiled(IError * value);
	Compiled(INull * value);
	Compiled(IVoid * value);
	Compiled(IBool * value);
	Compiled(IInt * value);
	Compiled(IFloat * value);
	Compiled(IString * value);

	virtual enum ICompiled::Type GetType();

	virtual IEof * GetEof();
	
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
		IEof * Eof;
		IError * Error;
		INull * Null;
		IVoid * Void;
		IBool * Bool;
		IInt * Int;
		IFloat * Float;
		IString * String;
	} m_Value;
};


struct __declspec(novtable) ICompiler abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual ICompiled * Compile_Function (Cursor & cursor) abstract = 0;
};


struct __declspec(novtable) ICompileArgs abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual ICompiled * CompileNextArg (ICompiler * compiler) abstract = 0;
};


/// <summary>ICompileArgs standard implementation</summary>
class CompileArgs : public Ref,
	public ICompileArgs
{
public:
	CompileArgs(Cursor & cursor, bool inParenthesis);

	virtual ICompiled * CompileNextArg (ICompiler * compiler);

	virtual ::Afx::IRef * Ref (void);

private:
	Cursor & m_Cursor;
	bool m_Eof;
	bool m_MoreArgs;
};


struct __declspec(novtable) ICompileable abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual ICompiled * Compile (ICompileArgs * args) abstract = 0;
};


/// <summary>ICompileable standard implementation base class.</summary>
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



} } // namespace Afx { namespace Expr {
