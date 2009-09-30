#pragma once

// Project :  Half-Life Advanced Effects
// File    :  hlae/system/globals.h
// Changed :  2008-10-18
// Created :  2008-10-18

// Authors : last change / first change / name
// 2008-10-18 / 2008-10-18 / Dominik Tugend


#include <system/debug.h>

namespace hlae {
namespace globals {

using namespace hlae;
using namespace hlae::debug;

//	CGlobals
//
//	The following rules apply when using CGlobals:
//	1) They are only valid during Application::Run
//	2) They may not change during Application::Run
//	3) Only threadsafe functions of exposed structures may be used
ref class CGlobals
{
public:
	DebugMaster^ debugMaster;
};

} // namespace globals
} // namespace hlae