#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2013-06-04 dominik.matrixstorm.com
//
// First changes:
// 2007-09-13T13:37Z dominik.matrixstorm.com

#include "hl_addresses.h"


AFXADDR_DEF(CL_EmitEntities)
AFXADDR_DEF(CL_EmitEntities_DSZ)
AFXADDR_DEF(CL_ParseServerMessage_CmdRead)
AFXADDR_DEF(CL_ParseServerMessage_CmdRead_DSZ)
AFXADDR_DEF(ClientFunctionTable)
AFXADDR_DEF(CmdTools_Ofs1)
AFXADDR_DEF(CmdTools_Ofs2)
AFXADDR_DEF(CmdTools_Ofs3)
AFXADDR_DEF(DTOURSZ_GetSoundtime)
AFXADDR_DEF(DTOURSZ_Mod_LeafPVS)
AFXADDR_DEF(DTOURSZ_R_DrawEntitiesOnList)
AFXADDR_DEF(DTOURSZ_R_DrawParticles)
AFXADDR_DEF(DTOURSZ_R_DrawViewModel)
AFXADDR_DEF(DTOURSZ_R_PolyBlend)
AFXADDR_DEF(DTOURSZ_R_RenderView)
AFXADDR_DEF(DTOURSZ_SND_PickChannel)
AFXADDR_DEF(DTOURSZ_S_PaintChannels)
AFXADDR_DEF(DTOURSZ_S_TransferPaintBuffer)
AFXADDR_DEF(GetSoundtime)
AFXADDR_DEF(Host_Frame)
AFXADDR_DEF(Host_Frame_DSZ)
AFXADDR_DEF(HudSpectator_cmp_tfc)
AFXADDR_DEF(HudSpectator_tfc)
AFXADDR_DEF(Mod_LeafPVS)
AFXADDR_DEF(R_DrawEntitiesOnList)
AFXADDR_DEF(R_DrawParticles)
AFXADDR_DEF(R_DrawSkyBoxEx)
AFXADDR_DEF(R_DrawSkyBoxEx_DSZ)
AFXADDR_DEF(R_DrawViewModel)
AFXADDR_DEF(R_PolyBlend)
AFXADDR_DEF(R_RenderView)
AFXADDR_DEF(SND_PickChannel)
AFXADDR_DEF(S_PaintChannels)
AFXADDR_DEF(S_TransferPaintBuffer)
AFXADDR_DEF(UnkDrawHudIn)
AFXADDR_DEF(UnkDrawHudInCall)
AFXADDR_DEF(UnkDrawHudInContinue)
AFXADDR_DEF(UnkDrawHudOut)
AFXADDR_DEF(UnkDrawHudOutCall)
AFXADDR_DEF(UnkDrawHudOutContinue)
AFXADDR_DEF(UnkGetDecalTexture)
AFXADDR_DEF(UnkGetDecalTexture_DSZ)
AFXADDR_DEF(UpdateSpectatorPanel_checkjmp_ag)
AFXADDR_DEF(UpdateSpectatorPanel_checkjmp_ns)
AFXADDR_DEF(UpdateSpectatorPanel_checkjmp_tfc)
AFXADDR_DEF(UpdateSpectatorPanel_checkjmp_valve)
AFXADDR_DEF(clientDll)
AFXADDR_DEF(cstrike_CHudDeathNotice_Draw)
AFXADDR_DEF(cstrike_CHudDeathNotice_Draw_DSZ)
AFXADDR_DEF(cstrike_CHudDeathNotice_MsgFunc_DeathMsg)
AFXADDR_DEF(cstrike_CHudDeathNotice_MsgFunc_DeathMsg_DSZ)
AFXADDR_DEF(cstrike_EV_CreateSmoke)
AFXADDR_DEF(cstrike_EV_CreateSmoke_DSZ)
AFXADDR_DEF(cstrike_MsgFunc_DeathMsg)
AFXADDR_DEF(cstrike_MsgFunc_DeathMsg_DSZ)
AFXADDR_DEF(cstrike_UnkCrosshairFn)
AFXADDR_DEF(cstrike_UnkCrosshairFn_DSZ)
AFXADDR_DEF(cstrike_UnkCrosshairFn_add_fac)
AFXADDR_DEF(cstrike_UnkCrosshairFn_mul_fac)
AFXADDR_DEF(cstrike_rgDeathNoticeList)
AFXADDR_DEF(hlExe)
AFXADDR_DEF(hwDll)
AFXADDR_DEF(msg_readcount)
AFXADDR_DEF(net_message)
AFXADDR_DEF(p_cl_enginefuncs_s)
AFXADDR_DEF(p_engine_studio_api_s)
AFXADDR_DEF(p_playermove_s)
AFXADDR_DEF(paintbuffer)
AFXADDR_DEF(paintedtime)
AFXADDR_DEF(r_refdef)
AFXADDR_DEF(shm)
AFXADDR_DEF(skytextures)
AFXADDR_DEF(soundtime)

//
// Documentation (in HLAE source code)
//
// *[1] doc/notes_goldsrc/debug_cstrike_crosshair.txt
// *[2] doc/notes_goldsrc/debug_cstrike_deathmessage.txt
// [3] doc/notes_goldsrc/debug_cstrike_smoke.txt
// *[4] doc/notes_goldsrc/debug_valve_UpdateSpectatorPanel.txt
// *[5] doc/notes_goldsrc/debug_engine_ifaces.txt
// *[6] doc/notes_goldsrc/debug_sound.txt
// *[7] doc/notes_goldsrc/debug_SCR_UpdateScreen.txt
// *[8] doc/notes_goldsrc/debug_Host_Frame.txt
// *[9] doc/notes_goldsrc/debug_ClientFunctionTable
// *[10] doc/notes_goldsrc/debug_CL_ParseServerMessage.txt
// *[11] doc/notes_goldsrc/debug_R_DrawWorld_and_sky.txt
// [12] doc/notes_goldsrc/debug_R_DecalShoot.txt
// *[13] AfxHookGoldSrc/cmd_tools.cpp/getCommandTreeBasePtr
//

void Addresses_InitHlExe(AfxAddr hlExe)
{
	AFXADDR_SET(hlExe, hlExe);
}

void Addresses_InitHwDll(AfxAddr hwDll)
{
	AFXADDR_SET(hwDll, hwDll);

	//
	// Engine-to-client interfaces:
	//
	
	AFXADDR_SET(p_cl_enginefuncs_s, hwDll + 0x134260); // *[5]
	AFXADDR_SET(p_playermove_s, hwDll + 0x10063E0); // *[5]
	AFXADDR_SET(p_engine_studio_api_s, hwDll + 0x1500A8); // *[5]
	
	//
	// General engine hooks:
	//
	
	AFXADDR_SET(Host_Frame, hwDll +0x56010); // *[8]
	AFXADDR_SET(Host_Frame_DSZ, 0x05); // *[8]
	
	AFXADDR_SET(CL_EmitEntities, hwDll + 0x14A30); // *[8]
	AFXADDR_SET(CL_EmitEntities_DSZ, 0x05); // *[8]
	
	AFXADDR_SET(ClientFunctionTable, hwDll +0x122EE40); // *[9]
	
	AFXADDR_SET(CmdTools_Ofs1, 0x19); // *[13]
	AFXADDR_SET(CmdTools_Ofs2, 0x0D); // *[13]
	AFXADDR_SET(CmdTools_Ofs3, 0x29); // *[13]
	
	//
	// Rendering related:
	//

	AFXADDR_SET(UnkDrawHudInCall, hwDll +0x43D00); // *[7]
	AFXADDR_SET(UnkDrawHudOutCall, hwDll +0x5C460); // *[7]
	AFXADDR_SET(UnkDrawHudIn, hwDll +0xB4FC1); // *[7]
	AFXADDR_SET(UnkDrawHudInContinue, AFXADDR_GET(UnkDrawHudIn) + 0x6); // *[7]
	AFXADDR_SET(UnkDrawHudOut, hwDll +0xB5014); // *[7]
	AFXADDR_SET(UnkDrawHudOutContinue, AFXADDR_GET(UnkDrawHudOut) + 0x5); // *[7]
		
	AFXADDR_SET(R_RenderView, hwDll + 0x45140); // *[7]
	AFXADDR_SET(DTOURSZ_R_RenderView, 0x6); // *[7]
	
	AFXADDR_SET(R_DrawViewModel, hwDll +0x437F0); // *[7]
	AFXADDR_SET(DTOURSZ_R_DrawViewModel, 0x06); // *[7]
	
	AFXADDR_SET(R_PolyBlend, hwDll +0x43D50); // *[7]
	AFXADDR_SET(DTOURSZ_R_PolyBlend, 0x06); // *[7]
	
	AFXADDR_SET(r_refdef, hwDll + 0xEC5EE0); // *[7]
	
	AFXADDR_SET(Mod_LeafPVS, hwDll + 0x28050); // *[7]
	AFXADDR_SET(DTOURSZ_Mod_LeafPVS, 0x06); // *[7]
	
	AFXADDR_SET(R_DrawEntitiesOnList, hwDll + 0x435B0); // *[7]
	AFXADDR_SET(DTOURSZ_R_DrawEntitiesOnList, 0x06); // *[7]
	
	AFXADDR_SET(R_DrawParticles, hwDll + 0x7AF80); // *[7]
	AFXADDR_SET(DTOURSZ_R_DrawParticles, 0x06); // *[7]
	
	AFXADDR_SET(R_DrawSkyBoxEx, hwDll + 0x4F38E); // *[11]
	AFXADDR_SET(R_DrawSkyBoxEx_DSZ,  0x06); // *[11]
	
	AFXADDR_SET(skytextures, hwDll + 0x63FC68); // *[11]
	
	AFXADDR_SET(UnkGetDecalTexture, hwDll + 0x2E8E0); // *[12]
	AFXADDR_SET(UnkGetDecalTexture_DSZ, 0x06); // *[12]
	
	//
	// Sound system related:
	//
	
	AFXADDR_SET(GetSoundtime, hwDll + 0x8AF10); // *[6]
	AFXADDR_SET(DTOURSZ_GetSoundtime, 0x0a); // *[6]
	
	AFXADDR_SET(S_PaintChannels, hwDll + 0x8CB00); // *[6]
	AFXADDR_SET(DTOURSZ_S_PaintChannels, 0x08); // *[6]
	
	AFXADDR_SET(paintedtime, hwDll + 0xA2B560); // *[6]
	AFXADDR_SET(shm, hwDll + 0x6B79F8); // *[6]
	AFXADDR_SET(soundtime, hwDll + 0xA2B55C); // *[6]
	
	AFXADDR_SET(paintbuffer, hwDll + 0xA21420); // *[6]
	
	AFXADDR_SET(S_TransferPaintBuffer, hwDll + 0x8C5A0); // *[6]
	AFXADDR_SET(DTOURSZ_S_TransferPaintBuffer, 0x06); // *[6]
	
	AFXADDR_SET(SND_PickChannel, hwDll + 0x8A1B0); // *[6]
	AFXADDR_SET(DTOURSZ_SND_PickChannel, 0x07); // *[6]
	
	//
	// Demo parsing related:
	//
	
	AFXADDR_SET(CL_ParseServerMessage_CmdRead, hwDll + 0x1CEE6); // *[10]
	AFXADDR_SET(CL_ParseServerMessage_CmdRead_DSZ, 0x07); // *[10]
	AFXADDR_SET(msg_readcount, hwDll + 0x1004628); // *[10]
	AFXADDR_SET(net_message, hwDll +0xA9EF30 - 0x10); // *[10]
	
	//
	// other
	//
	
	// tfc DemoEdit campath fix:
	// currently not supported // addr.HudSpectator_tfc = addr.hlExe + 0x509A00;
	// currently not supported // addr.HudSpectator_cmp_tfc = addr.HudSpectator_tfc + 0x23;
	
	// UpdateSpectatorPanel overviewmode check jump (for HLTV menu removal):
	// currently not supported // addr.UpdateSpectatorPanel_checkjmp_tfc = addr.hlExe + 0x5439A1; // Team Fortress Classic (tfc)
}

/// <remarks>Not called when no client.dll is loaded (tfc).</remarks>
void Addresses_InitClientDll(AfxAddr clientDll)
{
	AFXADDR_SET(clientDll, clientDll);

	//
	// game: cstrike
	//
	
	// cstrike CrossHair fix related:
	AFXADDR_SET(cstrike_UnkCrosshairFn, clientDll + 0x41640); // *[1]
	AFXADDR_SET(cstrike_UnkCrosshairFn_DSZ, 0x0c); // at least 8 bytes req. // *[1]
	AFXADDR_SET(cstrike_UnkCrosshairFn_add_fac, clientDll + 0xC32C8); // *[1]
	AFXADDR_SET(cstrike_UnkCrosshairFn_mul_fac, clientDll + 0xCD4C8); // *[1]
	
	// cstrike EV_CreateSmoke:
	AFXADDR_SET(cstrike_EV_CreateSmoke, clientDll + 0xA080); // *[3]
	AFXADDR_SET(cstrike_EV_CreateSmoke_DSZ, 0x0a); // *[3]
	
	// cstrike DeathMsg related (client.dll offsets):
	AFXADDR_SET(cstrike_MsgFunc_DeathMsg, clientDll + 0x44490); // *[2]
	AFXADDR_SET(cstrike_MsgFunc_DeathMsg_DSZ, 0x08); // *[2]
	AFXADDR_SET(cstrike_CHudDeathNotice_MsgFunc_DeathMsg, clientDll + 0x44970); // *[2]
	AFXADDR_SET(cstrike_CHudDeathNotice_MsgFunc_DeathMsg_DSZ, 0x08); // at least 8 bytes req. // *[2]
	AFXADDR_SET(cstrike_rgDeathNoticeList, clientDll + 0x124EC0); // *[2]
	AFXADDR_SET(cstrike_CHudDeathNotice_Draw, clientDll + 0x445F0); // *[2]
	AFXADDR_SET(cstrike_CHudDeathNotice_Draw_DSZ, 0x0a); // at least 8 bytes req. // *[2]
	
	//
	// other games
	//
	
	// UpdateSpectatorPanel overviewmode check jump (for HLTV menu removal):
	AFXADDR_SET(UpdateSpectatorPanel_checkjmp_ag, clientDll + 0x028091); // Adrenaline-Gamer (ag)
	AFXADDR_SET(UpdateSpectatorPanel_checkjmp_ns, clientDll + 0x033F3D); // Natuaral Selection (ns)
	AFXADDR_SET(UpdateSpectatorPanel_checkjmp_valve, clientDll + 0x4A904); // Half-Life (valve) // *[4]
}