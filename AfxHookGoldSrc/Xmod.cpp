#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-17 dominik.matrixstorm.com
//
// First changes
// 2010-11-17 dominik.matrixstorm.com

#include "Xmod.h"

#include <shared/vcpp/Ref.h>

#include <list>
#include <vector>

using namespace GlPrimMod;


struct __declspec(novtable) IXGlPrimMod abstract
{
	virtual IRef * Ref (void) abstract = 0;

	virtual void OnGlBegin(GLenum mode) abstract = 0;

	virtual void OnGlEnd() abstract = 0;
};

class XColor : public FunctionHost,
	public IXGlPrimMod
{
public:
	XColor(IFloat * red, IFloat * green, IFloat * blue, IFloat * alpha)
	: m_Red(red), m_Green(green), m_Blue(blue), m_Alpha(alpha)
	{
	}

	virtual void OnGlBegin(GLenum mode) {
		m_Color.SetRgba(
			(GLfloat)m_Red.eval(),
			(GLfloat)m_Green.eval(),
			(GLfloat)m_Blue.eval(),
			(GLfloat)m_Green.eval()
		);

		m_Color.OnGlBegin(mode);
	}

	virtual void OnGlEnd() {
		m_Color.OnGlEnd();
	}

	virtual IRef * Ref (void) {
		return this;
	}

private:
	Color m_Color;
	FloatRef m_Red;
	FloatRef m_Green;
	FloatRef m_Blue;
	FloatRef m_Alpha;
};


class XColorMask : public FunctionHost,
	public IXGlPrimMod
{
public:
	XColorMask(IInt * red, IInt * green, IInt * blue, IInt * alpha)
	: m_Red(red), m_Green(green), m_Blue(blue), m_Alpha(alpha)
	{
	}

	virtual void OnGlBegin(GLenum mode) {
		m_ColorMask.SetRgba(
			FromInt(m_Red.eval()),
			FromInt(m_Green.eval()),
			FromInt(m_Blue.eval()),
			FromInt(m_Alpha.eval())
		);

		m_ColorMask.OnGlBegin(mode);
	}

	virtual void OnGlEnd() {
		m_ColorMask.OnGlEnd();
	}

	virtual IRef * Ref (void) {
		return this;
	}

private:
	ColorMask m_ColorMask;
	IntRef m_Red;
	IntRef m_Green;
	IntRef m_Blue;
	IntRef m_Alpha;
};


class XDepthMask : public FunctionHost,
	public IXGlPrimMod
{
public:
	XDepthMask(IInt * depth)
	: m_Depth(depth)
	{
	}

	virtual void OnGlBegin(GLenum mode) {
		m_DepthMask.SetDepth(
			FromInt(m_Depth.eval())
		);

		m_DepthMask.OnGlBegin(mode);
	}

	virtual void OnGlEnd() {
		m_DepthMask.OnGlEnd();
	}

	virtual IRef * Ref (void) {
		return this;
	}

private:
	DepthMask m_DepthMask;
	IntRef m_Depth;
};


class XMask : public FunctionHost,
	public IXGlPrimMod
{
public:
	XMask(IInt * val)
	: m_Val(val)
	{
	}

	virtual void OnGlBegin(GLenum mode) {
		m_Mask.SetMask(
			FromInt(m_Val.eval())
		);

		m_Mask.OnGlBegin(mode);
	}

	virtual void OnGlEnd() {
		m_Mask.OnGlEnd();
	}

	virtual IRef * Ref (void) {
		return this;
	}

private:
	Mask m_Mask;
	IntRef m_Val;
};


class XReplace : public FunctionHost,
	public IXGlPrimMod
{
public:
	XReplace(IInt * red, IInt * green, IInt * blue)
	{
		IntRef refRed(red);
		IntRef refGreen(green);
		IntRef refBlue(blue);

		m_Replace.SetRgb(
			(GLubyte)refRed.eval(),
			(GLubyte)refGreen.eval(),
			(GLubyte)refBlue.eval()
		);
	}

	virtual void OnGlBegin(GLenum mode) {
		m_Replace.OnGlBegin(mode);
	}

	virtual void OnGlEnd() {
		m_Replace.OnGlEnd();
	}

	virtual IRef * Ref (void) {
		return this;
	}

private:
	Replace m_Replace;
};

class XStack : public Ref,
	public IXGlPrimMod
{
public:
	XStack()
	: m_Locked(false)
	{
	}

	void Add(IXGlPrimMod * mod)
	{
		mod->Ref()->AddRef();

		if(!m_Locked)
		{
			mod->Ref()->AddRef();

			m_Mods.push_back(mod);
		}

		mod->Ref()->Release();
	}

	virtual void OnGlBegin(GLenum mode) {
		m_Locked = true;

		for(ModsT::iterator it = m_Mods.begin(); it != m_Mods.end(); it++)
		{
			(*it)->OnGlBegin(mode);
		}
	}

	virtual void OnGlEnd() {
		for(ModsT::iterator it = m_Mods.begin(); it != m_Mods.end(); it++)
		{
			(*it)->OnGlEnd();
		}

		m_Locked = false;
	}

	virtual IRef * Ref (void) {
		return this;
	}

protected:
	~XStack()
	{
		for(ModsT::iterator it = m_Mods.begin(); it != m_Mods.end(); it++)
		{
			(*it)->Ref()->Release();
		}
	}

private:
	typedef list<IXGlPrimMod *> ModsT;
	
	bool m_Locked;
	ModsT m_Mods;

};


class Xmod : public FunctionHost,
	public IGlPrimMod,
	public IXmod
{
public:
	Xmod();

	virtual IBubble * GetBubble (void) {
		return m_Bubble;
	}

	virtual IGlPrimMod * GlPrimMod (void) {
		return this;
	}

	virtual void OnGlBegin(GLenum mode)
	{
		m_Locked = true;

		if(0 <= m_ActiveStack && m_ActiveStack < (int)m_Stacks.size())
			m_Stacks[m_ActiveStack]->OnGlBegin(mode);
	}

	virtual void OnGlEnd()
	{
		if(0 <= m_ActiveStack && m_ActiveStack < (int)m_Stacks.size())
			m_Stacks[m_ActiveStack]->OnGlEnd();

		m_Locked = false;
	}

	virtual ::Afx::IRef * Ref (void) {
		return this;
	}

protected:
	virtual ~Xmod();

private:
	typedef vector<XStack *> StacksT;

	bool m_Locked;
	IBubble * m_Bubble;
	StacksT m_Stacks;
	int m_ActiveStack;

	VoidT AddColor(Arguments args)
	{
		AddToStack(
			args[0].Int->EvalInt(),
			new XColor(
				args[1].Float,
				args[2].Float,
				args[3].Float,
				args[4].Float
			)
		);
	}

	VoidT AddColorMask(Arguments args)
	{
		AddToStack(
			args[0].Int->EvalInt(),
			new XColorMask(
				args[1].Int,
				args[2].Int,
				args[3].Int,
				args[4].Int
			)
		);
	}

	VoidT AddDepthMask(Arguments args)
	{
		AddToStack(
			args[0].Int->EvalInt(),
			new XDepthMask(
				args[1].Int
			)
		);
	}

	VoidT AddMask(Arguments args)
	{
		AddToStack(
			args[0].Int->EvalInt(),
			new XMask(
				args[1].Int
			)
		);
	}

	VoidT AddReplace(Arguments args)
	{
		AddToStack(
			args[0].Int->EvalInt(),
			new XReplace(
				args[1].Int,
				args[2].Int,
				args[3].Int
			)
		);
	}

	VoidT AddStack(Arguments args)
	{
		if(m_Locked) return;

		XStack * stack = new XStack();
		stack->Ref()->AddRef();

		m_Stacks.push_back(stack);
	}


	void AddToStack(int index, IXGlPrimMod * glPrimMod)
	{
		glPrimMod->Ref()->AddRef();

		if(!m_Locked && 0 <= index && index < (int)m_Stacks.size())
		{
			m_Stacks[index]->Add(glPrimMod);
		}

		glPrimMod->Ref()->Release();
	}

	// (Int)
	VoidT Clear(Arguments args)
	{
		if(m_Locked) return;

		int index = args[0].Int->EvalInt();

		if(0 <= index && index < (int)m_Stacks.size())
		{
			StacksT::iterator it = m_Stacks.begin() + index;

			(*it)->Ref()->Release();

			m_Stacks.erase(it);
		}
	}

	// ()
	VoidT ClearAll (Arguments args) {
		if(m_Locked) return;

		while(0 < m_Stacks.size())
		{
			StacksT::iterator it = m_Stacks.begin();

			(*it)->Ref()->Release();

			m_Stacks.erase(it);
		}
	}

	// ()
	IntT GetActive (Arguments args)
	{
		return m_ActiveStack;
	}

	// ()
	IntT GetCount (Arguments args)
	{
		return (int)m_Stacks.size();
	}

	// (Int)
	VoidT SetActive(Arguments args)
	{
		if(m_Locked) return;

		m_ActiveStack = args[0].Int->EvalInt();
	}
};


// Xmod ////////////////////////////////////////////////////////////////////////

Xmod::Xmod()
	: m_Locked(false)
{
	m_Bubble = Tools::StandardBubble();
	m_Bubble->Ref()->AddRef();

	m_Bubble->Add("Add", Delegate::New(
		m_Bubble->Compiler(),
		this,
		(VoidFunction)&Xmod::AddStack,
		ArgumentsT::New()
	));

	m_Bubble->Add("AddColor", Delegate::New(
		m_Bubble->Compiler(),
		this,
		(VoidFunction)&Xmod::AddColor,
		ArgumentsT::New(5, A_Int, A_Float, A_Float, A_Float, A_Float)
	));

	m_Bubble->Add("AddColorMask", Delegate::New(
		m_Bubble->Compiler(),
		this,
		(VoidFunction)&Xmod::AddColorMask,
		ArgumentsT::New(5, A_Int, A_Int, A_Int, A_Int, A_Int)
	));

	m_Bubble->Add("AddDepthMask", Delegate::New(
		m_Bubble->Compiler(),
		this,
		(VoidFunction)&Xmod::AddDepthMask,
		ArgumentsT::New(2, A_Int, A_Int)
	));

	m_Bubble->Add("AddMask", Delegate::New(
		m_Bubble->Compiler(),
		this,
		(VoidFunction)&Xmod::AddMask,
		ArgumentsT::New(2, A_Int, A_Int)
	));

	m_Bubble->Add("AddReplace", Delegate::New(
		m_Bubble->Compiler(),
		this,
		(VoidFunction)&Xmod::AddReplace,
		ArgumentsT::New(4, A_Int, A_Int, A_Int, A_Int)
	));

	m_Bubble->Add("Active", Delegate::New(
		m_Bubble->Compiler(),
		this,
		(IntFunction)&Xmod::GetActive,
		ArgumentsT::New()
	));

	m_Bubble->Add("Active", Delegate::New(
		m_Bubble->Compiler(),
		this,
		(VoidFunction)&Xmod::SetActive,
		ArgumentsT::New(1, A_Int)
	));

	m_Bubble->Add("Clear", Delegate::New(
		m_Bubble->Compiler(),
		this,
		(VoidFunction)&Xmod::ClearAll,
		ArgumentsT::New()
	));

	m_Bubble->Add("Count", Delegate::New(
		m_Bubble->Compiler(),
		this,
		(IntFunction)&Xmod::GetCount,
		ArgumentsT::New()
	));

	m_Bubble->Add("Remove", Delegate::New(
		m_Bubble->Compiler(),
		this,
		(VoidFunction)&Xmod::Clear,
		ArgumentsT::New(1, A_Int)
	));

}

Xmod::~Xmod() {
	ClearAll(0);

	m_Bubble->Ref()->Release();
}


// XmodFactory /////////////////////////////////////////////////////////////////

IXmod * XmodFactory::Create (void)
{
	return new Xmod();
}
