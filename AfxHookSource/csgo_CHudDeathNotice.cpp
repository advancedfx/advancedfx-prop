#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2014-10-21 by dominik.matrixstorm.com
//
// First changes:
// 2014-10-21 dominik.matrixstorm.com

#include "csgo_CHudDeathNotice.h"

#include "addresses.h"
#include "SourceInterfaces.h"

#include <shared/detours.h>
#include <shared/StringTools.h>

#include <list>

class csgo_IGameEvent abstract
{
public:
	virtual ~csgo_IGameEvent() {};
	virtual const char *GetName() const = 0;	// get event name
	
	virtual bool IsReliable() const = 0; // if event handled reliable
	virtual bool IsLocal() const = 0; // if event is never networked
	virtual bool IsEmpty(const char *keyName = NULL) = 0; // check if data field exists
	
	// Data access
	virtual bool GetBool( const char *keyName = NULL, bool defaultValue = false ) = 0;
	virtual int GetInt( const char *keyName = NULL, int defaultValue = 0 ) = 0;
	virtual float GetFloat( const char *keyName = NULL, float defaultValue = 0.0f ) = 0;
	virtual void __unkown_008(void);
	virtual const char *GetString( const char *keyName = NULL, const char *defaultValue = "" ) = 0;
	
	virtual void SetBool( const char *keyName, bool value ) = 0;
	virtual void SetInt( const char *keyName, int value ) = 0;
	virtual void SetFloat( const char *keyName, float value ) = 0;
	virtual void __unkown_013(void);
	virtual void SetString( const char *keyName, const char *value ) = 0;
};


typedef void (__stdcall *csgo_CHudDeathNotice_FireGameEvent_t)(DWORD *this_ptr, csgo_IGameEvent * event);

csgo_CHudDeathNotice_FireGameEvent_t detoured_csgo_CHudDeathNotice_FireGameEvent;

int csgo_debug_CHudDeathNotice_FireGameEvent = 0;

enum DeathMsgBlockMode
{
	DMBM_EQUAL,
	DMBM_EXCEPT,
	DMBM_ANY
};

struct DeathMsgBlockEntry
{
	int attackerId;
	DeathMsgBlockMode attackerMode;
	int victimId;
	DeathMsgBlockMode victimMode;
	int assisterId;
	DeathMsgBlockMode assisterMode;
};

std::list<DeathMsgBlockEntry> deathMessageBlock;

float csgo_CHudDeathNotice_nScrollInTime = -1;
float csgo_CHudDeathNotice_nFadeOutTime = -1;
float csgo_CHudDeathNotice_nNoticeLifeTime = -1;
float csgo_CHudDeathNotice_nLocalPlayerLifeTimeMod = -1;

float org_CHudDeathNotice_nScrollInTime;
float org_CHudDeathNotice_nFadeOutTime;
float org_CHudDeathNotice_nNoticeLifeTime;
float org_CHudDeathNotice_nLocalPlayerLifeTimeMod;

void __stdcall touring_csgo_CHudDeathNotice_FireGameEvent(DWORD *this_ptr, csgo_IGameEvent * event)
{
	static bool firstRun = true;
	if(firstRun)
	{
		firstRun = false;
		org_CHudDeathNotice_nScrollInTime = *(float *)((BYTE *)this_ptr+0x5c);
		org_CHudDeathNotice_nFadeOutTime = *(float *)((BYTE *)this_ptr+0x58);
		org_CHudDeathNotice_nNoticeLifeTime = *(float *)((BYTE *)this_ptr+0x54);
		org_CHudDeathNotice_nLocalPlayerLifeTimeMod = *(float *)((BYTE *)this_ptr+0x64);
	}

	*(float *)((BYTE *)this_ptr+0x5c) = 0 <= csgo_CHudDeathNotice_nScrollInTime ? csgo_CHudDeathNotice_nScrollInTime : org_CHudDeathNotice_nScrollInTime;
	*(float *)((BYTE *)this_ptr+0x58) = 0 <= csgo_CHudDeathNotice_nFadeOutTime ? csgo_CHudDeathNotice_nFadeOutTime : org_CHudDeathNotice_nScrollInTime;
	*(float *)((BYTE *)this_ptr+0x54) = 0 <= csgo_CHudDeathNotice_nNoticeLifeTime ? csgo_CHudDeathNotice_nNoticeLifeTime : org_CHudDeathNotice_nScrollInTime;
	*(float *)((BYTE *)this_ptr+0x64) = 0 <= csgo_CHudDeathNotice_nLocalPlayerLifeTimeMod ? csgo_CHudDeathNotice_nLocalPlayerLifeTimeMod : org_CHudDeathNotice_nScrollInTime;
	
	int uidAttacker = event->GetInt("attacker");
	int uidVictim = event->GetInt("userid");
	int uidAssister = event->GetInt("assister");
	bool blocked = false;

	if(0 < csgo_debug_CHudDeathNotice_FireGameEvent)
	{
		Tier0_Msg("CHudDeathNotice::FireGameEvent: uidAttaker=%i, uidVictim=%i, uidAssister=%i\n", uidAttacker, uidVictim, uidAssister);
		if(2 <= csgo_debug_CHudDeathNotice_FireGameEvent)
			Tier0_Msg(
				"org_scrollInTime=%f,org_fadeOutTime=%f,org_noticeLifeTime=%f,org_localPlayerLifeTimeMod=%f\n",
				org_CHudDeathNotice_nScrollInTime,
				org_CHudDeathNotice_nFadeOutTime,
				org_CHudDeathNotice_nNoticeLifeTime,
				org_CHudDeathNotice_nLocalPlayerLifeTimeMod
			);
	}

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
			attackerBlocked = e.attackerId != uidAttacker;
			break;
		case DMBM_EQUAL:
		default:
			attackerBlocked = e.attackerId == uidAttacker;
			break;
		}

		bool victimBlocked;
		switch(e.victimMode)
		{
		case DMBM_ANY:
			victimBlocked = true;
			break;
		case DMBM_EXCEPT:
			victimBlocked = e.victimId != uidVictim;
			break;
		case DMBM_EQUAL:
		default:
			victimBlocked = e.victimId == uidVictim;
			break;
		}

		bool assisterBlocked;
		switch(e.assisterMode)
		{
		case DMBM_ANY:
			assisterBlocked = true;
			break;
		case DMBM_EXCEPT:
			assisterBlocked = e.assisterId != uidAssister;
			break;
		case DMBM_EQUAL:
		default:
			assisterBlocked = e.assisterId == uidAssister;
			break;
		}

		blocked = attackerBlocked && victimBlocked && assisterBlocked;
	}

	if(!blocked) detoured_csgo_CHudDeathNotice_FireGameEvent(this_ptr, event);
}

bool csgo_CHudDeathNotice_Install(void)
{
	static bool firstResult;
	static bool firstRun = true;
	if(!firstRun) return firstResult;
	firstRun = false;

	detoured_csgo_CHudDeathNotice_FireGameEvent = (csgo_CHudDeathNotice_FireGameEvent_t)DetourClassFunc((BYTE *)AFXADDR_GET(csgo_CHudDeathNotice_FireGameEvent), (BYTE *)touring_csgo_CHudDeathNotice_FireGameEvent, (int)AFXADDR_GET(csgo_CHudDeathNotice_FireGameEvent_DSZ));

	firstResult = true;
	return firstResult;
}

void csgo_CHudDeathNotice_Block(char const * uidAttacker, char const * uidVictim, char const * uidAssister)
{
	char const * acmd;
	int attackerId = -1;
	int victimId = -1;
	int assisterId = -1;

	acmd = uidAttacker;
	bool anyAttacker = !strcmp("*", acmd);
	bool notAttacker = StringBeginsWith(acmd, "!");
	if(!anyAttacker) attackerId = atoi(notAttacker ? (acmd +1) : acmd);

	acmd = uidVictim;
	bool anyVictim = !strcmp("*", acmd);
	bool notVictim = StringBeginsWith(acmd, "!");
	if(!anyVictim) victimId = atoi(notVictim ? (acmd +1) : acmd);

	acmd = uidAssister;
	bool anyAssister = !strcmp("*", acmd);
	bool notAssister = StringBeginsWith(acmd, "!");
	if(!anyAssister) assisterId = atoi(notAssister ? (acmd +1) : acmd);

	DeathMsgBlockEntry entry = {
		attackerId,
		anyAttacker ? DMBM_ANY : (notAttacker ? DMBM_EXCEPT : DMBM_EQUAL),
		victimId,
		anyVictim ? DMBM_ANY : (notVictim ? DMBM_EXCEPT : DMBM_EQUAL),
		assisterId,
		anyAssister ? DMBM_ANY : (notAssister ? DMBM_EXCEPT : DMBM_EQUAL)
	};

	deathMessageBlock.push_back(entry);
}

void csgo_CHudDeathNotice_Block_List(void)
{
	Tier0_Msg("uidAttacker,uidVictim,uidAssister:\n");
	for(std::list<DeathMsgBlockEntry>::iterator it = deathMessageBlock.begin(); it != deathMessageBlock.end(); it++)
	{
		DeathMsgBlockEntry e = *it;
						
		if(DMBM_ANY == e.attackerMode)
			Tier0_Msg("*");
		else
		{
			if(DMBM_EXCEPT == e.attackerMode)
			{
				Tier0_Msg("!");
			}
			Tier0_Msg("%i", e.attackerId);
		}
		Tier0_Msg(",");
		if(DMBM_ANY == e.victimMode)
			Tier0_Msg("*");
		else
		{
			if(DMBM_EXCEPT == e.victimMode)
			{
				Tier0_Msg("!");
			}
			Tier0_Msg("%i", e.victimId);
		}
		Tier0_Msg(",");
		if(DMBM_ANY == e.assisterMode)
			Tier0_Msg("*");
		else
		{
			if(DMBM_EXCEPT == e.assisterMode)
			{
				Tier0_Msg("!");
			}
			Tier0_Msg("%i", e.assisterId);
		}
		Tier0_Msg("\n");
	}
}

void csgo_CHudDeathNotice_Block_Clear(void)
{
	deathMessageBlock.clear();
}
