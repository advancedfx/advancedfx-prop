#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2011-01-05 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com

#include "ParseArgs.h"

using namespace Afx;
using namespace Afx::Expressions;


// ParseArgs ///////////////////////////////////////////////////////////////////


ParseArgs::ParseArgs(ICompiler * argCompiler, ICompileNode * node)
: m_ArgCompiler(argCompiler), m_Node(node), m_SkipNull(true)
{
	argCompiler->Ref()->AddRef();
	if(node) node->Ref()->AddRef();
}

ParseArgs::~ParseArgs()
{
	for(VectorT::iterator it = m_Compileds.begin(); it != m_Compileds.end(); it++)
	{
		(*it)->Ref()->Release();
	}

	if(m_Node) m_Node->Ref()->Release();
	m_ArgCompiler->Ref()->Release();
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


ICompiled * ParseArgs::ParseNextArg_Internal (void)
{
	ICompiled * compiled;
	bool bSkip;
		
	do
	{
		if(m_Node)
		{
			compiled = m_ArgCompiler->Compile(m_Node);

			bSkip = m_SkipNull && ICompiled::T_Null == compiled->GetType();

			if(bSkip)
			{
				Ref::TouchRef(compiled->Ref());
			}

			// get next Node:
			ICompileNode * nextNode = m_Node->Next();
			if(nextNode) nextNode->Ref()->AddRef();
			m_Node->Ref()->Release();
			m_Node = nextNode;
		}
		else
		{
			compiled = new Compiled(new Eof());
			bSkip = false;
		}
	} while(bSkip);

	return compiled;
}

