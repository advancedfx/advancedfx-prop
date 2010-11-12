#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-10 dominik.matrixstorm.com
//
// First changes
// 2010-11-10 dominik.matrixstorm.com


#include <shared/vcpp/Expressions.h>


using namespace Afx;
using namespace Afx::Expressions;


class Xpress_t
{
public:
	IntVariable * CurrentGlMode;
	IntVariable * CurrentStreamIndex;
	BoolVariable * IsFilming;
	BoolVariable * InRDrawEntitiesOnList;
	BoolVariable * InRDrawParticles;
	BoolVariable * InRDrawViewModel;
	BoolVariable * InRRenderView;

	Xpress_t();

	~Xpress_t();

	ICompiled * CompileEx (char const * code)
	{
		return m_Bubbles.Root->Compile(code);
	}

	bool CompileMatteEx (char const * code)
	{
		ICompiled * compiled = m_Bubbles.Root->Compile(code);			
		compiled->Ref()->AddRef();

		SetMatteEx(compiled->GetInt());

		compiled->Ref()->Release();

		return HasMatteEx();
	}

	int EvalMatteEx (void)
	{
		return 0 != m_MatteEx ? m_MatteEx->EvalInt() : 0;
	}

	bool HasMatteEx (void)
	{
		return 0 != m_MatteEx;
	}

	void SetMatteEx(IInt * value)
	{
		if(value) value->Ref()->AddRef();

		if(m_MatteEx) m_MatteEx->Ref()->Release();
		m_MatteEx = value;
	}

private:
	struct {
		IBubble * Info;
		IBubble * Root;
	} m_Bubbles;
	IInt * m_MatteEx;

	class FnGetCurrentEntityIndex : public IntGetter
	{
	public:
		FnGetCurrentEntityIndex(ICompiler * compiler) : IntGetter(compiler) {}

		virtual IntT Get (void);
	};

};

extern Xpress_t g_Xpress;
