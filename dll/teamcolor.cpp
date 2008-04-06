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
	// clientIndex is supplied in ebx and color pointer has to be teturned in ecx
	__asm
	{
		PUSH	edx ; preserve other c++ registers
		PUSH	esi ; .
		PUSH	edi ; .

		PUSH	eax
		PUSH	ebx ; push clientIndex on stack
		CALL	New_unkInlineClientColorA
		POP		ebx
		MOV		ecx, eax ; move result back towhere it belongs
		POP		eax

		POP		edi
		POP		esi
		POP		edx

		JMP		[dwJmp_unkInlineClientColorA] ; jump back into HL code
	};
}

__declspec(naked) void tour_unkInlineClientColorV()
{
	// clientIndex is supplied in eax and color pointer has to be teturned in eax
	__asm
	{
		PUSH	ebx ; preserve other c++ registers
		PUSH	ecx ; .
		PUSH	edx ; .
		PUSH	esi ; .
		PUSH	edi ; .

		PUSH	eax ; push clientIndex on stack
		CALL	New_unkInlineClientColorV
		ADD		esp, 0x04

		POP		edi
		POP		esi
		POP		edx
		POP		ecx
		POP		ebx

		JMP		[dwJmp_unkInlineClientColorV]
	};
}

void InstallHook_GetClientColor( void )
{
	if (!g_bHooked)
	{
		g_bHooked = true;

		// prepare color struct:
		g_NewColors.defaultactive = false;
		g_NewColors.a_defaultactive = false;
		g_NewColors.v_defaultactive = false;

		// hook GetClientColor:
		if (HL_ADDR_GetClientColor!=NULL)
			g_Hooked_GetClientColor = (GetClientColor_t) DetourApply((BYTE *)HL_ADDR_GetClientColor, (BYTE *)Hooking_GetClientColor, (int)HL_ADDR_DTOURSZ_GetClientColor);

		// replace unkInlineClientColorA (Attacker frag coloring):
		if (HL_ADDR_unkInlineClientColorA != NULL)
		{
			dwJmp_unkInlineClientColorA = HL_ADDR_unkInlineClientColorA + HL_ADDR_SZ_unkInlineClientColorA;

			DWORD dwOldProt;

			VirtualProtect( (void *)HL_ADDR_unkInlineClientColorA, HL_ADDR_SZ_unkInlineClientColorA, PAGE_READWRITE, &dwOldProt);

			// make many NOPs:
			memset( (void *)HL_ADDR_unkInlineClientColorA, asmNOP, HL_ADDR_SZ_unkInlineClientColorA);

			// jmp on the naked guy:
			unsigned char ucJMPE9 = asmJMP;
			DWORD dwAddress = (DWORD)tour_unkInlineClientColorA - (DWORD)HL_ADDR_unkInlineClientColorA - JMP32_SZ;
			memcpy( (void *)HL_ADDR_unkInlineClientColorA, &ucJMPE9, sizeof(unsigned char));
			memcpy( (char *)HL_ADDR_unkInlineClientColorA+1, &dwAddress, sizeof(DWORD));

			VirtualProtect( (void *)HL_ADDR_unkInlineClientColorA, HL_ADDR_SZ_unkInlineClientColorA, dwOldProt, NULL);
		}

		// replace unkInlineClientColorV (Victim frag coloring):
		if (HL_ADDR_unkInlineClientColorV != NULL)
		{
			dwJmp_unkInlineClientColorV = HL_ADDR_unkInlineClientColorV + HL_ADDR_SZ_unkInlineClientColorV;

			DWORD dwOldProt;

			VirtualProtect( (void *)HL_ADDR_unkInlineClientColorV, HL_ADDR_SZ_unkInlineClientColorV, PAGE_READWRITE, &dwOldProt);

			// make many NOPs:
			memset( (void *)HL_ADDR_unkInlineClientColorV, asmNOP, HL_ADDR_SZ_unkInlineClientColorV);

			// jmp on the naked guy:
			unsigned char ucJMPE9 = asmJMP;
			DWORD dwAddress = (DWORD)tour_unkInlineClientColorV - (DWORD)HL_ADDR_unkInlineClientColorV - JMP32_SZ;
			memcpy( (void *)HL_ADDR_unkInlineClientColorV, &ucJMPE9, sizeof(unsigned char));
			memcpy( (char *)HL_ADDR_unkInlineClientColorV+1, &dwAddress, sizeof(DWORD));

			VirtualProtect( (void *)HL_ADDR_unkInlineClientColorV, HL_ADDR_SZ_unkInlineClientColorV, dwOldProt, NULL);
		}


	}
}


REGISTER_CMD_FUNC(playercolors)

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

g_ColorBlue:

019e662c 9a 99 19 3f cd cc 4c 3f 00 00 80 3f 00 00 80 3f 00 00 80 3e 00 00 80

0:000> s launcher 4000000 2c 66 9e 01
01945036  2c 66 9e 01 c3 b8 50 66-9e 01 c3 b8 44 66 9e 01  ,f....Pf....Df..
019455fe  2c 66 9e 01 eb 13 b9 50-66 9e 01 eb 0c b9 44 66  ,f.....Pf.....Df
01945688  2c 66 9e 01 eb 13 b8 50-66 9e 01 eb 0c b8 44 66  ,f.....Pf.....Df

01945035 b82c669e01      mov     eax,offset launcher!CreateInterface+0x5e523d (019e662c)
0194503a c3              ret

after ret:

01964776 55              push    ebp
01964777 e89408feff      call    launcher!CreateInterface+0x543c21 (01945010)
0196477c 83c40c          add     esp,0Ch


(01945036-1) --> GetClientColor function (uses in CS by HeadUp + Overview)
01945010 8b442404        mov     eax,dword ptr [esp+4]
01945014 8d0c40          lea     ecx,[eax+eax*2]
01945017 8d1488          lea     edx,[eax+ecx*4]
0194501a 0fbf04d59a99a201 movsx   eax,word ptr launcher!CreateInterface+0x6285ab (01a2999a)[edx*8]
01945022 48              dec     eax
01945023 83f803          cmp     eax,3
01945026 771f            ja      launcher!CreateInterface+0x543c58 (01945047)
01945028 ff248550509401  jmp     dword ptr launcher!CreateInterface+0x543c61 (01945050)[eax*4]
0194502f b838669e01      mov     eax,offset launcher!CreateInterface+0x5e5249 (019e6638)
01945034 c3              ret
01945035 b82c669e01      mov     eax,offset launcher!CreateInterface+0x5e523d (019e662c)
0194503a c3              ret
0194503b b850669e01      mov     eax,offset launcher!CreateInterface+0x5e5261 (019e6650)
01945040 c3              ret
01945041 b844669e01      mov     eax,offset launcher!CreateInterface+0x5e5255 (019e6644)
01945046 c3              ret
01945047 b85c669e01      mov     eax,offset launcher!CreateInterface+0x5e526d (019e665c)
0194504c c3              ret
0194504d .

(019455fe-1) --> inline function, used for attacker?
019455db 8d0c5b          lea     ecx,[ebx+ebx*2]
019455de 8d148b          lea     edx,[ebx+ecx*4]
019455e1 0fbf0cd59a99a201 movsx   ecx,word ptr launcher!CreateInterface+0x6285ab (01a2999a)[edx*8]
019455e9 49              dec     ecx
019455ea 83f903          cmp     ecx,3
019455ed 7723            ja      launcher!CreateInterface+0x544223 (01945612)
019455ef ff248d345a9401  jmp     dword ptr launcher!CreateInterface+0x544645 (01945a34)[ecx*4]
019455f6 b938669e01      mov     ecx,offset launcher!CreateInterface+0x5e5249 (019e6638)
019455fb eb1a            jmp     launcher!CreateInterface+0x544228 (01945617)
019455fd b92c669e01      mov     ecx,offset launcher!CreateInterface+0x5e523d (019e662c)
01945602 eb13            jmp     launcher!CreateInterface+0x544228 (01945617)
01945604 b950669e01      mov     ecx,offset launcher!CreateInterface+0x5e5261 (019e6650)
01945609 eb0c            jmp     launcher!CreateInterface+0x544228 (01945617)
0194560b b944669e01      mov     ecx,offset launcher!CreateInterface+0x5e5255 (019e6644)
01945610 eb05            jmp     launcher!CreateInterface+0x544228 (01945617)
01945612 b95c669e01      mov     ecx,offset launcher!CreateInterface+0x5e526d (019e665c)
01945617 .
--> playerid in ebx, return in ecx

(01945688-1) --> inline function, used for defender?
01945665 8d1440          lea     edx,[eax+eax*2]
01945668 8d0490          lea     eax,[eax+edx*4]
0194566b 0fbf04c59a99a201 movsx   eax,word ptr launcher!CreateInterface+0x6285ab (01a2999a)[eax*8]
01945673 48              dec     eax
01945674 83f803          cmp     eax,3
01945677 7723            ja      launcher!CreateInterface+0x5442ad (0194569c)
01945679 ff2485445a9401  jmp     dword ptr launcher!CreateInterface+0x544655 (01945a44)[eax*4]
01945680 b838669e01      mov     eax,offset launcher!CreateInterface+0x5e5249 (019e6638)
01945685 eb1a            jmp     launcher!CreateInterface+0x5442b2 (019456a1)
01945687 b82c669e01      mov     eax,offset launcher!CreateInterface+0x5e523d (019e662c)
0194568c eb13            jmp     launcher!CreateInterface+0x5442b2 (019456a1)
0194568e b850669e01      mov     eax,offset launcher!CreateInterface+0x5e5261 (019e6650)
01945693 eb0c            jmp     launcher!CreateInterface+0x5442b2 (019456a1)
01945695 b844669e01      mov     eax,offset launcher!CreateInterface+0x5e5255 (019e6644)
0194569a eb05            jmp     launcher!CreateInterface+0x5442b2 (019456a1)
0194569c b85c669e01      mov     eax,offset launcher!CreateInterface+0x5e526d (019e665c)
019456a1.
--> playerid in eax, return in eax

*/