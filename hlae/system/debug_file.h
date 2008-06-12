#pragma once

//  debug_file.h - Debug to file writer
//  Copyright (c) Half-Life Advanced Effects project

//  Last changes:
//	2008-06-12 by dominik.matrixstorm.com

//  First changes:
//	2008-006-07 by dominik.matrixstorm.com

#include <system/debug.h>

namespace hlae {
namespace debug {

//	FileDebugListener
//	
//	Dispose has to be explicetely called on this class, otherwise it might miss file contents.
ref class FileDebugListener : DebugListener
{
public:
	FileDebugListener(DebugMaster ^debugMaster, System::String ^path);
	~FileDebugListener();

private:
	DebugMessageState MySpewMessage( DebugMaster ^debugMaster, DebugMessage ^debugMessage );

	System::IO::StreamWriter ^logFile;

};

} // namespace debug
} // namespace hlae