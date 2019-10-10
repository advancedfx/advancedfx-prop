//===== Copyright © 2005-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: A higher level link library for general use in the game and tools.
//
//===========================================================================//


#ifndef SOURCESDK_BM_INTERFACES_H
#define SOURCESDK_BM_INTERFACES_H

#if defined( COMPILER_MSVC )
#pragma once
#endif

namespace SOURCESDK {
namespace BM {


//-----------------------------------------------------------------------------
// Interface creation function
//-----------------------------------------------------------------------------
typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);


//-----------------------------------------------------------------------------
// Macros to declare interfaces appropriate for various tiers
//-----------------------------------------------------------------------------
#if 1 || defined( SOURCESDK_BM_TIER1_LIBRARY ) || defined( SOURCESDK_BM_TIER2_LIBRARY ) || defined( SOURCESDK_BM_TIER3_LIBRARY ) || defined( SOURCESDK_BM_TIER4_LIBRARY ) || defined( SOURCESDK_BM_APPLICATION )
#define SOURCESDK_BM_DECLARE_TIER1_INTERFACE( _Interface, _Global )	extern _Interface * _Global;
#else
#define SOURCESDK_BM_DECLARE_TIER1_INTERFACE( _Interface, _Global )
#endif

#if 1 || defined( SOURCESDK_BM_TIER2_LIBRARY ) || defined( SOURCESDK_BM_TIER3_LIBRARY ) || defined( SOURCESDK_BM_TIER4_LIBRARY ) || defined( SOURCESDK_BM_APPLICATION )
#define SOURCESDK_BM_DECLARE_TIER2_INTERFACE( _Interface, _Global )	extern _Interface * _Global;
#else
#define SOURCESDK_BM_DECLARE_TIER2_INTERFACE( _Interface, _Global )
#endif

#if 1 || defined( SOURCESDK_BM_TIER3_LIBRARY ) || defined( SOURCESDK_BM_TIER4_LIBRARY ) || defined( SOURCESDK_BM_APPLICATION )
#define SOURCESDK_BM_DECLARE_TIER3_INTERFACE( _Interface, _Global )	extern _Interface * _Global;
#else
#define SOURCESDK_BM_DECLARE_TIER3_INTERFACE( _Interface, _Global )
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
#define SOURCESDK_BM_CVAR_INTERFACE_VERSION					"VEngineCvar004"
SOURCESDK_BM_DECLARE_TIER1_INTERFACE( ICvar, cvar );
SOURCESDK_BM_DECLARE_TIER1_INTERFACE( ICvar, g_pCVar )

#define SOURCESDK_BM_PROCESS_UTILS_INTERFACE_VERSION			"VProcessUtils001"
SOURCESDK_BM_DECLARE_TIER1_INTERFACE( IProcessUtils, g_pProcessUtils );

#define SOURCESDK_BM_VPHYSICS2_INTERFACE_VERSION				"Physics2 Interface v0.3" // not yet
SOURCESDK_BM_DECLARE_TIER1_INTERFACE( IPhysics2, g_pPhysics2 );

#define SOURCESDK_BM_VPHYSICS2_ACTOR_MGR_INTERFACE_VERSION	"Physics2 Interface ActorMgr v0.1" // not yet
SOURCESDK_BM_DECLARE_TIER1_INTERFACE( IPhysics2ActorManager, g_pPhysics2ActorManager );

#define SOURCESDK_BM_VPHYSICS2_RESOURCE_MGR_INTERFACE_VERSION "Physics2 Interface ResourceMgr v0.1" // not yet
SOURCESDK_BM_DECLARE_TIER1_INTERFACE( IPhysics2ResourceManager, g_pPhysics2ResourceManager );

#define SOURCESDK_BM_EVENTSYSTEM_INTERFACE_VERSION "EventSystem001" // not yet
SOURCESDK_BM_DECLARE_TIER1_INTERFACE( IEventSystem, g_pEventSystem );

#define SOURCESDK_BM_LOCALIZE_INTERFACE_VERSION 			"Localize_001"
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( ILocalize, g_pLocalize );
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( vgui::ILocalize, g_pVGuiLocalize );

#define SOURCESDK_BM_RENDER_DEVICE_MGR_INTERFACE_VERSION		"RenderDeviceMgr001" // not yet
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IRenderDeviceMgr, g_pRenderDeviceMgr );

#define SOURCESDK_BM_FILESYSTEM_INTERFACE_VERSION			"VFileSystem022"
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IFileSystem, g_pFullFileSystem );

#define SOURCESDK_BM_ASYNCFILESYSTEM_INTERFACE_VERSION		"VNewAsyncFileSystem001" // not yet
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IAsyncFileSystem, g_pAsyncFileSystem );

#define SOURCESDK_BM_RESOURCESYSTEM_INTERFACE_VERSION		"ResourceSystem004" // not yet
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IResourceSystem, g_pResourceSystem );

#define SOURCESDK_BM_MATERIAL_SYSTEM_INTERFACE_VERSION		"VMaterialSystem081"
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IMaterialSystem, materials );
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IMaterialSystem, g_pMaterialSystem );

#define SOURCESDK_BM_MATERIAL_SYSTEM2_INTERFACE_VERSION		"VMaterialSystem2_001" // not yet
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IMaterialSystem2, g_pMaterialSystem2 );

#define SOURCESDK_BM_INPUTSYSTEM_INTERFACE_VERSION			"InputSystemVersion001"
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IInputSystem, g_pInputSystem );

#define SOURCESDK_BM_INPUTSTACKSYSTEM_INTERFACE_VERSION		"InputStackSystemVersion001" // not yet
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IInputStackSystem, g_pInputStackSystem );

#define SOURCESDK_BM_NETWORKSYSTEM_INTERFACE_VERSION			"NetworkSystemVersion001"
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( INetworkSystem, g_pNetworkSystem );

#define SOURCESDK_BM_MATERIALSYSTEM_HARDWARECONFIG_INTERFACE_VERSION		"MaterialSystemHardwareConfig012"
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IMaterialSystemHardwareConfig, g_pMaterialSystemHardwareConfig );

#define SOURCESDK_BM_DEBUG_TEXTURE_INFO_VERSION				"DebugTextureInfo001"
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IDebugTextureInfo, g_pMaterialSystemDebugTextureInfo );

#define SOURCESDK_BM_VB_ALLOC_TRACKER_INTERFACE_VERSION		"VBAllocTracker001"
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IVBAllocTracker, g_VBAllocTracker );

#define SOURCESDK_BM_COLORCORRECTION_INTERFACE_VERSION		"COLORCORRECTION_VERSION_1"
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IColorCorrectionSystem, colorcorrection );

#define SOURCESDK_BM_P4_INTERFACE_VERSION					"VP4001"
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IP4, p4 );

#define SOURCESDK_BM_MDLLIB_INTERFACE_VERSION				"VMDLLIB001"
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IMdlLib, mdllib );

#define SOURCESDK_BM_QUEUEDLOADER_INTERFACE_VERSION			"QueuedLoaderVersion004"
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IQueuedLoader, g_pQueuedLoader );

#define SOURCESDK_BM_RESOURCE_ACCESS_CONTROL_INTERFACE_VERSION	"VResourceAccessControl001"
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IResourceAccessControl, g_pResourceAccessControl );

#define SOURCESDK_BM_PRECACHE_SYSTEM_INTERFACE_VERSION		"VPrecacheSystem001"
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IPrecacheSystem, g_pPrecacheSystem );

#if defined( _X360 )
#define SOURCESDK_BM_XBOXINSTALLER_INTERFACE_VERSION			"XboxInstallerVersion001" //not yet
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IXboxInstaller, g_pXboxInstaller );
#endif

#define SOURCESDK_BM_MATCHFRAMEWORK_INTERFACE_VERSION		"MATCHFRAMEWORK_001" // not yet
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IMatchFramework, g_pMatchFramework );

#define SOURCESDK_BM_GAMEUISYSTEMMGR_INTERFACE_VERSION	"GameUISystemMgr001" // not yet
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( IGameUISystemMgr, g_pGameUISystemMgr );


//-----------------------------------------------------------------------------
// Not exactly a global, but we're going to keep track of these here anyways
// NOTE: Appframework deals with connecting these bad boys. See materialsystem2app.cpp
//-----------------------------------------------------------------------------
#define SOURCESDK_BM_RENDER_DEVICE_INTERFACE_VERSION			"RenderDevice001"
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IRenderDevice, g_pRenderDevice );

#define SOURCESDK_BM_RENDER_HARDWARECONFIG_INTERFACE_VERSION		"RenderHardwareConfig001"
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( IRenderHardwareConfig, g_pRenderHardwareConfig );

#define SOURCESDK_BM_SOUNDSYSTEM_INTERFACE_VERSION		"SoundSystem001"
SOURCESDK_BM_DECLARE_TIER2_INTERFACE( ISoundSystem, g_pSoundSystem );

#define SOURCESDK_BM_MESHSYSTEM_INTERFACE_VERSION			"MeshSystem001" // not yet
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( IMeshSystem, g_pMeshSystem );

#define SOURCESDK_BM_STUDIO_RENDER_INTERFACE_VERSION			"VStudioRender025"
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( IStudioRender, g_pStudioRender );
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( IStudioRender, studiorender );

#define SOURCESDK_BM_MAT_SYSTEM_SURFACE_INTERFACE_VERSION	"MatSystemSurface008"
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( IMatSystemSurface, g_pMatSystemSurface );

#define SOURCESDK_BM_RENDER_SYSTEM_SURFACE_INTERFACE_VERSION	"RenderSystemSurface001" // not yet
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( IVGuiRenderSurface, g_pVGuiRenderSurface );

#define SOURCESDK_BM_SCENESYSTEM_INTERFACE_VERSION			"SceneSystem_001" // not yet
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( ISceneSystem, g_pSceneSystem );

#define SOURCESDK_BM_VGUI_SURFACE_INTERFACE_VERSION			"VGUI_Surface030"
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( vgui::ISurface, g_pVGuiSurface );

#define SOURCESDK_BM_SCHEME_SURFACE_INTERFACE_VERSION		"SchemeSurface001" // not yet

#define SOURCESDK_BM_VGUI_INPUT_INTERFACE_VERSION			"VGUI_Input005"
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( vgui::IInput, g_pVGuiInput );

#define SOURCESDK_BM_VGUI_IVGUI_INTERFACE_VERSION			"VGUI_ivgui008"
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( vgui::IVGui, g_pVGui );

#define SOURCESDK_BM_VGUI_PANEL_INTERFACE_VERSION			"VGUI_Panel009"
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( vgui::IPanel, g_pVGuiPanel );

#define SOURCESDK_BM_VGUI_SCHEME_INTERFACE_VERSION			"VGUI_Scheme010"
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( vgui::ISchemeManager, g_pVGuiSchemeManager );

#define SOURCESDK_BM_VGUI_SYSTEM_INTERFACE_VERSION			"VGUI_System010"
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( vgui::ISystem, g_pVGuiSystem );

#define SOURCESDK_BM_DATACACHE_INTERFACE_VERSION				"VDataCache003"
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( IDataCache, g_pDataCache );	// FIXME: Should IDataCache be in tier2?

#define SOURCESDK_BM_MDLCACHE_INTERFACE_VERSION				"MDLCache004"
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( IMDLCache, g_pMDLCache );
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( IMDLCache, mdlcache );

#define SOURCESDK_BM_AVI_INTERFACE_VERSION					"VAvi001" // not yet
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( IAvi, g_pAVI );

#define SOURCESDK_BM_BIK_INTERFACE_VERSION					"VBik001" // not yet
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( IBik, g_pBIK );

#define SOURCESDK_BM_DMEMAKEFILE_UTILS_INTERFACE_VERSION		"VDmeMakeFileUtils001"
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( IDmeMakefileUtils, g_pDmeMakefileUtils );

#define SOURCESDK_BM_VPHYSICS_COLLISION_INTERFACE_VERSION	"VPhysicsCollision007"
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( IPhysicsCollision, g_pPhysicsCollision );

#define SOURCESDK_BM_SOUNDEMITTERSYSTEM_INTERFACE_VERSION	"VSoundEmitter002"
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( ISoundEmitterSystemBase, g_pSoundEmitterSystem );

#define SOURCESDK_BM_WORLD_RENDERER_MGR_INTERFACE_VERSION	"WorldRendererMgr001" // not yet
SOURCESDK_BM_DECLARE_TIER3_INTERFACE( IWorldRendererMgr, g_pWorldRendererMgr );

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
} // namespace BM {


#endif // SOURCESDK_BM_INTERFACES_H

