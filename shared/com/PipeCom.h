#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-03-20 by dominik.matrixstorm.com
//
// First changes:
// 2010-01-12 by dominik.matrixstorm.com

//
// PipeCom
//
// As the name says this is (interprocess) communication
// between exactly two partners where the messages
// arrive in the order you sent them.
//

#include <windows.h>


// PipeCom /////////////////////////////////////////////////////////////////////

class PipeCom;

class PipeCom
{
public:
	PipeCom(HANDLE readPipe, HANDLE writePipe);

	HANDLE GetReadPipe();
	HANDLE GetWritePipe();

	DWORD IncomingBytes();

	void ReadBytes(LPVOID outBuffer, DWORD bytesToRead);
	void WriteBytes(LPVOID buffer, DWORD bytesToWrite);

private:
	HANDLE m_hRead;
	HANDLE m_hWrite;
};


// PipeComServer ///////////////////////////////////////////////////////////////

class PipeComServer : public PipeCom
{
public:
	PipeComServer();
	~PipeComServer();

	HANDLE GetClientReadPipe();
	HANDLE GetClientWritePipe();

private:
	HANDLE m_hClientRead;
	HANDLE m_hClientWrite;
};

