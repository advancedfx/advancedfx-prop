#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-17 dominik.matrixstorm.com
//
// First changes
// 2010-11-17 dominik.matrixstorm.com


#include <shared/vcpp/Expressions/Expressions.h>
#include "GlPrimMods.h"

using namespace Afx;
using namespace Afx::Expressions;


struct __declspec(novtable) IXmod abstract
{
	virtual ::Afx::IRef * Ref (void) abstract = 0;

	virtual IBubble * GetBubble(void) abstract = 0;

	virtual GlPrimMod::IGlPrimMod * GlPrimMod (void) abstract = 0;

};

struct __declspec(novtable) XmodFactory abstract
{
	static IXmod * Create (void);
};
