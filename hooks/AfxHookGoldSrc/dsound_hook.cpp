/*
File        : dsound_hook.cpp
Project     : Half-Life Advanced Effects (Mirv Demo Tool)
Description : see dsound_hok.h
              IDirectSound::CreateSoundBuffer (http://msdn2.microsoft.com/en-us/library/ms898123.aspx)



Last Change : - by Dominik Tugend
Started     : 2008-03-08 14:46 UTC
Authors     : Dominik Tugend // add yourself here if u change s.th.
*/

#include "mdt_debug.h"

#include <windows.h>
#include <shared/microsoft/dsound.h>
#include <stdio.h>

#include "dsound_hook.h"

typedef HRESULT (STDMETHODCALLTYPE *CreateSoundBuffer_t)(void *mythis,  LPCDSBUFFERDESC lpcDSBufferDesc, LPLPDIRECTSOUNDBUFFER lplpDirectSoundBuffer, IUnknown FAR* pUnkOuter );
typedef HRESULT (WINAPI *DirectSoundCreate_t)( LPGUID lpGuid, LPDIRECTSOUND* ppDS, LPUNKNOWN  pUnkOuter );

CreateSoundBuffer_t g_oldCreateSoundBuffer=NULL;
DirectSoundCreate_t g_oldDirectSoundCreate=NULL;

bool g_b_myIDirectSound_Memory_Used = false;
char g_myIDirectSound_Memory [sizeof(DWORD *)*IDirectSound_FUNCS_CNT];


HRESULT STDMETHODCALLTYPE myCreateSoundBuffer(void *mythis, LPCDSBUFFERDESC lpcDSBufferDesc, LPLPDIRECTSOUNDBUFFER lplpDirectSoundBuffer, IUnknown FAR* pUnkOuter )
{
	//MessageBoxA(0,"myCreateSoundBuffer",">>",MB_OK);

	// since H-L will reaccess the DSBufferDesc we supply we will change their buffer's flags directly:
	// we just circumvent the const defintion here:
	//MdtMemBlockInfos mbis;
	DWORD *dwAddr=(DWORD *)lpcDSBufferDesc; 
	//MdtMemAccessBegin(dwAddr, sizeof(DSBUFFERDESC), &mbis);
	((DSBUFFERDESC *)dwAddr)->dwFlags |= DSBCAPS_STICKYFOCUS;
	//MdtMemAccessEnd(&mbis);


	HRESULT hResult=g_oldCreateSoundBuffer(mythis, lpcDSBufferDesc, lplpDirectSoundBuffer, pUnkOuter );

	return hResult;
}

DWORD *g_newLPIDirectSound = NULL;

HRESULT WINAPI myDirectSoundCreate( LPGUID lpGuid, LPDIRECTSOUND* ppDS, LPUNKNOWN  pUnkOuter )
{
	HRESULT hResult=g_oldDirectSoundCreate( lpGuid, ppDS, pUnkOuter);

	if (hResult==DS_OK && !(g_b_myIDirectSound_Memory_Used))
	{
		g_b_myIDirectSound_Memory_Used = true;

		// we return a copy instead, that has our myCreateSoundBuffer in place:
		DWORD **pDS = (DWORD **)*ppDS;

		memcpy(g_myIDirectSound_Memory,*pDS,sizeof(g_myIDirectSound_Memory));

		memcpy(&g_oldCreateSoundBuffer,(DWORD *)g_myIDirectSound_Memory + IDirectSound_ZNUM_CreateSoundBuffer,sizeof(DWORD *));

		DWORD *pNewAddr = (DWORD *)&myCreateSoundBuffer;
		memcpy((DWORD *)g_myIDirectSound_Memory + IDirectSound_ZNUM_CreateSoundBuffer,&pNewAddr,sizeof(DWORD *));

		g_newLPIDirectSound = (DWORD *)g_myIDirectSound_Memory;
		memcpy(*ppDS,&g_newLPIDirectSound,sizeof(DWORD *));
	}

	return hResult;
}

FARPROC WINAPI Hook_DirectSoundCreate(FARPROC fpOldAddress)
{	
	if (!g_oldDirectSoundCreate) g_oldDirectSoundCreate=(DirectSoundCreate_t)fpOldAddress; // save old address for us
	return (FARPROC)myDirectSoundCreate; // set our hooked struct
}