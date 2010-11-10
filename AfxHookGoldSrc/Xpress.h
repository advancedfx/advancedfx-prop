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

	Xpress_t()
	{	
		m_MatteEx = 0;

		m_Bubble = Tools::StandardBubble();
		m_Bubble->Ref()->AddRef();

		m_Bubble->Add("CurrentGlMode", CurrentGlMode = new IntVariable(-1));
		m_Bubble->Add("CurrentStreamIndex", CurrentStreamIndex = new IntVariable(-1));
		m_Bubble->Add("GetCurrentEntityIndex", new FnGetCurrentEntityIndex());
		m_Bubble->Add("InRDrawEntitiesOnList", InRDrawEntitiesOnList = new BoolVariable(false));
		m_Bubble->Add("InRDrawParticles", InRDrawParticles = new BoolVariable(false));
		m_Bubble->Add("InRDrawViewModel", InRDrawViewModel = new BoolVariable(false));
		m_Bubble->Add("InRRenderView", InRRenderView = new BoolVariable(false));
		m_Bubble->Add("IsFilming", IsFilming = new BoolVariable(false));
	}

	~Xpress_t()
	{
		m_Bubble->Ref()->Release();

		if(m_MatteEx) m_MatteEx->Ref()->Release();
	}

	int EvalMatteEx (void)
	{
		return 0 != m_MatteEx ? m_MatteEx->EvalInt() : 0;
	}

	bool CompileMatteEx (char const * code)
	{
		if(m_MatteEx) m_MatteEx->Ref()->Release();

		ICompiled * compiled = m_Bubble->Compile(code);			
		compiled->Ref()->AddRef();			
			
		m_MatteEx = compiled->GetInt();
		if(m_MatteEx) m_MatteEx->Ref()->AddRef();

		compiled->Ref()->Release();

		return HasMatteEx();
	}

	bool HasMatteEx (void)
	{
		return 0 != m_MatteEx;
	}

private:
	IBubble * m_Bubble;
	IInt * m_MatteEx;

	class FnGetCurrentEntityIndex : public IntFunction
	{
	public:
		virtual int EvalInt (void);
	};

};

extern Xpress_t g_Xpress;
