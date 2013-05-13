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



typedef bool ComBoolean;
typedef unsigned __int8 ComByte;
typedef double ComDouble;
typedef __int32 ComInt32;
typedef unsigned __int32 ComUInt32;


// PipeCom /////////////////////////////////////////////////////////////////////

class PipeCom;

class PipeCom
{
public:
	PipeCom(HANDLE readPipe, HANDLE writePipe);

	HANDLE GetReadPipe();
	HANDLE GetWritePipe();

	DWORD IncomingBytes();

	ComBoolean ReadBoolean();
	ComByte ReadByte();

	void ReadBytes(LPVOID outBuffer, DWORD bytesToRead);

	ComDouble ReadDouble();
	ComInt32 ReadInt32();
	ComUInt32 ReadUInt32();

	void Write(ComBoolean value);
	void Write(ComByte value);
	void Write(ComDouble value);
	void Write(ComInt32 value);
	void Write(ComUInt32 value);

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

