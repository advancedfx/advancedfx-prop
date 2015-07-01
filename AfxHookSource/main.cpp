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
#include "AfxHookSourceInput.h"
#include "AfxClasses.h"
#include "AfxStreams.h"

//#include <string.h>

#include <map>
#include <set>


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
	__asm mov ecx, iface \
	__asm mov eax, [ecx] \
	__asm mov eax, [eax +4*index] \
	__asm jmp eax

#define JMP_CLASSMEMBERIFACE_FN(classType,classMemberIface,index) \
	__asm mov eax, this \
	__asm mov eax, [eax]classType.classMemberIface \
	__asm pop ecx \
	__asm mov esp, ebp \
	__asm pop ebp \
	__asm mov ecx, eax \
	__asm mov eax, [ecx] \
	__asm mov eax, [eax +4*index] \
	__asm jmp eax



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
	Tier0_Msg("| WrpConCommands::GetVEngineCvar007() == 0x%08x\n", WrpConCommands::GetVEngineCvar007());

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
			angles.x, angles.y, angles.z,
			fov
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

bool isCsgo = false;

IMaterialSystem_csgo * g_MaterialSystem_csgo = 0;


#pragma warning(push)
#pragma warning(disable:4731) // frame pointer register 'ebp' modified by inline assembly code

class CAfxVRenderView : public IVRenderView_csgo, public IAfxVRenderView
{
public:
	CAfxVRenderView(IVRenderView_csgo * parent)
	: m_Parent(parent)
	, m_OnSetColorModulation(0)
	{
	}

	//
	// IAfxVRenderView:

	virtual IVRenderView_csgo * GetParent()
	{
		return m_Parent;
	}

	virtual void OnSetColorModulation_set(IAfxVRenderViewSetColorModulation * value)
	{
		m_OnSetColorModulation = value;
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

	virtual void _UNKOWN_004(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 4) }

	virtual void _UNKOWN_005(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxVRenderView, m_Parent, 5) }

	// 006:
	virtual void SetColorModulation( float const* blend )
	{
		if(m_OnSetColorModulation)
		{
			m_OnSetColorModulation->SetColorModulation(this, blend);
		}
		else
		{
			m_Parent->SetColorModulation(blend);
		}
	}

	virtual void _UNKOWN_007(void)
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
	IVRenderView_csgo * m_Parent;
	IAfxVRenderViewSetColorModulation * m_OnSetColorModulation;
};

#pragma warning(pop)

CAfxVRenderView * g_AfxVRenderView = 0;

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

		if(isCsgo)
		{
			if(iface = appSystemFactory(MATERIAL_SYSTEM_INTERFACE_VERSION_CSGO_80, NULL))
			{
				g_MaterialSystem_csgo = (IMaterialSystem_csgo *)iface;
				g_AfxStreams.OnMaterialSystem(g_MaterialSystem_csgo);
			}
			else {
				ErrorBox("Could not get a supported VMaterialSystem interface.");
			}

			if(iface = appSystemFactory(VENGINE_RENDERVIEW_INTERFACE_VERSION_CSGO_013, NULL))
			{
				g_AfxVRenderView = new CAfxVRenderView((IVRenderView_csgo *)iface);
				g_AfxStreams.OnAfxVRenderView(g_AfxVRenderView);
			}
			else {
				ErrorBox("Could not get a supported VEngineRenderView interface.");
			}				
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
	else
	if(isCsgo && !strcmp(VENGINE_RENDERVIEW_INTERFACE_VERSION_CSGO_013, pName))
	{
		return g_AfxVRenderView;
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

bool g_CollectMaterialInfo = false;

std::set<CAfxMaterialKey> g_MaterialInfoSet_csgo;

void PrintMaterialInfoSetToFile(void)
{
	FILE *f1=NULL;

	f1=fopen("afx_material_info.txt","wb");

	if(!f1) return;

	for(std::set<CAfxMaterialKey>::iterator it = g_MaterialInfoSet_csgo.begin(); it != g_MaterialInfoSet_csgo.end(); ++it)
	{
		fprintf(f1,"%s|%s\n", it->GetMaterial()->GetTextureGroupName(), it->GetMaterial()->GetName());
	}

	fclose(f1);
}

#pragma warning(push)
#pragma warning(disable:4731) // frame pointer register 'ebp' modified by inline assembly code

class CAfxMatRenderContext : public IMatRenderContext_csgo, public IAfxMatRenderContext
{
public:
	CAfxMatRenderContext(IMatRenderContext_csgo * parent)
	: m_Parent(parent)
	, m_OnBind(0)
	, m_OnDrawInstances(0)
	{
	}

	//
	// IAfxMatRenderContext

	virtual IMatRenderContext_csgo * GetParent()
	{
		return m_Parent;
	}

	virtual void OnBind_set(IAfxMatRenderContextBind * value)
	{
		m_OnBind = value;
	}

	virtual void OnDrawInstances_set(IAfxMatRenderContextDrawInstances * value)
	{
		m_OnDrawInstances = value;
	}


	//
	// IMatRenderContext_csgo:

	// 000:
	virtual int AddRef()
	{
		return m_Parent->AddRef();
	}

	// 001:
	virtual int Release()
	{
		return m_Parent->Release();
	}

	// 002:
	virtual void BeginRender()
	{
		m_Parent->BeginRender();
	}

	// 003:
	virtual void EndRender()
	{
		m_Parent->EndRender();
	}

	// 004;
	virtual void Flush( bool flushHardware = false )
	{
		m_Parent->Flush(flushHardware);
	}

	virtual void _UNKNOWN_005(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 5) }

	virtual void _UNKNOWN_006(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 6) }

	virtual void _UNKNOWN_007(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 7) }

	virtual void _UNUSED_008_GetRenderTargetDimensions(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 8) }

	virtual void Bind( IMaterial_csgo * material, void *proxyData = 0 )
	{
		if(g_CollectMaterialInfo)
		{
			CAfxMaterialKey key(material);
			g_MaterialInfoSet_csgo.insert(key);
		}

		if(m_OnBind)
		{
			m_OnBind->Bind(this, material, proxyData);
		}
		else
		{
			m_Parent->Bind(material, proxyData);
		}
	}

	virtual void _UNKNOWN_010(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 10) }

	virtual void _UNKNOWN_011(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 11) }

	virtual void _UNKNOWN_012(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 12) }

	virtual void _UNKNOWN_013(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 13) }

	virtual void _UNKNOWN_014(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 14) }

	virtual void _UNKNOWN_015(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 15) }

	virtual void _UNKNOWN_016(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 16) }

	virtual void _UNKNOWN_017(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 17) }

	virtual void _UNKNOWN_018(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 18) }

	virtual void _UNKNOWN_019(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 19) }

	virtual void _UNKNOWN_020(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 20) }

	virtual void _UNKNOWN_021(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 21) }

	virtual void _UNKNOWN_022(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 22) }

	virtual void _UNKNOWN_023(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 23) }

	virtual void _UNKNOWN_024(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 24) }

	virtual void _UNKNOWN_025(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 25) }

	virtual void _UNKNOWN_026(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 26) }

	virtual void _UNKNOWN_027(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 27) }

	virtual void _UNKNOWN_028(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 28) }

	virtual void _UNKNOWN_029(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 29) }

	virtual void _UNKNOWN_030(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 30) }

	virtual void _UNKNOWN_031(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 31) }

	virtual void _UNKNOWN_032(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 32) }

	virtual void _UNKNOWN_033(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 33) }

	virtual void _UNKNOWN_034(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 34) }

	virtual void _UNKNOWN_035(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 35) }

	virtual void _UNKNOWN_036(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 36) }

	virtual void _UNKNOWN_037(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 37) }

	virtual void _UNKNOWN_038(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 38) }

	virtual void _UNKNOWN_039(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 39) }

	virtual void _UNKNOWN_040(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 40) }

	virtual void _UNKNOWN_041(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 41) }

	virtual void _UNKNOWN_042(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 42) }

	virtual void _UNKNOWN_043(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 43) }

	virtual void _UNKNOWN_044(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 44) }

	virtual void _UNKNOWN_045(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 45) }

	virtual void _UNKNOWN_046(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 46) }

	virtual void _UNKNOWN_047(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 47) }

	virtual void _UNKNOWN_048(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 48) }

	virtual void _UNKNOWN_049(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 49) }

	virtual void _UNKNOWN_050(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 50) }

	virtual void _UNKNOWN_051(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 51) }

	virtual void _UNKNOWN_052(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 52) }

	virtual void _UNKNOWN_053(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 53) }

	virtual void _UNKNOWN_054(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 54) }

	virtual void _UNKNOWN_055(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 55) }

	virtual void _UNKNOWN_056(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 56) }

	virtual void _UNKNOWN_057(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 57) }

	virtual void _UNKNOWN_058(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 58) }

	virtual void _UNKNOWN_059(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 59) }

	virtual void _UNKNOWN_060(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 60) }

	virtual void _UNKNOWN_061(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 61) }

	virtual void _UNKNOWN_062(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 62) }

	virtual void _UNKNOWN_063(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 63) }

	virtual void _UNKNOWN_064(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 64) }

	virtual void _UNKNOWN_065(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 65) }

	virtual void _UNKNOWN_066(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 66) }

	virtual void _UNKNOWN_067(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 67) }

	virtual void _UNKNOWN_068(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 68) }

	virtual void _UNKNOWN_069(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 69) }

	virtual void _UNKNOWN_070(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 70) }

	virtual void _UNKNOWN_071(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 71) }

	virtual void _UNKNOWN_072(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 72) }

	virtual void _UNKNOWN_073(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 73) }

	virtual void _UNKNOWN_074(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 74) }

	virtual void _UNKNOWN_075(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 75) }

	virtual void _UNKNOWN_076(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 76) }

	virtual void _UNKNOWN_077(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 77) }

	virtual void _UNKNOWN_078(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 78) }

	virtual void _UNKNOWN_079(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 79) }

	virtual void _UNKNOWN_080(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 80) }

	virtual void _UNKNOWN_081(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 81) }

	virtual void _UNKNOWN_082(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 82) }

	virtual void _UNKNOWN_083(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 83) }

	virtual void _UNKNOWN_084(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 84) }

	virtual void _UNKNOWN_085(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 85) }

	virtual void _UNKNOWN_086(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 86) }

	virtual void _UNKNOWN_087(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 87) }

	virtual void _UNKNOWN_088(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 88) }

	virtual void _UNKNOWN_089(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 89) }

	virtual void _UNKNOWN_090(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 90) }

	virtual void _UNKNOWN_091(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 91) }

	virtual void _UNKNOWN_092(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 92) }

	virtual void _UNKNOWN_093(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 93) }

	virtual void _UNKNOWN_094(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 94) }

	virtual void _UNKNOWN_095(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 95) }

	virtual void _UNKNOWN_096(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 96) }

	virtual void _UNKNOWN_097(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 97) }

	virtual void _UNKNOWN_098(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 98) }

	virtual void _UNKNOWN_099(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 99) }

	virtual void _UNKNOWN_100(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 100) }

	virtual void _UNKNOWN_101(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 101) }

	virtual void _UNKNOWN_102(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 102) }

	virtual void _UNKNOWN_103(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 103) }

	virtual void _UNKNOWN_104(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 104) }

	virtual void _UNKNOWN_105(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 105) }

	virtual void _UNKNOWN_106(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 106) }

	virtual void _UNKNOWN_107(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 107) }

	virtual void _UNKNOWN_108(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 108) }

	virtual void _UNKNOWN_109(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 109) }

	virtual void _UNKNOWN_110(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 110) }

	virtual void _UNKNOWN_111(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 111) }

	virtual void _UNKNOWN_112(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 112) }

	virtual void _UNKNOWN_113(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 113) }

	virtual void _UNKNOWN_114(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 114) }

	virtual void _UNKNOWN_115(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 115) }

	virtual void _UNKNOWN_116(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 116) }

	virtual void _UNKNOWN_117(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 117) }

	virtual void _UNKNOWN_118(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 118) }

	virtual void _UNKNOWN_119(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 119) }

	virtual void _UNKNOWN_120(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 120) }

	virtual void _UNKNOWN_121(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 121) }

	virtual void _UNKNOWN_122(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 122) }

	virtual void _UNKNOWN_123(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 123) }

	virtual void _UNKNOWN_124(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 124) }

	virtual void _UNKNOWN_125(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 125) }

	virtual void _UNKNOWN_126(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 126) }

	virtual void _UNKNOWN_127(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 127) }

	virtual void _UNKNOWN_128(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 128) }

	virtual void _UNKNOWN_129(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 129) }

	virtual void _UNKNOWN_130(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 130) }

	virtual void _UNKNOWN_131(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 131) }

	virtual void _UNKNOWN_132(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 132) }

	virtual void _UNKNOWN_133(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 133) }

	virtual void _UNKNOWN_134(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 134) }

	virtual void _UNKNOWN_135(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 135) }

	virtual void _UNKNOWN_136(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 136) }

	virtual void _UNKNOWN_137(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 137) }

	virtual void _UNKNOWN_138(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 138) }

	virtual void _UNKNOWN_139(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 139) }

	virtual void _UNKNOWN_140(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 140) }

	virtual void _UNKNOWN_141(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 141) }

	virtual void _UNKNOWN_142(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 142) }

	virtual void _UNKNOWN_143(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 143) }

	virtual void _UNKNOWN_144(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 144) }

	virtual void _UNKNOWN_145(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 145) }

	virtual void _UNKNOWN_146(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 146) }

	virtual void _UNKNOWN_147(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 147) }

	virtual void _UNKNOWN_148(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 148) }

	virtual void _UNKNOWN_149(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 149) }

	virtual void _UNKNOWN_150(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 150) }

	virtual void _UNKNOWN_151(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 151) }

	virtual void _UNKNOWN_152(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 152) }

	virtual void _UNKNOWN_153(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 153) }

	virtual void _UNKNOWN_154(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 154) }

	virtual void _UNKNOWN_155(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 155) }

	virtual void _UNKNOWN_156(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 156) }

	virtual void _UNKNOWN_157(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 157) }

	virtual void _UNKNOWN_158(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 158) }

	virtual void _UNKNOWN_159(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 159) }

	virtual void _UNKNOWN_160(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 160) }

	virtual void _UNKNOWN_161(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 161) }

	virtual void _UNKNOWN_162(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 162) }

	virtual void _UNKNOWN_163(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 163) }

	virtual void _UNKNOWN_164(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 164) }

	virtual void _UNKNOWN_165(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 165) }

	virtual void _UNKNOWN_166(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 166) }

	virtual void _UNKNOWN_167(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 167) }

	virtual void _UNKNOWN_168(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 168) }

	virtual void _UNKNOWN_169(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 169) }

	virtual void _UNKNOWN_170(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 170) }

	virtual void _UNKNOWN_171(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 171) }

	virtual void _UNKNOWN_172(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 172) }

	virtual void _UNKNOWN_173(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 173) }

	virtual void _UNKNOWN_174(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 174) }

	virtual void _UNKNOWN_175(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 175) }

	virtual void _UNKNOWN_176(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 176) }

	virtual void _UNKNOWN_177(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 177) }

	virtual void _UNKNOWN_178(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 178) }

	virtual void _UNKNOWN_179(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 179) }

	virtual void _UNKNOWN_180(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 180) }

	virtual void _UNKNOWN_181(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 181) }

	virtual void _UNKNOWN_182(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 182) }

	virtual void _UNKNOWN_183(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 183) }

	virtual void _UNKNOWN_184(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 184) }

	virtual void _UNKNOWN_185(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 185) }

	virtual void _UNKNOWN_186(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 186) }

	virtual void _UNKNOWN_187(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 187) }

	virtual void _UNKNOWN_188(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 188) }

	virtual void _UNKNOWN_189(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 189) }

	virtual void _UNKNOWN_190(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 190) }

	virtual void _UNKNOWN_191(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 191) }

	virtual void DrawInstances( int nInstanceCount, const MeshInstanceData_t_csgo *pInstance )
	{
		// JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 192)

		if(m_OnDrawInstances)
		{

			m_OnDrawInstances->DrawInstances(this, nInstanceCount, pInstance);
		}
		else
		{
			m_Parent->DrawInstances(nInstanceCount, pInstance);
		}
	}

	virtual void _UNKNOWN_193(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 193) }

	virtual void _UNKNOWN_194(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 194) }

	virtual void _UNKNOWN_195(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 195) }

	virtual void _UNKNOWN_196(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 196) }

	virtual void _UNKNOWN_197(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 197) }

	virtual void _UNKNOWN_198(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 198) }

	virtual void _UNKNOWN_199(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 199) }

	virtual void _UNKNOWN_200(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 200) }

	virtual void _UNKNOWN_201(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 201) }

	virtual void _UNKNOWN_202(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 202) }

	virtual void _UNKNOWN_203(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 203) }

	virtual void _UNKNOWN_204(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 204) }

	virtual void _UNKNOWN_205(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 205) }

	virtual void _UNKNOWN_206(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 206) }

	virtual void _UNKNOWN_207(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 207) }

	virtual void _UNKNOWN_208(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 208) }

	virtual void _UNKNOWN_209(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 209) }

	virtual void _UNKNOWN_210(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 210) }

	virtual void _UNKNOWN_211(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 211) }

	virtual void _UNKNOWN_212(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 212) }

	virtual void _UNKNOWN_213(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 213) }

	virtual void _UNKNOWN_214(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 214) }

	virtual void _UNKNOWN_215(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 215) }

	virtual void _UNKNOWN_216(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 216) }

	virtual void _UNKNOWN_217(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 217) }

	virtual void _UNKNOWN_218(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 218) }

	virtual void _UNKNOWN_219(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 219) }

	virtual void _UNKNOWN_220(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 220) }

	virtual void _UNKNOWN_221(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 221) }

	virtual void _UNKNOWN_222(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 222) }

	virtual void _UNKNOWN_223(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 223) }

	virtual void _UNKNOWN_224(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 224) }

	virtual void _UNKNOWN_225(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 225) }

	virtual void _UNKNOWN_226(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 226) }

	virtual void _UNKNOWN_227(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 227) }

	virtual void _UNKNOWN_228(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 228) }

	virtual void _UNKNOWN_229(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 229) }

	virtual void _UNKNOWN_230(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 230) }

	virtual void _UNKNOWN_231(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 231) }

	virtual void _UNKNOWN_232(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 232) }

	virtual void _UNKNOWN_233(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 233) }

	virtual void _UNKNOWN_234(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 234) }

	virtual void _UNKNOWN_235(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 235) }

	virtual void _UNKNOWN_236(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 236) }

	virtual void _UNKNOWN_237(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 237) }

	virtual void _UNKNOWN_238(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 238) }

	virtual void _UNKNOWN_239(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 239) }

	virtual void _UNKNOWN_240(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 240) }

	virtual void _UNKNOWN_241(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 241) }

	virtual void _UNKNOWN_242(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 242) }

	virtual void _UNKNOWN_243(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 243) }

	virtual void _UNKNOWN_244(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 244) }

	virtual void _UNKNOWN_245(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 245) }

	virtual void _UNKNOWN_246(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 246) }

	virtual void _UNKNOWN_247(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 247) }

	virtual void _UNKNOWN_248(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 248) }

	virtual void _UNKNOWN_249(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 249) }

	virtual void _UNKNOWN_250(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 250) }

	virtual void _UNKNOWN_251(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 251) }

	virtual void _UNKNOWN_252(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 252) }

	virtual void _UNKNOWN_253(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 253) }

	virtual void _UNKNOWN_254(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxMatRenderContext, m_Parent, 254) }

private:
	IMatRenderContext_csgo * m_Parent;
	IAfxMatRenderContextBind * m_OnBind;
	IAfxMatRenderContextDrawInstances * m_OnDrawInstances;
};

#pragma warning(pop)

std::map<IMatRenderContext_csgo *,CAfxMatRenderContext *> g_RenderContextMap_csgo;

#pragma warning(push)
#pragma warning(disable:4731) // frame pointer register 'ebp' modified by inline assembly code

class CAfxBaseClientDll
: public IBaseClientDLL_csgo
, public IAfxBaseClientDll
, public IAfxFreeMaster
{
public:
	CAfxBaseClientDll(IBaseClientDLL_csgo * parent)
	: m_Parent(parent)
	, m_OnShutdown(0)
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

	virtual void OnView_Render_set(IAfxBaseClientDllView_Render * value)
	{
		m_OnView_Render = value;
	}

	//
	// IBaseClientDll_csgo:

	virtual int Connect( CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals )
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 0) }

	virtual void Disconnect()
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 1) }

	virtual int Init( CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals )
	{
		// JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 2)

		static bool bFirstCall = true;

		if(bFirstCall)
		{
			bFirstCall = false;

			MySetup(appSystemFactory, new WrpGlobalsCsGo(pGlobals));
		}

		int result = m_Parent->Init(AppSystemFactory_ForClient, pGlobals);

		return result;
	}

	virtual void PostInit()
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 3) }

	virtual void Shutdown( void )
	{
		// JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 4)

		if(m_OnShutdown) m_OnShutdown->Shutdown(this);

		m_FreeMaster.AfxFree();

		g_MaterialInfoSet_csgo.clear();

		m_Parent->Shutdown();
	}
	
	virtual void _UNKOWN_005(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 5) }

	virtual void _UNKOWN_006(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 6) }

	virtual void _UNKOWN_007(void)
	{ JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 7) }

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

	virtual void View_Render( vrect_t_csgo *rect )
	{
		// JMP_CLASSMEMBERIFACE_FN(CAfxBaseClientDll, m_Parent, 26)

		bool rectNull = rect->width == 0 || rect->height == 0;

		if(!rectNull && g_MaterialSystem_csgo)
		{
			CAfxMatRenderContext * wrapper;
			IMatRenderContext_csgo * oldContext = g_MaterialSystem_csgo->GetRenderContext();

			std::map<IMatRenderContext_csgo *, CAfxMatRenderContext *>::iterator it = g_RenderContextMap_csgo.find(oldContext);

			if(it != g_RenderContextMap_csgo.end())
			{
				if(it->first == it->second)
				{
					//Tier0_Msg("Found own context 0x%08x.\n", (DWORD)oldContext);
				}
				else
				{
					//Tier0_Msg("Found known context 0x%08x.\n", (DWORD)oldContext);
				}
				wrapper = it->second; // re-use
			}
			else
			{
				//Tier0_Msg("New context 0x%08x.\n", (DWORD)oldContext);
				wrapper = new CAfxMatRenderContext(oldContext);

				g_RenderContextMap_csgo[oldContext] = wrapper; // track new context
				g_RenderContextMap_csgo[wrapper] = wrapper; // make it possible to detect our self
			}

			if((IMatRenderContext_csgo *)wrapper != oldContext)
			{
				g_MaterialSystem_csgo->SetRenderContext(wrapper);
				wrapper->Release(); // SetRenderContext calls AddRef
			}

			if(m_OnView_Render)
			{
				m_OnView_Render->View_Render(this, wrapper, rect);
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

	virtual void RenderView( const CViewSetup_csgo &view, int nClearFlags, int whatToDraw )
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
	IAfxBaseClientDllView_Render * m_OnView_Render;
	CAfxFreeMaster m_FreeMaster;
};

CAfxBaseClientDll * g_AfxBaseClientDll = 0;

#pragma warning(pop)

void HookClientDllInterface_011_Init(void * iface)
{
	old_Client_Init = HookInterfaceFn(iface, 0, (void *)hook_Client_Init);
}

CreateInterfaceFn old_Client_CreateInterface = 0;

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

	if(isCsgo && !strcmp(pName, CLIENT_DLL_INTERFACE_VERSION_CSGO_017))
	{
		if(!g_AfxBaseClientDll)
		{
			g_Info_VClient = CLIENT_DLL_INTERFACE_VERSION_CSGO_017 " (CS:GO)";
			g_AfxBaseClientDll = new CAfxBaseClientDll((IBaseClientDLL_csgo *)pRet);
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


HMODULE WINAPI new_LoadLibraryA(LPCSTR lpLibFileName);
HMODULE WINAPI new_LoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);

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

		// actually this is not required, since engine.dll calls first and thus is lower in the chain:
		InterceptDllCall(hModule, "USER32.dll", "GetWindowLongW", (DWORD) &new_GetWindowLongW);
		InterceptDllCall(hModule, "USER32.dll", "SetWindowLongW", (DWORD) &new_SetWindowLongW);

		// Init the hook early, so we don't run into issues with threading:
		Hook_csgo_SndMixTimeScalePatch();
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

			for(std::map<IMatRenderContext_csgo *, CAfxMatRenderContext *>::iterator it = g_RenderContextMap_csgo.begin(); it != g_RenderContextMap_csgo.end(); ++it)
			{
				delete it->second;
			}

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
