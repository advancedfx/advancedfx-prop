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

		m_Bubbles.Info = Tools::StandardBubble();
		m_Bubbles.Info->Ref()->AddRef();

		m_Bubbles.Root = Tools::StandardBubble();
		m_Bubbles.Root->Ref()->AddRef();

		m_Bubbles.Root->Add("./", Tools::FnDoCompileable(m_Bubbles.Root->Compiler()));
		m_Bubbles.Root->Add("..", Tools::FnDoCompileable(m_Bubbles.Root->Compiler()));
		m_Bubbles.Root->Add(".info", Tools::FnDoCompileable(m_Bubbles.Info->Compiler()));

		m_Bubbles.Info->Add("./", Tools::FnDoCompileable(m_Bubbles.Root->Compiler()));
		m_Bubbles.Info->Add("..", Tools::FnDoCompileable(m_Bubbles.Root->Compiler()));
		m_Bubbles.Info->Add("CurrentGlMode", CurrentGlMode = new IntVariable(m_Bubbles.Info->Compiler(), IntVariable::CA_Getter, -1));
		m_Bubbles.Info->Add("CurrentStreamIndex", CurrentStreamIndex = new IntVariable(m_Bubbles.Info->Compiler(), IntVariable::CA_Getter, -1));
		m_Bubbles.Info->Add("GetCurrentEntityIndex", new FnGetCurrentEntityIndex(m_Bubbles.Info->Compiler()));
		m_Bubbles.Info->Add("IsFilming", IsFilming = new BoolVariable(m_Bubbles.Info->Compiler(), BoolVariable::CA_Getter, false));
		m_Bubbles.Info->Add("InRDrawEntitiesOnList", InRDrawEntitiesOnList = new BoolVariable(m_Bubbles.Info->Compiler(), BoolVariable::CA_Getter, false));
		m_Bubbles.Info->Add("InRDrawParticles", InRDrawParticles = new BoolVariable(m_Bubbles.Info->Compiler(), BoolVariable::CA_Getter, false));
		m_Bubbles.Info->Add("InRDrawViewModel", InRDrawViewModel = new BoolVariable(m_Bubbles.Info->Compiler(), BoolVariable::CA_Getter, false));
		m_Bubbles.Info->Add("InRRenderView", InRRenderView = new BoolVariable(m_Bubbles.Info->Compiler(), BoolVariable::CA_Getter, false));
	}

	~Xpress_t()
	{
		m_Bubbles.Info->Ref()->Release();
		m_Bubbles.Root->Ref()->Release();

		if(m_MatteEx) m_MatteEx->Ref()->Release();
	}

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

		virtual int Get (void);
	};

};

extern Xpress_t g_Xpress;
