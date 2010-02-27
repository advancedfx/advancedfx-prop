#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2010-01-16 dominik.matrixtstorm.com
//
// First changes:
// 2010-01-16 dominik.matrixtstorm.com

#include <hlsdk.h>

#include "cmdregister.h"
#include "hl_addresses.h"

#include <hooks/shared/detours.h>

// >> HLSDK

#define MAX_PLAYER_NAME_LENGTH		32
#define MAX_DEATHNOTICES 4

// the structs size is 0xa0, however the fields might be in wrong oder,
// since I don't access them atm and thus did not validate em.
struct cstrike_DeathNoticeItem {
	char szKiller[MAX_PLAYER_NAME_LENGTH*2];
	char szVictim[MAX_PLAYER_NAME_LENGTH*2];
	int iId;	// the index number of the associated sprite
	int iId2; // the index number of the second sprite or -1
	int iSuicide;
	int iTeamKill;
	int iNonPlayerKill;
	float flDisplayTime;
	float *KillerColor;
	float *VictimColor;
};
// << HLSDK

extern cl_enginefuncs_s *pEngfuncs;

#include <list>

// these make sense when you look into debug_cstrike_deathmsg.txt in the docs:

#define DLEN_MsgFunc_DeathMsg 0x0C
#define DLEN_DeathMsg_Draw	0x09
#define DLEN_DeathMsg_Msg	0x0a

#define OFS_Draw_Check			0x74
#define OFS_Draw_AfterCheck		0x82
#define OFS_Draw_LoopCont		0x33A
#define OFS_Draw_LoopDone		0x370
#define OFS_Draw_YRes			0x11E
#define OFS_Draw_AfterYRes		0x130


#define OFS_Msg_Check		0x9D
#define OFS_Msg_AfterCheck	0xD7



DWORD cstrike_rgDeathNoticeList;

DWORD cstrike_DeathMsg_Draw_Check;
DWORD cstrike_DeathMsg_Draw_AfterCheck;
DWORD cstrike_DeathMsg_Draw_LoopCont;
DWORD cstrike_DeathMsg_Draw_LoopDone;
DWORD cstrike_DeathMsg_Msg_AfterCheck;
DWORD cstrike_DeathMsg_Draw_AfterYRes;


std::list<cstrike_DeathNoticeItem> cstrike_DeathNotices;
std::list<cstrike_DeathNoticeItem>::iterator cstrike_DeathNotices_it;

BOOL  cstrike_DeathMsg_Draw_First;
DWORD cstrike_DeathMsg_Draw_Item;
BYTE cstrike_DeathMsg_Draw_Mem[4*5];

typedef int (* cstrike_MsgFunc_DeathMsg_t)(const char *pszName, int iSize, void *pbuf);

typedef int (__stdcall *cstrike_DeathMsg_Draw_t)(DWORD *this_ptr, float flTime );
typedef int (__stdcall *cstrike_DeathMsg_Msg_t)(DWORD *this_ptr, const char *pszName, int iSize, void *pbuf );

cstrike_DeathMsg_Draw_t detoured_cstrike_DeathMsg_Draw;
cstrike_DeathMsg_Msg_t detoured_cstrike_DeathMsg_Msg;
cstrike_MsgFunc_DeathMsg_t detoured_cstrike_MsgFunc_DeathMsg;


struct DeathMsgFilterItem {
	BYTE attackerId;
	bool anyAttacker;
	BYTE victimId;
	bool anyVictim;
};

std::list<DeathMsgFilterItem> deathMessageFilter;

int deathMessagesMax = MAX_DEATHNOTICES;

int touring_cstrike_MsgFunc_DeathMsg(const char *pszName, int iSize, void *pbuf)
{
	if(2 <= iSize && 0 < deathMessageFilter.size()) {

		bool bBlock = false;
		for(
			std::list<DeathMsgFilterItem>::iterator it = deathMessageFilter.begin();
			it != deathMessageFilter.end() && !bBlock; it++
		) {
			DeathMsgFilterItem df = *it;

			bBlock = 
				(df.anyAttacker || df.attackerId == ((BYTE *)pbuf)[0])
				&&(df.anyVictim || df.victimId == ((BYTE *)pbuf)[1])
			;
		}
		if(bBlock)
			return 1;
	}

	return detoured_cstrike_MsgFunc_DeathMsg(pszName, iSize, pbuf);
}


void cstrike_DeathMsg_CheckRemoveItem() {
	if(0 == ((cstrike_DeathNoticeItem *)cstrike_rgDeathNoticeList)[0].iId)
	{
		// we are supposed to remove the last item.
		cstrike_DeathMsg_Draw_Item--;
		cstrike_DeathNotices_it--;

		cstrike_DeathNotices_it = cstrike_DeathNotices.erase(cstrike_DeathNotices_it);
	}
}

BOOL cstrike_DeathMsg_GetItem()
{
	// get next item (if any is left):
	if(cstrike_DeathNotices_it != cstrike_DeathNotices.end()) {
		((cstrike_DeathNoticeItem *)cstrike_rgDeathNoticeList)[0] = *cstrike_DeathNotices_it;
		//memcpy(&((cstrike_DeathNoticeItem *)cstrike_rgDeathNoticeList)[0], &(*cstrike_DeathNotices_it), sizeof(cstrike_DeathNoticeItem));
		((cstrike_DeathNoticeItem *)cstrike_rgDeathNoticeList)[1].iId = 0; // force marker

		cstrike_DeathNotices_it++;
		cstrike_DeathMsg_Draw_Item++;

		return 1; // yes we got one.
	}

	return 0; // no more items left.
}


int __stdcall touring_cstrike_DeathMsg_Draw(DWORD *this_ptr, float flTime )
{
	cstrike_DeathMsg_Draw_Item = -1;
	cstrike_DeathMsg_Draw_First = 1;
	cstrike_DeathNotices_it = cstrike_DeathNotices.begin();

	// place remove item markers:
	((cstrike_DeathNoticeItem *)cstrike_rgDeathNoticeList)[0].iId = 1; // marker in case the list is empty
	((cstrike_DeathNoticeItem *)cstrike_rgDeathNoticeList)[1].iId = 0; // marker

	int iRet = detoured_cstrike_DeathMsg_Draw(this_ptr, flTime);

	return iRet;
}

int __stdcall touring_cstrike_DeathMsg_Msg(DWORD *this_ptr, const char *pszName, int iSize, void *pbuf )
{
	int i = detoured_cstrike_DeathMsg_Msg(this_ptr, pszName, iSize, pbuf);

	if(i)
	{
		if(0 < deathMessagesMax)
		{
			// make space for new element:
			while(deathMessagesMax <= cstrike_DeathNotices.size())
				cstrike_DeathNotices.pop_front();

			// Pick up the message:
			cstrike_DeathNoticeItem di = ((cstrike_DeathNoticeItem *)cstrike_rgDeathNoticeList)[0];
			//memcpy(&di, &((cstrike_DeathNoticeItem *)cstrike_rgDeathNoticeList)[0], sizeof(cstrike_DeathNoticeItem));

			cstrike_DeathNotices.push_back(di);
		}
		else
			cstrike_DeathNotices.clear();
	}

	return i;
}

__declspec(naked) void cstrike_DeathMsg_DrawHelper()
{
	__asm {
		MOV ebp, cstrike_DeathMsg_Draw_First;
		TEST ebp, ebp
		JZ  __cont1

		MOV dword ptr cstrike_DeathMsg_Draw_Mem[4*0], esi

		MOV ebp, dword ptr [esp+0x1C]
		MOV dword ptr cstrike_DeathMsg_Draw_Mem[4*1], ebp
		MOV ebp, dword ptr [esp+0x14]
		MOV dword ptr cstrike_DeathMsg_Draw_Mem[4*2], ebp
		MOV ebp, dword ptr [esp+0x10]
		MOV dword ptr cstrike_DeathMsg_Draw_Mem[4*3], ebp
		MOV ebp, dword ptr [esp+0x34]
		MOV dword ptr cstrike_DeathMsg_Draw_Mem[4*4], ebp

		MOV cstrike_DeathMsg_Draw_First, 0

		JMP __cont2

		__cont1:
		CALL cstrike_DeathMsg_CheckRemoveItem

		__cont2:
		MOV ebp, dword ptr cstrike_DeathMsg_Draw_Mem[4*4]
		MOV dword ptr [esp+0x34], ebp
		MOV ebp, dword ptr cstrike_DeathMsg_Draw_Mem[4*3]
		MOV dword ptr [esp+0x10], ebp
		MOV ebp, dword ptr cstrike_DeathMsg_Draw_Mem[4*2]
		MOV dword ptr [esp+0x14], ebp
		MOV ebp, dword ptr cstrike_DeathMsg_Draw_Mem[4*1]
		MOV dword ptr [esp+0x1C], ebp

		MOV esi, dword ptr cstrike_DeathMsg_Draw_Mem[4*0]
		MOV ebp, 0

		PUSH eax
		CALL cstrike_DeathMsg_GetItem
		TEST eax, eax
		POP eax

		JZ __done1
		JMP [cstrike_DeathMsg_Draw_AfterCheck]

		__done1:
		JMP [cstrike_DeathMsg_Draw_LoopDone]
	}
}

__declspec(naked) void cstrike_DeathMsg_DrawHelperY() {
	__asm {
		mov  eax,dword ptr [ebx+0x18]
		mov  edx,dword ptr [esp+0x38]
		; imul eax,dword ptr [esp+0x1C]
		imul eax, cstrike_DeathMsg_Draw_Item
		add  eax,edx
		mov  dword ptr [esp+0x20],eax
		JMP [cstrike_DeathMsg_Draw_AfterYRes]
	}
}

__declspec(naked) void cstrike_DeathMsg_MsgHelper()
{
	__asm {
		MOV esi, 0
		JMP [cstrike_DeathMsg_Msg_AfterCheck]
	}
}



bool Install_cstrike_DeatMsg() {
	static bool bFirstRun = true;
	if(!bFirstRun)
		return true;

	bFirstRun = false;

	DWORD dwClientDLL = (DWORD)GetModuleHandle("client.dll");

	// 1. Fill addresses:

	cstrike_rgDeathNoticeList = dwClientDLL +(DWORD)HL_ADDR_GET(cstrike_rgDeathNoticeList);

	DWORD dwDraw = dwClientDLL +(DWORD)HL_ADDR_GET(cstrike_CHudDeathNotice_Draw);
	DWORD dwMsg = dwClientDLL +(DWORD)HL_ADDR_GET(cstrike_CHudDeathNotice_MsgFunc_DeathMsg);

	cstrike_DeathMsg_Draw_Check = dwDraw + OFS_Draw_Check;
	cstrike_DeathMsg_Draw_AfterCheck = dwDraw +OFS_Draw_AfterCheck;
	cstrike_DeathMsg_Draw_LoopCont = dwDraw +OFS_Draw_LoopCont;
	cstrike_DeathMsg_Draw_LoopDone = dwDraw +OFS_Draw_LoopDone;
	cstrike_DeathMsg_Draw_AfterYRes = dwDraw +OFS_Draw_AfterYRes;

	cstrike_DeathMsg_Msg_AfterCheck = dwMsg +OFS_Msg_AfterCheck;

	// Detour Functions:
	detoured_cstrike_MsgFunc_DeathMsg = (cstrike_MsgFunc_DeathMsg_t)DetourApply((BYTE *)((DWORD)dwClientDLL +(DWORD)HL_ADDR_GET(cstrike_MsgFunc_DeathMsg)), (BYTE *)touring_cstrike_MsgFunc_DeathMsg, DLEN_MsgFunc_DeathMsg);

	detoured_cstrike_DeathMsg_Draw = (cstrike_DeathMsg_Draw_t)DetourClassFunc((BYTE *)dwDraw, (BYTE *)touring_cstrike_DeathMsg_Draw, DLEN_DeathMsg_Draw);
	detoured_cstrike_DeathMsg_Msg = (cstrike_DeathMsg_Msg_t)DetourClassFunc((BYTE *)dwMsg, (BYTE *)touring_cstrike_DeathMsg_Msg, DLEN_DeathMsg_Msg);

	// Patch Draw fn:
	Asm32ReplaceWithJmp((void *)(dwDraw + OFS_Draw_Check), OFS_Draw_AfterCheck - OFS_Draw_Check, (void *)cstrike_DeathMsg_DrawHelper);
	Asm32ReplaceWithJmp((void *)(dwDraw + OFS_Draw_YRes), OFS_Draw_AfterYRes - OFS_Draw_YRes, (void *)cstrike_DeathMsg_DrawHelperY);

	// Patch Msg fn:
	Asm32ReplaceWithJmp((void *)(dwMsg +OFS_Msg_Check), OFS_Msg_AfterCheck - OFS_Msg_Check, (void *)cstrike_DeathMsg_MsgHelper);

	return true;
}



REGISTER_CMD_FUNC(cstrike_deathmsg)
{
	if(!Install_cstrike_DeatMsg()) {
		pEngfuncs->Con_Printf("Hook not installed.\n");
		return;
	}

	int argc = pEngfuncs->Cmd_Argc();

	if(2 <= argc) {
		char * acmd = pEngfuncs->Cmd_Argv(1);

		if(!stricmp(acmd, "block")) {
			if(4 == argc) {
				DeathMsgFilterItem di;

				acmd = pEngfuncs->Cmd_Argv(2);
				di.anyAttacker = !strcmp("*", acmd);
				if(!di.anyAttacker) di.attackerId = atoi(acmd);

				acmd = pEngfuncs->Cmd_Argv(3);
				di.anyVictim = !strcmp("*", acmd);
				if(!di.anyVictim) di.victimId = atoi(acmd);

				deathMessageFilter.push_back(di);
				return;

			}
			if(3 == argc) {
				acmd = pEngfuncs->Cmd_Argv(2);
				if(!stricmp(acmd, "list")) {
					pEngfuncs->Con_Printf("Attacker -> Victim\n");
					for(std::list<DeathMsgFilterItem>::iterator it = deathMessageFilter.begin(); it != deathMessageFilter.end(); it++) {
						DeathMsgFilterItem di = *it;
						if(di.anyAttacker) pEngfuncs->Con_Printf("*"); else pEngfuncs->Con_Printf("%i", di.attackerId);
						pEngfuncs->Con_Printf(" -> ");
						if(di.anyVictim) pEngfuncs->Con_Printf("*"); else pEngfuncs->Con_Printf("%i", di.victimId);
						pEngfuncs->Con_Printf("\n");
					}
					return;
				}
				if(!stricmp(acmd, "clear")) {
					deathMessageFilter.clear();
					return;
				}
			}
			pEngfuncs->Con_Printf(
				"mirv_cstrike_deathmsg block\n"
				"\t<attackerId> <victimId> - adds a block, use * to match any id\n"
				"\tlist - lists current blocks\n"
				"\tclear - clears all blocks\n"
			);
			return;
		}
		if(!stricmp(acmd, "fake")) {
			if(6 == argc) {
				int iA = atoi(pEngfuncs->Cmd_Argv(2));
				int iV = atoi(pEngfuncs->Cmd_Argv(3));
				int iT = atoi(pEngfuncs->Cmd_Argv(4));
				acmd = pEngfuncs->Cmd_Argv(5);
				int sz = 3*sizeof(BYTE)+sizeof(char)*(1+strlen(acmd));

				BYTE * pMem = (BYTE *)malloc(sz);
				pMem[0] = iA;
				pMem[1] = iV;
				pMem[2] = iT;
				strcpy((char *)(&pMem[3]), acmd);
				detoured_cstrike_MsgFunc_DeathMsg("DeathMsg", (int)sz, pMem);
				free(pMem);
				return;
			}
			pEngfuncs->Con_Printf(
				"mirv_cstrike_deathmsg fake <attackerId> <victimId> <0|1> <weaponString> - Use at your own risk.\n"
			);
			return;
		}
		if(!stricmp(acmd, "max")) {
			if(3==argc) {
				deathMessagesMax = atoi(pEngfuncs->Cmd_Argv(2));
		
				if(deathMessagesMax < 1) {
					deathMessagesMax = 0;
					cstrike_DeathNotices.clear();
				} else {
					while(deathMessagesMax <= cstrike_DeathNotices.size())
						cstrike_DeathNotices.pop_front();
				}
				return;
			}
			
			pEngfuncs->Con_Printf(
				"mirv_cstrike_deathmsg max <value>\n"
				"Current: %i\n",
				deathMessagesMax
			);
			return;
		}
	}

	pEngfuncs->Con_Printf(
		"mirv_cstrike_deathmsg\n"
		"\tblock - block messages\n"
		"\tfake - fake a message\n"
		"\tmax - maximum hud history row count\n"
	);
}