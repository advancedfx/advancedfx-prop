#include "stdafx.h"

#ifndef HLAE_LOADER_H
#define HLAE_LOADER_H

// When time clean LoaderThread etcl up a bit please!
//
// Project :  Half-Life Advanced Effects
// File    :  gui/loader.cpp
// Changed :  2008-03-20
// Created :  2008-03-19

// Authors : last change / first change / name
// 2008-03-20 / 2008-03-19 / Dominik Tugend
// 2007 / / Gavin Bramhill

// Comment: for launching the game and the hook

bool InitLoader(unsigned int uiUnused, System::String ^m_path, System::String ^m_cmdline);

#endif