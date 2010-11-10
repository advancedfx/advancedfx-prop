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

		m_Bubble->Add("CurrentGlMode", CurrentGlMode = new IntVariable(IntVariable::CA_Getter, -1));
		m_Bubble->Add("CurrentStreamIndex", CurrentStreamIndex = new IntVariable(IntVariable::CA_Getter, -1));
		m_Bubble->Add("GetCurrentEntityIndex", new FnGetCurrentEntityIndex());
		m_Bubble->Add("IsFilming", IsFilming = new BoolVariable(BoolVariable::CA_Getter, false));
		m_Bubble->Add("InRDrawEntitiesOnList", InRDrawEntitiesOnList = new BoolVariable(BoolVariable::CA_Getter, false));
		m_Bubble->Add("InRDrawParticles", InRDrawParticles = new BoolVariable(BoolVariable::CA_Getter, false));
		m_Bubble->Add("InRDrawViewModel", InRDrawViewModel = new BoolVariable(BoolVariable::CA_Getter, false));
		m_Bubble->Add("InRRenderView", InRRenderView = new BoolVariable(BoolVariable::CA_Getter, false));
	}

	~Xpress_t()
	{
		m_Bubble->Ref()->Release();

		if(m_MatteEx) m_MatteEx->Ref()->Release();
	}

	bool CompileMatteEx (char const * code)
	{
		ICompiled * compiled = m_Bubble->Compile(code);			
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
	IBubble * m_Bubble;
	IInt * m_MatteEx;

	class FnGetCurrentEntityIndex : public IntGetter
	{
	public:
		virtual int Get (void);
	};

};

extern Xpress_t g_Xpress;
