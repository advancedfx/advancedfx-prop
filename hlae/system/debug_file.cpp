#include <stdafx.h>

//  debug_file.cpp - Debug to file writer
//  Copyright (c) Half-Life Advanced Effects project

//  Last changes:
//	2008-06-07 by dominik.matrixstorm.com

//  First changes:
//	2008-006-07 by dominik.matrixstorm.com

#include "debug_file.h"
#include <system/debug.h>

using namespace System;
using namespace hlae;
using namespace hlae::debug;

FileDebugListener::FileDebugListener(DebugMaster ^debugMaster, System::String ^path) : DebugListener( true, debugMaster )
{
	logFile = System::IO::File::CreateText( path );

	// Provide new SpewMessage:
	OnSpewMessage = gcnew OnSpewMessageDelegate( this, &FileDebugListener::MySpewMessage );	
};

FileDebugListener::~FileDebugListener()
{
	if(logFile) delete logFile;
	logFile = nullptr;
}

void FileDebugListener::Flush()
{
	if(logFile)
	{
		try
		{
			Monitor::Enter( spewMessageSyncer );

			logFile->Flush();
		}
		finally
		{
			Monitor::Exit( spewMessageSyncer );
		}
	}
}

DebugMessageState FileDebugListener::MySpewMessage( DebugMaster ^debugMaster, DebugMessage ^debugMessage )
{
	if (!logFile)
		return DebugMessageState::DMS_FAILED;

	System::String ^tmsg= "Unknown Message Type: " ;

	switch( debugMessage->type )
	{
	case DebugMessageType::DMT_ERROR:
		tmsg= "Error: " ;
		break;
	case DebugMessageType::DMT_WARNING:
		tmsg= "Warning: " ;
		break;
	case DebugMessageType::DMT_INFO:
		tmsg=  "Info: " ;
		break;
	case DebugMessageType::DMT_VERBOSE:
		tmsg= "Verbose: " ;
		break;
	case DebugMessageType::DMT_DEBUG:
		tmsg= "Debug: " ;
		break;
	}

	bool bPosted=false;

	try
	{
		logFile->Write( tmsg );
		logFile->WriteLine( debugMessage->string );

		bPosted = true;
	}
	catch( System::Exception ^e)
	{
#ifdef _DEBUG
		throw e;
#endif
		if(logFile)
		{
			delete logFile;
			logFile = nullptr;
		}
	}

	return bPosted ? DebugMessageState::DMS_POSTED : DebugMessageState::DMS_FAILED ;
}

