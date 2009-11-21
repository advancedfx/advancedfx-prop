// WE SUGGEST TO NOT EDIT THIS FILE!
// This file carries out CRUCIAL operations and we might change it occasionally.

////////////////////////////////////////////////////////////////////////////////
//
// Init addresses:

//
// Engine-to-client interfaces:
//

addr.p_cl_enginefuncs_s = addr.hw_dll + 0x166A98;
addr.p_engine_studio_api_s = addr.hw_dll + 0x1829B8;
addr.p_playermove_s = addr.hw_dll + 0xFD5AE0;


//
// General engine hooks:
//

addr.Host_Frame = addr.hw_dll +0xD82A0;
addr.DTOURSZ_Host_Frame = 0x0C;


//
// Rendering related:
//

addr.SCR_UpdateScreen = addr.hw_dll + 0xCE570;
addr.DTOURSZ_SCR_UpdateScreen = 0x09;

// Hud drawing:
addr.HUD_TOURIN = addr.SCR_UpdateScreen + 0xBD;
addr.HUD_TOUROUT = addr.HUD_TOURIN + 0x87;

addr.R_RenderView = addr.hw_dll + 0x52130;
addr.DTOURSZ_R_RenderView = 0x6;

addr.r_refdef = addr.hw_dll + 0xE95600;

addr.R_DrawParticles = addr.hw_dll + 0x91C10;
addr.DTOURSZ_R_DrawParticles = 0x08;

addr.R_DrawEntitiesOnList = addr.hw_dll + 0x504a0;
addr.DTOURSZ_R_DrawEntitiesOnList = 0x0c;

addr.R_DrawViewModel = addr.hw_dll +0x506f0;
addr.DTOURSZ_R_DrawViewModel = 0x09;

addr.R_PolyBlend = addr.hw_dll +0x50C60;
addr.DTOURSZ_R_PolyBlend  = 0x09;

addr.Mod_LeafPVS = addr.hw_dll + 0x35E30;
addr.DTOURSZ_Mod_LeafPVS = 0x0e;

// DecalTexture hook:
//   this function called in a unkonown sub function of R_DrawWorld that is
//   called before R_BlendLightmaps. the unknown functions draws out all
//   decals of the map as it seems or s.th. and uses this one to get
//   a decal's texture
addr.UnkGetDecalTexture = addr.hw_dll + 0x3C600;
addr.DTOURSZ_UnkGetDecalTexture = 0x05;


//
// Sound system related:
//

addr.GetSoundtime = addr.hw_dll + 0xA3B30;
addr.DTOURSZ_GetSoundtime = 0x06;

addr.S_PaintChannels = addr.hw_dll + 0xA60E0;
addr.DTOURSZ_S_PaintChannels = 0x06;

addr.S_TransferPaintBuffer = addr.hw_dll + 0xA58F0;
addr.DTOURSZ_S_TransferPaintBuffer = 0x05;

addr.shm = addr.hw_dll + 0x73F758;
addr.paintedtime = addr.hw_dll + 0x9FA880;
addr.paintbuffer = addr.paintedtime - 0xC160;
addr.soundtime = addr.paintbuffer + 0xC15C;


//
// Demo parsing related:
//

addr.CL_ParseServerMessage = addr.hw_dll + 0x2A660;
addr.net_message_cursize = addr.hw_dll + 0xA6E630;
addr.net_message = addr.net_message_cursize - 0x10;
addr.msg_readcount = addr.hw_dll + 0xFD3D28;
addr.CL_ParseServerMessage_CmdRead = addr.CL_ParseServerMessage + 0x0E3;
addr.DTOURSZ_CL_ParseServerMessage_CmdRead = 0x07;


//
// Various hooks
//

// tfc DemoEdit campath fix:
addr.HudSpectator_tfc = 0x01909A00; //unchecked
addr.HudSpectator_cmp_tfc = addr.HudSpectator_tfc + 0x23; //unchecked

// UpdateSpectatorPanel overviewmode check jump (for HLTV menu removal):
addr.UpdateSpectatorPanel_checkjmp_ag_clofs = 0x028091; // Adrenaline-Gamer (ag), client.dll ofs //unchecked
addr.UpdateSpectatorPanel_checkjmp_tfc = 0x019439a1; // Team Fortress Classic (tfc) //unchecked
addr.UpdateSpectatorPanel_checkjmp_ns_clofs = 0x033F3D; // Natuaral Selection (ns), client.dll ofs //unchecked
addr.UpdateSpectatorPanel_checkjmp_valve = 0x01940221; //Half-Life (valve) //unchecked


//
// game: cstrike
//

// cstrike Team color hook:
// ALL OFFSETS ARE RELATIVE TO client.dll!
addr.GetClientColor = 0x45CD0; // function, used by head-up and overview i.e. //checked
addr.DTOURSZ_GetClientColor = 0x0a; //checked
addr.unkInlineClientColorA = 0x4629A; // inline, used to draw attacker color, also check hook //checked
addr.SZ_unkInlineClientColorA = 0x3E; //checked
addr.unkInlineClientColorV = 0x46320; // inline, used to draw victim color, also check hook // todo
addr.SZ_unkInlineClientColorV = 0x3C; //checked

// cstrike CrossHair fix related:
addr.CLOFS_UnkCstrikeCrosshairFn = 0x42EB0; //checked
addr.DTOURSZ_UnkCstrikeCrosshairFn = 0x09; //checked, at least 8 bytes req.
addr.CLOFS_UnkCstrikeCh_mul_fac = 0xCD450; //checked
addr.CLOFS_UnkCstrikeCh_add_fac = 0xC3298; //checked

// cstrike EV_CreateSmoke:
addr.cstrike_EV_CreateSmoke_CLOFS = 0xa420;


// unused // p_enginefuncs_s = addr.hw_dll + 0x18B9F0 // may be unused
// unused // p_globalvars_s = addr.hw_dll + 0x7CD0E0 // may be unused
