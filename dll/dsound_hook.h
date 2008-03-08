/*
File        : dsound_hook.h
Project     : Half-Life Advanced Effects (Mirv Demo Tool)
Description : Functions to hook some of H-L's DirectSound requests in order to fix sound stopping when focus lost.

Last Change : - by Dominik Tugend
Started     : 2008-03-08 14:46 UTC
Authors     : Dominik Tugend // add yourself here if u change s.th.
*/

#ifndef DSOUND_HOOK_H
#define DSOUND_HOOK_H

#include <windows.h>

#define IDirectSound_FUNCS_CNT 23
#define IDirectSound_ZNUM_CreateSoundBuffer 3

FARPROC WINAPI Hook_DirectSoundCreate(FARPROC fpOldAddress);
// Remarks:	This can be used to hook in a GetProcAddress Situation.
//			In the first call fpOldAddress is used to fill the global in order to remember the address (other calls are not examined).
// Returns:	Address of our own function that should be called instead


#endif // #ifndef DD_HOOK_H