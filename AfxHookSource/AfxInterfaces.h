#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-06-26 dominik.matrixstorm.com
//
// First changes:
// 2015-06-26 dominik.matrixstorm.com

#include "SourceInterfaces.h"

class IAfxFreeable abstract
{
public:
	virtual void AfxFree(void) = 0;
};

class IAfxFreeMaster abstract
{
public:
	virtual void AfxFreeable_Register(IAfxFreeable * value) = 0;
	virtual void AfxFreeable_Unregister(IAfxFreeable * value) = 0;
};

class IAfxMaterial abstract
{
public:
	virtual IMaterial_csgo * GetMaterial() const = 0;
};

class IAfxVRenderView;

class IAfxVRenderViewSetColorModulation abstract
{
public:
	virtual void SetColorModulation(IAfxVRenderView * rv, float const* blend ) = 0;
};

class IAfxVRenderView abstract
{
public:
	virtual IVRenderView_csgo * GetParent() = 0;

	virtual void OnSetColorModulation_set(IAfxVRenderViewSetColorModulation * value) = 0;
};


class IAfxMatRenderContext;

class IAfxMatRenderContextBind abstract
{
public:
	virtual void Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData = 0 ) = 0;
};

class IAfxMatRenderContextOverrideDepthEnable abstract
{
public:
	virtual void OverrideDepthEnable(IAfxMatRenderContext * ctx, bool bEnable, bool bDepthEnable, bool bUnknown = false) = 0;
};

class IAfxMatRenderContextDrawInstances abstract
{
public:
	virtual void DrawInstances(IAfxMatRenderContext * ctx, int nInstanceCount, const MeshInstanceData_t_csgo *pInstance ) = 0;
};

class IAfxMatRenderContextOverrideAlphaWriteEnable abstract
{
public:
	virtual void OverrideAlphaWriteEnable(IAfxMatRenderContext * ctx, bool bOverrideEnable, bool bAlphaWriteEnable ) = 0;
};
	
class IAfxMatRenderContextOverrideColorWriteEnable abstract
{
public:
	virtual void OverrideColorWriteEnable(IAfxMatRenderContext * ctx, bool bOverrideEnable, bool bColorWriteEnable ) = 0;
};

class IAfxMatRenderContext abstract
{
public:
	virtual IMatRenderContext_csgo * GetParent() = 0;

	virtual void OnBind_set(IAfxMatRenderContextBind * value) = 0;
	virtual void OnOverrideDepthEnable_set(IAfxMatRenderContextOverrideDepthEnable * value) = 0;
	virtual void OnDrawInstances_set(IAfxMatRenderContextDrawInstances * value) = 0;
	virtual void OnOverrideAlphaWriteEnable_set(IAfxMatRenderContextOverrideAlphaWriteEnable * value) = 0;
	virtual void OnOverrideColorWriteEnable_set(IAfxMatRenderContextOverrideColorWriteEnable * value) = 0;
};


class IAfxBaseClientDll;

class IAfxBaseClientDllShutdown abstract
{
public:
	virtual void Shutdown(IAfxBaseClientDll * cl) = 0;
};

class IAfxBaseClientDllView_Render abstract
{
public:
	virtual void View_Render(IAfxBaseClientDll * cl, IAfxMatRenderContext * cx, vrect_t_csgo *rect) = 0;
};

class IAfxBaseClientDll abstract
{
public:
	virtual IBaseClientDLL_csgo * GetParent() = 0;
	virtual IAfxFreeMaster * GetFreeMaster() = 0;

	virtual void OnShutdown_set(IAfxBaseClientDllShutdown * value) = 0;
	virtual void OnView_Render_set(IAfxBaseClientDllView_Render * value) = 0;
};

class IAfxStreams4Stream abstract
{
public:
	virtual IMaterialSystem_csgo * GetMaterialSystem(void) = 0;
	virtual IAfxFreeMaster * GetFreeMaster(void) = 0;
	virtual IAfxMatRenderContext * GetCurrentContext(void) = 0;

	virtual void OnBind_set(IAfxMatRenderContextBind * value) = 0;
	virtual void OnOverrideDepthEnable_set(IAfxMatRenderContextOverrideDepthEnable * value) = 0;
	virtual void OnDrawInstances_set(IAfxMatRenderContextDrawInstances * value) = 0;
	virtual void OnOverrideAlphaWriteEnable_set(IAfxMatRenderContextOverrideAlphaWriteEnable * value) = 0;
	virtual void OnOverrideColorWriteEnable_set(IAfxMatRenderContextOverrideColorWriteEnable * value) = 0;

	virtual void OnSetColorModulation_set(IAfxVRenderViewSetColorModulation * value) = 0;
};

class IAfxStream4Streams abstract
{
public:
	virtual void StreamAttach(IAfxStreams4Stream * streams) = 0;
	virtual void StreamDetach(IAfxStreams4Stream * streams) = 0;
};
