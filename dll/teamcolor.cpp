// BEGIN HLSDK includes
//
// HACK: prevent cldll_int.h from messing the HSPRITE definition,
// HLSDK's HSPRITE --> MDTHACKED_HSPRITE
#pragma push_macro("HSPRITE")
#define HSPRITE MDTHACKED_HSPRITE
//
#include <wrect.h>
#include <cl_dll.h>
#include <cdll_int.h>
#include <r_efx.h>
#include <com_model.h>
#include <r_studioint.h>
#include <pm_defs.h>
#include <cvardef.h>
#include <entity_types.h>
//
#undef HSPRITE
#pragma pop_macro("HSPRITE")
// END HLSDK includes

#include "cmdregister.h"

#include "hl_addresses.h"
#include "detours.h"

#include <list>

extern cl_enginefuncs_s *pEngfuncs;


// asm related definitons we will use:
#define asmNOP 0x90 // opcode for NOP
#define asmJMP	0xE9 // opcode for JUMP
#define JMP32_SZ 5	// the size of JMP <address>


bool g_bHooked=false;

typedef float *(*GetClientColor_t)( int clientIndex );

GetClientColor_t g_Hooked_GetClientColor = NULL;

struct NewColorEntry_s
{
	int id;
	float rgb[3];
};

struct g_NewColors_s
{
	std::list<NewColorEntry_s> colorarray; // GetClientColor function
	float defaultrgb[3];
	bool defaultactive;
	std::list<NewColorEntry_s> a_colorarray; // inline ClientColor Attacker
	float a_defaultrgb[3];
	bool a_defaultactive;
	std::list<NewColorEntry_s> v_colorarray; // inline ClientColor Victim
	float v_defaultrgb[3];
	bool v_defaultactive;
} g_NewColors;

void AddReplaceListEntry( std::list<NewColorEntry_s> *pColorAray, NewColorEntry_s* newentry )
{
	static NewColorEntry_s s_entry;
	int iIndex = newentry->id;
	bool bfound = false;
	std::list<NewColorEntry_s>::iterator iterend = pColorAray->end();
	for (std::list<NewColorEntry_s>::iterator iter = pColorAray->begin(); iter != iterend; iter++)
	{
		if (iter->id == iIndex)
		{
			bfound=true;
			*iter = *newentry;
			break;
		}
	}
	if (!bfound) pColorAray->push_front(*newentry);
}

bool DeleteListEntry( std::list<NewColorEntry_s> *pColorAray, int iIndex )
{
	static NewColorEntry_s s_entry;
	bool bfound = false;
	std::list<NewColorEntry_s>::iterator iterend = pColorAray->end();
	for (std::list<NewColorEntry_s>::iterator iter = pColorAray->begin(); iter != iterend; iter++)
	{
		if (iter->id == iIndex)
		{
			bfound=true;
			pColorAray->erase(iter);
			break;
		}
	}
	return bfound;
}

NewColorEntry_s *GetListEntry( std::list<NewColorEntry_s> *pColorAray, int iIndex )
// reutrns a pointer onto a copy!
{
	static NewColorEntry_s s_entry;
	std::list<NewColorEntry_s>::iterator iterend = pColorAray->end();
	for (std::list<NewColorEntry_s>::iterator iter = pColorAray->begin(); iter != iterend; iter++)
	{
		if (iter->id == iIndex)
		{
			s_entry = *iter;
			return &s_entry;
		}
	}

	return NULL;
}


float testrgb[3]={0.0f,1.0f,1.0f};

float *Hooking_GetClientColor( int clientIndex )
{
	NewColorEntry_s *tentry=GetListEntry(&(g_NewColors.colorarray),clientIndex);
	if (tentry)
		return tentry->rgb;
	else if(g_NewColors.defaultactive)
		return g_NewColors.defaultrgb;

	return g_Hooked_GetClientColor (clientIndex );
}

float * New_unkInlineClientColorA( int clientIndex )
{
	NewColorEntry_s *tentry=GetListEntry(&(g_NewColors.a_colorarray),clientIndex);
	if (tentry)
		return tentry->rgb;
	else if(g_NewColors.a_defaultactive)
		return g_NewColors.a_defaultrgb;

	return g_Hooked_GetClientColor (clientIndex );
}

float * New_unkInlineClientColorV( int clientIndex )
{
	NewColorEntry_s *tentry=GetListEntry(&(g_NewColors.v_colorarray),clientIndex);
	if (tentry)
		return tentry->rgb;
	else if(g_NewColors.v_defaultactive)
		return g_NewColors.v_defaultrgb;

	return g_Hooked_GetClientColor (clientIndex );
}

DWORD dwJmp_unkInlineClientColorA=0;
DWORD dwJmp_unkInlineClientColorV=0;

__declspec(naked) void tour_unkInlineClientColorA()
{
	// clientIndex is supplied in ebp and color pointer has to be teturned in ecx
	__asm
	{
		; preserve other c++ registers:
		; PUSH	eax
		PUSH	ebx
		; PUSH	ecx
		PUSH	edx
		PUSH	esi
		; PUSH	edi

		PUSH	eax
		PUSH	edi ; push clientIndex on stack
		CALL	New_unkInlineClientColorA
		POP		edi
		MOV		ecx, eax ; move result back to where it belongs
		POP		eax

		POP		esi
		POP		edx
		POP		ebx

		JMP		[dwJmp_unkInlineClientColorA] ; jump back into HL code
	};
}

__declspec(naked) void tour_unkInlineClientColorV()
{
	// clientIndex is supplied in ebx and color pointer has to be teturned in eax
	__asm
	{
		; preserve other c++ registers:
		; PUSH	eax
		; PUSH	ebx
		PUSH	ecx
		PUSH	edx
		PUSH	esi
		PUSH	edi

		PUSH	ebx ; push clientIndex on stack
		CALL	New_unkInlineClientColorV
		ADD		esp, 0x04

		POP		edi
		POP		esi
		POP		edx
		POP		ecx

		JMP		[dwJmp_unkInlineClientColorV]
	};
}

void InstallHook_GetClientColor( void )
{
	if (!g_bHooked)
	{

		const char *gamedir = pEngfuncs->pfnGetGameDirectory();
		DWORD dwClientDLL = NULL;

		if( !strcmp("cstrike",gamedir) )
		{
			dwClientDLL = (DWORD)GetModuleHandle("client.dll");
			pEngfuncs->Con_DPrintf("0x%08x\n",dwClientDLL);
		}
		else
		{
			pEngfuncs->Con_Printf( "Sorry, your mod (%s) is not supported for this command.\n",	gamedir );
			return; // quit
		}

		g_bHooked = true;

		int i_ok_cnt = 0;

		// prepare color struct:
		g_NewColors.defaultactive = false;
		g_NewColors.a_defaultactive = false;
		g_NewColors.v_defaultactive = false;

		// hook GetClientColor:
		if (HL_ADDR_GetClientColor!=NULL)
		{
			g_Hooked_GetClientColor = (GetClientColor_t) DetourApply((BYTE *)(dwClientDLL + HL_ADDR_GetClientColor), (BYTE *)Hooking_GetClientColor, (int)HL_ADDR_DTOURSZ_GetClientColor);
			i_ok_cnt++;
		}

		// replace unkInlineClientColorA (Attacker frag coloring):
		if (HL_ADDR_unkInlineClientColorA != NULL)
		{
			dwJmp_unkInlineClientColorA = (dwClientDLL + HL_ADDR_unkInlineClientColorA) + HL_ADDR_SZ_unkInlineClientColorA;

			DWORD dwOldProt;

			VirtualProtect( (void *)(dwClientDLL + HL_ADDR_unkInlineClientColorA), HL_ADDR_SZ_unkInlineClientColorA, PAGE_READWRITE, &dwOldProt);

			// make many NOPs:
			memset( (void *)(dwClientDLL + HL_ADDR_unkInlineClientColorA), asmNOP, HL_ADDR_SZ_unkInlineClientColorA);

			// jmp on the naked guy:
			unsigned char ucJMPE9 = asmJMP;
			DWORD dwAddress = (DWORD)tour_unkInlineClientColorA - (DWORD)(dwClientDLL + HL_ADDR_unkInlineClientColorA) - JMP32_SZ;
			memcpy( (void *)(dwClientDLL + HL_ADDR_unkInlineClientColorA), &ucJMPE9, sizeof(unsigned char));
			memcpy( (char *)(dwClientDLL + HL_ADDR_unkInlineClientColorA)+1, &dwAddress, sizeof(DWORD));

			VirtualProtect( (void *)(dwClientDLL + HL_ADDR_unkInlineClientColorA), HL_ADDR_SZ_unkInlineClientColorA, dwOldProt, NULL);

			i_ok_cnt++;
		}

		// replace unkInlineClientColorV (Victim frag coloring):
		if (HL_ADDR_unkInlineClientColorV != NULL)
		{
			dwJmp_unkInlineClientColorV = (dwClientDLL + HL_ADDR_unkInlineClientColorV) + HL_ADDR_SZ_unkInlineClientColorV;

			DWORD dwOldProt;

			VirtualProtect( (void *)(dwClientDLL + HL_ADDR_unkInlineClientColorV), HL_ADDR_SZ_unkInlineClientColorV, PAGE_READWRITE, &dwOldProt);

			// make many NOPs:
			memset( (void *)(dwClientDLL + HL_ADDR_unkInlineClientColorV), asmNOP, HL_ADDR_SZ_unkInlineClientColorV);

			// jmp on the naked guy:
			unsigned char ucJMPE9 = asmJMP;
			DWORD dwAddress = (DWORD)tour_unkInlineClientColorV - (DWORD)(dwClientDLL + HL_ADDR_unkInlineClientColorV) - JMP32_SZ;
			memcpy( (void *)(dwClientDLL + HL_ADDR_unkInlineClientColorV), &ucJMPE9, sizeof(unsigned char));
			memcpy( (char *)(dwClientDLL + HL_ADDR_unkInlineClientColorV)+1, &dwAddress, sizeof(DWORD));

			VirtualProtect( (void *)(dwClientDLL + HL_ADDR_unkInlineClientColorV), HL_ADDR_SZ_unkInlineClientColorV, dwOldProt, NULL);

			i_ok_cnt++;
		}

		if( i_ok_cnt != 3 )
		{
			pEngfuncs->Con_Printf("WARNING: only %i of %i hooks were enabled.\n" );
		}
	}
}


REGISTER_CMD_FUNC(playercolors_cs)
{
	InstallHook_GetClientColor();
	bool bShowHelp=true;
	
	int icarg=pEngfuncs->Cmd_Argc();

	if (icarg>=4)
	{
		char *pattacker = pEngfuncs->Cmd_Argv(1);

		bool bTGeneral = strchr(pattacker,'g');
		bool bTAttacker = strchr(pattacker,'a');
		bool bTVictim = strchr(pattacker,'v');

		if (bTGeneral || bTAttacker || bTVictim)
		{

			char *psub = pEngfuncs->Cmd_Argv(2);

			if (!lstrcmp(psub,"show"))
			{
				bShowHelp=false;

				int iid = atoi(pEngfuncs->Cmd_Argv(3));
				float *ptfloat;

				if (bTGeneral)
				{
					NewColorEntry_s *tentry=GetListEntry(&(g_NewColors.colorarray),iid);
					if (tentry)
						pEngfuncs->Con_Printf("%i (g, hook): %f, %f, %f\n",iid,tentry->rgb[0],tentry->rgb[1],tentry->rgb[2]);
					else if(g_NewColors.defaultactive)
						pEngfuncs->Con_Printf("%i (g, hook default): %f, %f, %f\n",iid,g_NewColors.defaultrgb[0],g_NewColors.defaultrgb[1],g_NewColors.defaultrgb[2]);
					else if(g_Hooked_GetClientColor)
					{
						ptfloat = g_Hooked_GetClientColor(iid);
						pEngfuncs->Con_Printf("%i (g, game): %f, %f, %f\n",iid,ptfloat[0],ptfloat[1],ptfloat[2]);
					}
				} //bTGenreal

				if (bTAttacker)
				{
					NewColorEntry_s *tentry=GetListEntry(&(g_NewColors.a_colorarray),iid);
					if (tentry)
						pEngfuncs->Con_Printf("%i (a, hook): %f, %f, %f\n",iid,tentry->rgb[0],tentry->rgb[1],tentry->rgb[2]);
					else if(g_NewColors.defaultactive)
						pEngfuncs->Con_Printf("%i (a, hook default): %f, %f, %f\n",iid,g_NewColors.a_defaultrgb[0],g_NewColors.a_defaultrgb[1],g_NewColors.a_defaultrgb[2]);
					else if(g_Hooked_GetClientColor)
					{
						ptfloat = g_Hooked_GetClientColor(iid);
						pEngfuncs->Con_Printf("%i (a, game): %f, %f, %f\n",iid,ptfloat[0],ptfloat[1],ptfloat[2]);
					}
				} //bTAttacker

				if (bTVictim)
				{
					NewColorEntry_s *tentry=GetListEntry(&(g_NewColors.v_colorarray),iid);
					if (tentry)
						pEngfuncs->Con_Printf("%i (v, hook): %f, %f, %f\n",iid,tentry->rgb[0],tentry->rgb[1],tentry->rgb[2]);
					else if(g_NewColors.defaultactive)
						pEngfuncs->Con_Printf("%i (v, hook default): %f, %f, %f\n",iid,g_NewColors.v_defaultrgb[0],g_NewColors.v_defaultrgb[1],g_NewColors.v_defaultrgb[2]);
					else if(g_Hooked_GetClientColor)
					{
						ptfloat = g_Hooked_GetClientColor(iid);
						pEngfuncs->Con_Printf("%i (v, game): %f, %f, %f\n",iid,ptfloat[0],ptfloat[1],ptfloat[2]);
					}
				} //bTVictim

			}
			else if (!lstrcmp(psub,"hook")&&icarg>=7)
			{
				bShowHelp=false;

				int iid=0;
				float fr=atof(pEngfuncs->Cmd_Argv(4));
				float fg=atof(pEngfuncs->Cmd_Argv(5));
				float fb=atof(pEngfuncs->Cmd_Argv(6));

				if (!lstrcmp(pEngfuncs->Cmd_Argv(3),"default"))
				{
					if (bTGeneral)
					{
						g_NewColors.defaultrgb[0]=fr;
						g_NewColors.defaultrgb[1]=fg;
						g_NewColors.defaultrgb[2]=fb;
						g_NewColors.defaultactive=true;
					}
					if (bTAttacker)
					{
						g_NewColors.a_defaultrgb[0]=fr;
						g_NewColors.a_defaultrgb[1]=fg;
						g_NewColors.a_defaultrgb[2]=fb;
						g_NewColors.a_defaultactive=true;
					}
					if (bTVictim)
					{
						g_NewColors.v_defaultrgb[0]=fr;
						g_NewColors.v_defaultrgb[1]=fg;
						g_NewColors.v_defaultrgb[2]=fb;
						g_NewColors.v_defaultactive=true;
					}
				} else {
					iid = atoi(pEngfuncs->Cmd_Argv(3));

					NewColorEntry_s tnewentry;
					tnewentry.id = iid;
					tnewentry.rgb[0]=fr;
					tnewentry.rgb[1]=fg;
					tnewentry.rgb[2]=fb;
					
					if (bTGeneral)
						AddReplaceListEntry(&(g_NewColors.colorarray),&tnewentry);
					if (bTAttacker)
						AddReplaceListEntry(&(g_NewColors.a_colorarray),&tnewentry);
					if (bTVictim)
						AddReplaceListEntry(&(g_NewColors.v_colorarray),&tnewentry);

				}

			}
			else if (!lstrcmp(psub,"reset"))
			{
				bShowHelp=false;

				char *psub2 = pEngfuncs->Cmd_Argv(3);

				if (!lstrcmp(psub2,"all"))
				{
					if (bTGeneral)
					{
						g_NewColors.colorarray.clear();
						g_NewColors.defaultactive = false;
					}
					if (bTAttacker)
					{
						g_NewColors.a_colorarray.clear();
						g_NewColors.a_defaultactive = false;
					}
					if (bTVictim)
					{
						g_NewColors.v_colorarray.clear();
						g_NewColors.v_defaultactive = false;
					}
				}
				else if (!lstrcmp(psub2,"default"))
				{
					if (bTGeneral)
						g_NewColors.defaultactive = false;
					if (bTAttacker)
						g_NewColors.a_defaultactive = false;
					if (bTVictim)
						g_NewColors.v_defaultactive = false;
				} else {
					int iid=atoi(psub2);
					
					if (bTGeneral)
						if (!DeleteListEntry(&(g_NewColors.colorarray),iid))
							pEngfuncs->Con_Printf("g: %i not hooked.\n",iid);
					if (bTAttacker)
						if (!DeleteListEntry(&(g_NewColors.a_colorarray),iid))
							pEngfuncs->Con_Printf("a: %i not hooked.\n",iid);
					if (bTVictim)
						if (!DeleteListEntry(&(g_NewColors.v_colorarray),iid))
							pEngfuncs->Con_Printf("v: %i not hooked.\n",iid);
				}
			}
		} // if (bTGeneral || bTAttacker || bTVictim)
	}

	if (bShowHelp)
	{
		pEngfuncs->Con_Printf(
			"This command only works in cstrike.\n"
			"\n"
			"Colors: <Red>, <Green> and <Blue> are floating point values between 0.0 to 1.0\n"
			"\n"
			"Targets (insert for <traget> in the commands):\n"
			"\tg - general text (playername head-up, map overview)\n"
			"\ta - attacker text of the deathnotice (frags)\n"
			"\tv - victim text of the deathnotice (frags)\n"
			"You can address multiple targets at once by specfiying more than one letter: i,.e. gav would address all at once.\n"
			"\n"
			"Commands:\n"
			"\t" PREFIX "playercolors <target> show <playerid> - shows current setting\n"
			"\t" PREFIX "playercolors <target> hook <playerid> <Red> <Green> <Blue> - forces a color for a player\n"
			"\t" PREFIX "playercolors <target> hook default <Red> <Green> <Blue> - forces a color for all non-hooked player ids\n"
			"\t" PREFIX "playercolors <target> reset <playerid> - restores the game's setting for this player\n"
			"\t" PREFIX "playercolors <target> reset default - restores the game's setting for non-hooked player id's\n"
			"\t" PREFIX "playercolors <target> reset all - cleans up and resets everything\n"
			"You can use __mirv_listplayers to look up the id of a player.\n"
			"\n"
			"For more info see HLAE wiki.\n"
		);
	}
}
/*
hlsdk cl_dll death.cpp:
...
float g_ColorBlue[3]	= { 0.6, 0.8, 1.0 };
float g_ColorRed[3]		= { 1.0, 0.25, 0.25 };
float g_ColorGreen[3]	= { 0.6, 1.0, 0.6 };
float g_ColorYellow[3]	= { 1.0, 0.7, 0.0 };
float g_ColorGrey[3]	= { 0.8, 0.8, 0.8 };

float *GetClientColor( int clientIndex )
{
	switch ( g_PlayerExtraInfo[clientIndex].teamnumber )
	{
	case 1:	return g_ColorBlue;
	case 2: return g_ColorRed;
	case 3: return g_ColorYellow;
	case 4: return g_ColorGreen;
	case 0: return g_ColorYellow;

		default	: return g_ColorGrey;
	}

	return NULL;
}


----

client.dll: 0x0D4A0000

g_ColorBlue:

0d5877a4 9a 99 19 3f cd cc 4c 3f 00 00 80 3f 00 00 80 3f 00 00 80 3e 00 00 80

0:016> s client L1000000 a4 77 58 0d
0d4e5cf6  a4 77 58 0d c3 b8 c8 77-58 0d c3 b8 bc 77 58 0d  .wX....wX....wX.
0d4e62bf  a4 77 58 0d eb 13 b9 c8-77 58 0d eb 0c b9 bc 77  .wX.....wX.....w
0d4e6343  a4 77 58 0d eb 13 b8 c8-77 58 0d eb 0c b8 bc 77  .wX.....wX.....w

01945035 b82c669e01      mov     eax,offset launcher!CreateInterface+0x5e523d (019e662c)
0194503a c3              ret

after ret:

0d54a1dc 50              push    eax
0d54a1dd e8eebaf9ff      call    client!F+0x950 (0d4e5cd0)
0d54a1e2 8bf0            mov     esi,eax


0d4e5cd0 --> GetClientColor function (uses in CS by HeadUp + Overview)

0d4e5cd0 8b442404        mov     eax,dword ptr [esp+4]
0d4e5cd4 8d0c40          lea     ecx,[eax+eax*2]
0d4e5cd7 8d1488          lea     edx,[eax+ecx*4]
0d4e5cda 0fbf04d50ab35c0d movsx   eax,word ptr client!V_CalcRefdef+0xbcdba (0d5cb30a)[edx*8]
0d4e5ce2 48              dec     eax
0d4e5ce3 83f803          cmp     eax,3
0d4e5ce6 771f            ja      client!F+0x987 (0d4e5d07)
0d4e5ce8 ff2485105d4e0d  jmp     dword ptr client!F+0x990 (0d4e5d10)[eax*4]

...

(0d4e62bf) --> used by inline function, used for attacker:

0d4e629a 8d546d00        lea     edx,[ebp+ebp*2]
0d4e629e 8d4c9500        lea     ecx,[ebp+edx*4]
0d4e62a2 0fbf0ccd0ab35c0d movsx   ecx,word ptr client!V_CalcRefdef+0xbcdba (0d5cb30a)[ecx*8]
0d4e62aa 49              dec     ecx
0d4e62ab 83f903          cmp     ecx,3
0d4e62ae 7723            ja      client!F+0xf53 (0d4e62d3)
0d4e62b0 ff248df8664e0d  jmp     dword ptr client!F+0x1378 (0d4e66f8)[ecx*4]
0d4e62b7 b9b077580d      mov     ecx,offset client!V_CalcRefdef+0x79260 (0d5877b0)
0d4e62bc eb1a            jmp     client!F+0xf58 (0d4e62d8)
0d4e62be b9a477580d      mov     ecx,offset client!V_CalcRefdef+0x79254 (0d5877a4)
0d4e62c3 eb13            jmp     client!F+0xf58 (0d4e62d8)
0d4e62c5 b9c877580d      mov     ecx,offset client!V_CalcRefdef+0x79278 (0d5877c8)
0d4e62ca eb0c            jmp     client!F+0xf58 (0d4e62d8)
0d4e62cc b9bc77580d      mov     ecx,offset client!V_CalcRefdef+0x7926c (0d5877bc)
0d4e62d1 eb05            jmp     client!F+0xf58 (0d4e62d8)
0d4e62d3 b9d477580d      mov     ecx,offset client!V_CalcRefdef+0x79284 (0d5877d4)
0d4e62d8 .

--> playerid in ebp, return in ecx
--> we will detour this full block


(0d54a1e2) --> used by inline function, used for defender?

0d4e6320 8d045b          lea     eax,[ebx+ebx*2]
0d4e6323 8d1483          lea     edx,[ebx+eax*4]
0d4e6326 0fbf04d50ab35c0d movsx   eax,word ptr client!V_CalcRefdef+0xbcdba (0d5cb30a)[edx*8]
0d4e632e 48              dec     eax
0d4e632f 83f803          cmp     eax,3
0d4e6332 7723            ja      client!F+0xfd7 (0d4e6357)
0d4e6334 ff248508674e0d  jmp     dword ptr client!F+0x1388 (0d4e6708)[eax*4]
0d4e633b b8b077580d      mov     eax,offset client!V_CalcRefdef+0x79260 (0d5877b0)
0d4e6340 eb1a            jmp     client!F+0xfdc (0d4e635c)
0d4e6342 b8a477580d      mov     eax,offset client!V_CalcRefdef+0x79254 (0d5877a4)
0d4e6347 eb13            jmp     client!F+0xfdc (0d4e635c)
0d4e6349 b8c877580d      mov     eax,offset client!V_CalcRefdef+0x79278 (0d5877c8)
0d4e634e eb0c            jmp     client!F+0xfdc (0d4e635c)
0d4e6350 b8bc77580d      mov     eax,offset client!V_CalcRefdef+0x7926c (0d5877bc)
0d4e6355 eb05            jmp     client!F+0xfdc (0d4e635c)
0d4e6357 b8d477580d      mov     eax,offset client!V_CalcRefdef+0x79284 (0d5877d4)
0d4e635c .

--> playerid in ebx, return in eax
--> we will detour this full block

*/