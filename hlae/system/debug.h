#pragma once
#if 0
#define HLAE_DEBUG_READY 1

//  debug.h - Debug system
//  Copyright (c) Half-Life Advanced Effects project

//  Last changes:
//	2008-05-30 by dominik.matrixstorm.com

//  First changes:
//	2008-05-28 by dominik.matrixstorm.com

//
//  A note to programers about threading safety:
//
//    If you do any changes to the DebugMaster or other sensitive classes,
//    do them carefully to ensure that functions that are guaranteed to be
//    threadsafe remain threadsafe.
//
//    Functions not explicetly marked as threadsafe must not be asumed to be
//    threadsafe.
//
//  Some of the classes are extensively documented, make sure to update
//  the documentation in case your changes require that, also be aware that
//  users of the clase might already make asumptions from the previous
//  documenation.
//

using namespace System;
using namespace System::Windows::Forms;

namespace hlae {
namespace debug {

enum class DebugMessageType
{
	none,
	DMT_ERROR,
	DMT_WARNING,
	DMT_INFO,
	DMT_VERBOSE,
	DMT_DEBUG
};

enum class DebugMessageState
{
	none,
	DMS_POSTED,
	DMS_FAILED,
	DMS_IGNORED
};

enum class DebugFilterSetting
{
	DFS_DEFAULT=0,
	DFS_IGNORE,
	DFS_NODROP
};

enum class LogFileState
{
	none,
	LFS_NOTSET,
	LFS_OK,
	LFS_ERROR,
};

enum class DebugQueueState
{
	none,
	DQS_OK,
	DQS_WASFULL,
	DQS_WASCONGESTED
};

enum class DebugAttachState
{
	none,
	DAS_ATTACHED,
	DAS_DEATTACHED,
	DAS_ATTACHFAILED
};

ref class DebugMaster; // forward decleration

ref class DebugListenerBridge
{
public:
	void DoDeattach( DebugMaster ^debugMaster );
public:
	delegate void DoDeattachDelegate( DebugMaster ^debugMaster );
	DebugListenerBridge( DoDeattachDelegate doDeattachDelegate );
private:
	DoDeattachDelegate doDeattachDelegate;
};

//  DebugListener:
//
//  Base class for attaching to one or several debugMasters and recieving.
abstract ref class DebugListener
{
public:
	//
	// Threadsafe functions:
	//
	abstract DebugMessageState SpewMessage(
		DebugMaster ^debugMaster,
		System::String ^debugMessage,
		DebugMessageType debugMessageType
	);

	DebugAttachState Attach( DebugMaster ^debugMaster );
	DebugAttachState Deattach( DebugMaster ^debugMaster );
	DebugAttachState Deattach();

	DebugAttachState GetAttachState( DebugMaster ^debugMaster );
	DebugAttachState GetAttachState();

	//
	// non threadsafe functions:
	//

	DebugListener();
	DebugListener( DebugMaster ^debugMaster );
	~DebugListener();

private:
	DebugListenerBridge debugListenerBridge;
	System::Collections::Generic::LinkedList<debugMaster ^> debugMasters; // SyncRoot is used for locking

	void InitDebugListener();

	void DoDeattach( DebugMaster ^debugMaster ); // threadsafe
};


//  DebugMaster:
//
//  The debug system handles debug messages and how to save them to a file and
//  deliver them to a UI display (DebugControl).
//
//  Messages that have been successfuly enqued won't be dropped drom the que.
//  This is intended by design, since earlier messages are consindered more
//  important than later ones.
//
//  By default only DMT_ERROR messages may not be dropped, since not dropping
//  a message can have significant performance hits (see "Incoming messages").
//
//  Incoming messages (PostMessages()) are handled in the following order:
//    In case the filter is set to ignore the message, it is ignored.
//	  If the que is full and the filter allows dropping the message, the
//      message is dropped.
//    If the que is full and the filter is set to not allow dropping the
//       message, the que is delivered and if message still does not fit into
//       the now empty que it is delivered directly, otherwise it is enqued.
//       Please be aware that this situation means a significant performance
//       hit:
//       Incoming messages are either dropped or the PostMessage call is
//       blocked until the delivery of the current message has been
//       carried out.
//    Otherwise the message is enqued.
//
//  The message que is "full" when:
//    - Adding the message would void maxMessages
//    - Adding the message would void maxSumLengths
//
//  The message que is "congested" when:
//    - The que is full and a incoming message may not be dropped
//
//  Delivery of the que is trigered by the following events:
//    - The que is congested
//    - thresholdMaxMessages has been hit
//    - thresholdMaxSumLengths has been hit
//
//  Delivery works as follows:
//  A worker thread is triggered, pops out a message of the que and delivers it.
//  The work thread repeats this operation until one of the following conditions are met:
//    - number of qued messages <= thresholdMinMessages
//    - sum of the qued messages' lengths <= thresholdMinSumLengths
//    - in case the delivery was requested due to 
ref class DebugMaster
{
public:
	//
	// Threadsafe functions:
	//

	DebugMessageState PostMessage( System::String ^debugMessage, DebugMessageType debugMessageType );

	//  Result:
	//    the last tracked DebugQueueState since the last time this function was called.
	DebugQueueState GetLastQueueState();

	DebugFilterSetting GetFilter( DebugMessageType debugMessageType );
	void SetFilter( DebugMessageType debugMessageTyp, DebugFilterSetting debugFilterSetting );

	void RegisterListener( DebugListenerBridge^ debugListenerBridge);

	//
	//  non threadsafe functions:
	//

	DebugMaster();

	~DebugMaster();

private:
	//
	// Threadsafe functions:
	//


	//
	//  non threadsafe functions:
	//

	void InitDebugMaster(
		unsigned int maxMessages,
		unsigned int thresholdMinMessages,
		unsigned int thresholdMaxMessages,
		unsigned int maxSumLengths,
		unsigned int thresholdMinSumLengths,
		unsigned int thresholdMaxSumLengths,
		unsigned int thresholdMinIdleMilliSeconds
	); 

private:
	unsigned int maxMessages,
	unsigned int thresholdMinMessages,
	unsigned int thresholdMaxMessages,
	unsigned int maxSumLengths,
	unsigned int thresholdMinSumLengths,
	unsigned int thresholdMaxSumLengths,
	unsigned int thresholdMinIdleMilliSeconds
	unsigned int curSumLenghts;

	DebugFilterSetting filterError;
	DebugFilterSetting filterWarning;
	DebugFilterSetting filterInfo;
	DebugFilterSetting filterVerbose;
	DebugFilterSetting filterDebug;

	System::Collections::Generic::Queue<System::String ^> messageQue; // SyncRoot is used for locking

};

}	// namespace debug
}	// namespace hlae
#endif