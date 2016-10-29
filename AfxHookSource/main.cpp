#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-08-30 dominik.matrixstorm.com
//
// First changes:
// 2009-09-29 dominik.matrixstorm.com


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
#include "d3d9Hooks.h"
#include "csgo_SndMixTimeScalePatch.h"
#include "csgo_CSkyBoxView.h"
#include "AfxHookSourceInput.h"
#include "AfxClasses.h"
#include "AfxStreams.h"
#include "hlaeFolder.h"
#include "CampathDrawer.h"
#include "csgo_ScaleForm_Hooks.h"
#include "asmClassTools.h"
#include "csgo_Stdshader_dx9_Hooks.h"
#include "AfxShaders.h"
#include "csgo_CViewRender.h"
#include "CommandSystem.h"
#include "csgo_writeWaveConsoleCheck.h"
#include "ClientTools.h"
#include "MatRenderContextHook.h"
#include "csgo_GameEventManager.h"

#include <set>
#include <map>
#include <string>

WrpVEngineClient * g_VEngineClient = 0;
SOURCESDK::ICvar_003 * g_Cvar = 0;


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
		"| AfxHookSource (" __DATE__ " " __TIME__ ")" "\n"
		"| http://advancedfx.org/" "\n"
		"|" "\n"
	);

	Tier0_Msg("| VClient: %s\n", g_Info_VClient);
	Tier0_Msg("| VEngineClient: %s\n", g_Info_VEngineClient);
	Tier0_Msg("| VEngineCvar: %s\n", g_Info_VEngineCvar);
	Tier0_Msg("| GameDirectory: %s\n", g_VEngineClient ? g_VEngineClient->GetGameDirectory() : "n/a");
	Tier0_Msg("| WrpConCommands::GetVEngineCvar007() == 0x%08x\n", WrpConCommands::GetVEngineCvar007());

	Tier0_Msg("|" "\n");
}

SOURCESDK::IClientEngineTools_001 * g_Engine_ClientEngineTools;

class ClientEngineTools : public SOURCESDK::IClientEngineTools_001
{
public:
	// TODO: we should call the destructor of g_Engine_ClientEngineTools on destuction?

	virtual void LevelInitPreEntityAllTools() { g_Engine_ClientEngineTools->LevelInitPreEntityAllTools(); }
	virtual void LevelInitPostEntityAllTools() { g_Engine_ClientEngineTools->LevelInitPostEntityAllTools(); }
	virtual void LevelShutdownPreEntityAllTools() { g_Engine_ClientEngineTools->LevelShutdownPreEntityAllTools(); }
	virtual void LevelShutdownPostEntityAllTools() { g_Engine_ClientEngineTools->LevelShutdownPostEntityAllTools(); }
	virtual void PreRenderAllTools()
	{
		//Tier0_Msg("ClientEngineTools::PreRenderAllTools\n");
		g_Engine_ClientEngineTools->PreRenderAllTools();
	}
	
	virtual void PostRenderAllTools()
	{
		// Warning: This can be called multiple times during a frame (i.e. for skybox view and normal world view)!

		//Tier0_Msg("ClientEngineTools::PostRenderAllTools\n");

		g_CampathDrawer.OnPostRenderAllTools();

		if(g_Hook_VClient_RenderView.IsInstalled())
		{
			g_CommandSystem.Do_Queue_Commands(g_Hook_VClient_RenderView.GetCurTime());
		}

		g_Engine_ClientEngineTools->PostRenderAllTools();
	}

	virtual void PostToolMessage(SOURCESDK::HTOOLHANDLE hEntity, SOURCESDK::KeyValues_something *msg )
	{
		g_ClientTools.OnPostToolMessage((SOURCESDK::CSGO::HTOOLHANDLE)hEntity, (SOURCESDK::CSGO::KeyValues *)msg);
		g_Engine_ClientEngineTools->PostToolMessage(hEntity, msg);
	}
	virtual void AdjustEngineViewport( int& x, int& y, int& width, int& height ) { g_Engine_ClientEngineTools->AdjustEngineViewport(x, y, width, height); }
	
	virtual bool SetupEngineView(SOURCESDK::Vector &origin, SOURCESDK::QAngle &angles, float &fov )
	{
		//Tier0_Msg("ClientEngineTools::SetupEngineView\n");
		bool bRet = g_Engine_ClientEngineTools->SetupEngineView(origin, angles, fov);

		g_Hook_VClient_RenderView.OnViewOverride(
			origin.x, origin.y, origin.z,
			angles.x, angles.y, angles.z,
			fov
		);

		return bRet;
	}
	
	virtual bool SetupAudioState(SOURCESDK::AudioState_t &audioState ) { return g_Engine_ClientEngineTools->SetupAudioState(audioState); }
	
	virtual void VGui_PreRenderAllTools( int paintMode )
	{
		//Tier0_Msg("ClientEngineTools::VGui_PreRenderAllTools\n");
		g_Engine_ClientEngineTools->VGui_PreRenderAllTools(paintMode);
	}
	
	virtual void VGui_PostRenderAllTools( int paintMode )
	{
		//Tier0_Msg("ClientEngineTools::VGui_PostRenderAllTools\n");
		g_Engine_ClientEngineTools->VGui_PostRenderAllTools(paintMode);
	}

	virtual bool IsThirdPersonCamera() { return g_Engine_ClientEngineTools->IsThirdPersonCamera(); }

	virtual bool InToolMode()
	{
		return g_Engine_ClientEngineTools->InToolMode() || g_ClientTools.GetRecording();
	}

} g_ClientEngineTools;

SOURCESDK::CreateInterfaceFn g_AppSystemFactory = 0;

bool isCsgo = false;
bool isV34 = false;

SOURCESDK::IMaterialSystem_csgo * g_MaterialSystem_csgo = 0;


#pragma warning(push)
#pragma warning(disable:4731) // frame pointer register 'ebp' modified by inline assembly code

class CAfxVRenderView : public SOURCESDK::IVRenderView_csgo, public IAfxVRenderView
{
public:
	CAfxVRenderView(SOURCESDK::IVRenderView_csgo * parent)
	: m_Parent(parent)
	{
	}

	//
	// IAfxVRenderView:

	virtual SOURCESDK::IVRenderView_csgo * GetParent()
	{
		return m_Parent;
	}

	//
	// IVRenderView_csgo:

	virtual void _UNKOWN_000(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 0) }

	virtual void _UNKOWN_001(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 1) }

	virtual void _UNKOWN_002(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 2) }

	virtual void _UNKOWN_003(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 3) }

	// 004:
	virtual void SetBlend( float blend )
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 4) }

	virtual float GetBlend( void )
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 5) }

	// 006:
	virtual void SetColorModulation( float const* blend )
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 6) }

	virtual void GetColorModulation( float* blend )
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 7) }

	virtual void _UNKOWN_008(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 8) }

	virtual void _UNKOWN_009(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 9) }

	virtual void _UNKOWN_010(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 10) }

	virtual void _UNKOWN_011(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 11) }

	virtual void _UNKOWN_012(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 12) }

	virtual void _UNKOWN_013(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 13) }

	virtual void _UNKOWN_014(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 14) }

	virtual void _UNKOWN_015(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 15) }

	virtual void _UNKOWN_016(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 16) }

	virtual void _UNKOWN_017(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 17) }

	virtual void _UNKOWN_018(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 18) }

	virtual void _UNKOWN_019(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 19) }

	virtual void _UNKOWN_020(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 20) }

	virtual void _UNKOWN_021(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 21) }

	virtual void _UNKOWN_022(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 22) }

	virtual void _UNKOWN_023(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 23) }

	virtual void _UNKOWN_024(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 24) }

	virtual void _UNKOWN_025(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 25) }

	virtual void _UNKOWN_026(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 26) }

	virtual void _UNKOWN_027(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 27) }

	virtual void _UNKOWN_028(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 28) }

	virtual void _UNKOWN_029(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 29) }

	virtual void _UNKOWN_030(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 30) }

	virtual void _UNKOWN_031(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 31) }

	virtual void _UNKOWN_032(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 32) }

	virtual void _UNKOWN_033(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 33) }

	virtual void _UNKOWN_034(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 34) }

	virtual void _UNKOWN_035(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 35) }

	virtual void _UNKOWN_036(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 36) }

	virtual void _UNKOWN_037(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 37) }

	virtual void _UNKOWN_038(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 38) }

	virtual void _UNKOWN_039(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 39) }

	virtual void _UNKOWN_040(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 40) }

	virtual void _UNKOWN_041(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 41) }

	virtual void _UNKOWN_042(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 42) }

	virtual void _UNKOWN_043(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 43) }

	virtual void _UNKOWN_044(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 44) }

	virtual void _UNKOWN_045(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 45) }

	virtual void _UNKOWN_046(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 46) }

	virtual void _UNKOWN_047(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 47) }

	virtual void _UNKOWN_048(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 48) }

	virtual void _UNKOWN_049(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 49) }

	virtual void _UNKOWN_050(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 50) }

	virtual void _UNKOWN_051(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 51) }

	virtual void _UNKOWN_052(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 52) }

	virtual void _UNKOWN_053(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 53) }

	virtual void _UNKOWN_054(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 54) }

	virtual void _UNKOWN_055(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 55) }

	virtual void _UNKOWN_056(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 56) }

	virtual void _UNKOWN_057(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 57) }

	virtual void _UNKOWN_058(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 58) }

	virtual void _UNKOWN_059(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 59) }

private:
	SOURCESDK::IVRenderView_csgo * m_Parent;
};

#pragma warning(pop)

//CAfxVRenderView * g_AfxVRenderView = 0;

SOURCESDK::IFileSystem_csgo * g_FileSystem_csgo = 0;

void AfxV34HookWindow(void);

void MySetup(SOURCESDK::CreateInterfaceFn appSystemFactory, WrpGlobals *pGlobals)
{
	static bool bFirstRun = true;

	if(bFirstRun)
	{
		bFirstRun = false;

		void *iface , *iface2;

		g_AppSystemFactory = appSystemFactory;

		if (!isCsgo && (iface = appSystemFactory(VENGINE_CLIENT_INTERFACE_VERSION_015, NULL)))
		{
			// This is not really 100% backward compatible, there is a problem with the CVAR interface or s.th..
			// But the guy that tested it wasn't available for further debugging, so I'll just leave it as
			// it is now. Will crash as soon as i.e. ExecuteCliendCmd is used, due to some crash
			// related to CVAR system.
			
			g_Info_VEngineClient = VENGINE_CLIENT_INTERFACE_VERSION_015;
			g_VEngineClient = new WrpVEngineClient_013((SOURCESDK::IVEngineClient_013 *)iface);
		}
		else
		if(isCsgo && (iface = appSystemFactory(VENGINE_CLIENT_INTERFACE_VERSION_014_CSGO, NULL)))
		{
			g_Info_VEngineClient = VENGINE_CLIENT_INTERFACE_VERSION_014_CSGO " (CS:GO)";
			g_VEngineClient = new WrpVEngineClient_014_csgo((SOURCESDK::IVEngineClient_014_csgo *)iface);
		}
		else
		if(iface = appSystemFactory(VENGINE_CLIENT_INTERFACE_VERSION_013, NULL))
		{
			g_Info_VEngineClient = VENGINE_CLIENT_INTERFACE_VERSION_013;
			g_VEngineClient = new WrpVEngineClient_013((SOURCESDK::IVEngineClient_013 *)iface);
		}
		else if(iface = appSystemFactory(VENGINE_CLIENT_INTERFACE_VERSION_012, NULL)) {
			g_Info_VEngineClient = VENGINE_CLIENT_INTERFACE_VERSION_012;
			g_VEngineClient = new WrpVEngineClient_012((SOURCESDK::IVEngineClient_012 *)iface);
		}
		else {
			ErrorBox("Could not get a supported VEngineClient interface.");
		}

		if((iface = appSystemFactory( CVAR_INTERFACE_VERSION_007, NULL )))
		{
			g_Info_VEngineCvar = CVAR_INTERFACE_VERSION_007;
			WrpConCommands::RegisterCommands((SOURCESDK::ICvar_007 *)iface);
		}
		else if((iface = appSystemFactory( VENGINE_CVAR_INTERFACE_VERSION_004, NULL )))
		{
			g_Info_VEngineCvar = VENGINE_CVAR_INTERFACE_VERSION_004;
			WrpConCommands::RegisterCommands((SOURCESDK::ICvar_004 *)iface);
		}
		else if(
			(iface = appSystemFactory( VENGINE_CVAR_INTERFACE_VERSION_003, NULL ))
			&& (iface2 = appSystemFactory(VENGINE_CLIENT_INTERFACE_VERSION_012, NULL))
		) {
			g_Info_VEngineCvar = VENGINE_CVAR_INTERFACE_VERSION_003 " & " VENGINE_CLIENT_INTERFACE_VERSION_012;
			WrpConCommands::RegisterCommands((SOURCESDK::ICvar_003 *)iface, (SOURCESDK::IVEngineClient_012 *)iface2);
		}
		else {
			ErrorBox("Could not get a supported VEngineCvar interface.");
		}

		if(iface = appSystemFactory(VCLIENTENGINETOOLS_INTERFACE_VERSION_001, NULL))
		{
			g_Engine_ClientEngineTools = (SOURCESDK::IClientEngineTools_001 *)iface;
		}
		else {
			ErrorBox("Could not get a supported VClientEngineTools interface.");
		}

		if(isCsgo)
		{
			if(iface = appSystemFactory(MATERIAL_SYSTEM_INTERFACE_VERSION_CSGO_80, NULL))
			{
				g_MaterialSystem_csgo = (SOURCESDK::IMaterialSystem_csgo *)iface;
				g_AfxStreams.OnMaterialSystem(g_MaterialSystem_csgo);
			}
			else {
				ErrorBox("Could not get a supported VMaterialSystem interface.");
			}

			//if(iface = appSystemFactory(VENGINE_RENDERVIEW_INTERFACE_VERSION_CSGO, NULL))
			//{
			//	g_AfxVRenderView = new CAfxVRenderView((SOURCESDK::IVRenderView_csgo *)iface);
			//}
			//else {
			//	ErrorBox("Could not get a supported VEngineRenderView interface.");
			//}

			if(iface = appSystemFactory(FILESYSTEM_INTERFACE_VERSION_CSGO_017, NULL))
			{
				g_FileSystem_csgo = (SOURCESDK::IFileSystem_csgo *)iface;
			}
			else {
				ErrorBox("Could not get a supported VFileSystem interface.");
			}

			if(iface = appSystemFactory(SHADERSHADOW_INTERFACE_VERSION_CSGO, NULL))
			{
				g_AfxStreams.OnShaderShadow((SOURCESDK::IShaderShadow_csgo *)iface);
			}
			else {
				ErrorBox("Could not get a supported ShaderShadow interface.");
			}
		}
		
		g_Hook_VClient_RenderView.Install(pGlobals);

		AfxV34HookWindow();

		PrintInfo();
	}
}

void* AppSystemFactory_ForClient(const char *pName, int *pReturnCode)
{
	if(!strcmp(VCLIENTENGINETOOLS_INTERFACE_VERSION_001, pName))
	{
		return &g_ClientEngineTools;
	}
	//else
	//if(isCsgo && !strcmp(VENGINE_RENDERVIEW_INTERFACE_VERSION_CSGO, pName) && g_AfxVRenderView)
	//{
	//	return g_AfxVRenderView;
	//}
	return g_AppSystemFactory(pName, pReturnCode);
}

void * old_Client_Init;

int __stdcall new_Client_Init(DWORD *this_ptr, SOURCESDK::CreateInterfaceFn appSystemFactory, SOURCESDK::CreateInterfaceFn physicsFactory, SOURCESDK::CGlobalVarsBase *pGlobals) {
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

bool g_DebugEnabled = false;


#pragma warning(push)
#pragma warning(disable:4731) // frame pointer register 'ebp' modified by inline assembly code

class CAfxBaseClientDll
: public SOURCESDK::IBaseClientDLL_csgo
, public IAfxBaseClientDll
, public IAfxFreeMaster
{
public:
	CAfxBaseClientDll(IBaseClientDLL_csgo * parent)
	: m_Parent(parent)
	, m_OnShutdown(0)
	, m_OnLevelShutdown(0)
	, m_OnView_Render(0)
	{
	}

	~CAfxBaseClientDll()
	{
	}

	//
	// IAfxFreeMaster:

	virtual void AfxFreeable_Register(IAfxFreeable * value)
	{
		m_FreeMaster.AfxFreeable_Register(value);
	}

	virtual void AfxFreeable_Unregister(IAfxFreeable * value)
	{
		m_FreeMaster.AfxFreeable_Unregister(value);
	}

	//
	// IAfxBaseClientDll:

	virtual IBaseClientDLL_csgo * GetParent()
	{
		return m_Parent;
	}

	virtual IAfxFreeMaster * GetFreeMaster()
	{
		return &m_FreeMaster;
	}

	virtual void OnShutdown_set(IAfxBaseClientDllShutdown * value)
	{
		m_OnShutdown = value;
	}

	virtual void OnLevelShutdown_set(IAfxBaseClientDllLevelShutdown * value)
	{
		m_OnLevelShutdown = value;
	}

	virtual void OnView_Render_set(IAfxBaseClientDllView_Render * value)
	{
		m_OnView_Render = value;
	}

	//
	// IBaseClientDll_csgo:

	virtual int Connect(SOURCESDK::CreateInterfaceFn appSystemFactory, SOURCESDK::CGlobalVarsBase *pGlobals )
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 0) }

	virtual void Disconnect()
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 1) }

	virtual int Init(SOURCESDK::CreateInterfaceFn appSystemFactory, SOURCESDK::CGlobalVarsBase *pGlobals )
	{
		// JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 2)

		static bool bFirstCall = true;

		if(bFirstCall)
		{
			bFirstCall = false;

			MySetup(appSystemFactory, new WrpGlobalsCsGo(pGlobals));
		}

		int result = m_Parent->Init(AppSystemFactory_ForClient, pGlobals);

		// Add file system search path for our assets:
		if(g_FileSystem_csgo)
		{
			std::string path(GetHlaeFolder());

			path.append("resources\\AfxHookSource\\assets\\csgo"); 

			g_FileSystem_csgo->AddSearchPath(path.c_str(), "GAME", SOURCESDK::PATH_ADD_TO_TAIL);
		}

		return result;
	}

	virtual void PostInit()
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 3) }

	virtual void Shutdown( void )
	{
		// JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 4)

		if(m_OnShutdown) m_OnShutdown->Shutdown(this);

		m_FreeMaster.AfxFree();

		m_Parent->Shutdown();
	}

	virtual void LevelInitPreEntity( char const* pMapName )
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 5) }

	virtual void LevelInitPostEntity( )
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 6) }

	virtual void LevelShutdown( void )
	{
		// JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 7)

		if(m_OnLevelShutdown) m_OnLevelShutdown->LevelShutdown(this);

		csgo_Stdshader_dx9_Hooks_OnLevelShutdown();

		g_AfxShaders.ReleaseUnusedShaders();

		m_Parent->LevelShutdown();
	}

	virtual void _UNKOWN_008(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 8) }

	virtual void _UNKOWN_009(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 9) }

	virtual void _UNKOWN_010(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 10) }

	virtual void _UNKOWN_011(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 11) }

	virtual void _UNKOWN_012(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 12) }

	virtual void _UNKOWN_013(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 13) }

	virtual void _UNKOWN_014(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 14) }

	virtual void _UNKOWN_015(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 15) }

	virtual void _UNKOWN_016(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 16) }

	virtual void _UNKOWN_017 (void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 17) }

	virtual void _UNKOWN_018 (void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 18) }

	virtual void _UNKOWN_019(void )
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 19) }

	virtual void _UNKOWN_020(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 20) }

	virtual void _UNKOWN_021 (void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 21) }
		
	virtual void _UNKOWN_022(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 22) }

	virtual void _UNKOWN_023(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 23) }

	virtual void _UNKOWN_024(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 24) }

	virtual void _UNKOWN_025(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 25) }

	virtual void View_Render(SOURCESDK::vrect_t_csgo *rect )
	{
		// JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 26)

		bool rectNull = rect->width == 0 || rect->height == 0;

		if(g_MaterialSystem_csgo && !rectNull)
		{

			if(m_OnView_Render)
			{
				m_OnView_Render->View_Render(this, rect);
			}
			else
			{
				m_Parent->View_Render( rect );
			}
		}
		else
		{
			m_Parent->View_Render( rect );
		}
	}

	virtual void RenderView( const SOURCESDK::CViewSetup_csgo &view, int nClearFlags, int whatToDraw )
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 27) }

	virtual void _UNKOWN_028(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 28) }

	virtual void _UNKOWN_029(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 29) }

	virtual void _UNKOWN_030(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 30) }

	virtual void _UNKOWN_031(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 31) }

	virtual void _UNKOWN_032(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 32) }

	virtual void _UNKOWN_033(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 33) }

	virtual void _UNKOWN_034(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 34) }

	virtual void _UNKOWN_035(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 35) }

	virtual void _UNKOWN_036(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 36) }

	virtual void _UNKOWN_037(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 37) }

	virtual void _UNKOWN_038(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 38) }

	virtual void _UNKOWN_039(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 39) }

	virtual void _UNKOWN_040(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 40) }

	virtual void _UNKOWN_041(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 41) }

	virtual void _UNKOWN_042(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 42) }

	virtual void _UNKOWN_043(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 43) }

	virtual void _UNKOWN_044(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 44) }

	virtual void _UNKOWN_045(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 45) }

	virtual void _UNKOWN_046(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 46) }

	virtual void _UNKOWN_047(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 47) }

	virtual void _UNKOWN_048(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 48) }

	virtual void _UNKOWN_049(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 49) }

	virtual void _UNKOWN_050(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 50) }

	virtual void _UNKOWN_051(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 51) }

	virtual void _UNKOWN_052(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 52) }

	virtual void _UNKOWN_053(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 53) }

	virtual void _UNKOWN_054(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 54) }

	virtual void _UNKOWN_055(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 55) }

	virtual void _UNKOWN_056(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 56) }

	virtual void _UNKOWN_057(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 57) }

	virtual void _UNKOWN_058(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 58) }

	virtual void _UNKOWN_059(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 59) }

	virtual void _UNKOWN_060(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 60) }

	virtual void _UNKOWN_061(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 61) }

	virtual void _UNKOWN_062(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 62) }

	virtual void _UNUSED_GetScreenWidth(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 63) }

	virtual void _UNUSED_GetScreenHeight(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 64) }

	virtual void WriteSaveGameScreenshotOfSize( const char *pFilename, int width, int height, bool bCreatePowerOf2Padded = false, bool bWriteVTF = false )
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 65) }

	virtual void _UNKOWN_066(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 66) }

	virtual void _UNKOWN_067(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 67) }

	virtual void _UNKOWN_068(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 68) }

	virtual void _UNKOWN_069(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 69) }

	virtual void _UNKOWN_070(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 70) }

	virtual void _UNKOWN_071(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 71) }

	virtual void _UNKOWN_072(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 72) }

	virtual void _UNKOWN_073(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 73) }

	virtual void _UNKOWN_074(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 74) }

	virtual void _UNKOWN_075(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 75) }

	virtual void _UNKOWN_076(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 76) }

	virtual void _UNKOWN_077(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 77) }

	virtual void _UNKOWN_078(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 78) }

	virtual void _UNKOWN_079(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 79) }

	virtual void _UNKOWN_080(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 80) }

	virtual void _UNKOWN_081(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 81) }

	virtual void _UNKOWN_082(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 82) }

	virtual void _UNKOWN_083(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 83) }

	virtual void _UNKOWN_084(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 84) }

	virtual void _UNKOWN_085(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 85) }

	virtual void _UNKOWN_086(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 86) }

	virtual void _UNKOWN_087(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 87) }

	virtual void _UNKOWN_088(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 88) }

	virtual void _UNKOWN_089(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 89) }

	virtual void _UNKOWN_090(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 90) }

	virtual void _UNKOWN_091(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 91) }

	virtual void _UNKOWN_092(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 92) }

	virtual void _UNKOWN_093(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 93) }

	virtual void _UNKOWN_094(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 94) }

	virtual void _UNKOWN_095(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 95) }

	virtual void _UNKOWN_096(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 96) }

	virtual void _UNKOWN_097(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 97) }

	virtual void _UNKOWN_098(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 98) }

	virtual void _UNKOWN_099(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 99) }

	virtual void _UNKOWN_100(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 100) }

	virtual void _UNKOWN_101(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 101) }

	virtual void _UNKOWN_102(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 102) }

	virtual void _UNKOWN_103(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 103) }

	virtual void _UNKOWN_104(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 104) }

	virtual void _UNKOWN_105(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 105) }

	virtual void _UNKOWN_106(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 106) }

	virtual void _UNKOWN_107(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 107) }

	virtual void _UNKOWN_108(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 108) }

	virtual void _UNKOWN_109(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 109) }

	virtual void _UNKOWN_110(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 110) }

	// and a few more to be save from updates:

	virtual void _UNKOWN_111(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 111) }

	virtual void _UNKOWN_112(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 112) }

	virtual void _UNKOWN_113(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 113) }

	virtual void _UNKOWN_114(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 114) }

	virtual void _UNKOWN_115(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 115) }

	virtual void _UNKOWN_116(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 116) }

	virtual void _UNKOWN_117(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 117) }

	virtual void _UNKOWN_118(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 118) }

	virtual void _UNKOWN_119(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 119) }

	virtual void _UNKOWN_120(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 120) }

	virtual void _UNKOWN_121(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 121) }

	virtual void _UNKOWN_122(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 122) }

	virtual void _UNKOWN_123(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 123) }

	virtual void _UNKOWN_124(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 124) }

	virtual void _UNKOWN_125(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 125) }

	virtual void _UNKOWN_126(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 126) }

	virtual void _UNKOWN_127(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 127) }

	virtual void _UNKOWN_128(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 128) }

	virtual void _UNKOWN_129(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 129) }

	virtual void _UNKOWN_130(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 130) }

private:
	IBaseClientDLL_csgo * m_Parent;
	IAfxBaseClientDllShutdown * m_OnShutdown;
	IAfxBaseClientDllLevelShutdown * m_OnLevelShutdown;
	IAfxBaseClientDllView_Render * m_OnView_Render;
	CAfxFreeMaster m_FreeMaster;
};

CAfxBaseClientDll * g_AfxBaseClientDll = 0;

#pragma warning(pop)

void HookClientDllInterface_011_Init(void * iface)
{
	old_Client_Init = HookInterfaceFn(iface, 0, (void *)hook_Client_Init);
}

SOURCESDK::IClientEntityList_csgo * SOURCESDK::g_Entitylist_csgo = 0;

SOURCESDK::CreateInterfaceFn old_Client_CreateInterface = 0;

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
			if (iface = old_Client_CreateInterface(CLIENT_DLL_INTERFACE_VERSION_018, NULL)) {
				g_Info_VClient = CLIENT_DLL_INTERFACE_VERSION_018;
				HookClientDllInterface_011_Init(iface);
			}
			else
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
		else
		{
			// isCsgo.

			SOURCESDK::g_Entitylist_csgo = (SOURCESDK::IClientEntityList_csgo *)old_Client_CreateInterface(VCLIENTENTITYLIST_INTERFACE_VERSION_CSGO, NULL);

			g_ClientTools.SetClientTools((SOURCESDK::CSGO::IClientTools *)old_Client_CreateInterface(SOURCESDK_CSGO_VCLIENTTOOLS_INTERFACE_VERSION, NULL));
		}
	}

	if(isCsgo && !strcmp(pName, CLIENT_DLL_INTERFACE_VERSION_CSGO_018))
	{
		if(!g_AfxBaseClientDll)
		{
			g_Info_VClient = CLIENT_DLL_INTERFACE_VERSION_CSGO_018 " (CS:GO)";
			g_AfxBaseClientDll = new CAfxBaseClientDll((SOURCESDK::IBaseClientDLL_csgo *)pRet);
			g_AfxStreams.OnAfxBaseClientDll(g_AfxBaseClientDll);
		}

		pRet = g_AfxBaseClientDll;
	}

	return pRet;
}


HMODULE g_H_ClientDll = 0;

FARPROC WINAPI new_Engine_GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	FARPROC nResult;
	nResult = GetProcAddress(hModule, lpProcName);

	if (!nResult)
		return nResult;

	if (HIWORD(lpProcName))
	{
		if (!lstrcmp(lpProcName, "GetProcAddress"))
			return (FARPROC) &new_Engine_GetProcAddress;

		if (
			hModule == g_H_ClientDll
			&& !lstrcmp(lpProcName, "CreateInterface")
		) {
			old_Client_CreateInterface = (SOURCESDK::CreateInterfaceFn)nResult;
			return (FARPROC) &new_Client_CreateInterface;
		}

	}

	return nResult;
}

WNDPROC g_NextWindProc;
static bool g_afxWindowProcSet = false;

LRESULT CALLBACK new_Afx_WindowProc(
	__in HWND hwnd,
	__in UINT uMsg,
	__in WPARAM wParam,
	__in LPARAM lParam
)
{
	switch(uMsg)
	{
	case WM_ACTIVATE:
		g_AfxHookSourceInput.Supply_Focus(LOWORD(wParam) != 0);
		break;
	case WM_CHAR:
		if(g_AfxHookSourceInput.Supply_CharEvent(wParam, lParam))
			return 0;
		break;
	case WM_KEYDOWN:
		if(g_AfxHookSourceInput.Supply_KeyEvent(AfxHookSourceInput::KS_DOWN, wParam, lParam))
			return 0;
		break;
	case WM_KEYUP:
		if(g_AfxHookSourceInput.Supply_KeyEvent(AfxHookSourceInput::KS_UP,wParam, lParam))
			return 0;
		break;
	case WM_INPUT:
		{
			HRAWINPUT hRawInput = (HRAWINPUT)lParam;
			RAWINPUT inp;
			UINT size = sizeof(inp);

			GetRawInputData(hRawInput, RID_INPUT, &inp, &size, sizeof(RAWINPUTHEADER));

			if(inp.header.dwType == RIM_TYPEMOUSE)
			{
				RAWMOUSE * rawmouse = &inp.data.mouse;
				LONG dX, dY;

				if((rawmouse->usFlags & 0x01) == MOUSE_MOVE_RELATIVE)
				{
					dX = rawmouse->lLastX;
					dY = rawmouse->lLastY;
				}
				else
				{
					static bool initial = true;
					static LONG lastX = 0;
					static LONG lastY = 0;

					if(initial)
					{
						initial = false;
						lastX = rawmouse->lLastX;
						lastY = rawmouse->lLastY;
					}

					dX = rawmouse->lLastX -lastX;
					dY = rawmouse->lLastY -lastY;

					lastX = rawmouse->lLastX;
					lastY = rawmouse->lLastY;
				}

				if(g_AfxHookSourceInput.Supply_RawMouseMotion(dX,dY))
					return 0;
			}
		}
		break;
	}

	return CallWindowProcW(g_NextWindProc, hwnd, uMsg, wParam, lParam);
}


// TODO: this is risky, actually we should track the hWnd maybe.
LONG WINAPI new_GetWindowLongW(
	__in HWND hWnd,
	__in int nIndex
)
{
	if(nIndex == GWL_WNDPROC)
	{
		if(g_afxWindowProcSet)
		{
			return (LONG)g_NextWindProc;
		}
	}

	return GetWindowLongW(hWnd, nIndex);
}

// TODO: this is risky, actually we should track the hWnd maybe.
LONG WINAPI new_SetWindowLongW(
	__in HWND hWnd,
	__in int nIndex,
	__in LONG dwNewLong
)
{
	if(nIndex == GWL_WNDPROC)
	{
		LONG lResult = SetWindowLongW(hWnd, nIndex, (LONG)new_Afx_WindowProc);

		if(!g_afxWindowProcSet)
		{
			g_afxWindowProcSet = true;
		}
		else
		{
			lResult = (LONG)g_NextWindProc;
		}

		g_NextWindProc = (WNDPROC)dwNewLong;

		return lResult;
	}

	return SetWindowLongW(hWnd, nIndex, dwNewLong);
}

BOOL WINAPI new_GetCursorPos(
	__out LPPOINT lpPoint
)
{
	BOOL result = GetCursorPos(lpPoint);

	g_AfxHookSourceInput.Supply_GetCursorPos(lpPoint);

	return result;
}

BOOL WINAPI new_SetCursorPos(
	__in int X,
	__in int Y
)
{
	g_AfxHookSourceInput.Supply_SetCursorPos(X,Y);

	return SetCursorPos(X,Y);
}

void AfxV34HookWindow(void)
{
	if(isV34)
	{
		HWND hWnd = FindWindowW(L"Valve001",NULL);

		if(!hWnd)
			ErrorBox("AfxV34HookWindow: FindWindowW failed.");
		else
			g_NextWindProc = (WNDPROC)SetWindowLongW(hWnd,GWL_WNDPROC,(LONG)new_Afx_WindowProc);
	}
}

FARPROC WINAPI new_shaderapidx9_GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	FARPROC nResult;
	nResult = GetProcAddress(hModule, lpProcName);

	if (!nResult)
		return nResult; // This can happen on Windows XP for Direct3DCreateEx9

	if (HIWORD(lpProcName))
	{
		if (
			!lstrcmp(lpProcName, "Direct3DCreate9Ex")
		) {
			old_Direct3DCreate9Ex = (Direct3DCreate9Ex_t)nResult;
			return (FARPROC) &new_Direct3DCreate9Ex;
		}
	}

	return nResult;
}

HMODULE WINAPI new_LoadLibraryA(LPCSTR lpLibFileName);
HMODULE WINAPI new_LoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);

extern HMODULE g_H_EngineDll;

void LibraryHooksA(HMODULE hModule, LPCSTR lpLibFileName)
{
	static bool bFirstRun = true;
	static bool bFirstClient = true;
	static bool bFirstEngine = true;
	static bool bFirstInputsystem = true;
	static bool bFirstTier0 = true;
	//static bool bFirstGameOverlayRenderer = true;
	static bool bFirstLauncher = true;
	static bool bFirstfilesystem_stdio = true;
	static bool bFirstShaderapidx9 = true;
	static bool bFirstMaterialsystem = true;
	static bool bFirstScaleformui = true;
	static bool bFirstStdshader_dx9 = true;

	if(!hModule || !lpLibFileName)
		return;

#if 0
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

		if(wcsstr(GetCommandLineW(),L"-afxV34"))
		{
			isV34 = true;
		}
		
		//ScriptEngine_StartUp();
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

		if(isV34)
		{
			InterceptDllCall(hTier0, "USER32.dll", "GetCursorPos", (DWORD) &new_GetCursorPos);
			InterceptDllCall(hTier0, "USER32.dll", "SetCursorPos", (DWORD) &new_SetCursorPos);
		}

	}

	if(bFirstLauncher && StringEndsWith( lpLibFileName, "launcher.dll"))
	{
		bFirstLauncher = false;
		
		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryExA", (DWORD) &new_LoadLibraryExA);
		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryA", (DWORD) &new_LoadLibraryA);
	}
	else
	if(bFirstfilesystem_stdio && StringEndsWith( lpLibFileName, "filesystem_steam.dll")) // v34
	{
		bFirstfilesystem_stdio = false;
		
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

		g_H_EngineDll = hModule;

		Addresses_InitEngineDll((AfxAddr)hModule, isCsgo);

		InterceptDllCall(hModule, "Kernel32.dll", "GetProcAddress", (DWORD) &new_Engine_GetProcAddress);
		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryExA", (DWORD) &new_LoadLibraryExA);
		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryA", (DWORD) &new_LoadLibraryA);

		// actually this is not required, since engine.dll calls first and thus is lower in the chain:
		InterceptDllCall(hModule, "USER32.dll", "GetWindowLongW", (DWORD) &new_GetWindowLongW);
		InterceptDllCall(hModule, "USER32.dll", "SetWindowLongW", (DWORD) &new_SetWindowLongW);

		if(isV34)
		{
			InterceptDllCall(hModule, "USER32.dll", "GetCursorPos", (DWORD) &new_GetCursorPos);
			InterceptDllCall(hModule, "USER32.dll", "SetCursorPos", (DWORD) &new_SetCursorPos);
		}

		// Init the hook early, so we don't run into issues with threading:
		Hook_csgo_SndMixTimeScalePatch();
		//csgo_GameEventManager_Install();
	}
	else
	if(bFirstInputsystem && StringEndsWith( lpLibFileName, "inputsystem.dll"))
	{
		bFirstInputsystem = false;

		InterceptDllCall(hModule, "USER32.dll", "GetWindowLongW", (DWORD) &new_GetWindowLongW);
		InterceptDllCall(hModule, "USER32.dll", "SetWindowLongW", (DWORD) &new_SetWindowLongW);

		InterceptDllCall(hModule, "USER32.dll", "GetCursorPos", (DWORD) &new_GetCursorPos);
		InterceptDllCall(hModule, "USER32.dll", "SetCursorPos", (DWORD) &new_SetCursorPos);
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

		InterceptDllCall(hModule, "kernel32.dll", "GetProcAddress", (DWORD) &new_shaderapidx9_GetProcAddress);

		old_Direct3DCreate9 = (Direct3DCreate9_t)InterceptDllCall(hModule, "d3d9.dll", "Direct3DCreate9", (DWORD) &new_Direct3DCreate9);
	}
	else
	if(bFirstClient && StringEndsWith( lpLibFileName, "client.dll"))
	{
		bFirstClient = false;

		g_H_ClientDll = hModule;

		Addresses_InitClientDll((AfxAddr)g_H_ClientDll, isCsgo);

		//
		// Install early hooks:

		csgo_CSkyBoxView_Draw_Install();
		csgo_CViewRender_Install();
		Hook_csgo_writeWaveConsoleCheck();
		Hook_C_BaseEntity_ToolRecordEnties();
	}
	else
	if(bFirstScaleformui && StringEndsWith( lpLibFileName, "scaleformui.dll"))
	{
		bFirstScaleformui = false;

		Addresses_InitScaleformuiDll((AfxAddr)hModule, isCsgo);

		//
		// Install early hooks:

		csgo_ScaleFormDll_Hooks_Init();
	}
	else
	if(bFirstStdshader_dx9 && StringEndsWith( lpLibFileName, "stdshader_dx9.dll"))
	{
		bFirstStdshader_dx9 = false;

		Addresses_InitStdshader_dx9Dll((AfxAddr)hModule, isCsgo);

		//
		// Install early hooks:

		csgo_Stdshader_dx9_Hooks_Init();
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

			//
			// Remember we are not on the main program thread here,
			// instead we are on our own thread, so don't run
			// things here that would have problems with that.
			//

			break;
		}
		case DLL_PROCESS_DETACH:
		{
			// actually this gets called now.

			MatRenderContextHook_Shutdown();

			if(g_AfxBaseClientDll) { delete g_AfxBaseClientDll; g_AfxBaseClientDll = 0; }

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
