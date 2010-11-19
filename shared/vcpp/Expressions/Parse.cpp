#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-17 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com

#include "Parse.h"

using namespace Afx;
using namespace Afx::Expressions;


// ParseArgs ///////////////////////////////////////////////////////////////////


ParseArgs::ParseArgs(ICompiler * compiler, ICompileArgs * args)
: m_Args(args), m_Compiler(compiler), m_SkipNull(true)
{
	args->Ref()->AddRef();
	compiler->Ref()->AddRef();
}


ICompiled * ParseArgs::GetArg(int index) {
	return m_Compileds[index];
}


int ParseArgs::GetCount (void) {
	return m_Compileds.size();
}


bool ParseArgs::GetSkipNull (void) const {
	return m_SkipNull;
}


bool ParseArgs::ParseEof (void)
{
	ICompiled * nextArg = ParseNextArg_Internal();

	nextArg->Ref()->AddRef();

	bool isEof = 0 != nextArg->GetEof();

	nextArg->Ref()->Release();

	return isEof;
}


ICompiled * ParseArgs::ParseNextArg(void)
{
	ICompiled * compiled = ParseNextArg_Internal();

	compiled->Ref()->AddRef();
	m_Compileds.push_back(compiled);

	return compiled;
}


ICompiled::Type ParseArgs::ParseNextArgT (void)
{
	return ParseNextArg()->GetType();
}


ICompiled::Type ParseArgs::ParseNextArgTE (void)
{
	ICompiled * compiled = ParseNextArg_Internal();

	compiled->Ref()->AddRef();

	ICompiled::Type type = compiled->GetType();

	if(ICompiled::T_Eof == type)
	{
		compiled->Ref()->Release();
	}
	else
	{
		m_Compileds.push_back(compiled);
	}

	return type;
}


bool ParseArgs::ParseNextArgTC (ICompiled::Type type)
{
	return type == ParseNextArgT();
}


bool ParseArgs::ParseNextArgTCE (ICompiled::Type type)
{
	return type == ParseNextArgTE();
}


bool ParseArgs::ParseNextArgTCEX(ICompiled::Type type, bool & outMatchedOrEof)
{
	ICompiled::Type curType = ParseNextArgTE();

	outMatchedOrEof = type == curType || ICompiled::T_Eof == curType;

	return ICompiled::T_Eof != curType;
}


::Afx::IRef * ParseArgs::Ref (void) {
	return dynamic_cast<::Afx::IRef *>(this);
}

void ParseArgs::SetArg(int index, ICompiled * value) {
	m_Compileds[index] = value;
}

void ParseArgs::SetSkipNull (bool value) {
	m_SkipNull = value;
}


ParseArgs::~ParseArgs()
{
	for(VectorT::iterator it = m_Compileds.begin(); it != m_Compileds.end(); it++)
	{
		(*it)->Ref()->Release();
	}

	m_Args->Ref()->Release();
	m_Compiler->Ref()->Release();
}


ICompiled * ParseArgs::ParseNextArg_Internal (void)
{
	ICompiled * compiled;
	bool bSkip;
		
	do {
		compiled = m_Args->CompileNextArg(m_Compiler);

		bSkip = m_SkipNull && 0 != compiled->GetNull();

		if(bSkip)
		{
			Ref::TouchRef(compiled->Ref());
		}

	} while(bSkip);

	return compiled;
}
