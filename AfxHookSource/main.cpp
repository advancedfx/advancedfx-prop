#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2014-04-30 by dominik.matrixstorm.com
//
// First changes:
// 2009-09-29 by dominik.matrixstorm.com


// TODO:
// - Memory for Wrp* is never freed atm

#include <windows.h>

#include <shared/StringTools.h>

#include <shared/detours.h>

#include "addresses.h"
#include "RenderView.h"
#include "SourceInterfaces.h"
#include "WrpVEngineClient.h"
#include "WrpConsole.h"
#include "WrpGlobals.h"
//#include "d3d9Hooks.h"
#include "csgo_SndMixTimeScalePatch.h"


WrpVEngineClient * g_VEngineClient = 0;
ICvar_003 * g_Cvar = 0;


void ErrorBox(char const * messageText) {
	MessageBoxA(0, messageText, "Error - AfxHookSource", MB_OK|MB_ICONERROR);
}

void ErrorBox() {
	ErrorBox("Something went wrong.");
}

char const * g_Info_VClient = "NULL";
char const * g_Info_VEngineClient = "NULL";
char const * g_Info_VEngineCvar = "NULL";

void PrintInfo() {
	Tier0_Msg(
		"|" "\n"
		"| AfxHookSource ("  __DATE__ " "__TIME__ ")" "\n"
		"| http://advancedfx.org/" "\n"
		"|" "\n"
	);

	Tier0_Msg("| VClient: %s\n", g_Info_VClient);
	Tier0_Msg("| VEngineClient: %s\n", g_Info_VEngineClient);
	Tier0_Msg("| VEngineCvar: %s\n", g_Info_VEngineCvar);
	Tier0_Msg("| GameDirectory: %s\n", g_VEngineClient->GetGameDirectory());

	Tier0_Msg("|" "\n");
}

IClientEngineTools_001 * g_Engine_ClientEngineTools;

class ClientEngineTools : public IClientEngineTools_001
{
public:
	// TODO: we should call the destructor of g_Engine_ClientEngineTools on destuction?

	virtual void LevelInitPreEntityAllTools() { g_Engine_ClientEngineTools->LevelInitPreEntityAllTools(); }
	virtual void LevelInitPostEntityAllTools() { g_Engine_ClientEngineTools->LevelInitPostEntityAllTools(); }
	virtual void LevelShutdownPreEntityAllTools() { g_Engine_ClientEngineTools->LevelShutdownPreEntityAllTools(); }
	virtual void LevelShutdownPostEntityAllTools() { g_Engine_ClientEngineTools->LevelShutdownPostEntityAllTools(); }
	virtual void PreRenderAllTools() { g_Engine_ClientEngineTools->PreRenderAllTools(); }
	virtual void PostRenderAllTools() { g_Engine_ClientEngineTools->PostRenderAllTools(); }
	virtual void PostToolMessage( HTOOLHANDLE hEntity, KeyValues *msg ) { g_Engine_ClientEngineTools->PostToolMessage(hEntity, msg); }
	virtual void AdjustEngineViewport( int& x, int& y, int& width, int& height ) { g_Engine_ClientEngineTools->AdjustEngineViewport(x, y, width, height); }
	
	virtual bool SetupEngineView( Vector &origin, QAngle &angles, float &fov )
	{
		bool bRet = g_Engine_ClientEngineTools->SetupEngineView(origin, angles, fov);

		g_Hook_VClient_RenderView.OnViewOverride(
			origin.x, origin.y, origin.z,
			angles.x, angles.y, angles.z
		);

		return bRet;
	}
	
	virtual bool SetupAudioState( AudioState_t &audioState ) { return g_Engine_ClientEngineTools->SetupAudioState(audioState); }
	virtual void VGui_PreRenderAllTools( int paintMode ) { g_Engine_ClientEngineTools->VGui_PreRenderAllTools(paintMode); }
	virtual void VGui_PostRenderAllTools( int paintMode ) { g_Engine_ClientEngineTools->VGui_PostRenderAllTools(paintMode); }
	virtual bool IsThirdPersonCamera() { return g_Engine_ClientEngineTools->IsThirdPersonCamera(); }
	virtual bool InToolMode()  { return g_Engine_ClientEngineTools->InToolMode(); }
} g_ClientEngineTools;

CreateInterfaceFn g_AppSystemFactory = 0;

void MySetup(CreateInterfaceFn appSystemFactory, WrpGlobals *pGlobals)
{
	static bool bFirstRun = true;

	if(bFirstRun)
	{
		bFirstRun = false;

		void *iface , *iface2;

		g_AppSystemFactory = appSystemFactory;

		if(iface = appSystemFactory(VENGINE_CLIENT_INTERFACE_VERSION_013, NULL)) {
			g_Info_VEngineClient = VENGINE_CLIENT_INTERFACE_VERSION_013;
			g_VEngineClient = new WrpVEngineClient_013((IVEngineClient_013 *)iface);
		}
		else if(iface = appSystemFactory(VENGINE_CLIENT_INTERFACE_VERSION_012, NULL)) {
			g_Info_VEngineClient = VENGINE_CLIENT_INTERFACE_VERSION_012;
			g_VEngineClient = new WrpVEngineClient_012((IVEngineClient_012 *)iface);
		}
		else {
			ErrorBox("Could not get a supported VEngineClient interface.");
		}

		if((iface = appSystemFactory( CVAR_INTERFACE_VERSION_007, NULL )))
		{
			g_Info_VEngineCvar = CVAR_INTERFACE_VERSION_007;
			WrpConCommands::RegisterCommands((ICvar_007 *)iface);
		}
		else if((iface = appSystemFactory( VENGINE_CVAR_INTERFACE_VERSION_004, NULL )))
		{
			g_Info_VEngineCvar = VENGINE_CVAR_INTERFACE_VERSION_004;
			WrpConCommands::RegisterCommands((ICvar_004 *)iface);
		}
		else if(
			(iface = appSystemFactory( VENGINE_CVAR_INTERFACE_VERSION_003, NULL ))
			&& (iface2 = appSystemFactory(VENGINE_CLIENT_INTERFACE_VERSION_012, NULL))
		) {
			g_Info_VEngineCvar = VENGINE_CVAR_INTERFACE_VERSION_003 " & " VENGINE_CLIENT_INTERFACE_VERSION_012;
			WrpConCommands::RegisterCommands((ICvar_003 *)iface, (IVEngineClient_012 *)iface2);
		}
		else {
			ErrorBox("Could not get a supported VEngineCvar interface.");
		}

		if(iface = appSystemFactory(VCLIENTENGINETOOLS_INTERFACE_VERSION_001, NULL))
		{
			g_Engine_ClientEngineTools = (IClientEngineTools_001 *)iface;
		}
		else {
			ErrorBox("Could not get a supported VClientEngineTools interface.");
		}
		
		g_Hook_VClient_RenderView.Install(pGlobals);

		PrintInfo();
	}
}

void* AppSystemFactory_ForClient(const char *pName, int *pReturnCode)
{
	if(!strcmp(VCLIENTENGINETOOLS_INTERFACE_VERSION_001, pName))
	{
		return &g_ClientEngineTools;
	}

	return g_AppSystemFactory(pName, pReturnCode);
}

void * old_Client_Init;

int __stdcall new_Client_Init(DWORD *this_ptr, CreateInterfaceFn appSystemFactory, CreateInterfaceFn physicsFactory, CGlobalVarsBase *pGlobals) {
	static bool bFirstCall = true;
	int myret;

	if(bFirstCall) {
		bFirstCall = false;

		MySetup(appSystemFactory, new WrpGlobalsOther(pGlobals));
	}

	__asm {
		MOV ecx, pGlobals
		PUSH ecx
		MOV ecx, physicsFactory
		PUSH ecx
		MOV ecx, AppSystemFactory_ForClient
		PUSH ecx

		MOV ecx, this_ptr		
		CALL old_Client_Init
		MOV	myret, eax
	}

	return myret;
}

__declspec(naked) void hook_Client_Init() {
	static unsigned char * tempMem[8];
	__asm {
		POP eax
		MOV tempMem[0], eax
		MOV tempMem[4], ecx

		PUSH ecx
		CALL new_Client_Init

		MOV ecx, tempMem[4]
		PUSH 0
		PUSH eax
		MOV eax, tempMem[0]
		MOV [esp+4], eax
		POP eax

		RET
	}
}


void * HookInterfaceFn(void * iface, int idx, void * fn)
{
	MdtMemBlockInfos mbis;
	void * ret = 0;

	if(iface)
	{
		void **padr =  *(void ***)iface;
		ret = *(padr+idx);
		MdtMemAccessBegin((padr+idx), sizeof(void *), &mbis);
		*padr = fn;
		MdtMemAccessEnd(&mbis);
	}

	return ret;
}

void * csgo_VClient = 0;

#pragma warning(push)
#pragma warning(disable:4731) // frame pointer register 'ebp' modified by inline assembly code

// The first 3 asm instructions unroll the compiler epiloge code,
// this might need to be updated. I couldn't find a better way yet,
// since __declspec(naked) won't work on member functions.
// TODO:
// A better way might be filling a struct with function pointers
// and returning a pointer to that maybe.
#define JMP_IFACE_FN(iface,index) \
	__asm pop ecx \
	__asm mov esp, ebp \
	__asm pop ebp \
	__asm mov ecx, csgo_VClient \
	__asm mov eax, [ecx] \
	__asm mov eax, [eax +4*index] \
	__asm jmp eax

class CBaseClientDllWrapper_csgo
{
public:
	virtual int Connect( CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals )
	{ JMP_IFACE_FN(csgo_VClient, 0) }

	virtual void Disconnect()
	{ JMP_IFACE_FN(csgo_VClient, 1) }

	virtual int Init( CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals )
	{
		static bool bFirstCall = true;
		int myret;

		if(bFirstCall)
		{
			bFirstCall = false;

			MySetup(appSystemFactory, new WrpGlobalsCsGo(pGlobals));
		}

		__asm {
			MOV ecx, pGlobals
			PUSH ecx
			MOV ecx, AppSystemFactory_ForClient
			PUSH ecx

			mov ecx, csgo_VClient
			mov eax, [ecx]
			mov eax, [eax +4*2]
			call eax

			MOV	myret, eax
		}

		return myret;
	}

	virtual void			PostInit()
	{ JMP_IFACE_FN(csgo_VClient, 3) }


	virtual void Shutdown( void )
	{ JMP_IFACE_FN(csgo_VClient, 4) }

	
	virtual void _UNUSED_LevelInitPreEntity(void)
	{ JMP_IFACE_FN(csgo_VClient, 5) }

	virtual void _UNUSED_LevelInitPostEntity(void)
	{ JMP_IFACE_FN(csgo_VClient, 6) }

	virtual void _UNUSED_LevelShutdown(void)
	{ JMP_IFACE_FN(csgo_VClient, 7) }

	virtual void _UNUSED_GetAllClasses(void)
	{ JMP_IFACE_FN(csgo_VClient, 8) }

	virtual void _UNUSED_HudVidInit(void)
	{ JMP_IFACE_FN(csgo_VClient, 9) }

	virtual void _UNUSED_HudProcessInput(void)
	{ JMP_IFACE_FN(csgo_VClient, 10) }

	virtual void _UNUSED_HudUpdate(void)
	{ JMP_IFACE_FN(csgo_VClient, 11) }

	virtual void _UNUSED_HudReset(void)
	{ JMP_IFACE_FN(csgo_VClient, 12) }

	virtual void _UNUSED_HudText(void)
	{ JMP_IFACE_FN(csgo_VClient, 13) }

	virtual void _UNUSED_ShouldDrawDropdownConsole(void)
	{ JMP_IFACE_FN(csgo_VClient, 14) }

	virtual void _UNUSED_IN_ActivateMouse(void)
	{ JMP_IFACE_FN(csgo_VClient, 15) }

	virtual void _UNUSED_IN_DeactivateMouse(void)
	{ JMP_IFACE_FN(csgo_VClient, 16) }

	virtual void _UNUSED_IN_Accumulate (void)
	{ JMP_IFACE_FN(csgo_VClient, 17) }

	virtual void _UNUSED_IN_ClearStates (void)
	{ JMP_IFACE_FN(csgo_VClient, 18) }

	virtual void _UNUSED_IN_IsKeyDown(void )
	{ JMP_IFACE_FN(csgo_VClient, 19) }

	virtual void _UNUSED_IN_KeyEvent(void)
	{ JMP_IFACE_FN(csgo_VClient, 20) }

	virtual void _UNUSED_CreateMove (void)
	{ JMP_IFACE_FN(csgo_VClient, 21) }
		
	virtual void _UNUSED_ExtraMouseSample(void)
	{ JMP_IFACE_FN(csgo_VClient, 22) }

	virtual void _UNUSED_WriteUsercmdDeltaToBuffer(void)
	{ JMP_IFACE_FN(csgo_VClient, 23) }

	virtual void _UNUSED_EncodeUserCmdToBuffer(void)
	{ JMP_IFACE_FN(csgo_VClient, 24) }

	virtual void _UNUSED_DecodeUserCmdFromBuffer(void)
	{ JMP_IFACE_FN(csgo_VClient, 25) }

	virtual void _UNUSED_View_Render(void)
	{ JMP_IFACE_FN(csgo_VClient, 26) }

	virtual void _UNUSED_RenderView(void)
	{ JMP_IFACE_FN(csgo_VClient, 27) }

	virtual void _UNUSED_View_Fade(void)
	{ JMP_IFACE_FN(csgo_VClient, 28) }

	virtual void _UNUSED_SetCrosshairAngle(void)
	{ JMP_IFACE_FN(csgo_VClient, 29) }

	virtual void _UNUSED_InitSprite(void)
	{ JMP_IFACE_FN(csgo_VClient, 30) }

	virtual void _UNUSED_ShutdownSprite(void)
	{ JMP_IFACE_FN(csgo_VClient, 31) }

	virtual void _UNUSED_GetSpriteSize(void) const
	{ JMP_IFACE_FN(csgo_VClient, 32) }

	virtual void _UNUSED_VoiceStatus(void)
	{ JMP_IFACE_FN(csgo_VClient, 33) }

	virtual void _UNUSED_PlayerAudible(void)
	{ JMP_IFACE_FN(csgo_VClient, 34) }

	virtual void _UNUSED_InstallStringTableCallback(void)
	{ JMP_IFACE_FN(csgo_VClient, 35) }

	virtual void _UNUSED_FrameStageNotify(void)
	{ JMP_IFACE_FN(csgo_VClient, 36) }

	virtual void _UNUSED_DispatchUserMessage(void)
	{ JMP_IFACE_FN(csgo_VClient, 37) }

	virtual void _UNUSED_SaveInit(void)
	{ JMP_IFACE_FN(csgo_VClient, 38) }

	virtual void _UNUSED_SaveWriteFields(void)
	{ JMP_IFACE_FN(csgo_VClient, 39) }

	virtual void _UNUSED_SaveReadFields(void)
	{ JMP_IFACE_FN(csgo_VClient, 40) }

	virtual void _UNUSED_PreSave(void)
	{ JMP_IFACE_FN(csgo_VClient, 41) }

	virtual void _UNUSED_Save(void)
	{ JMP_IFACE_FN(csgo_VClient, 42) }

	virtual void _UNUSED_WriteSaveHeaders(void)
	{ JMP_IFACE_FN(csgo_VClient, 43) }

	virtual void _UNUSED_ReadRestoreHeaders(void)
	{ JMP_IFACE_FN(csgo_VClient, 44) }

	virtual void _UNUSED_Restore(void)
	{ JMP_IFACE_FN(csgo_VClient, 45) }

	virtual void _UNUSED_DispatchOnRestore(void)
	{ JMP_IFACE_FN(csgo_VClient, 46) }

	virtual void _UNUSED_GetStandardRecvProxies(void)
	{ JMP_IFACE_FN(csgo_VClient, 47) }

	virtual void _UNUSED_WriteSaveGameScreenshot(void)
	{ JMP_IFACE_FN(csgo_VClient, 48) }

	virtual void _UNUSED_EmitSentenceCloseCaption(void)
	{ JMP_IFACE_FN(csgo_VClient, 49) }

	virtual void _UNUSED_EmitCloseCaption(void)
	{ JMP_IFACE_FN(csgo_VClient, 50) }

	virtual void _UNUSED_CanRecordDemo(void) const
	{ JMP_IFACE_FN(csgo_VClient, 51) }

	virtual void _UNUSED_OnDemoRecordStart(void)
	{ JMP_IFACE_FN(csgo_VClient, 52) }

	virtual void _UNUSED_OnDemoRecordStop(void)
	{ JMP_IFACE_FN(csgo_VClient, 53) }

	virtual void _UNUSED_OnDemoPlaybackStart(void)
	{ JMP_IFACE_FN(csgo_VClient, 54) }

	virtual void _UNUSED_OnDemoPlaybackStop(void)
	{ JMP_IFACE_FN(csgo_VClient, 55) }

	virtual void _UNUSED_RecordDemoPolishUserInput(void)
	{ JMP_IFACE_FN(csgo_VClient, 56) }

	virtual void _UNUSED_CacheReplayRagdolls(void)
	{ JMP_IFACE_FN(csgo_VClient, 57) }

	virtual void _UNUSED_ReplayUI_SendMessage(void)
	{ JMP_IFACE_FN(csgo_VClient, 58) }

	virtual void _UNUSED_GetReplayFactory(void)
	{ JMP_IFACE_FN(csgo_VClient, 59) }

	virtual void _UNUSED_ClearLocalPlayerReplayPtr(void)
	{ JMP_IFACE_FN(csgo_VClient, 60) }

	virtual void _UNUSED_GetScreenWidth(void)
	{ JMP_IFACE_FN(csgo_VClient, 61) }

	virtual void _UNUSED_GetScreenHeight(void)
	{ JMP_IFACE_FN(csgo_VClient, 62) }

	virtual void _UNUSED_WriteSaveGameScreenshotOfSize(void)
	{ JMP_IFACE_FN(csgo_VClient, 63) }

	virtual void _UNUSED_WriteReplayScreenshotBadParams(void)
	{ JMP_IFACE_FN(csgo_VClient, 64) }

	virtual void _UNUSED_UpdateReplayScreenshotCache(void)
	{ JMP_IFACE_FN(csgo_VClient, 65) }

	virtual void _UNUSED_GetPlayerView(void)
	{ JMP_IFACE_FN(csgo_VClient, 66) }

	virtual void _UNUSED_ShouldHideLoadingPlaque(void)
	{ JMP_IFACE_FN(csgo_VClient, 67) }

	virtual void _UNUSED_InvalidateMdlCache(void)
	{ JMP_IFACE_FN(csgo_VClient, 68) }

	virtual void _UNUSED_IN_SetSampleTime(void)
	{ JMP_IFACE_FN(csgo_VClient, 69) }

	virtual void _UNUSED_OnActiveSplitscreenPlayerChanged(void)
	{ JMP_IFACE_FN(csgo_VClient, 70) }

	virtual void _UNUSED_OnSplitScreenStateChanged(void)
	{ JMP_IFACE_FN(csgo_VClient, 71) }

	virtual void _UNUSED_CenterStringOff(void)
	{ JMP_IFACE_FN(csgo_VClient, 72) }

	virtual void _UNUSED_OnScreenSizeChanged(void)
	{ JMP_IFACE_FN(csgo_VClient, 73) }

	virtual void _UNUSED_InstantiateMaterialProxy(void)
	{ JMP_IFACE_FN(csgo_VClient, 74) }

	virtual void _UNUSED_GetFullscreenClientDLLVPanel(void)
	{ JMP_IFACE_FN(csgo_VClient, 75) }

	virtual void _UNUSED_MarkEntitiesAsTouching(void)
	{ JMP_IFACE_FN(csgo_VClient, 76) }

	virtual void _UNUSED_OnKeyBindingChanged(void)
	{ JMP_IFACE_FN(csgo_VClient, 77) }

	virtual void _UNUSED_SetBlurFade(void)
	{ JMP_IFACE_FN(csgo_VClient, 78) }

	virtual void _UNUSED_ResetHudCloseCaption(void)
	{ JMP_IFACE_FN(csgo_VClient, 79) }

	virtual void _UNUSED_HandleGameUIEvent(void)
	{ JMP_IFACE_FN(csgo_VClient, 80) }

	virtual void _UNUSED_GetSoundSpatializationBadParams(void)
	{ JMP_IFACE_FN(csgo_VClient, 81) }

	virtual void _UNUSED_Hud_SaveStarted(void)
	{ JMP_IFACE_FN(csgo_VClient, 82) }

	virtual void _UNUSED_ShutdownMovies(void)
	{ JMP_IFACE_FN(csgo_VClient, 83) }

	virtual void _UNUSED_IsChatRaised(void)
	{ JMP_IFACE_FN(csgo_VClient, 84) }

	virtual void _UNUSED_IsRadioPanelRaised(void)
	{ JMP_IFACE_FN(csgo_VClient, 85) }

	virtual void _UNUSED_IsBindMenuRaised(void)
	{ JMP_IFACE_FN(csgo_VClient, 86) }

	virtual void _UNUSED_IsTeamMenuRaised(void)
	{ JMP_IFACE_FN(csgo_VClient, 87) }

	virtual void _UNUSED_IsLoadingScreenRaised(void)
	{ JMP_IFACE_FN(csgo_VClient, 88) }

	virtual void _UNKOWN_089(void)
	{ JMP_IFACE_FN(csgo_VClient, 89) }

	virtual void _UNKOWN_090(void)
	{ JMP_IFACE_FN(csgo_VClient, 90) }

	virtual void _UNKOWN_091(void)
	{ JMP_IFACE_FN(csgo_VClient, 91) }

	virtual void _UNKOWN_092(void)
	{ JMP_IFACE_FN(csgo_VClient, 92) }

	virtual void _UNKOWN_093(void)
	{ JMP_IFACE_FN(csgo_VClient, 93) }

	virtual void _UNKOWN_094(void)
	{ JMP_IFACE_FN(csgo_VClient, 94) }

	virtual void _UNKOWN_095(void)
	{ JMP_IFACE_FN(csgo_VClient, 95) }

	virtual void _UNKOWN_096(void)
	{ JMP_IFACE_FN(csgo_VClient, 96) }

	virtual void _UNKOWN_097(void)
	{ JMP_IFACE_FN(csgo_VClient, 97) }

	virtual void _UNKOWN_098(void)
	{ JMP_IFACE_FN(csgo_VClient, 98) }

	virtual void _UNKOWN_099(void)
	{ JMP_IFACE_FN(csgo_VClient, 99) }

	virtual void _UNKOWN_100(void)
	{ JMP_IFACE_FN(csgo_VClient, 100) }

	virtual void _UNKOWN_101(void)
	{ JMP_IFACE_FN(csgo_VClient, 101) }

	virtual void _UNKOWN_102(void)
	{ JMP_IFACE_FN(csgo_VClient, 102) }

	virtual void _UNKOWN_103(void)
	{ JMP_IFACE_FN(csgo_VClient, 103) }

	virtual void _UNKOWN_104(void)
	{ JMP_IFACE_FN(csgo_VClient, 104) }

	virtual void _UNKOWN_105(void)
	{ JMP_IFACE_FN(csgo_VClient, 105) }

	virtual void _UNKOWN_106(void)
	{ JMP_IFACE_FN(csgo_VClient, 106) }

	virtual void _UNKOWN_107(void)
	{ JMP_IFACE_FN(csgo_VClient, 107) }

	virtual void _UNKOWN_108(void)
	{ JMP_IFACE_FN(csgo_VClient, 108) }

	// and a few more to be save from updates:

	virtual void _UNKOWN_109(void)
	{ JMP_IFACE_FN(csgo_VClient, 109) }

	virtual void _UNKOWN_110(void)
	{ JMP_IFACE_FN(csgo_VClient, 110) }

	virtual void _UNKOWN_111(void)
	{ JMP_IFACE_FN(csgo_VClient, 111) }

	virtual void _UNKOWN_112(void)
	{ JMP_IFACE_FN(csgo_VClient, 112) }

	virtual void _UNKOWN_113(void)
	{ JMP_IFACE_FN(csgo_VClient, 113) }

	virtual void _UNKOWN_114(void)
	{ JMP_IFACE_FN(csgo_VClient, 114) }

	virtual void _UNKOWN_115(void)
	{ JMP_IFACE_FN(csgo_VClient, 115) }

	virtual void _UNKOWN_116(void)
	{ JMP_IFACE_FN(csgo_VClient, 116) }

	virtual void _UNKOWN_117(void)
	{ JMP_IFACE_FN(csgo_VClient, 117) }

	virtual void _UNKOWN_118(void)
	{ JMP_IFACE_FN(csgo_VClient, 118) }

	virtual void _UNKOWN_119(void)
	{ JMP_IFACE_FN(csgo_VClient, 119) }

	virtual void _UNKOWN_120(void)
	{ JMP_IFACE_FN(csgo_VClient, 120) }
} g_BaseClientDllWrapper_csgo;

#pragma warning(pop)

void HookClientDllInterface_011_Init(void * iface)
{
	old_Client_Init = HookInterfaceFn(iface, 0, (void *)hook_Client_Init);
}

CreateInterfaceFn old_Client_CreateInterface = 0;
bool isCsgo = false;

void* new_Client_CreateInterface(const char *pName, int *pReturnCode)
{
	static bool bFirstCall = true;

	void * pRet = old_Client_CreateInterface(pName, pReturnCode);

	if(bFirstCall)
	{
		bFirstCall = false;

		void * iface = NULL;
		
		if(!isCsgo)
		{
			if(iface = old_Client_CreateInterface(CLIENT_DLL_INTERFACE_VERSION_017, NULL)) {
				g_Info_VClient = CLIENT_DLL_INTERFACE_VERSION_017;
				HookClientDllInterface_011_Init(iface);
			}
			else if(iface = old_Client_CreateInterface(CLIENT_DLL_INTERFACE_VERSION_016, NULL)) {
				g_Info_VClient = CLIENT_DLL_INTERFACE_VERSION_016;
				HookClientDllInterface_011_Init(iface);
			}
			else if(iface = old_Client_CreateInterface(CLIENT_DLL_INTERFACE_VERSION_015, NULL)) {
				g_Info_VClient = CLIENT_DLL_INTERFACE_VERSION_015;
				HookClientDllInterface_011_Init(iface);
			}
			else if(iface = old_Client_CreateInterface(CLIENT_DLL_INTERFACE_VERSION_013, NULL)) {
				g_Info_VClient = CLIENT_DLL_INTERFACE_VERSION_013;
				HookClientDllInterface_011_Init(iface);
			}
			else if(iface = old_Client_CreateInterface(CLIENT_DLL_INTERFACE_VERSION_012, NULL)) {
				g_Info_VClient = CLIENT_DLL_INTERFACE_VERSION_012;
				HookClientDllInterface_011_Init(iface);
			}
			else if(iface = old_Client_CreateInterface(CLIENT_DLL_INTERFACE_VERSION_011, NULL)) {
				g_Info_VClient = CLIENT_DLL_INTERFACE_VERSION_011;
				HookClientDllInterface_011_Init(iface);
			}
			else
			{
				ErrorBox("Could not get a supported VClient interface.");
			}
		}
	}

	if(isCsgo && !strcmp(pName, CLIENT_DLL_INTERFACE_VERSION_CSGO_016))
	{
		if(!csgo_VClient)
		{
			g_Info_VClient = CLIENT_DLL_INTERFACE_VERSION_CSGO_016 " (CS:GO)";
			csgo_VClient = pRet;
		}

		pRet = &g_BaseClientDllWrapper_csgo;
	}

	return pRet;
}


HMODULE g_H_ClientDll = 0;

FARPROC WINAPI new_Engine_GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	FARPROC nResult;
	nResult = GetProcAddress(hModule, lpProcName);

	if (HIWORD(lpProcName))
	{
		if (!lstrcmp(lpProcName, "GetProcAddress"))
			return (FARPROC) &new_Engine_GetProcAddress;

		if (
			hModule == g_H_ClientDll
			&& !lstrcmp(lpProcName, "CreateInterface")
		) {
			old_Client_CreateInterface = (CreateInterfaceFn)nResult;
			return (FARPROC) &new_Client_CreateInterface;
		}

	}

	return nResult;


}


HMODULE WINAPI new_LoadLibraryA(LPCSTR lpLibFileName);
HMODULE WINAPI new_LoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);

void LibraryHooksA(HMODULE hModule, LPCSTR lpLibFileName)
{
	static bool bFirstRun = true;
	static bool bFirstClient = true;
	static bool bFirstEngine = true;
	static bool bFirstTier0 = true;
	//static bool bFirstGameOverlayRenderer = true;
	static bool bFirstLauncher = true;
	static bool bFirstfilesystem_stdio = true;
	static bool bFirstShaderapidx9 = true;
	static bool bFirstMaterialsystem = true;

	if(!hModule || !lpLibFileName)
		return;

#if 1
	static FILE *f1=NULL;

	if( !f1 ) f1=fopen("mdt_log_LibraryHooksA.txt","wb");
	fprintf(f1,"%s\n", lpLibFileName);
	fflush(f1);
#endif

	// do not use messageboxes here, there is some friggin hooking going on in between by the
	// Source engine.

	if(bFirstRun)
	{
		bFirstRun = false;

		// detect if we are csgo:

		char filePath[MAX_PATH] = { 0 };
		GetModuleFileName( 0, filePath, MAX_PATH );

		if(StringEndsWith(filePath,"csgo.exe"))
		{
			isCsgo = true;
		}
	}

	if(bFirstTier0)
	{
		HMODULE hTier0;
		if(hTier0 = GetModuleHandleA("tier0.dll"))
		{
			bFirstTier0 = false;

			Tier0_Msg = (Tier0MsgFn)GetProcAddress(hTier0, "Msg");
			Tier0_DMsg = (Tier0DMsgFn)GetProcAddress(hTier0, "DMsg");
			Tier0_Warning = (Tier0MsgFn)GetProcAddress(hTier0, "Warning");
			Tier0_DWarning = (Tier0DMsgFn)GetProcAddress(hTier0, "DWarning");
			Tier0_Log = (Tier0MsgFn)GetProcAddress(hTier0, "Log");
			Tier0_DLog = (Tier0DMsgFn)GetProcAddress(hTier0, "DLog");
			Tier0_Error = (Tier0MsgFn)GetProcAddress(hTier0, "Error");
			Tier0_ConMsg = (Tier0MsgFn)GetProcAddress(hTier0, "ConMsg");
			Tier0_ConWarning = (Tier0MsgFn)GetProcAddress(hTier0, "ConWarning");
			Tier0_ConLog = (Tier0MsgFn)GetProcAddress(hTier0, "ConLog");
		}
	}

	if(bFirstLauncher && StringEndsWith( lpLibFileName, "launcher.dll"))
	{
		bFirstLauncher = false;
		
		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryExA", (DWORD) &new_LoadLibraryExA);
		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryA", (DWORD) &new_LoadLibraryA);
	}
	else
	if(bFirstfilesystem_stdio && StringEndsWith( lpLibFileName, "filesystem_stdio.dll"))
	{
		bFirstfilesystem_stdio = false;
		
		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryExA", (DWORD) &new_LoadLibraryExA);
		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryA", (DWORD) &new_LoadLibraryA);
	}
	else
	if(bFirstEngine && StringEndsWith( lpLibFileName, "engine.dll"))
	{
		bFirstEngine = false;

		Addresses_InitEngineDll((AfxAddr)hModule, isCsgo);

		InterceptDllCall(hModule, "Kernel32.dll", "GetProcAddress", (DWORD) &new_Engine_GetProcAddress);
		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryExA", (DWORD) &new_LoadLibraryExA);
		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryA", (DWORD) &new_LoadLibraryA);

		// Init the hook early, so we don't run into issues with threading:
		Hook_csgo_SndMixTimeScalePatch();
	}
	else
	if(bFirstMaterialsystem && StringEndsWith( lpLibFileName, "materialsystem.dll"))
	{
		bFirstMaterialsystem = false;
		
		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryExA", (DWORD) &new_LoadLibraryExA);
		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryA", (DWORD) &new_LoadLibraryA);
	}
	else
	if(bFirstShaderapidx9 && StringEndsWith( lpLibFileName, "shaderapidx9.dll"))
	{
		bFirstShaderapidx9 = false;

		//old_Direct3DCreate9 = (Direct3DCreate9_t)InterceptDllCall(hModule, "d3d9.dll", "Direct3DCreate9", (DWORD) &new_Direct3DCreate9);
	}
	else
	if(bFirstClient && StringEndsWith( lpLibFileName, "client.dll"))
	{
		bFirstClient = false;

		g_H_ClientDll = hModule;

		Addresses_InitClientDll((AfxAddr)g_H_ClientDll, isCsgo);
	}
}


// i.e. called by Counter-Strike Source
HMODULE WINAPI new_LoadLibraryA( LPCSTR lpLibFileName ) {
	HMODULE hRet = LoadLibraryA(lpLibFileName);

	LibraryHooksA(hRet, lpLibFileName);

	return hRet;
}


// i.e. called by Portal First Slice
HMODULE WINAPI new_LoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
	HMODULE hRet = LoadLibraryExA(lpLibFileName, hFile, dwFlags);

	LibraryHooksA(hRet, lpLibFileName);

	return hRet;
}


bool WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason) 
	{ 
		case DLL_PROCESS_ATTACH:
		{
#if 0
			MessageBox(0,"DLL_PROCESS_ATTACH","MDT_DEBUG",MB_OK);
#endif

			if(!(
				InterceptDllCall(GetModuleHandle(NULL), "Kernel32.dll", "LoadLibraryExA", (DWORD) &new_LoadLibraryExA)
				||InterceptDllCall(GetModuleHandle(NULL), "Kernel32.dll", "LoadLibraryA", (DWORD) &new_LoadLibraryA)
			))
				ErrorBox();

			break;
		}
		case DLL_PROCESS_DETACH:
		{
			// source terminates the process, this will never get called

			break;
		}
		case DLL_THREAD_ATTACH:
		{
			break;
		}
		case DLL_THREAD_DETACH:
		{
			break;
		}
	}
	return true;
}