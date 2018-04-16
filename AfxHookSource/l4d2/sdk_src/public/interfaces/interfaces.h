//===== Copyright © 2005-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: A higher level link library for general use in the game and tools.
//
//===========================================================================//


#ifndef SOURCESDK_L4D2_INTERFACES_H
#define SOURCESDK_L4D2_INTERFACES_H

#if defined( COMPILER_MSVC )
#pragma once
#endif

namespace SOURCESDK {
namespace L4D2 {


//-----------------------------------------------------------------------------
// Interface creation function
//-----------------------------------------------------------------------------
typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);


//-----------------------------------------------------------------------------
// Macros to declare interfaces appropriate for various tiers
//-----------------------------------------------------------------------------
#if 1 || defined( SOURCESDK_L4D2_TIER1_LIBRARY ) || defined( SOURCESDK_L4D2_TIER2_LIBRARY ) || defined( SOURCESDK_L4D2_TIER3_LIBRARY ) || defined( SOURCESDK_L4D2_TIER4_LIBRARY ) || defined( SOURCESDK_L4D2_APPLICATION )
#define SOURCESDK_L4D2_DECLARE_TIER1_INTERFACE( _Interface, _Global )	extern _Interface * _Global;
#else
#define SOURCESDK_L4D2_DECLARE_TIER1_INTERFACE( _Interface, _Global )
#endif

#if 1 || defined( SOURCESDK_L4D2_TIER2_LIBRARY ) || defined( SOURCESDK_L4D2_TIER3_LIBRARY ) || defined( SOURCESDK_L4D2_TIER4_LIBRARY ) || defined( SOURCESDK_L4D2_APPLICATION )
#define SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( _Interface, _Global )	extern _Interface * _Global;
#else
#define SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( _Interface, _Global )
#endif

#if 1 || defined( SOURCESDK_L4D2_TIER3_LIBRARY ) || defined( SOURCESDK_L4D2_TIER4_LIBRARY ) || defined( SOURCESDK_L4D2_APPLICATION )
#define SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( _Interface, _Global )	extern _Interface * _Global;
#else
#define SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( _Interface, _Global )
#endif


//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class ICvar;
class IProcessUtils;
class ILocalize;
class IPhysics2;
class IPhysics2ActorManager;
class IPhysics2ResourceManager;
class IEventSystem;

class IAsyncFileSystem;
class IColorCorrectionSystem;
class IDebugTextureInfo;
class IFileSystem;
class IRenderHardwareConfig;
class IInputSystem;
class IInputStackSystem;
class IMaterialSystem;
class IMaterialSystem2;
class IMaterialSystemHardwareConfig;
class IMdlLib;
class INetworkSystem;
class IP4;
class IQueuedLoader;
class IResourceAccessControl;
class IPrecacheSystem;
class IRenderDevice;
class IRenderDeviceMgr;
class IResourceSystem;
class IVBAllocTracker;
class IXboxInstaller;
class IMatchFramework;
class ISoundSystem;
class IStudioRender;
class IMatSystemSurface;
class IGameUISystemMgr;
class IDataCache;
class IMDLCache;
class IAvi;
class IBik;
class IDmeMakefileUtils;
class IPhysicsCollision;
class ISoundEmitterSystemBase;
class IMeshSystem;
class IWorldRendererMgr;
class ISceneSystem;
class IVGuiRenderSurface;

namespace vgui
{
	class ISurface;
	class IVGui;
	class IInput;
	class IPanel;
	class ILocalize;
	class ISchemeManager;
	class ISystem;
}



//-----------------------------------------------------------------------------
// Fills out global DLL exported interface pointers
//-----------------------------------------------------------------------------
#define SOURCESDK_L4D2_CVAR_INTERFACE_VERSION					"VEngineCvar007"
SOURCESDK_L4D2_DECLARE_TIER1_INTERFACE( ICvar, cvar );
SOURCESDK_L4D2_DECLARE_TIER1_INTERFACE( ICvar, g_pCVar )

#define SOURCESDK_L4D2_PROCESS_UTILS_INTERFACE_VERSION			"VProcessUtils002"
SOURCESDK_L4D2_DECLARE_TIER1_INTERFACE( IProcessUtils, g_pProcessUtils );

#define SOURCESDK_L4D2_VPHYSICS2_INTERFACE_VERSION				"Physics2 Interface v0.3"
SOURCESDK_L4D2_DECLARE_TIER1_INTERFACE( IPhysics2, g_pPhysics2 );

#define SOURCESDK_L4D2_VPHYSICS2_ACTOR_MGR_INTERFACE_VERSION	"Physics2 Interface ActorMgr v0.1"
SOURCESDK_L4D2_DECLARE_TIER1_INTERFACE( IPhysics2ActorManager, g_pPhysics2ActorManager );

#define SOURCESDK_L4D2_VPHYSICS2_RESOURCE_MGR_INTERFACE_VERSION "Physics2 Interface ResourceMgr v0.1"
SOURCESDK_L4D2_DECLARE_TIER1_INTERFACE( IPhysics2ResourceManager, g_pPhysics2ResourceManager );

#define SOURCESDK_L4D2_EVENTSYSTEM_INTERFACE_VERSION "EventSystem001"
SOURCESDK_L4D2_DECLARE_TIER1_INTERFACE( IEventSystem, g_pEventSystem );

#define SOURCESDK_L4D2_LOCALIZE_INTERFACE_VERSION 			"Localize_001"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( ILocalize, g_pLocalize );
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( vgui::ILocalize, g_pVGuiLocalize );

#define SOURCESDK_L4D2_RENDER_DEVICE_MGR_INTERFACE_VERSION		"RenderDeviceMgr001"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IRenderDeviceMgr, g_pRenderDeviceMgr );

#define SOURCESDK_L4D2_FILESYSTEM_INTERFACE_VERSION			"VFileSystem017"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IFileSystem, g_pFullFileSystem );

#define SOURCESDK_L4D2_ASYNCFILESYSTEM_INTERFACE_VERSION		"VNewAsyncFileSystem001"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IAsyncFileSystem, g_pAsyncFileSystem );

#define SOURCESDK_L4D2_RESOURCESYSTEM_INTERFACE_VERSION		"ResourceSystem004"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IResourceSystem, g_pResourceSystem );

#define SOURCESDK_L4D2_MATERIAL_SYSTEM_INTERFACE_VERSION		"VMaterialSystem080"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IMaterialSystem, materials );
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IMaterialSystem, g_pMaterialSystem );

#define SOURCESDK_L4D2_MATERIAL_SYSTEM2_INTERFACE_VERSION		"VMaterialSystem2_001"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IMaterialSystem2, g_pMaterialSystem2 );

#define SOURCESDK_L4D2_INPUTSYSTEM_INTERFACE_VERSION			"InputSystemVersion001"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IInputSystem, g_pInputSystem );

#define SOURCESDK_L4D2_INPUTSTACKSYSTEM_INTERFACE_VERSION		"InputStackSystemVersion001"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IInputStackSystem, g_pInputStackSystem );

#define SOURCESDK_L4D2_NETWORKSYSTEM_INTERFACE_VERSION			"NetworkSystemVersion001"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( INetworkSystem, g_pNetworkSystem );

#define SOURCESDK_L4D2_MATERIALSYSTEM_HARDWARECONFIG_INTERFACE_VERSION		"MaterialSystemHardwareConfig013"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IMaterialSystemHardwareConfig, g_pMaterialSystemHardwareConfig );

#define SOURCESDK_L4D2_DEBUG_TEXTURE_INFO_VERSION				"DebugTextureInfo001"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IDebugTextureInfo, g_pMaterialSystemDebugTextureInfo );

#define SOURCESDK_L4D2_VB_ALLOC_TRACKER_INTERFACE_VERSION		"VBAllocTracker001"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IVBAllocTracker, g_VBAllocTracker );

#define SOURCESDK_L4D2_COLORCORRECTION_INTERFACE_VERSION		"COLORCORRECTION_VERSION_1"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IColorCorrectionSystem, colorcorrection );

#define SOURCESDK_L4D2_P4_INTERFACE_VERSION					"VP4002"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IP4, p4 );

#define SOURCESDK_L4D2_MDLLIB_INTERFACE_VERSION				"VMDLLIB001"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IMdlLib, mdllib );

#define SOURCESDK_L4D2_QUEUEDLOADER_INTERFACE_VERSION			"QueuedLoaderVersion001"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IQueuedLoader, g_pQueuedLoader );

#define SOURCESDK_L4D2_RESOURCE_ACCESS_CONTROL_INTERFACE_VERSION	"VResourceAccessControl001"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IResourceAccessControl, g_pResourceAccessControl );

#define SOURCESDK_L4D2_PRECACHE_SYSTEM_INTERFACE_VERSION		"VPrecacheSystem001"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IPrecacheSystem, g_pPrecacheSystem );

#if defined( _X360 )
#define SOURCESDK_L4D2_XBOXINSTALLER_INTERFACE_VERSION			"XboxInstallerVersion001"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IXboxInstaller, g_pXboxInstaller );
#endif

#define SOURCESDK_L4D2_MATCHFRAMEWORK_INTERFACE_VERSION		"MATCHFRAMEWORK_001"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IMatchFramework, g_pMatchFramework );

#define SOURCESDK_L4D2_GAMEUISYSTEMMGR_INTERFACE_VERSION	"GameUISystemMgr001"
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( IGameUISystemMgr, g_pGameUISystemMgr );


//-----------------------------------------------------------------------------
// Not exactly a global, but we're going to keep track of these here anyways
// NOTE: Appframework deals with connecting these bad boys. See materialsystem2app.cpp
//-----------------------------------------------------------------------------
#define SOURCESDK_L4D2_RENDER_DEVICE_INTERFACE_VERSION			"RenderDevice001"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IRenderDevice, g_pRenderDevice );

#define SOURCESDK_L4D2_RENDER_HARDWARECONFIG_INTERFACE_VERSION		"RenderHardwareConfig001"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( IRenderHardwareConfig, g_pRenderHardwareConfig );

#define SOURCESDK_L4D2_SOUNDSYSTEM_INTERFACE_VERSION		"SoundSystem001"
SOURCESDK_L4D2_DECLARE_TIER2_INTERFACE( ISoundSystem, g_pSoundSystem );

#define SOURCESDK_L4D2_MESHSYSTEM_INTERFACE_VERSION			"MeshSystem001"
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( IMeshSystem, g_pMeshSystem );

#define SOURCESDK_L4D2_STUDIO_RENDER_INTERFACE_VERSION			"VStudioRender026"
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( IStudioRender, g_pStudioRender );
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( IStudioRender, studiorender );

#define SOURCESDK_L4D2_MAT_SYSTEM_SURFACE_INTERFACE_VERSION	"MatSystemSurface006"
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( IMatSystemSurface, g_pMatSystemSurface );

#define SOURCESDK_L4D2_RENDER_SYSTEM_SURFACE_INTERFACE_VERSION	"RenderSystemSurface001"
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( IVGuiRenderSurface, g_pVGuiRenderSurface );

#define SOURCESDK_L4D2_SCENESYSTEM_INTERFACE_VERSION			"SceneSystem_001"
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( ISceneSystem, g_pSceneSystem );

#define SOURCESDK_L4D2_VGUI_SURFACE_INTERFACE_VERSION			"VGUI_Surface031"
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( vgui::ISurface, g_pVGuiSurface );

#define SOURCESDK_L4D2_SCHEME_SURFACE_INTERFACE_VERSION		"SchemeSurface001"

#define SOURCESDK_L4D2_VGUI_INPUT_INTERFACE_VERSION			"VGUI_Input005"
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( vgui::IInput, g_pVGuiInput );

#define SOURCESDK_L4D2_VGUI_IVGUI_INTERFACE_VERSION			"VGUI_ivgui008"
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( vgui::IVGui, g_pVGui );

#define SOURCESDK_L4D2_VGUI_PANEL_INTERFACE_VERSION			"VGUI_Panel009"
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( vgui::IPanel, g_pVGuiPanel );

#define SOURCESDK_L4D2_VGUI_SCHEME_INTERFACE_VERSION			"VGUI_Scheme010"
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( vgui::ISchemeManager, g_pVGuiSchemeManager );

#define SOURCESDK_L4D2_VGUI_SYSTEM_INTERFACE_VERSION			"VGUI_System010"
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( vgui::ISystem, g_pVGuiSystem );

#define SOURCESDK_L4D2_DATACACHE_INTERFACE_VERSION				"VDataCache003"
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( IDataCache, g_pDataCache );	// FIXME: Should IDataCache be in tier2?

#define SOURCESDK_L4D2_MDLCACHE_INTERFACE_VERSION				"MDLCache004"
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( IMDLCache, g_pMDLCache );
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( IMDLCache, mdlcache );

#define SOURCESDK_L4D2_AVI_INTERFACE_VERSION					"VAvi001"
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( IAvi, g_pAVI );

#define SOURCESDK_L4D2_BIK_INTERFACE_VERSION					"VBik001"
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( IBik, g_pBIK );

#define SOURCESDK_L4D2_DMEMAKEFILE_UTILS_INTERFACE_VERSION		"VDmeMakeFileUtils001"
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( IDmeMakefileUtils, g_pDmeMakefileUtils );

#define SOURCESDK_L4D2_VPHYSICS_COLLISION_INTERFACE_VERSION	"VPhysicsCollision007"
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( IPhysicsCollision, g_pPhysicsCollision );

#define SOURCESDK_L4D2_SOUNDEMITTERSYSTEM_INTERFACE_VERSION	"VSoundEmitter003"
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( ISoundEmitterSystemBase, g_pSoundEmitterSystem );

#define SOURCESDK_L4D2_WORLD_RENDERER_MGR_INTERFACE_VERSION	"WorldRendererMgr001"
SOURCESDK_L4D2_DECLARE_TIER3_INTERFACE( IWorldRendererMgr, g_pWorldRendererMgr );

//-----------------------------------------------------------------------------
// Fills out global DLL exported interface pointers
//-----------------------------------------------------------------------------
void ConnectInterfaces( CreateInterfaceFn *pFactoryList, int nFactoryCount );
void DisconnectInterfaces();


//-----------------------------------------------------------------------------
// Reconnects an interface
//-----------------------------------------------------------------------------
void ReconnectInterface( CreateInterfaceFn factory, const char *pInterfaceName );


} // namespace SOURCESDK {
} // namespace L4D2 {


#endif // SOURCESDK_L4D2_INTERFACES_H

