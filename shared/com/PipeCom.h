#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 20010-01-12 by dominik.matrixstorm.com
//
// First changes:
// 20010-01-12 by dominik.matrixstorm.com

#include <windows.h>

//
// PipeCom
//
// As the name says this is (interprocess) communication
// between exactly two partners where the messages
// arrive in the order you sent them.

//
//
// Message modes:
//
// Normal mode:
// - Use  for occassional messages (notifications, ...)
//   that do not need an instant reply.
//
// - Your reply to those must be in form of a new message, since
//   you cannot know if the sender and reciever pipes are in sync
//   or if there are other messages in between that have yet to be
//   processed.
//

//
// Burst mode:
// - Use this in situations where your require a lot of instant
//   back and forth communication
//   or where your code shall not continue normal operation without an reply
//   , however until PipeCom can enter Burst Message Mode it will process
//   incoming messages.
//
// - Once in burst mode you can exchange data back and forth
//   using the read and the write pipe, since the pipes
//   are in sync then.
//


// PipeCom /////////////////////////////////////////////////////////////////////

class PipeCom;

typedef void (* PipeComMessageHandler)(PipeCom *pipeCom, bool isBurst);

class PipeCom
{
public:
	PipeCom(HANDLE readPipe, HANDLE writePipe, PipeComMessageHandler messageHandler);

	void BeginMessage(bool asBurst);

	HANDLE GetReadPipe();
	HANDLE GetWritePipe();

	void ReceiveMessages();

private:
	PipeComMessageHandler m_Handler;
	HANDLE m_hRead;
	HANDLE m_hWrite;

	/// <summary> Checks if there is a message and recieves it's header </summary>
	/// <returns> if a message was recieved </returns>
	/// <param name="outHeader"> message header (if any) </param>
	bool CheckHeader(BYTE &outHeader);

	void SendHeader(BYTE header);
};


// PipeComServer ///////////////////////////////////////////////////////////////

class PipeComServer : public PipeCom
{
public:
	PipeComServer(PipeComMessageHandler messageHandler);
	~PipeComServer();

	HANDLE GetClientReadPipe();
	HANDLE GetClientWritePipe();

private:
	HANDLE m_hClientRead;
	HANDLE m_hClientWrite;
};

