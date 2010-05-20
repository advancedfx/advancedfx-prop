#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-05-20 dominik.matrixstorm.com
//
// First changes
// 2009-12-03 dominik.matrixstorm.com

#include <string>

bool SuggestTakePath(wchar_t const * takePath, int takeDigits, std::wstring & outPath);

bool CreatePath(wchar_t const * path, std::wstring & outPath);
