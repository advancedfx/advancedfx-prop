#pragma once
#if 0

//  debug.h - Debug system
//  Copyright (c) Half-Life Advanced Effects project

//  Last changes:
//	2008-05-29 by dominik.matrixstorm.com

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

ref class DebugControl : ListView
{
public:
	DebugControl();
	DebugControl( DebugMaster ^debugMaster );
	~DebugControl();

	DebugAttachState GetAttachState();
	DebugAttachState Attach( DebugMaster ^debugMaster  );
	DebugAttachState Deattach();

private:
	DebugMessageState SpewMessage( System::String ^debugMessage, DebugMessageType debugMessageType );
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

	//  Posts a debug message.
	//	Params:
	//		debugMessage // String message to post
	//		debugMessageType // DebugMessageType
	//  Result:
	//    DebugMessageState
	DebugMessageState PostMessage( System::String ^debugMessage, DebugMessageType debugMessageType );

	//  Result:
	//    the last tracked DebugQueueState since the last time this function was called.
	//    DQS_OK would mean there was no problem noticed since the last time the
	//    function was called.
	DebugQueueState GetLastDebugQueueState();

	//  Sets a new LogFile where things will be written out too from cache.
	//  Things already written out or lost remain lost.
	//  Params:
	//    logfilePath // new filepath to write to
	//  Result:
	//    LFS_OK in case there was no problem so far
	LogFileState SetLogFile( System::String ^logFilePath );

	//  Result:
	//    the logfile state
	LogFileState GetLogFileState();

	//  If you don't want to write to any logfile any longer call this.
	void UnsetLogFile();

	//
	//  non threadsafe functions:
	//

	//  Constructs a DebugMaster object.
	DebugMaster();

	//  Destructs a DebugMaster object.
	~DebugMaster();

private:
	//
	// Threadsafe functions:
	//

	//  Attaches a DebugControl to which DebugMessages shall be written out.
	//  Params:
	//    textBox // the text box to attach
	//  Result:
	//    InvalidTextBoxHandle on error, otherwise a handle
	//  Remarks:
	//    The HlaeDebugMaster should be the only one accessing that box.
	//    Before the box is deleted DeattachTextBox has to be called with the handle.
	DebugAttachState AttachDebugControl( DebugControl ^debugControl );

	//  Deattaches a DebugControl from the debugging system.
	//  Params:
	//    textBoxHandle // the handle of the box to be deattached
	DebugAttachState DeattachDebugControl( DebugControl ^debugControl );

	//
	//  non threadsafe functions:
	//

	//  Initiates the DebugMaster object.
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
};

}	// namespace debug
}	// namespace hlae
#endif