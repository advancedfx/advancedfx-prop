#include <stdafx.h>

//  debug.cpp - Debug system
//  Copyright (c) Half-Life Advanced Effects project

//  Last changes:
//	2008-05-30 by dominik.matrixstorm.com

//  First changes:
//	2008-05-28 by dominik.matrixstorm.com

#include "debug.h"
#if HLAE_DEBUG_READY

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Threading;
using namespace hlae;
using namespace hlae::debug;

//
//  DebugListenerBridge:
//

DebugListenerBridge::DebugListenerBridge( DoDeattachDelegate doDeattachDelegate )
{
	this->doDeattachDelegate = doDeattachDelegate;
}

void DebugListenerBridge::DoDeattach( DebugMaster ^debugMaster )
{
	doDeattachDelegate( debugMaster );
}

//
//  DebugListener:
//

DebugListener::DebugListener()
{
	InitDebugListener();
}

DebugListener::DebugListener( DebugMaster ^debugMaster )
{
	InitDebugListener();
	Attach( debugMaster );
}

DebugListener::~DebugListener()
{
	Deattach();
}


//
//  DebugMaster:
//

DebugMaster::DebugFilterSetting DebugMaster::GetDebugFilter( DebugMessageType debugMessageType )
{
	switch (debugMessageType)
	{
	case DebugMessageType::DMT_ERROR:
		return filterError;
	case DebugMessageType::DMT_WARNING:
		return filterWarning;
	case DebugMessageType::DMT_INFO:
		return filterInfo;
	case DebugMessageType::DMT_VERBOSE:
		return filterVerbose;
	case DebugMessageType::DMT_DEBUG:
		return filterDebug;
	default:
	}
	return DebugMessageType::none;
}

void DebugMaster::SetDebugFilter( DebugMessageType debugMessageTyp, DebugFilterSetting debugFilterSetting )
{
	switch ( debugFilterSetting )
	{
	case DebugFilterSetting::DFS_IGNORE:
	case DebugFilterSetting::DFS_NODROP:
		break;
	default:
		debugFilterSetting = DebugFilterSetting::DFS_DEFAULT;
	}

	switch ( debugMessageType )
	{
	case DebugMessageType::DMT_ERROR:
		filterError = debugFilterSetting ;
		break;
	case DebugMessageType::DMT_WARNING:
		filterWarning = debugFilterSetting ;
		break;
	case DebugMessageType::DMT_INFO:
		filterInfo = debugFilterSetting ;
		break;
	case DebugMessageType::DMT_VERBOSE:
		filterVerbose = debugFilterSetting ;
		break;
	case DebugMessageType::DMT_DEBUG:
		filterDebug = debugFilterSetting ;
		break;
	default:
	}
}

void DebugMaster::InitDebugMaster(
	unsigned int maxMessages,
	unsigned int thresholdMinMessages,
	unsigned int thresholdMaxMessages,
	unsigned int maxSumLengths,
	unsigned int thresholdMinSumLengths,
	unsigned int thresholdMaxSumLengths,
	unsigned int thresholdMinIdleMilliSeconds
)
{
	this->maxMessages = maxMessages;
	this->thresholdMinMessages = thresholdMinMessages;
	this->thresholdMaxMessages = thresholdMaxMessages;
	this->maxSumLengths = maxSumLengths;
	this->thresholdMinSumLengths = thresholdMinSumLengths;
	this->thresholdMaxSumLengths = thresholdMaxSumLengths;
	this->thresholdMinIdleMilliSeconds = thresholdMinIdleMilliSeconds;

	SetDebugFilter( filterError, DebugFilterSetting::DFS_NODROP );
	SetDebugFilter( filterWarning, DebugFilterSetting::DFS_DEFAULT );
	SetDebugFilter( filterInfo, DebugFilterSetting::DFS_DEFAULT );
	SetDebugFilter( filterVerbose, DebugFilterSetting::DFS_DEFAULT );
	SetDebugFilter( filterDebug, DebugFilterSetting::DFS_DEFAULT );
	SetDebugFilter( filterError, DebugFilterSetting::DFS_DEFAULT );

}
#endif