#include "stdafx.h"

#include "R_DrawParticles.h"

#include <hl_addresses.h>

#include <hooks/shared/detours.h>

#include "../../modules/ModInfo.h"

typedef void (*R_DrawParticles_t) (void);
R_DrawParticles_t g_Old_R_DrawParticles = 0;

void New_R_DrawParticles (void)
{
	g_ModInfo.SetIn_R_DrawParticles(true);
	g_Old_R_DrawParticles();
	g_ModInfo.SetIn_R_DrawParticles(false);
}

void Hook_R_DrawParticles()
{
	if( !g_Old_R_DrawParticles && 0 != HL_ADDR_GET(R_DrawParticles) )
	{
		g_Old_R_DrawParticles = (R_DrawParticles_t) DetourApply((BYTE *)HL_ADDR_GET(R_DrawParticles), (BYTE *)New_R_DrawParticles, (int)HL_ADDR_GET(DTOURSZ_R_DrawParticles));
	}
}
