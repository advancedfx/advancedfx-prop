#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "wrect.h"
#include "cl_dll.h"
#include "cdll_int.h"
#include "r_efx.h"
#include "com_model.h"
#include "r_studioint.h"
#include "pm_defs.h"
#include "cvardef.h"
#include "entity_types.h"
#include "cmdregister.h"

#include "detours.h"

#include "../shared/hldemo/hldemo.h"

extern cl_enginefuncs_s *pEngfuncs;

//
// Addresses:
//

#define ADDRESS_net_message_cursize 0x02817970
#define ADDRESS_msg_readcount 0x02d7c908
#define ADDRESS_CL_ParseServerMessage 0x01d2a270

#define ADDRESS_CL_ParseServerMessage_CmdRead (ADDRESS_CL_ParseServerMessage + 0x0E3)
#define DETOURSIZE_CL_ParseServerMessage_CmdRead 0x07
#define ADDRESS_net_message (ADDRESS_net_message_cursize - 0x10 )

//
// Types and structs
//

typedef int qboolean;

// Quake 1 common.h:
typedef struct sizebuf_s
{
	qboolean	allowoverflow;	// if false, do a Sys_Error
	qboolean	overflowed;		// set to true if the buffer size failed
	byte	*data;
	int		maxsize;
	int		cursize;
} sizebuf_t;


//
// MSG Handler (by peeking ahead)
//

#define MAXCLIENTS 32

void Handle_CmdRead_Intercepted(void)
{
	int *pmsg_readcount = (int *)ADDRESS_msg_readcount;
	sizebuf_t * pnet_message = (sizebuf_t *)ADDRESS_net_message;

	int myreadcount=*pmsg_readcount;

	// now similar to Quake's MSG_ReadByte():

	if (myreadcount+1 > pnet_message->cursize)
		return; // msg_badread

	unsigned char uc = (unsigned char)(pnet_message->data[myreadcount]);
	myreadcount++;

	switch (uc)
	{
	case svc_serverinfo:
		// 4 + 4 + 4 + 16
		int myt;
		unsigned char maxclients;
		myt = 4 + 4 + 4 + 16;
		if (myreadcount+myt > pnet_message->cursize)
			return; // msg_badread

		maxclients = (unsigned char)(pnet_message->data[myreadcount+myt]);
		if (maxclients<MAXCLIENTS) (unsigned char)(pnet_message->data[myreadcount+myt])=maxclients+1;

		break;
	}

}

//
// Hooking related
//

DWORD dwAddress_CL_ParseServerMessage_CmdRead_continue=NULL;

__declspec(naked) void tour_CL_ParseServerMessage_CmdRead()
{
	Handle_CmdRead_Intercepted();
	__asm
	{
		JMP [dwAddress_CL_ParseServerMessage_CmdRead_continue]
	}
}

void install_tour_CL_ParseServerMessage_CmdRead()
{
	if(dwAddress_CL_ParseServerMessage_CmdRead_continue) return;

	dwAddress_CL_ParseServerMessage_CmdRead_continue = (DWORD)DetourApply((BYTE *)ADDRESS_CL_ParseServerMessage_CmdRead,(BYTE *)tour_CL_ParseServerMessage_CmdRead,DETOURSIZE_CL_ParseServerMessage_CmdRead);

	// adjust call in detoured part:
	DWORD *pdwFixAdr = (DWORD *)((char *)dwAddress_CL_ParseServerMessage_CmdRead_continue + 0x02 + 0x01);

	DWORD dwAdr = *pdwFixAdr;

	dwAdr -= dwAddress_CL_ParseServerMessage_CmdRead_continue - ADDRESS_CL_ParseServerMessage_CmdRead;

	*pdwFixAdr = dwAdr;
}

REGISTER_DEBUGCMD_FUNC(test_dem_livetools)
{
	install_tour_CL_ParseServerMessage_CmdRead();
}



/*
Hints (also see Quake 1 source code):

01ecfa9c "Serverinfo Packet recieved\n"

01d293e2 e8193d0300      call    hl!CreateInterface+0x95bd11 (01d5d100)
01d293e7 689cfaec01      push    offset hl!CreateInterface+0xace6ad (01ecfa9c)
...
ret

01d2ac40 e84be7ffff      call    hl!CreateInterface+0x927fa1 (01d29390)
01d2ac45 c705ccc9820201000000 mov dword ptr [hl!CreateInterface+0x142b5dd (0282c9cc)],1 ds:0023:0282c9cc=00000000
01d2ac4f c70550c6ed010000803f mov dword ptr [hl!CreateInterface+0xadb261 (01edc650)],3F800000h
01d2ac59 c3              ret

Jumps back in ParseServerInfo Message

0:000> s -a hl L4000000 "CL_ParseServerMessage: Bad server"
01ecfd6c  43 4c 5f 50 61 72 73 65-53 65 72 76 65 72 4d 65  CL_ParseServerMe

0:003> s hl L4000000 6c fd ec 01
01d2a341  6c fd ec 01 e8 c6 5d 03-00 8b 3d 08 c9 d7 02 83  l.....]...=.....

01d2a340 686cfdec01      push    offset hl!CreateInterface+0xace97d (01ecfd6c)
01d2a345 e8c65d0300      call    hl!CreateInterface+0x95ed21 (01d60110)
01d2a34a 8b3d08c9d702    mov     edi,dword ptr [hl!CreateInterface+0x197b519 (02d7c908)]
01d2a350 83c404          add     esp,4


01d2a353 8bdf            mov     ebx,edi
01d2a355 e846d10000      call    hl!CreateInterface+0x9360b1 (01d374a0)
01d2a35a 8bf0            mov     esi,eax
01d2a35c 83feff          cmp     esi,0FFFFFFFFh
01d2a35f 0f8496020000    je      hl!CreateInterface+0x92920c (01d2a5fb)


--> 01d374a0 == MSG_ReadByte()

...

01d2a5f6 e93cfdffff      jmp     hl!CreateInterface+0x928f48 (01d2a337)
01d2a5fb a130c6ec01      mov     eax,dword ptr [hl!CreateInterface+0xacb241 (01ecc630)]

....

01d2a69f e8091bffff      call    hl!CreateInterface+0x91adbe (01d1c1ad)
01d2a6a4 83c408          add     esp,8
01d2a6a7 5e              pop     esi
01d2a6a8 5d              pop     ebp
01d2a6a9 c3              ret


rets to:
01d23d6b 6a01            push    1
01d23d6d e8fe640000      call    hl!CreateInterface+0x928e81 (01d2a270)
01d23d72 83c404          add     esp,4

01d2a270 --> CL_ParseServerMessage == 01d2a270

01d2a270 a130c6ec01      mov     eax,dword ptr [hl!CreateInterface+0xacb241 (01ecc630)] ds:0023:01ecc630=00000000
01d2a275 55              push    ebp
01d2a276 8b2d08c9d702    mov     ebp,dword ptr [hl!CreateInterface+0x197b519 (02d7c908)]
01d2a27c 3d0000803f      cmp     eax,3F800000h

// we have a point where we can fetch server messages now
// now we need to get to the message buffer:

01d374a0 == Message_ReadByte

01d374a0 a108c9d702      mov     eax,dword ptr [hl!CreateInterface+0x197b519 (02d7c908)]
--> 02d7c908 == msg_readcount
01d374a5 8b1570798102    mov     edx,dword ptr [hl!CreateInterface+0x1416581 (02817970)]
--> 02817970 == net_message.cursize
01d374ab 8d4801          lea     ecx,[eax+1]
01d374ae 3bca            cmp     ecx,edx
01d374b0 7e0e            jle     hl!CreateInterface+0x9360d1 (01d374c0)
01d374b2 c7050cc9d70201000000 mov dword ptr [hl!CreateInterface+0x197b51d (02d7c90c)],1
01d374bc 83c8ff          or      eax,0FFFFFFFFh
01d374bf c3              ret
01d374c0 56              push    esi
01d374c1 8b3568798102    mov     esi,dword ptr [hl!CreateInterface+0x1416579 (02817968)]
*/

