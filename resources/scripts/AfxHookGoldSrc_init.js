// WE SUGGEST TO NOT EDIT THIS FILE!
// This file carries out CRUCIAL operations and we might change it occasionally.


////////////////////////////////////////////////////////////////////////////////
//
// Address system:

function afx_OnHwDllLoaded()
{
	//
	// Engine-to-client interfaces:
	//

	addr.p_cl_enginefuncs_s = addr.hwDll + 0x166A98;
	addr.p_engine_studio_api_s = addr.hwDll + 0x1829B8;
	addr.p_playermove_s = addr.hwDll + 0xFD5AE0;

	//
	// General engine hooks:
	//

	addr.UnkDevCapsChecks = addr.hwDll +0xC6A26;
	addr.UnkDevCapsChecks_BYTES = 0x14;

	//
	// Rendering related:
	//
	
	addr.CL_EmitEntities = addr.hwDll + 0x21C40;
	addr.CL_EmitEntities_DSZ = 0x05;

	addr.SCR_UpdateScreen = addr.hwDll + 0xCE570;
	addr.DTOURSZ_SCR_UpdateScreen = 0x09;

	addr.HUD_TOURIN = addr.SCR_UpdateScreen + 0xBD;
	addr.HUD_TOUROUT = addr.HUD_TOURIN + 0x87;

	addr.R_RenderView = addr.hwDll + 0x52130;
	addr.DTOURSZ_R_RenderView = 0x6;

	addr.r_refdef = addr.hwDll + 0xE95600;

	addr.R_DrawParticles = addr.hwDll + 0x91C10;
	addr.DTOURSZ_R_DrawParticles = 0x08;

	addr.R_DrawEntitiesOnList = addr.hwDll + 0x504a0;
	addr.DTOURSZ_R_DrawEntitiesOnList = 0x0c;

	addr.R_DrawViewModel = addr.hwDll +0x506f0;
	addr.DTOURSZ_R_DrawViewModel = 0x09;

	addr.R_PolyBlend = addr.hwDll +0x50C60;
	addr.DTOURSZ_R_PolyBlend  = 0x09;

	addr.Mod_LeafPVS = addr.hwDll + 0x35E30;
	addr.DTOURSZ_Mod_LeafPVS = 0x0e;

	// DecalTexture hook:
	//   this function is called in a unknown sub function of R_DrawWorld that is
	//   called before R_BlendLightmaps. the unknown functions draws out all
	//   decals of the map as it seems or s.th. and uses this one to get
	//   a decal's texture
	addr.UnkGetDecalTexture = addr.hwDll + 0x3C600;
	addr.DTOURSZ_UnkGetDecalTexture = 0x05;


	//
	// Sound system related:
	//

	addr.GetSoundtime = addr.hwDll + 0xA3B30;
	addr.DTOURSZ_GetSoundtime = 0x06;

	addr.S_PaintChannels = addr.hwDll + 0xA60E0;
	addr.DTOURSZ_S_PaintChannels = 0x06;

	addr.S_TransferPaintBuffer = addr.hwDll + 0xA58F0;
	addr.DTOURSZ_S_TransferPaintBuffer = 0x05;

	addr.shm = addr.hwDll + 0x73F758;
	addr.paintedtime = addr.hwDll + 0x9FA880;
	addr.paintbuffer = addr.paintedtime - 0xC160;
	addr.soundtime = addr.paintbuffer + 0xC15C;

	addr.SND_PickChannel = addr.hwDll + 0xA25F0;
	addr.DTOURSZ_SND_PickChannel = 0x05;


	//
	// Demo parsing related:
	//

	addr.CL_ParseServerMessage = addr.hwDll + 0x2A660;
	addr.net_message_cursize = addr.hwDll + 0xA6E630;
	addr.net_message = addr.net_message_cursize - 0x10;
	addr.msg_readcount = addr.hwDll + 0xFD3D28;
	addr.CL_ParseServerMessage_CmdRead = addr.CL_ParseServerMessage + 0x0E3;
	addr.DTOURSZ_CL_ParseServerMessage_CmdRead = 0x07;
	
	//
	// other
	//

	// tfc DemoEdit campath fix:
	addr.HudSpectator_tfc = addr.hlExe + 0x509A00
	addr.HudSpectator_cmp_tfc = addr.HudSpectator_tfc + 0x23;
	
	// UpdateSpectatorPanel overviewmode check jump (for HLTV menu removal):
	addr.UpdateSpectatorPanel_checkjmp_tfc = addr.hlExe + 0x5439A1; // Team Fortress Classic (tfc)
	addr.UpdateSpectatorPanel_checkjmp_valve = addr.hlExe + 0x540221; //Half-Life (valve)
	
	// unused // p_enginefuncs_s = addr.hwDll + 0x18B9F0 // may be unused
	// unused // p_globalvars_s = addr.hwDll + 0x7CD0E0 // may be unused
}
events.onHwDllLoaded = afx_OnHwDllLoaded;
delete afx_OnHwDllLoaded;

function afx_OnClientDllLoaded()
{
	//
	// game: cstrike
	//

	// cstrike Team color hook:
	addr.GetClientColor = addr.clientDll + 0x45CD0; // function, used by head-up and overview i.e.
	addr.DTOURSZ_GetClientColor = 0x0a;
	addr.unkInlineClientColorA = addr.clientDll + 0x4629A; // inline, used to draw attacker color, also check hook
	addr.SZ_unkInlineClientColorA = 0x3E;
	addr.unkInlineClientColorV = addr.clientDll + 0x46320; // inline, used to draw victim color, also check hook // todo
	addr.SZ_unkInlineClientColorV = 0x3C;

	// cstrike CrossHair fix related:
	addr.UnkCstrikeCrosshairFn = addr.clientDll + 0x42EB0;
	addr.DTOURSZ_UnkCstrikeCrosshairFn = 0x09; // at least 8 bytes req.
	addr.UnkCstrikeCh_mul_fac = addr.clientDll + 0xCD450;
	addr.UnkCstrikeCh_add_fac = addr.clientDll + 0xC3298;

	// cstrike EV_CreateSmoke:
	addr.cstrike_EV_CreateSmoke = addr.clientDll + 0xa420;

	// cstrike DeathMsg related (client.dll offsets):
	addr.cstrike_rgDeathNoticeList = addr.clientDll + 0x121DF8;
	addr.cstrike_CHudDeathNotice_Draw = addr.clientDll + 0x45E10;
	addr.cstrike_CHudDeathNotice_MsgFunc_DeathMsg = addr.clientDll + 0x46190;
	addr.cstrike_MsgFunc_DeathMsg = addr.clientDll + 0x45CB0;
	
	//
	// other games
	//
	
	// UpdateSpectatorPanel overviewmode check jump (for HLTV menu removal):
	addr.UpdateSpectatorPanel_checkjmp_ag = addr.clientDll + 0x028091; // Adrenaline-Gamer (ag)
	addr.UpdateSpectatorPanel_checkjmp_ns = addr.clientDll + 0x033F3D; // Natuaral Selection (ns)
}
events.onClientDllLoaded = afx_OnClientDllLoaded;
delete afx_OnClientDllLoaded;


////////////////////////////////////////////////////////////////////////////////
//
// Provide additional OpenGL helpers:

ogl.GL_TRUE = 1;
ogl.GL_FALSE = 0;
ogl.GL_COLOR_BUFFER_BIT = 0x00004000;
ogl.GL_DEPTH_BUFFER_BIT = 0x00000100;
ogl.GL_ACCUM_BUFFER_BIT = 0x00000200;
ogl.GL_STENCIL_BUFFER_BIT = 0x00000400;