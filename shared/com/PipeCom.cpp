#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 20010-01-12 by dominik.matrixstorm.com
//
// First changes:
// 20010-01-12 by dominik.matrixstorm.com

#include "PipeCom.h"


// PipeCom /////////////////////////////////////////////////////////////////////

PipeCom::PipeCom(HANDLE readPipe, HANDLE writePipe) {
	m_hRead = readPipe;
	m_hWrite = writePipe;
}




HANDLE PipeCom::GetReadPipe() {
	return m_hRead;
}


HANDLE PipeCom::GetWritePipe() {
	return m_hWrite;
}


DWORD PipeCom::IncomingBytes()
{
	DWORD bytesAvail = 0;
	if(!PeekNamedPipe(m_hRead, 0, 0, 0, &bytesAvail, 0))
		throw "";

	return bytesAvail;
}


void PipeCom::ReadBytes(LPVOID outBuffer, DWORD bytesToRead)
{
	DWORD bytesRead;

	while(bytesToRead)
	{
		if(!ReadFile(m_hRead, outBuffer, bytesToRead, &bytesRead, NULL))
			throw "PipeCom::ReadBytes";
	
		outBuffer = (unsigned char *)outBuffer +bytesRead;
		bytesToRead -= bytesRead;
	}
}


void PipeCom::WriteBytes(LPVOID buffer, DWORD bytesToWrite)
{
	DWORD bytesWritten = 0;

	while(bytesToWrite)
	{
		if(!WriteFile(m_hWrite, buffer, bytesToWrite, &bytesWritten, NULL))
			throw "PipeCom::WriteBytes";
		
		buffer = (unsigned char *)buffer +bytesWritten;
		bytesToWrite -= bytesWritten;
	}
}


// PipeComServer ///////////////////////////////////////////////////////////////

struct TempServerPipes_s {
	HANDLE serverRead;
	HANDLE serverWrite;
	HANDLE clientRead;
	HANDLE clientWrite;
} g_TempServerPipes;

bool g_CreatingTempServerPipes = false;

void TempServerPipesCreate()
{
	if(g_CreatingTempServerPipes)
		return;

	g_CreatingTempServerPipes = true;

	SECURITY_ATTRIBUTES secAttrib;
	secAttrib.nLength = sizeof(secAttrib);
	secAttrib.lpSecurityDescriptor = 0;
	secAttrib.bInheritHandle = TRUE;

	if(!CreatePipe(&g_TempServerPipes.clientRead, &g_TempServerPipes.serverWrite, &secAttrib, 0))
		throw "Pipe creation failed."
	;

	if(!CreatePipe(&g_TempServerPipes.serverRead, &g_TempServerPipes.clientWrite, &secAttrib, 0))
		throw "Pipe creation failed."
	;
}

HANDLE TempServerPipesGetRead()
{
	TempServerPipesCreate();

	return g_TempServerPipes.serverRead;
}

HANDLE TempServerPipesGetWrite()
{
	TempServerPipesCreate();

	return g_TempServerPipes.serverWrite;
}

void TempServerPipesFinish()
{
	g_CreatingTempServerPipes = false;
}


PipeComServer::PipeComServer()
: PipeCom(TempServerPipesGetRead(), TempServerPipesGetWrite())
{
	TempServerPipesFinish();

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
