#include <stdafx.h>

//  debug.cpp - Debug system
//  Copyright (c) Half-Life Advanced Effects project

//  Last changes:
//	2008-06-12 by dominik.matrixstorm.com

//  First changes:
//	2008-05-28 by dominik.matrixstorm.com

#include "debug.h"

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

DebugMessageState DebugListenerBridge::MasterMessage( DebugMaster ^debugMaster, DebugMessage ^debugMessage )
{
	return masterMessageDelegate( debugMaster, debugMessage );
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

void DebugListener::InitDebugListener( bool bInterLockOnSpewMessage )
{
	this->bInterLockOnSpewMessage = bInterLockOnSpewMessage;

	debugListenerBridge = gcnew DebugListenerBridge( gcnew MasterDeattachDelegate( this, &hlae::debug::DebugListener::MasterDeattach ), gcnew MasterMessageDelegate( this, &hlae::debug::DebugListener::MasterMessage ) );
	debugMasters = gcnew System::Collections::Generic::LinkedList<DebugMaster ^>;

	spewMessageSyncer = gcnew System::Object;

	OnSpewMessage = nullptr;
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

DebugMessageState DebugListener::MasterMessage( DebugMaster ^debugMaster, DebugMessage ^debugMessage )
{
	if (nullptr == OnSpewMessage)
		return DebugMessageState::DMS_FAILED;

	DebugMessageState debugMessageState = DebugMessageState::none;
	if ( bInterLockOnSpewMessage )
	{

		try
		{
			Monitor::Enter( spewMessageSyncer );

			debugMessageState = OnSpewMessage( debugMaster, debugMessage );
		}
		finally
		{
			Monitor::Exit( spewMessageSyncer );
		}
	} else
		debugMessageState =  OnSpewMessage( debugMaster, debugMessage );

	return debugMessageState;
}

////////////////////////////////////////////////////////////////////////////////
//
//  DebugMaster:
//

DebugMessageState DebugMaster::PostMessage( System::String ^debugMessageString, DebugMessageType debugMessageType )
{
#ifdef NO_HLAE_DEBUG_SYSTEM
	return DebugMessageState::DMS_IGNORED;
#endif

	// ignore ignored messages:
	if( DebugFilterSetting::DFS_IGNORE == GetFilter( debugMessageType  ))
		return DebugMessageState::DMS_IGNORED;

	DebugMessageState debugMessageState = DebugMessageState::DMS_FAILED;

	bool bTriggerWorker=false;
	try
	{
		Monitor::Enter( messageQue );

		bool bQueueFull=false;
		bQueueFull |= (unsigned int)(messageQue->Count) >= maxMessages;
		bQueueFull |= curSumLenghts >= maxSumLengths;

		if ( bQueueFull )
		{
			if ( DebugFilterSetting::DFS_NODROP == GetFilter( debugMessageType ) )
			{

				debugQueueState =DebugQueueState::DQS_WASCONGESTED;

				// Don't even think about triggering that lazy debug worker,
				// we better please the PostWomen ourself:

				DebugMessage ^workMessage=nullptr;
				while (messageQue->Count > 0)
				{
					workMessage = messageQue->Dequeue();
					if (workMessage)
					{
						curSumLenghts -= workMessage->string->Length;
						PostWomen( workMessage );
					}
				}

				// now post the message that congested the queue directly:
				workMessage = gcnew DebugMessage();
				workMessage->string = debugMessageString; // we pass the string directly
				workMessage->type = debugMessageType;
				PostWomen( workMessage );
			} else {
				if ( DebugQueueState::DQS_OK == debugQueueState)
					debugQueueState =DebugQueueState::DQS_WASFULL;

				// dropped and thus failed ( default result )
			}
		} else {
			DebugMessage ^queueMessage = gcnew DebugMessage();
			queueMessage->string = gcnew System::String( debugMessageString ); // we store a copy of the string
			queueMessage->type = debugMessageType;

			messageQue->Enqueue( queueMessage );
			curSumLenghts += queueMessage->string->Length;
		}
	}
	finally
	{
		bTriggerWorker |= (unsigned int)(messageQue->Count) >= thresholdMinSumLengths;
		bTriggerWorker |= curSumLenghts >= thresholdMinSumLengths;

		Monitor::Exit( messageQue );
	}

	if (bTriggerWorker)
	{
		// Trigger the debug worker:
		try
		{
			Monitor::Enter( debugWorkerTrigger );
			Monitor::Pulse( debugWorkerTrigger );
		}
		finally
		{
			Monitor::Exit( debugWorkerTrigger );
		}
	}

	return debugMessageState;
}

void DebugMaster::Flush( )
{
	try
	{
		Monitor::Enter( messageQue );

		// Flush it directly to the PostWomen:

		DebugMessage ^workMessage=nullptr;
		while (messageQue->Count > 0)
		{
			workMessage = messageQue->Dequeue();
			if (workMessage)
			{
				curSumLenghts -= workMessage->string->Length;
				PostWomen( workMessage );
			}
		}
	}
	finally
	{
		Monitor::Exit( messageQue );
	}
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

		debugAttachState = DebugAttachState::DAS_ATTACHED;
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
			listenerBridges->Remove( node );
		}
	}
	finally
	{
		Monitor::Exit( listenerBridges );
	}
}

DebugMaster::~DebugMaster()
{
	//  Singal class shutdown:
	systemShuttingDown = true;

	// Trigger the debug worker:
	// FLAW WARNING: his step might need rework, since the DebugWorker might miss the trigger and thus we would have to wait until it triggers itself due to timeout
	try
	{
		Monitor::Enter( debugWorkerTrigger );
		Monitor::Pulse( debugWorkerTrigger );
	}
	finally
	{
		Monitor::Exit( debugWorkerTrigger );
	}

	// Wait for the DebugWorker thread to stop:
	debugWorkerThread->Join();

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
#ifdef _DEBUG
	SetFilter( DebugMessageType::DMT_DEBUG, DebugFilterSetting::DFS_DEFAULT );
#else
	SetFilter( DebugMessageType::DMT_DEBUG, DebugFilterSetting::DFS_IGNORE );
#endif
	SetFilter( DebugMessageType::DMT_ERROR, DebugFilterSetting::DFS_DEFAULT );

	messageQue = gcnew System::Collections::Generic::Queue<DebugMessage ^>;
	listenerBridges = gcnew System::Collections::Generic::LinkedList<DebugListenerBridge ^>;

	debugQueueStateSyncer = gcnew System::Object();
	debugQueueState = DebugQueueState::DQS_OK;

	congestionModeSyncer = gcnew System::Object();
	congestionMode = false;

	debugWorkerTrigger = gcnew System::Object();

	systemShuttingDown = false;

	// Start up the debug worker:
	debugWorkerThread = gcnew Thread (gcnew ThreadStart( this, &DebugMaster::DebugWorker ));
	debugWorkerThread->Name = "hlae::debug::DebugMaster::DebugWorker";
	debugWorkerThread->Start();
}

void DebugMaster::DebugWorker()
{
	while (!systemShuttingDown)
	{
		bool bTimeElapsed =	false;
		try
		{
			Monitor::Enter( debugWorkerTrigger );
			bTimeElapsed =	!(Monitor::Wait( debugWorkerTrigger, thresholdMinIdleMilliSeconds ));
		}
		finally
		{
			Monitor::Exit( debugWorkerTrigger );
		}

		bool bDoWork=false;
		DebugMessage ^workMessage=nullptr;

		do
		{
			// check if we still have work to do
			try
			{
				Monitor::Enter( messageQue );

				// determine if there is work to do:
				bDoWork = false;
				bDoWork = bDoWork || (unsigned int)(messageQue->Count) >= thresholdMinMessages; // minimum Message thereshold active
				bDoWork = bDoWork || curSumLenghts >= thresholdMinSumLengths; // ninimum message len thereshold active
				bDoWork = bDoWork || bTimeElapsed; // idle time passed, deliver anyways
				bDoWork = bDoWork && (messageQue->Count > 0); // there must be at least 1 message
				bDoWork = bDoWork && (listenerBridges->Count > 0); // there must be at least [about, not interlocked] 1 listener

				if (bDoWork)
				{
					workMessage = messageQue->Dequeue();
					if (workMessage)
						curSumLenghts -= workMessage->string->Length;
				}
			}
			finally
			{
				Monitor::Exit( messageQue );
			}

			if (bDoWork && workMessage)
			{
				// deliver the message:
				PostWomen( workMessage );
			}
		}
		while( bDoWork );

	}
}

void DebugMaster::PostWomen( DebugMessage ^debugMessage )
{
	try
	{
		Monitor::Enter( listenerBridges );

		for each( DebugListenerBridge ^bridge in listenerBridges )
		{
			bridge->MasterMessage( this, debugMessage );
		}
	}
	finally
	{
		Monitor::Exit( listenerBridges );
	}
}
