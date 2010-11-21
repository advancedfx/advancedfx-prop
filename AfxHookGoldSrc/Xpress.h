#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-10 dominik.matrixstorm.com
//
// First changes
// 2010-11-10 dominik.matrixstorm.com


#include <shared/vcpp/Expressions/Expressions.h>
#include "Xmod.h"


using namespace Afx;
using namespace Afx::Expressions;


class Xpress : public FunctionHost
{
public:
	struct {
		VoidEvent * GlBegin;
		VoidEvent * GlEnd;
		VoidEvent * FilmingStart;
		VoidEvent * FilmingStop;
		IntEvent * Matte; // -1 standard behaviour, 0 draw, 1 mask, 2 hide
		BoolEvent * RenderViewBegin; // true (default): render, false: skip;
		BoolEvent * RenderViewEnd; // false (default): don't loop, true: loop;
	} Events;

	struct {
		int CurrentGlMode;
		int CurrentStreamIndex; // -1 = unknown , 0/1 stream x
		bool IsFilming;
		bool InRDrawEntitiesOnList;
		bool InRDrawParticles;
		bool InRDrawViewModel;
		bool InRRenderView;
	} Info;

	static Xpress * Get (void);

	IXmod * Mod;

	Xpress();

	ICompiled * CompileEx (char const * code)
	{
		return m_Bubbles.Root->Compile(code);
	}

protected:
	virtual ~Xpress();

private:
	static Xpress * m_Xpress;

	struct {
		IBubble * Root;
	} m_Bubbles;


	VoidT XExec(Arguments args);

	IntT XGetCurrentEntityIndex(Arguments args);

	BoolT XGetIsPlayer(Arguments args);

	IntT XCurrentGlMode(Arguments args)
	{
		return Info.CurrentGlMode;
	}

	IntT XCurrentStreamIndex(Arguments args)
	{
		return Info.CurrentStreamIndex;
	}
	
	BoolT XIsFilming(Arguments args)
	{
		return Info.IsFilming;
	}

	BoolT XInRDrawEntitiesOnList(Arguments args)
	{
		return Info.InRDrawEntitiesOnList;
	}
	BoolT XInRDrawParticles(Arguments args)
	{
		return Info.InRDrawParticles;
	}
	BoolT XInRDrawViewModel(Arguments args)
	{
		return Info.InRDrawViewModel;
	}
	BoolT XInRRenderView(Arguments args)
	{
		return Info.InRRenderView;
	}

};


void xpress_execute(char const * textCode);

