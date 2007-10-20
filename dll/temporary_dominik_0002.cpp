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

extern cl_enginefuncs_s *pEngfuncs;
extern engine_studio_api_s *pEngStudio;
extern playermove_s* ppmove;

REGISTER_DEBUGCVAR(test_snd_show,"1",0);

#include "detours.h"

// see Quake 1 Source snd_dma.c for more info

#define ADDRESS_S_Update 0x01da4200
#define DETOURSIZE_S_Update 0x07

#define ADDRESS_CALL_S_Update_ (ADDRESS_S_Update+0x160)

#define ADDRESS_S_Update_ 0x01da4490
#define DETOURSIZE_S_Update_ 0x06

#define ADDRESS_total_channels 0x027a8ecc
#define ADDRESS_channels (0x027a8f04-0x04)
// those brackets are very improtant, never forget them!!!
// since the result of the experssion might be converted to another type and +x might mean +x*sizeof(type_being_pointed_too).

//QW/bothdefs.h:
#define	MAX_QPATH		64			// max length of a quake game pathname

// QW/zone.h:
// typedef struct cache_user_s
// {
//	void	*data;
// } cache_user_t;
// defined in hlsdk/common/com_model.h

//>>QW/snd_dma.c:

typedef struct sfx_s
{
	char 	name[MAX_QPATH];
	cache_user_t	cache;
} sfx_t;

typedef struct
{
	sfx_t	*sfx;			// sfx number
	int		leftvol;		// 0-255 volume
	int		rightvol;		// 0-255 volume
	int		end;			// end time in global paintsamples
	int 	pos;			// sample position in sfx
	int		looping;		// where to loop, -1 = no looping
	int		entnum;			// to allow overriding a specific sound
	int		entchannel;		//
	vec3_t	origin;			// origin of sound effect
	vec_t	dist_mult;		// distance multiplier (attenuation/clipK)
	int		master_vol;		// 0-255 master volume

	DWORD	unknown0;
	DWORD	unknown1;
	DWORD	unknown2;
	DWORD	unknown3;
	DWORD	unknown4;
} channel_t;

//<<QW/snd_dma.c:
channel_t channels;

//#include "util_vector.h" // vec_3t

typedef void (*S_Update_t)(vec3_t origin, vec3_t forward, vec3_t right, vec3_t up);
typedef void (*S_Update__t)(void);

S_Update_t detoured_S_Update=NULL;
S_Update__t detoured_S_Update_=NULL;

bool g_bIn_S_Update=false;

void touring_S_Update(vec3_t origin, vec3_t forward, vec3_t right, vec3_t up)
{
	g_bIn_S_Update=true;
	detoured_S_Update(origin, forward, right, up);
	g_bIn_S_Update=false;
}

void touring_S_Update_(void)
{
	int total;
	channel_t* ch;
	int i;

	if (g_bIn_S_Update)
	{
		// we can do our sound processing here since we were called by S_Update (which is called once per H-L / Quake Main loop (frame))

		int total_channels = *(int *)ADDRESS_total_channels;
		channel_t *channels = (channel_t *)ADDRESS_channels;

		if (test_snd_show->value!=0)
		{
			total = 0;
			ch = channels;
			for (i=0 ; i<total_channels; i++, ch++)
				if (ch->sfx && (ch->leftvol || ch->rightvol) )
				{
					pEngfuncs->Con_Printf ("%3i %3i %s\n", ch->leftvol, ch->rightvol, ch->sfx->name);
					//pEngfuncs->Con_Printf ("%3i %3i 0x%08x\n", ch->leftvol, ch->rightvol, ch->sfx);
					total++;
				}
			
			pEngfuncs->Con_Printf ("----(%i/%i)----\n", total,total_channels);
		}

	}
	detoured_S_Update_();
}

REGISTER_DEBUGCMD_FUNC(test_snd_inst)
{
	if (!detoured_S_Update_) detoured_S_Update_ = (S_Update__t) DetourApply((BYTE *)ADDRESS_S_Update_, (BYTE *)touring_S_Update_, (int)DETOURSIZE_S_Update_);
	if (!detoured_S_Update) detoured_S_Update = (S_Update_t) DetourApply((BYTE *)ADDRESS_S_Update, (BYTE *)touring_S_Update, (int)DETOURSIZE_S_Update);
	pEngfuncs->Con_Printf("__mirv_test_snd: tried to install hooks.\n");
}