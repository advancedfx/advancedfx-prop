#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2009-09-29 by dominik.matrixstorm.com
//
// First changes:
// 2008-03-19 by dominik.matrixstorm.com
// 2007 by Gavin Bramhill

// Description:
// For launching the game and the hook.

bool InitLoader(unsigned int uiUnused, System::String ^m_path, System::String ^m_cmdline);

bool CustomLoader(System::String ^ hookPath, System::String ^ programPath, System::String ^ cmdline);
