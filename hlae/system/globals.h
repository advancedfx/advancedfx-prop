#pragma once

// Project :  Half-Life Advanced Effects
// File    :  hlae/system/globals.h
// Changed :  2008-10-18
// Created :  2008-10-18

// Authors : last change / first change / name
// 2008-10-18 / 2008-10-18 / Dominik Tugend


#include <system/debug.h>
#include <system/config.h>

namespace hlae {
namespace globals {

using namespace hlae;
using namespace hlae::debug;
using namespace hlae::config;

ref class CGlobals
{
public:
	DebugMaster^ debugMaster;
	CConfigMaster^ ConfigMaster;
};

} // namespace globals
} // namespace hlae