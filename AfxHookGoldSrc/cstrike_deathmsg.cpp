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

#include <shared/detours.h>
#include <shared/StringTools.h>

#include <list>


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

cstrike_DeathNoticeItem * cstrike_rgDeathNoticeList;

#define OFS_Draw_YRes 0x114
#define OFS_Draw_AfterYRes (OFS_Draw_YRes +0x0A)

DWORD cstrike_DeathMsg_Draw_AfterYRes;


std::list<cstrike_DeathNoticeItem> cstrike_DeathNotices;

DWORD cstrike_DeathMsg_Draw_ItemIndex;

typedef int (* cstrike_MsgFunc_DeathMsg_t)(const char *pszName, int iSize, void *pbuf);

typedef int (__stdcall *cstrike_DeathMsg_Draw_t)(DWORD *this_ptr, float flTime );
typedef int (__stdcall *cstrike_DeathMsg_Msg_t)(DWORD *this_ptr, const char *pszName, int iSize, void *pbuf );

cstrike_DeathMsg_Draw_t detoured_cstrike_DeathMsg_Draw;
cstrike_DeathMsg_Msg_t detoured_cstrike_DeathMsg_Msg;
cstrike_MsgFunc_DeathMsg_t detoured_cstrike_MsgFunc_DeathMsg;

enum DeathMsgBlockMode
{
	DMBM_EQUAL,
	DMBM_EXCEPT,
	DMBM_ANY
};

struct DeathMsgBlockEntry
{
	BYTE attackerId;
	DeathMsgBlockMode attackerMode;
	BYTE victimId;
	DeathMsgBlockMode victimMode;
};

std::list<DeathMsgBlockEntry> deathMessageBlock;


size_t deathMessagesMax = MAX_DEATHNOTICES;

int touring_cstrike_MsgFunc_DeathMsg(const char *pszName, int iSize, void *pbuf)
{
	if(2 <= iSize && 0 < deathMessageBlock.size())
	{
		for(std::list<DeathMsgBlockEntry>::iterator it = deathMessageBlock.begin(); it != deathMessageBlock.end(); it++)
		{
			DeathMsgBlockEntry e = *it;

			bool attackerBlocked;
			switch(e.attackerMode)
			{
			case DMBM_ANY:
				attackerBlocked = true;
				break;
			case DMBM_EXCEPT:
				attackerBlocked = e.attackerId != ((BYTE *)pbuf)[0];
				break;
			case DMBM_EQUAL:
			default:
				attackerBlocked = e.attackerId == ((BYTE *)pbuf)[0];
				break;
			}

			bool victimBlocked;
			switch(e.victimMode)
			{
			case DMBM_ANY:
				victimBlocked = true;
				break;
			case DMBM_EXCEPT:
				victimBlocked = e.victimId != ((BYTE *)pbuf)[1];
				break;
			case DMBM_EQUAL:
			default:
				victimBlocked = e.victimId == ((BYTE *)pbuf)[1];
				break;
			}

			if(attackerBlocked && victimBlocked)
				return 1;
		}
	}

	return detoured_cstrike_MsgFunc_DeathMsg(pszName, iSize, pbuf);
}


int __stdcall touring_cstrike_DeathMsg_Draw(DWORD *this_ptr, float flTime )
{
	int iRet = 1;
	cstrike_DeathMsg_Draw_ItemIndex = -1;

	for(
		std::list<cstrike_DeathNoticeItem>::iterator it = cstrike_DeathNotices.begin();
		1 == iRet && it != cstrike_DeathNotices.end();
		it++
	) {
		cstrike_DeathMsg_Draw_ItemIndex++;
		cstrike_rgDeathNoticeList[0] = *it;

		iRet = detoured_cstrike_DeathMsg_Draw(this_ptr, flTime);

		// check if the message has been deleted:
		if(
			0 == cstrike_rgDeathNoticeList[0].iId
		) {
			cstrike_DeathMsg_Draw_ItemIndex--;
			it = cstrike_DeathNotices.erase(it);
		}

		cstrike_rgDeathNoticeList[0].iId = 0;
	}

	return iRet;
}

int __stdcall touring_cstrike_DeathMsg_Msg(DWORD *this_ptr, const char *pszName, int iSize, void *pbuf )
{
	for(int i=0; i<MAX_DEATHNOTICES; i++)
	{
		memset(&cstrike_rgDeathNoticeList[i], 0, sizeof(cstrike_DeathNoticeItem));
	}

	int i = detoured_cstrike_DeathMsg_Msg(this_ptr, pszName, iSize, pbuf);

	if(i)
	{
		if(0 < deathMessagesMax)
		{
			// make space for new element:
			while(deathMessagesMax <= cstrike_DeathNotices.size())
				cstrike_DeathNotices.pop_front();

			// Pick up the message:
			cstrike_DeathNoticeItem di = cstrike_rgDeathNoticeList[0];

			cstrike_DeathNotices.push_back(di);
		}
		else
			cstrike_DeathNotices.clear();
	}

	memset(&cstrike_rgDeathNoticeList[0], 0, sizeof(cstrike_DeathNoticeItem));

	return i;
}

bool g_DeathMsg_ForceOffset = false;
int g_DeathMsg_Offset;

__declspec(naked) void cstrike_DeathMsg_DrawHelperY() {
	__asm {
		mov     ebp,dword ptr [esi+18h] ; original code

		mov dl, g_DeathMsg_ForceOffset
		test dl, dl
		JZ __OrgValue

		mov edx, g_DeathMsg_Offset
		JMP __Continue

		__OrgValue:
		mov     edx,dword ptr [esp+38h] ; original code
		
		__Continue:
		; imul    ebp,ebx ; original code
		imul    ebp, cstrike_DeathMsg_Draw_ItemIndex
		
		JMP [cstrike_DeathMsg_Draw_AfterYRes]
	}
}



bool Install_cstrike_DeatMsg()
{
	static bool bFirstRun = true;
	if(!bFirstRun)
		return true;

	bFirstRun = false;

	// 1. Fill addresses:

	cstrike_rgDeathNoticeList = (cstrike_DeathNoticeItem *)HL_ADDR_GET(cstrike_rgDeathNoticeList);

	DWORD dwDraw = (DWORD)HL_ADDR_GET(cstrike_CHudDeathNotice_Draw);
	DWORD dwMsg = (DWORD)HL_ADDR_GET(cstrike_CHudDeathNotice_MsgFunc_DeathMsg);

	cstrike_DeathMsg_Draw_AfterYRes = dwDraw +OFS_Draw_AfterYRes;

	// Detour Functions:
	detoured_cstrike_MsgFunc_DeathMsg = (cstrike_MsgFunc_DeathMsg_t)DetourApply((BYTE *)HL_ADDR_GET(cstrike_MsgFunc_DeathMsg), (BYTE *)touring_cstrike_MsgFunc_DeathMsg, (int)HL_ADDR_GET(cstrike_MsgFunc_DeathMsg_DSZ));

	detoured_cstrike_DeathMsg_Draw = (cstrike_DeathMsg_Draw_t)DetourClassFunc((BYTE *)dwDraw, (BYTE *)touring_cstrike_DeathMsg_Draw, (int)HL_ADDR_GET(cstrike_CHudDeathNotice_Draw_DSZ));
	detoured_cstrike_DeathMsg_Msg = (cstrike_DeathMsg_Msg_t)DetourClassFunc((BYTE *)dwMsg, (BYTE *)touring_cstrike_DeathMsg_Msg, (int)HL_ADDR_GET(cstrike_CHudDeathNotice_MsgFunc_DeathMsg_DSZ));

	// Patch Draw fn:
	Asm32ReplaceWithJmp((void *)(dwDraw + OFS_Draw_YRes), OFS_Draw_AfterYRes - OFS_Draw_YRes, (void *)cstrike_DeathMsg_DrawHelperY);

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

		if(!_stricmp(acmd, "block")) {
			if(4 == argc)
			{
				int attackerId = -1;
				int victimId = -1;

				acmd = pEngfuncs->Cmd_Argv(2);
				bool anyAttacker = !strcmp("*", acmd);
				bool notAttacker = StringBeginsWith(acmd, "!");
				if(!anyAttacker) attackerId = atoi(notAttacker ? (acmd +1) : acmd);

				acmd = pEngfuncs->Cmd_Argv(3);
				bool anyVictim = !strcmp("*", acmd);
				bool notVictim = StringBeginsWith(acmd, "!");
				if(!anyVictim) victimId = atoi(notVictim ? (acmd +1) : acmd);

				DeathMsgBlockEntry entry = {
					attackerId,
					anyAttacker ? DMBM_ANY : (notAttacker ? DMBM_EXCEPT : DMBM_EQUAL),
					victimId,
					anyVictim ? DMBM_ANY : (notVictim ? DMBM_EXCEPT : DMBM_EQUAL)
				};

				deathMessageBlock.push_back(entry);

				return;
			}
			if(3 == argc)
			{
				acmd = pEngfuncs->Cmd_Argv(2);
				if(!_stricmp(acmd, "list"))
				{
					pEngfuncs->Con_Printf("Attacker -> Victim\n");
					for(std::list<DeathMsgBlockEntry>::iterator it = deathMessageBlock.begin(); it != deathMessageBlock.end(); it++)
					{
						DeathMsgBlockEntry e = *it;
						
						if(DMBM_ANY == e.attackerMode)
							pEngfuncs->Con_Printf("*");
						else
						{
							if(DMBM_EXCEPT == e.attackerMode)
							{
								pEngfuncs->Con_Printf("!");
							}
							pEngfuncs->Con_Printf("%i", e.attackerId);
						}
						pEngfuncs->Con_Printf(" -> ");
						if(DMBM_ANY == e.victimMode)
							pEngfuncs->Con_Printf("*");
						else
						{
							if(DMBM_EXCEPT == e.victimMode)
							{
								pEngfuncs->Con_Printf("!");
							}
							pEngfuncs->Con_Printf("%i", e.victimId);
						}
						pEngfuncs->Con_Printf("\n");
					}
					return;
				}
				if(!_stricmp(acmd, "clear")) {
					deathMessageBlock.clear();
					return;
				}
			}
			pEngfuncs->Con_Printf(
				"mirv_cstrike_deathmsg block\n"
				"\t<attackerId> <victimId> - adds a block, use * to match any id, use !x to match any id apart from x\n"
				"\tlist - lists current blocks\n"
				"\tclear - clears all blocks\n"
			);
			return;
		}
		if(!_stricmp(acmd, "fake")) {
			if(6 == argc) {
				int iA = atoi(pEngfuncs->Cmd_Argv(2));
				int iV = atoi(pEngfuncs->Cmd_Argv(3));
				int iT = atoi(pEngfuncs->Cmd_Argv(4));
				acmd = pEngfuncs->Cmd_Argv(5);
				size_t sz = 3*sizeof(BYTE)+sizeof(char)*(1+strlen(acmd));

				BYTE * pMem = (BYTE *)malloc(sz);
				pMem[0] = iA;
				pMem[1] = iV;
				pMem[2] = iT;
				strcpy_s((char *)(&pMem[3]), sz-3, acmd);
				detoured_cstrike_MsgFunc_DeathMsg("DeathMsg", (int)sz, pMem);
				free(pMem);
				return;
			}
			pEngfuncs->Con_Printf(
				"mirv_cstrike_deathmsg fake <attackerId> <victimId> <0|1> <weaponString> - Use at your own risk.\n"
			);
			return;
		}
		if(!_stricmp(acmd, "max")) {
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
		if(!_stricmp(acmd, "offset"))
		{
			if(3==argc)
			{
				acmd = pEngfuncs->Cmd_Argv(2);
				g_DeathMsg_ForceOffset = 0 != _stricmp(acmd, "default");
				if(g_DeathMsg_ForceOffset) g_DeathMsg_Offset = atoi(acmd);
				return;
			}

			pEngfuncs->Con_Printf(
				"mirv_cstrike_deathmsg offset default|<value>\n"
				"If you want the same height like in in-eye demos for HLTV demos use 40 as value.\n"
			);
			if(!g_DeathMsg_ForceOffset)
				pEngfuncs->Con_Printf("Current: default\n");
			else
				pEngfuncs->Con_Printf("Current: %i\n", g_DeathMsg_Offset);

			return;
		}
	}

	pEngfuncs->Con_Printf(
		"mirv_cstrike_deathmsg\n"
		"\tblock - block messages\n"
		"\tfake - fake a message\n"
		"\tmax - maximum hud history row count\n"
		"\toffset - set death message screen offset\n"
	);
}