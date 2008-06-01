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

////////////////////////////////////////////////////////////////////////////////
//
//  DebugListenerBridge:
//

DebugListenerBridge::DebugListenerBridge( MasterDeattachDelegate ^masterDeattachDelegate, MasterMessageDelegate ^masterMessageDelegate )
{
	this->masterDeattachDelegate = masterDeattachDelegate;
	this->masterMessageDelegate = masterMessageDelegate;
}

void DebugListenerBridge::MasterDeattach( DebugMaster ^debugMaster )
{
	masterDeattachDelegate( debugMaster );
}

DebugMessageState DebugListenerBridge::MasterMessage( DebugMaster ^debugMaster, System::String ^debugMessage, DebugMessageType debugMessageType )
{
	return masterMessageDelegate( debugMaster, debugMessage, debugMessageType );
}

////////////////////////////////////////////////////////////////////////////////
//
//  DebugListener:
//

DebugAttachState DebugListener::Attach( DebugMaster ^debugMaster )
{
	DebugAttachState debugAttachState = DebugAttachState::none;
	

	try
	{
		Monitor::Enter( debugMasters );

		System::Collections::Generic::LinkedListNode<DebugMaster ^> ^node =  debugMasters->Find( debugMaster );
		if( !node )
		{
			debugAttachState = debugMaster->RegisterListener( this->debugListenerBridge );
			if ( DebugAttachState::DAS_ATTACHED == debugAttachState )
				debugMasters->AddFirst( debugMaster );
		}
	}
	finally 
	{
		Monitor::Exit( debugMasters );
	}

	return debugAttachState;
}

void DebugListener::Deattach( DebugMaster ^debugMaster )
{
	try
	{
		Monitor::Enter( debugMasters );

		System::Collections::Generic::LinkedListNode<DebugMaster ^> ^node =  debugMasters->Find( debugMaster );
		if( node )
		{
			debugMaster->UnregisterListener( this->debugListenerBridge );
			debugMasters->Remove( node );
		}
	}
	finally 
	{
		Monitor::Exit( debugMasters );
	}
}

void DebugListener::Deattach()
{
	try
	{
		Monitor::Enter( debugMasters );

		while( 0 < debugMasters->Count )
		{
			debugMasters->First->Value->UnregisterListener( this->debugListenerBridge );
			debugMasters->RemoveFirst();
		}
	}
	finally 
	{
		Monitor::Exit( debugMasters );
	}
}

DebugAttachState DebugListener::GetAttachState( DebugMaster ^debugMaster )
{
	DebugAttachState debugAttachState = DebugAttachState::none;

	try 
	{
		Monitor::Enter( debugMasters );

		System::Collections::Generic::LinkedListNode<DebugMaster ^> ^node =  debugMasters->Find( debugMaster );
		if( node )
			debugAttachState = DebugAttachState::DAS_ATTACHED;
		else
			debugAttachState = DebugAttachState::DAS_DEATTACHED;
	}
	finally 
	{
		Monitor::Exit( debugMasters );
	}

	return debugAttachState;
}

DebugAttachState DebugListener::GetAttachState()
{
	DebugAttachState debugAttachState = DebugAttachState::none;

	try 
	{
		Monitor::Enter( debugMasters );

		if( 0<debugMasters->Count )
			debugAttachState = DebugAttachState::DAS_ATTACHED;
		else
			debugAttachState = DebugAttachState::DAS_DEATTACHED;
	}
	finally 
	{
		Monitor::Exit( debugMasters );
	}

	return debugAttachState;
}

DebugListener::DebugListener( bool bInterLockOnSpewMessage )
{
	InitDebugListener( bInterLockOnSpewMessage );
}

DebugListener::DebugListener( bool bInterLockOnSpewMessage, DebugMaster ^debugMaster )
{
	InitDebugListener( bInterLockOnSpewMessage );
	Attach( debugMaster );
}

DebugListener::~DebugListener()
{
	Deattach();
}

DebugMessageState DebugListener::OnSpewMessage(
		DebugMaster ^debugMaster,
		System::String ^debugMessage,
		DebugMessageType debugMessageType
)
{
	return DebugMessageState::DMS_FAILED;
}


void DebugListener::InitDebugListener( bool bInterLockOnSpewMessage )
{
	this->bInterLockOnSpewMessage = bInterLockOnSpewMessage;

	debugListenerBridge = gcnew DebugListenerBridge( gcnew MasterDeattachDelegate( this, &hlae::debug::DebugListener::MasterDeattach ), gcnew MasterMessageDelegate( this, &hlae::debug::DebugListener::MasterMessage ) );
	debugMasters = gcnew System::Collections::Generic::LinkedList<DebugMaster ^>;

	spewMessageSyncer = gcnew System::Object;
}

void DebugListener::MasterDeattach( DebugMaster ^debugMaster )
{
	try
	{
		Monitor::Enter( debugMasters );
		System::Collections::Generic::LinkedListNode<DebugMaster ^> ^node =  debugMasters->Find( debugMaster );
		if( node )
			debugMasters->Remove( node );
	}
	finally 
	{
		Monitor::Exit( debugMasters );
	}
}

DebugMessageState DebugListener::MasterMessage( DebugMaster ^debugMaster, System::String ^debugMessage, DebugMessageType debugMessageType )
{
	DebugMessageState debugMessageState = DebugMessageState::none;
	if ( bInterLockOnSpewMessage )
	{

		try
		{
			Monitor::Enter( spewMessageSyncer );

			debugMessageState = OnSpewMessage( debugMaster, debugMessage, debugMessageType );
		}
		finally
		{
			Monitor::Exit( spewMessageSyncer );
		}
	} else
		debugMessageState =  OnSpewMessage( debugMaster, debugMessage, debugMessageType );

	return debugMessageState;
}

////////////////////////////////////////////////////////////////////////////////
//
//  DebugMaster:
//

DebugMessageState DebugMaster::PostMessage( System::String ^debugMessage, DebugMessageType debugMessageType )
{
	// not implemented yet
	return DebugMessageState::DMS_FAILED;
}

DebugQueueState DebugMaster::GetLastQueueState()
{
	return debugQueueState;
}

DebugFilterSetting DebugMaster::GetFilter( DebugMessageType debugMessageType )
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
	}
	return DebugFilterSetting::DFS_DEFAULT;
}

void DebugMaster::SetFilter( DebugMessageType debugMessageType, DebugFilterSetting debugFilterSetting )
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
	}
}

DebugAttachState DebugMaster::RegisterListener( DebugListenerBridge ^debugListenerBridge )
{
	DebugAttachState debugAttachState = DebugAttachState::none;
	try
	{
		Monitor::Enter( listenerBridges );

		System::Collections::Generic::LinkedListNode<DebugListenerBridge ^> ^node =  listenerBridges->Find( debugListenerBridge );
		if( !node )
			listenerBridges->AddFirst( debugListenerBridge );

		return DebugAttachState::DAS_ATTACHED;
	}
	finally
	{
		Monitor::Exit( listenerBridges );
	}
	return debugAttachState;
}

void DebugMaster::UnregisterListener( DebugListenerBridge ^debugListenerBridge )
{
	try
	{
		Monitor::Enter( listenerBridges );
		System::Collections::Generic::LinkedListNode<DebugListenerBridge ^> ^node =  listenerBridges->Find( debugListenerBridge );
		if( node )
		{
			// we also need to interlock the listener bridge itself to make sure the master does currently not perform operations on it (i.e. posting a message):
			// WARNING: this code is easily breakable and need to be in harmony with the DebugWorker code!

			DebugListenerBridge ^thebridge = node->Value;
			try
			{
				Monitor::Enter( thebridge );
				listenerBridges->Remove( node );
			}
			finally
			{
				Monitor::Exit( thebridge );
			}
		}
	}
	finally
	{
		Monitor::Exit( listenerBridges );
	}
}

DebugMaster::DebugMaster()
{
	InitDebugMaster(
		2000,
		10,
		500,
		200000,
		1000,
		50000,
		1000
	);
}

DebugMaster::~DebugMaster()
{
	// inform listeners about the master deattach and unregister them:
	try
	{
		Monitor::Enter( listenerBridges );

		while( 0 < listenerBridges->Count )
		{
			listenerBridges->First->Value->MasterDeattach( this );
			listenerBridges->RemoveFirst();
		}
	}
	finally 
	{
		Monitor::Exit( listenerBridges );
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

	SetFilter( DebugMessageType::DMT_ERROR, DebugFilterSetting::DFS_NODROP );
	SetFilter( DebugMessageType::DMT_WARNING, DebugFilterSetting::DFS_DEFAULT );
	SetFilter( DebugMessageType::DMT_INFO, DebugFilterSetting::DFS_DEFAULT );
	SetFilter( DebugMessageType::DMT_VERBOSE, DebugFilterSetting::DFS_DEFAULT );
	SetFilter( DebugMessageType::DMT_DEBUG, DebugFilterSetting::DFS_DEFAULT );
	SetFilter( DebugMessageType::DMT_ERROR, DebugFilterSetting::DFS_DEFAULT );

	messageQue = gcnew System::Collections::Generic::Queue<System::String ^>;
	listenerBridges = gcnew System::Collections::Generic::LinkedList<DebugListenerBridge ^>;

	debugQueueStateSyncer = gcnew System::Object();
	debugQueueState = DebugQueueState::DQS_OK;
}

#endif