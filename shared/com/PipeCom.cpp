#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 20010-01-12 by dominik.matrixstorm.com
//
// First changes:
// 20010-01-12 by dominik.matrixstorm.com

#include "PipeCom.h"

#define PIPECOM_MSG_NORMAL   0x00
#define PIPECOM_MSG_BURSTREQ 0x01
#define PIPECOM_MSG_BURSTACC 0x02

#define PIPECOM_MSG_RESERVED 0xFF


// PipeCom /////////////////////////////////////////////////////////////////////

PipeCom::PipeCom(HANDLE readPipe, HANDLE writePipe, PipeComMessageHandler messageHandler) {
	m_Handler = messageHandler;
	m_hRead = readPipe;
	m_hWrite = writePipe;
}

bool PipeCom::CheckHeader(BYTE &outHeader) {
	// check for incoming data:
	{
		DWORD bytesAvail = 0;
		if(!PeekNamedPipe(m_hRead, 0, 0, 0, &bytesAvail, 0))
			throw ""; 

		if(bytesAvail <= 0)
			return false; // no data
	}

	// data available.

	// read header:
	{
		DWORD bytesRead = 0;
		if(!ReadFile(m_hRead, &outHeader, sizeof(outHeader), &bytesRead, 0)
			|| bytesRead != sizeof(outHeader)
		) throw "";

	}

	return true;
}

void PipeCom::BeginMessage(bool asBurst) {
	if(!asBurst) {
		// normal message:
		SendHeader(PIPECOM_MSG_NORMAL);
		return;
	}

	// burst-message:

	SendHeader(PIPECOM_MSG_BURSTREQ);

	BYTE header;
	do {
		DWORD bytesRead = 0;
		if(!ReadFile(m_hRead, &header, sizeof(header), &bytesRead, 0)
			|| bytesRead != sizeof(header)
		) throw "";

		switch(header) {
		case PIPECOM_MSG_NORMAL:
			m_Handler(this, false); // handle async message
			break;
		case PIPECOM_MSG_BURSTREQ:
			SendHeader(PIPECOM_MSG_BURSTACC); // ack
			m_Handler(this, true); // handle synchronus message
			break;
		case PIPECOM_MSG_BURSTACC:
			// we are in sync now.
			break;
		default:
			throw "";
		}

	} while(header != PIPECOM_MSG_BURSTACC); 

	// we can burst now.
}


HANDLE PipeCom::GetReadPipe() {
	return m_hRead;
}


HANDLE PipeCom::GetWritePipe() {
	return m_hWrite;
}


void PipeCom::ReceiveMessages() {
	BYTE header;

	while(CheckHeader(header)) {
		switch(header) {
		case PIPECOM_MSG_NORMAL:
			m_Handler(this, false); // handle async message
			break;
		case PIPECOM_MSG_BURSTREQ:
			SendHeader(PIPECOM_MSG_BURSTACC); // ack
			m_Handler(this, true); // handle synchronus message
			break;
		default:
			throw "";
		}
	}
}


void PipeCom::SendHeader(BYTE header) {
	DWORD bytesWritten = 0;
	if(!WriteFile(m_hWrite, &header, sizeof(header), &bytesWritten, 0)
		|| bytesWritten != sizeof(header)
	) throw "";
}


// PipeComServer ///////////////////////////////////////////////////////////////

struct TempServerPipes_s {
	HANDLE serverRead;
	HANDLE serverWrite;
	HANDLE clientRead;
	HANDLE clientWrite;
} g_TempServerPipes;

HANDLE TempServerPipes1() {
	SECURITY_ATTRIBUTES secAttrib;
	secAttrib.nLength = sizeof(secAttrib);
	secAttrib.lpSecurityDescriptor = 0;
	secAttrib.bInheritHandle = true;

	if(!CreatePipe(&g_TempServerPipes.clientRead, &g_TempServerPipes.serverWrite, &secAttrib, 0))
		throw "Pipe creation failed."
	;

	if(!CreatePipe(&g_TempServerPipes.serverRead, &g_TempServerPipes.clientWrite, &secAttrib, 0))
		throw "Pipe creation failed."
	;

	return g_TempServerPipes.serverRead;
}

HANDLE TempServerPipes2() {
	return g_TempServerPipes.serverWrite;
}

PipeComServer::PipeComServer(PipeComMessageHandler messageHandler)
: PipeCom(TempServerPipes1(), TempServerPipes2(), messageHandler)
{
	m_hClientRead = g_TempServerPipes.clientRead;
	m_hClientWrite = g_TempServerPipes.clientWrite;
}

PipeComServer::~PipeComServer() {
	CloseHandle(m_hClientRead);
	CloseHandle(m_hClientWrite);
	CloseHandle(GetReadPipe());
	CloseHandle(GetWritePipe());
}

HANDLE PipeComServer::GetClientReadPipe() {
	return m_hClientRead;
}

HANDLE PipeComServer::GetClientWritePipe() {
	return m_hClientWrite;
}
