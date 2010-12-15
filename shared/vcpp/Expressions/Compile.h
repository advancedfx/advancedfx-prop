#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-12-15 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


#include "Types.h"
#include "Cursor.h"

#include <list>

using namespace std;

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


/// <summary>Plain-text compiler</summary>
struct __declspec(novtable) ICompiler abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual ICompiled * Compile (Cursor * cursor) abstract = 0;
};

class CompilerRef : public RefIPtr<ICompiler>
{
public:
	CompilerRef(ICompiler * val) : RefIPtr(val) {}
};

struct __declspec(novtable) IArgumentCompiler abstract
{
	virtual IRef * Ref (void) abstract = 0;

	virtual ICompiled * CompileArgument (void) abstract = 0;
};


class ArgumentCompiler : public Ref,
	public IArgumentCompiler
{
public:
	ArgumentCompiler(ICompiler * compiler, Cursor * cursor);

	virtual ICompiled * CompileArgument (void);

	virtual IRef * Ref (void);

private:
	RefIPtr<ICompiler> m_Compiler;
	RefPtr<Cursor> m_Cursor;
};


/// <summary>
/// Compiles boolValue.
/// </summary>
class BoolCompiler : public Ref,
	public ICompiler
{
public:
	virtual ICompiled * Compile (Cursor * cursor);

	virtual ::Afx::IRef * Ref (void);
};


/// <summary>
/// Compiles intValue.
/// </summary>
class IntCompiler : public Ref,
	public ICompiler
{
public:
	virtual ICompiled * Compile (Cursor * cursor);

	virtual ::Afx::IRef * Ref (void);
};


/// <summary>
/// Compiles floatValue.
/// </summary>
class FloatCompiler : public Ref,
	public ICompiler
{
public:
	virtual ICompiled * Compile (Cursor * cursor);

	virtual ::Afx::IRef * Ref (void);
};


class StaticFunctionCompiler : public Ref,
	public ICompiler
{
public:
	typedef ICompiled * (* StaticCompileFunction)(IArgumentCompiler * compiler);

	StaticFunctionCompiler(ICompiler * compiler, StaticCompileFunction compileFunction);

	virtual ICompiled * Compile (Cursor * cursor);

	virtual IRef * Ref (void);

protected:

private:
	StaticCompileFunction m_CompileFunction;
	RefIPtr<ICompiler> m_Compiler;
};


/// <summary>
/// Compiles stringText.
/// </summary>
class StringTextCompiler : public Ref,
	public ICompiler
{
public:
	virtual ICompiled * Compile (Cursor * cursor);

	virtual IRef * Ref (void);

private:
	static bool ReadString(Cursor * cursor, int & outLength, char * & outData);
};


} } // namespace Afx { namespace Expr {
