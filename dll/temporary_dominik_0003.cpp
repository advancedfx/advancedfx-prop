#include "windows.h" // we need access to virtualprotect etc.

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

extern cl_enginefuncs_s *pEngfuncs;
extern engine_studio_api_s *pEngStudio;
extern playermove_s* ppmove;


/* hints:

01edcc04 "%3ifps %3i ms %4i wpoly %4i epoly\n"

01d51eb5 push offset 01edcc04

01dd4f75 call R_RenderView

0x01dd0370 SCR_UpdateScreen
...
01dd041e e83d74f8ff      call    launcher!CreateInterface+0x956471 (01d57860) == SCR_SetUpToDrawConsole ()
01dd0423 e8a8480000      call    launcher!CreateInterface+0x9d38e1 (01dd4cd0) == V_RenderView
01dd0428 e823b5f8ff      call    launcher!CreateInterface+0x95a561 (01d5b950) == GL_Set2D


0x01d51d90 R_RenderView:
...
01d51e13 85c0            test    eax,eax
01d51e15 7505            jne     launcher!CreateInterface+0x950a2d (01d51e1c)
01d51e17 e8d4e9ffff      call    launcher!CreateInterface+0x94f401 (01d507f0)
01d51e1c e83ffeffff      call    launcher!CreateInterface+0x950871 (01d51c60) // R_Renderscene
01d51e21 a1cc27c402      mov     eax,dword ptr [launcher!CreateInterface+0x18413dd (02c427cc)]
01d51e26 85c0            test    eax,eax
...


01d51c60 R_RenderScene:
...
01d51cc2 e8d9320000      call    launcher!CreateInterface+0x953bb1 (01d54fa0) // R_MarkLeaves
01d51cc7 a1cc27c402      mov     eax,dword ptr [launcher!CreateInterface+0x18413dd (02c427cc)]
01d51ccc 85c0            test    eax,eax
01d51cce 7534            jne     launcher!CreateInterface+0x950915 (01d51d04)
01d51cd0 e82bbcfdff      call    launcher!CreateInterface+0x92c511 (01d2d900) // dev_overview related
...
01d51cf5 e826310000      call    launcher!CreateInterface+0x953a31 (01d54e20) // R_DrawWorld
01d51cfa e871270500      call    launcher!CreateInterface+0x9a3081 (01da4470)
01d51cff e8dce4ffff      call    launcher!CreateInterface+0x94edf1 (01d501e0)
01d51d04 a1c4b47a02      mov     eax,dword ptr [launcher!CreateInterface+0x13aa0d5 (027ab4c4)]
....


01d54e20 R_DrawWorld:

...
01d54f6b 83c404          add     esp,4
01d54f6e c70554e43e0200000000 mov dword ptr [launcher!CreateInterface+0xfed065 (023ee454)],0
01d54f78 e803f5ffff      call    launcher!CreateInterface+0x953091 (01d54480)
01d54f7d e8eef40400      call    launcher!CreateInterface+0x9a3081 (01da4470)
01d54f82 e87989fdff      call    launcher!CreateInterface+0x92c511 (01d2d900) // checks dev_overview and returns it I gues
01d54f87 85c0            test    eax,eax
01d54f89 750e            jne     launcher!CreateInterface+0x953baa (01d54f99)
01d54f8b 50              push    eax
01d54f8c e8cf240000      call    launcher!CreateInterface+0x956071 (01d57460) // probably ads
01d54f91 83c404          add     esp,4
01d54f94 e827efffff      call    launcher!CreateInterface+0x952ad1 (01d53ec0) // R_BlendLightmaps
01d54f99 81c4b80b0000    add     esp,0BB8h
01d54f9f c3              ret

*/

#define ADDRESS_R_DrawWorld 0x01d54e20
#define DETOURSIZE_R_DrawWorld 0x06

#define ADDRESS_UnkIGAWorld 0x01d57460
#define DETOURSIZE_UnkIGAWorld 0x05

typedef void (*R_DrawWorld_t) (void);
typedef void (*UnkIGAWorld_t) (DWORD dwUnkown1);

R_DrawWorld_t detoured_R_DrawWorld=NULL;
UnkIGAWorld_t detoured_UnkIGAWorld=NULL;

REGISTER_DEBUGCVAR(test3_bitchworld, "0", 0);
REGISTER_DEBUGCVAR(test3_bitchigaworld, "0", 0);

void touring_R_DrawWorld (void)
{
	if (test3_bitchworld->value==0.0f)detoured_R_DrawWorld(); // appearently the level ads are drawn in drawworld?
}

void touring_UnkIGAWorld (DWORD dwUnkown1)
{
	if (test3_bitchigaworld->value==0.0f)detoured_UnkIGAWorld(dwUnkown1);
}

REGISTER_DEBUGCMD_FUNC(test3_install)
{
	if (!detoured_R_DrawWorld)
	{

		detoured_R_DrawWorld = (R_DrawWorld_t) DetourApply((BYTE *)ADDRESS_R_DrawWorld, (BYTE *)touring_R_DrawWorld, (int)DETOURSIZE_R_DrawWorld);
		detoured_UnkIGAWorld = (UnkIGAWorld_t) DetourApply((BYTE *)ADDRESS_UnkIGAWorld, (BYTE *)touring_UnkIGAWorld, (int)DETOURSIZE_UnkIGAWorld);

		pEngfuncs->Con_Printf("__mirv_noadverts: tried to install hooks.\n");
	}
}