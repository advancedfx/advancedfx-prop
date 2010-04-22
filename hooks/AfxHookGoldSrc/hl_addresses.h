#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2009-11-37 dominik.matrixstorm.com
//
// First changes:
// 2007-09-13T13:37Z dominik.matrixstorm.com

typedef unsigned long HlAddress_t;

#define HL_ADDR_GET(name) \
	(g_HlAddr_##name)

#define HL_ADDR_SET(name,value) \
	(g_HlAddr_##name = value)

#define HL_ADDR_EXPOSE(name) \
	extern HlAddress_t g_HlAddr_##name;

HL_ADDR_EXPOSE(CL_EmitEntities)
HL_ADDR_EXPOSE(CL_EmitEntities_DSZ)
HL_ADDR_EXPOSE(CL_ParseServerMessage)
HL_ADDR_EXPOSE(CL_ParseServerMessage_CmdRead)
HL_ADDR_EXPOSE(DTOURSZ_CL_ParseServerMessage_CmdRead)
HL_ADDR_EXPOSE(DTOURSZ_GetClientColor)
HL_ADDR_EXPOSE(DTOURSZ_GetSoundtime)
HL_ADDR_EXPOSE(DTOURSZ_Mod_LeafPVS)
HL_ADDR_EXPOSE(DTOURSZ_R_DrawEntitiesOnList)
HL_ADDR_EXPOSE(DTOURSZ_R_DrawParticles)
HL_ADDR_EXPOSE(DTOURSZ_R_DrawViewModel)
HL_ADDR_EXPOSE(DTOURSZ_R_PolyBlend)
HL_ADDR_EXPOSE(DTOURSZ_R_RenderView)
HL_ADDR_EXPOSE(DTOURSZ_SCR_UpdateScreen)
HL_ADDR_EXPOSE(DTOURSZ_SND_PickChannel)
HL_ADDR_EXPOSE(DTOURSZ_S_PaintChannels)
HL_ADDR_EXPOSE(DTOURSZ_S_TransferPaintBuffer)
HL_ADDR_EXPOSE(DTOURSZ_UnkCstrikeCrosshairFn)
HL_ADDR_EXPOSE(DTOURSZ_UnkGetDecalTexture)
HL_ADDR_EXPOSE(GetClientColor)
HL_ADDR_EXPOSE(GetSoundtime)
HL_ADDR_EXPOSE(HUD_TOURIN)
HL_ADDR_EXPOSE(HUD_TOUROUT)
HL_ADDR_EXPOSE(HudSpectator_cmp_tfc)
HL_ADDR_EXPOSE(HudSpectator_tfc)
HL_ADDR_EXPOSE(Mod_LeafPVS)
HL_ADDR_EXPOSE(R_DrawEntitiesOnList)
HL_ADDR_EXPOSE(R_DrawParticles)
HL_ADDR_EXPOSE(R_DrawViewModel)
HL_ADDR_EXPOSE(R_PolyBlend)
HL_ADDR_EXPOSE(R_RenderView)
HL_ADDR_EXPOSE(SCR_UpdateScreen)
HL_ADDR_EXPOSE(SND_PickChannel)
HL_ADDR_EXPOSE(SZ_unkInlineClientColorA)
HL_ADDR_EXPOSE(SZ_unkInlineClientColorV)
HL_ADDR_EXPOSE(S_PaintChannels)
HL_ADDR_EXPOSE(S_TransferPaintBuffer)
HL_ADDR_EXPOSE(UnkCstrikeCh_add_fac)
HL_ADDR_EXPOSE(UnkCstrikeCh_mul_fac)
HL_ADDR_EXPOSE(UnkCstrikeCrosshairFn)
HL_ADDR_EXPOSE(UnkDevCapsChecks)
HL_ADDR_EXPOSE(UnkDevCapsChecks_BYTES)
HL_ADDR_EXPOSE(UnkGetDecalTexture)
HL_ADDR_EXPOSE(UpdateSpectatorPanel_checkjmp_ag)
HL_ADDR_EXPOSE(UpdateSpectatorPanel_checkjmp_ns)
HL_ADDR_EXPOSE(UpdateSpectatorPanel_checkjmp_tfc)
HL_ADDR_EXPOSE(UpdateSpectatorPanel_checkjmp_valve)
HL_ADDR_EXPOSE(clientDll)
HL_ADDR_EXPOSE(cstrike_CHudDeathNotice_Draw)
HL_ADDR_EXPOSE(cstrike_CHudDeathNotice_MsgFunc_DeathMsg)
HL_ADDR_EXPOSE(cstrike_EV_CreateSmoke)
HL_ADDR_EXPOSE(cstrike_MsgFunc_DeathMsg)
HL_ADDR_EXPOSE(cstrike_rgDeathNoticeList)
HL_ADDR_EXPOSE(hlExe)
HL_ADDR_EXPOSE(hwDll)
HL_ADDR_EXPOSE(msg_readcount)
HL_ADDR_EXPOSE(net_message)
HL_ADDR_EXPOSE(net_message_cursize)
HL_ADDR_EXPOSE(p_cl_enginefuncs_s)
HL_ADDR_EXPOSE(p_engine_studio_api_s)
HL_ADDR_EXPOSE(p_playermove_s)
HL_ADDR_EXPOSE(paintbuffer)
HL_ADDR_EXPOSE(paintedtime)
HL_ADDR_EXPOSE(r_refdef)
HL_ADDR_EXPOSE(shm)
HL_ADDR_EXPOSE(soundtime)
HL_ADDR_EXPOSE(unkInlineClientColorA)
HL_ADDR_EXPOSE(unkInlineClientColorV)


//
////

HlAddress_t * HlAddr_GetByName(char const * name);

// slow!
bool HlAddr_Debug_GetAt(unsigned int index, HlAddress_t & outAddr, char const * & outName);
unsigned int HlAddr_Debug_GetCount();
