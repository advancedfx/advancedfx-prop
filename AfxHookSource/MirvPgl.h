#pragma once

#define AFX_MIRV_PGL

#ifdef AFX_MIRV_PGL

#include "CamIO.h"

/*

Version: 0


Usage:


It is a good idea to run with the FPS limited (either by vsync (recommended) or by host_framerate).
Otherwise the network will be flooded with "cam" messages, since there is currently not throttling.


Console commands:

mirv_pgl start "ws://host:port/path" - (Re-)Starts connectinion to server.*
mirv_pgl stop - Stops connection to server.*

* It is NOT safe to execute this command using the "exec" command from the server (deadlock will occur).


Connection loss:

If the connection is lost and not stopped, it is retried every 5 seconds.
The server-side state should be reset upon connection loss (as if mirv_pgl stop had been called).


Messages:
  The messages are exchanged as binary frames.

  Multiple messages can be in a single frame!!!!

  The message data is not aligned / padded!

  CString is a null-terminated string as in C-Language.


Messages sent to server:

"hello"
Purpose:
  Is sent upon (re)-connecting.
  If received with unexpected version, server should close the connection.
Format:
  CString cmd = "hello"
  UInt32 version = 0;

"levelInit"
Purpose:
  Is sent if in a level upon (re)-connecting after "hello" message or if a new level is loaded.
Format:
  CString cmd = "levelInit"
  CString levelName;

"levelShutdown"
Purpose:
  Is sent after "levelInit" when level is shut down.
Format:
  CString cmd = "levelShutdown"
  CString levelName;

"cam"
Purpose:
  Is sent after "levelInit" after the frame has been presented on screen.
  The data content is from when the frame presented has been calculated by the engine.
  The fov is currently automatically converted according to Alien Swarm SDK (suitable for CS:GO).
Format:
  CString cmd = "cam";
  Double time;
  Double xPosition;
  Double yPosition;
  Double zPoisiton;
  Double xRotation;
  Double yRotation;
  Double zRotation;
  Double fov;


Messages received:

"exec"
Purpose:
  Schedules cmds for console execution.
  Use with CAUTION: Flooding the client with too many commands will crash the game. Also don't use mirv_pgl start / stop (will cause deadlock).
Format:
  CString cmd = "exec";
  CString cmds;


Ideas for the future:
- Implement black image command with feedback when presented.
- Implement white image command with feedback when presented.
- Implement optional time-code (float) graphic overlay at top of screen, this would allow syncing the images and the camdata on remote PC perfectly (as long as turned on).

*/

namespace MirvPgl
{
	void Init();
	void Shutdown();

	void Start(char const * url);
	void Stop();
	bool IsStarted();

	void CheckStartedAndRestoreIfDown();

	void ExecuteQueuedCommands();

	void SupplyLevelInit(char const * mapName);
	void SupplyLevelShutdown();

	void DrawingThread_SupplyCamData(CamIO::CamData const & camData);

	void DrawingThread_PresentedUnleashCamDataOnFirstCall();
}

#endif
