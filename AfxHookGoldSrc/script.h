#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-02-23 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#define XP_WIN
#include <js/src/jsapi.h>
#include <js/src/jsstr.h>

#define JSMIRVPROP JSPROP_ENUMERATE|JSPROP_PERMANENT
#define JSMIRVSHAREDPROP JSMIRVPROP|JSPROP_SHARED