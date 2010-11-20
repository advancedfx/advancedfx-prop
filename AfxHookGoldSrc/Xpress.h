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
		IntVariable * CurrentGlMode;
		IntVariable * CurrentStreamIndex; // -1 = unknown , 0/1 stream x
		BoolVariable * IsFilming;
		BoolVariable * InRDrawEntitiesOnList;
		BoolVariable * InRDrawParticles;
		BoolVariable * InRDrawViewModel;
		BoolVariable * InRRenderView;
	} Info;

	static Xpress * Get (void);

	IXmod * Mod;

	Xpress();

	~Xpress();

	ICompiled * CompileEx (char const * code)
	{
		return m_Bubbles.Root->Compile(code);
	}

private:
	static Xpress * m_Xpress;

	struct {
		IBubble * Root;
	} m_Bubbles;

	VoidT Exec(Arguments args);

	IntT GetCurrentEntityIndex(Arguments args);

	BoolT GetIsPlayer(Arguments args);
};


void xpress_execute(char const * textCode);

