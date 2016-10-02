#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-10-02 dominik.matrixstorm.com
//
// First changes:
// 2016-09-19 dominik.matrixstorm.com

#include "MatRenderContextHook.h"

#include "AfxStreams.h"
#include "asmClassTools.h"
#include <shared/detours.h>

#include <map>
#include <set>
#include <mutex>


#pragma warning(push)
#pragma warning(disable:4731) // frame pointer register 'ebp' modified by inline assembly code

class CAfxMesh
	: public SOURCESDK::IMeshEx_csgo
	, public IAfxMesh
{
public:
	CAfxMesh(SOURCESDK::IMeshEx_csgo * parent)
		: m_Parent(parent)
	{
	}

	void AfxMatRenderContext_set(IAfxMatRenderContext * value)
	{
		m_AfxMatRenderContext = value;
	}

private:
	void Debug(int ofs, int index)
	{
		if (g_DebugEnabled)
			Tier0_Msg("CAfxMesh::Debug(%i,%i)\n", ofs, index);
	}

public:

	//
	// IAfxMesh:

	virtual SOURCESDK::IMeshEx_csgo * GetParent(void)
	{
		return m_Parent;
	}

	virtual IAfxMatRenderContext * GetContext(void)
	{
		return m_AfxMatRenderContext;
	}

	//
	// IVertexBuffer_csgo:

	virtual int VertexCount() const
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 0)
	}

	virtual SOURCESDK::VertexFormat_t_csgo GetVertexFormat() const
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 1)
	}

	virtual bool IsDynamic() const
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 2)
	}

	virtual void BeginCastBuffer(SOURCESDK::VertexFormat_t_csgo format)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 3)
	}

	virtual void EndCastBuffer()
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 4)
	}

	virtual int GetRoomRemaining() const
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 5)
	}

	virtual bool Lock(int nVertexCount, bool bAppend, SOURCESDK::VertexDesc_t_csgo &desc)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 6)
	}

	virtual void Unlock(int nVertexCount, SOURCESDK::VertexDesc_t_csgo &desc)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 7)
	}

	virtual void Spew(int nVertexCount, const SOURCESDK::VertexDesc_t_csgo &desc)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 8)
	}

	virtual void ValidateData(int nVertexCount, const SOURCESDK::VertexDesc_t_csgo & desc)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 9)
	}

	virtual void _Unknown_10_IVertexBuffer_csgo(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 10)
	}


	//
	// IMesh_csgo:

	virtual void Draw(int firstIndex = -1, int numIndices = 0)
	{
		//JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh,m_Parent,0,12) /* !ofs different due to overloaded method! */

		Debug(0, 12);

		IAfxContextHook * stream = m_AfxMatRenderContext->Hook_get();

		if (stream)
			stream->Draw(this, firstIndex, numIndices);
		else
			m_Parent->Draw(firstIndex, numIndices);
	}

	virtual void SetColorMesh(IMesh_csgo *pColorMesh, int nVertexOffset)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 13) /* !ofs different due to overloaded method! */
	}

	virtual void Draw(SOURCESDK::CPrimList_csgo *pLists, int nLists)
	{
		// JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh,m_Parent,0,11) /* !ofs different due to overloaded method! */

		Debug(0, 11);

		IAfxContextHook * stream = m_AfxMatRenderContext->Hook_get();

		if (stream)
			stream->Draw_2(this, pLists, nLists);
		else
			m_Parent->Draw(pLists, nLists);
	}

	virtual void CopyToMeshBuilder(int iStartVert, int nVerts, int iStartIndex, int nIndices, int indexOffset, SOURCESDK::CMeshBuilder_csgo &builder)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 14)
	}

	virtual void Spew(int numVerts, int numIndices, const SOURCESDK::MeshDesc_t_csgo &desc)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 15)
	}

	virtual void ValidateData(int numVerts, int numIndices, const SOURCESDK::MeshDesc_t_csgo &desc)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 16)
	}

	virtual void LockMesh(int numVerts, int numIndices, SOURCESDK::MeshDesc_t_csgo &desc, SOURCESDK::MeshBuffersAllocationSettings_t_csgo *pSettings)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 17)
	}

	virtual void ModifyBegin(int firstVertex, int numVerts, int firstIndex, int numIndices, SOURCESDK::MeshDesc_t_csgo& desc)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 18)
	}

	virtual void ModifyEnd(SOURCESDK::MeshDesc_t_csgo& desc)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 19)
	}

	virtual void UnlockMesh(int numVerts, int numIndices, SOURCESDK::MeshDesc_t_csgo &desc)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 20)
	}

	virtual void ModifyBeginEx(bool bReadOnly, int firstVertex, int numVerts, int firstIndex, int numIndices, SOURCESDK::MeshDesc_t_csgo &desc)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 21)
	}

	virtual void SetFlexMesh(IMesh_csgo *pMesh, int nVertexOffset)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 22)
	}

	virtual void DisableFlexMesh()
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 23)
	}

	virtual void MarkAsDrawn()
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 24)
	}

	virtual void DrawModulated(const SOURCESDK::Vector4D_csgo &vecDiffuseModulation, int firstIndex = -1, int numIndices = 0)
	{
		// JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh,m_Parent,0,25)

		IAfxContextHook * stream = m_AfxMatRenderContext->Hook_get();

		if (stream)
			stream->DrawModulated(this, vecDiffuseModulation, firstIndex, numIndices);
		else
			m_Parent->DrawModulated(vecDiffuseModulation, firstIndex, numIndices);
	}

	virtual unsigned int ComputeMemoryUsed()
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 26)
	}

	virtual void *AccessRawHardwareDataStream(SOURCESDK::uint8 nRawStreamIndex, SOURCESDK::uint32 numBytes, SOURCESDK::uint32 uiFlags, void *pvContext)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 27)
	}

	virtual SOURCESDK::ICachedPerFrameMeshData_csgo *GetCachedPerFrameMeshData()
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 28)
	}

	virtual void ReconstructFromCachedPerFrameMeshData(SOURCESDK::ICachedPerFrameMeshData_csgo *pData)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 29)
	}

	//
	// IMeshEx_csgo:

	virtual void _UNKNOWN_030(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 30)
	}

	virtual void _UNKNOWN_031(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 31)
	}

	virtual void _UNKNOWN_032(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 32)
	}

	virtual void _UNKNOWN_033(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 33)
	}

	virtual void _UNKNOWN_034(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 34)
	}

	virtual void _UNKNOWN_035(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 35)
	}

	virtual void _UNKNOWN_036(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 36)
	}

	virtual void _UNKNOWN_037(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 37)
	}

	virtual void _UNKNOWN_038(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 38)
	}

	virtual void _UNKNOWN_039(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 39)
	}

	virtual void _UNKNOWN_040(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 40)
	}

	virtual void _UNKNOWN_041(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 41)
	}

	virtual void _UNKNOWN_042(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 42)
	}

	virtual void _UNKNOWN_043(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 43)
	}

	virtual void _UNKNOWN_044(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 44)
	}

	virtual void _UNKNOWN_045(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 45)
	}

	virtual void _UNKNOWN_046(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 46)
	}

	virtual void _UNKNOWN_047(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 47)
	}

	virtual void _UNKNOWN_048(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 0, 48)
	}

	//
	// IIndexBuffer_csgo:

	virtual int IndexCount() const
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 1, 0)
	}

	virtual SOURCESDK::MaterialIndexFormat_t_csgo IndexFormat() const
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 1, 1)
	}

	//virtual bool IsDynamic() const
	//{ JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh,m_Parent,1,2) }

	virtual void BeginCastBuffer(SOURCESDK::MaterialIndexFormat_t_csgo format)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 1, 3)
	}

	//virtual void EndCastBuffer()
	//{ JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh,m_Parent,1,4) }

	//virtual int GetRoomRemaining() const
	//{ JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh,m_Parent,1,5) }

	virtual bool Lock(int nMaxIndexCount, bool bAppend, SOURCESDK::IndexDesc_t_csgo &desc)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 1, 6)
	}

	virtual void Unlock(int nWrittenIndexCount, SOURCESDK::IndexDesc_t_csgo &desc)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 1, 7)
	}

	virtual void ModifyBegin(bool bReadOnly, int nFirstIndex, int nIndexCount, SOURCESDK::IndexDesc_t_csgo& desc)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 1, 8)
	}

	virtual void ModifyEnd(SOURCESDK::IndexDesc_t_csgo& desc)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 1, 9)
	}

	virtual void Spew(int nIndexCount, const SOURCESDK::IndexDesc_t_csgo &desc)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 1, 10)
	}

	virtual void ValidateData(int nIndexCount, const SOURCESDK::IndexDesc_t_csgo &desc)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 1, 11)
	}

	virtual IMesh_csgo* GetMesh()
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 1, 12)
	}

	virtual void _Unknown_13_IIndexBuffer_csgo(void * arg0)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 1, 13)
	}

	virtual bool _Unknown_14_IIndexBuffer_csgo(void)
	{
		JMP_CLASSMEMBERIFACE_OFS_FN_DBG(CAfxMesh, m_Parent, 1, 14)
	}

private:
	IMeshEx_csgo * m_Parent;
	IAfxMatRenderContext * m_AfxMatRenderContext;
};

#pragma warning(pop)

std::map<SOURCESDK::IMeshEx_csgo *, CAfxMesh *> g_MeshMap_csgo;
std::mutex g_MeshMap_csgo_Mutex;


class CAfxCallQueue
	: public SOURCESDK::CSGO::ICallQueue
	, public IAfxCallQueue
{
public:
	CAfxCallQueue(SOURCESDK::CSGO::ICallQueue * parent)
		: m_Parent(parent)
	{
	}

	void AfxMatRenderContext_set(IAfxMatRenderContext * value)
	{
		m_AfxMatRenderContext = value;
	}

public:

	//
	// IAfxCallQueue:

	virtual SOURCESDK::CSGO::ICallQueue * GetParent(void)
	{
		return m_Parent;
	}


protected:
	virtual void QueueFunctorInternal(SOURCESDK::CSGO::CFunctor *pFunctor)
	{
		IAfxContextHook * stream = m_AfxMatRenderContext->Hook_get();

		if (stream)
			stream->QueueFunctorInternal(this, pFunctor);
		else
			m_Parent->QueueFunctorInternal(pFunctor);
	}

private:
	SOURCESDK::CSGO::ICallQueue * m_Parent;
	IAfxMatRenderContext * m_AfxMatRenderContext;
};

std::map<SOURCESDK::CSGO::ICallQueue *, CAfxCallQueue *> g_CallQueueMap_csgo;
std::mutex g_CallQueueMap_csgo_Mutex;


//:009
typedef void (__stdcall * MatRenderContextHook_Bind_t)(
	DWORD *this_ptr,
	SOURCESDK::IMaterial_csgo * material,
	void *proxyData);

//:062
typedef SOURCESDK::IMeshEx_csgo* (_stdcall * MatRenderContextHook_GetDynamicMesh_t)(
	DWORD *this_ptr,
	bool buffered,
	SOURCESDK::IMesh_csgo* pVertexOverride,
	SOURCESDK::IMesh_csgo* pIndexOverride,
	SOURCESDK::IMaterial_csgo *pAutoBind);

//:081
typedef void (_stdcall * MatRenderContextHook_DrawScreenSpaceQuad_t)(
	DWORD *this_ptr,
	SOURCESDK::IMaterial_csgo * pMaterial);

//:113
typedef void (_stdcall * MatRenderContextHook_DrawScreenSpaceRectangle_t)(
	DWORD *this_ptr,
	SOURCESDK::IMaterial_csgo *pMaterial,
	int destx, int desty,
	int width, int height,
	float src_texture_x0, float src_texture_y0,
	float src_texture_x1, float src_texture_y1,
	int src_texture_width, int src_texture_height,
	void *pClientRenderable,
	int nXDice,
	int nYDice);

//:150
typedef SOURCESDK::CSGO::ICallQueue * (_stdcall * MatRenderContextHook_GetCallQueue_t)(
	DWORD *this_ptr);

//:167
typedef SOURCESDK::IMeshEx_csgo* (_stdcall * MatRenderContextHook_GetDynamicMeshEx_t)(
	DWORD *this_ptr,
	unsigned __int32 _unknown1,
	bool buffered,
	unsigned __int32 _unknown2,
	SOURCESDK::IMesh_csgo* pVertexOverride,
	SOURCESDK::IMesh_csgo* pIndexOverride,
	SOURCESDK::IMaterial_csgo *pAutoBind);

//:192
typedef void (_stdcall * MatRenderContextHook_DrawInstances_t)(
	DWORD *this_ptr,
	int nInstanceCount,
	const SOURCESDK::MeshInstanceData_t_csgo *pInstance);

struct CMatRenderContextDetours
{
	//:009
	MatRenderContextHook_Bind_t Bind;

	//:062
	MatRenderContextHook_GetDynamicMesh_t GetDynamicMesh;

	//:081
	MatRenderContextHook_DrawScreenSpaceQuad_t DrawScreenSpaceQuad;

	//:113
	MatRenderContextHook_DrawScreenSpaceRectangle_t DrawScreenSpaceRectangle;

	//:150
	MatRenderContextHook_GetCallQueue_t GetCallQueue;

	//:167
	MatRenderContextHook_GetDynamicMeshEx_t GetDynamicMeshEx;

	//:192
	MatRenderContextHook_DrawInstances_t DrawInstances;
};


class CMatRenderContextHook
	: public IAfxMatRenderContext
	, public IAfxMatRenderContextOrg
{
public:
	static std::map<SOURCESDK::IMatRenderContext_csgo *, CMatRenderContextHook *> m_Map;
	static std::mutex m_MapMutex;

	/// <remarks>
	/// This can be called from yet unknown contexts outside of the current material system context,
	/// due to the vtable being hooked!
	/// <remarks>
	static CMatRenderContextHook * GetMatRenderContextHook(SOURCESDK::IMatRenderContext_csgo * ctx)
	{
		CMatRenderContextHook * result;

		CMatRenderContextHook::m_MapMutex.lock();

		std::map<SOURCESDK::IMatRenderContext_csgo *, CMatRenderContextHook *>::iterator it = CMatRenderContextHook::m_Map.find(ctx);

		if (it != CMatRenderContextHook::m_Map.end())
		{
			result = it->second;
		}
		else
		{
			result = CMatRenderContextHook::m_Map[ctx] = new CMatRenderContextHook(ctx);
		}

		CMatRenderContextHook::m_MapMutex.unlock();

		return result;
	}

	/// <remarks>When calling it has to be made sure that no other ctx with the same vtbale is being hooked at the same time. (Use a mutex i.e.).</remarks>
	CMatRenderContextHook(SOURCESDK::IMatRenderContext_csgo * orgCtx)
		: m_Ctx(orgCtx)
		, m_Hook(0)
	{
		HooKVtable(orgCtx);
	}

	//
	// IAfxMatRenderContext:

	virtual IAfxMatRenderContextOrg * GetOrg(void)
	{
		return this;
	}

	virtual IAfxContextHook * Hook_get(void)
	{
		return m_Hook;
	}

	virtual void Hook_set(IAfxContextHook * value)
	{
		m_Hook = value;

	}

	//
	////

	virtual void ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false)
	{
		// This is unhooked, so pass through:

		m_Ctx->ClearBuffers(bClearColor, bClearDepth, bClearStencil);
	}

	virtual void ReadPixels(int x, int y, int width, int height, unsigned char *data, SOURCESDK::ImageFormat_csgo dstFormat, unsigned __int32 _unknown7 = 0)
	{
		// This is unhooked, so pass through:

		m_Ctx->ReadPixels(x, y, width, height, data, dstFormat, _unknown7);
	}

	virtual void ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		// This is unhooked, so pass through:

		m_Ctx->ClearColor4ub(r, g, b, a);
	}

	virtual void PushRenderTargetAndViewport(SOURCESDK::ITexture_csgo *pTexture, SOURCESDK::ITexture_csgo *pDepthTexture, int nViewX, int nViewY, int nViewW, int nViewH)
	{
		// This is unhooked, so pass through:

		m_Ctx->PushRenderTargetAndViewport(pTexture, pDepthTexture, nViewX, nViewY, nViewW, nViewH);
	}

	virtual void PopRenderTargetAndViewport(void)
	{
		// This is unhooked, so pass through:

		m_Ctx->PopRenderTargetAndViewport();
	}

	virtual SOURCESDK::CSGO::ICallQueue *GetCallQueue()
	{
		// This is hooked, so use detour:

		return m_Detours->GetCallQueue((DWORD *)m_Ctx);
	}

	virtual void DrawInstances(int nInstanceCount, const SOURCESDK::MeshInstanceData_t_csgo *pInstance)
	{
		// This is hooked, so use detour:

		m_Detours->DrawInstances((DWORD *)m_Ctx,
			nInstanceCount,
			pInstance);
	}

	//
	////

	void Hook_Bind(
		SOURCESDK::IMaterial_csgo * material,
		void *proxyData)
	{
		m_Detours->Bind((DWORD *)m_Ctx, DoOnMaterialHook(material), proxyData);
	}

	SOURCESDK::IMeshEx_csgo* Hook_GetDynamicMesh(
		bool buffered,
		SOURCESDK::IMesh_csgo* pVertexOverride,
		SOURCESDK::IMesh_csgo* pIndexOverride,
		SOURCESDK::IMaterial_csgo *pAutoBind)
	{
		SOURCESDK::IMeshEx_csgo * iMesh = m_Detours->GetDynamicMesh((DWORD *)m_Ctx,
			buffered,
			pVertexOverride,
			pIndexOverride,
			DoOnMaterialHook(pAutoBind));

		return AfxWrapMesh(iMesh);
	}

	void Hook_DrawScreenSpaceQuad(
		SOURCESDK::IMaterial_csgo * pMaterial)
	{
		m_Detours->DrawScreenSpaceQuad((DWORD *)m_Ctx,
			DoOnMaterialHook(pMaterial)
		);
	}

	void Hook_DrawScreenSpaceRectangle(
		SOURCESDK::IMaterial_csgo *pMaterial,
		int destx, int desty,
		int width, int height,
		float src_texture_x0, float src_texture_y0,
		float src_texture_x1, float src_texture_y1,
		int src_texture_width, int src_texture_height,
		void *pClientRenderable,
		int nXDice,
		int nYDice)
	{
		m_Detours->DrawScreenSpaceRectangle((DWORD *)m_Ctx,
			DoOnMaterialHook(pMaterial),
			destx, desty,
			width, height,
			src_texture_x0, src_texture_y0,
			src_texture_x1, src_texture_y1,
			src_texture_width, src_texture_height,
			pClientRenderable,
			nXDice,
			nYDice);
	}

	SOURCESDK::CSGO::ICallQueue * Hook_GetCallQueue()
	{
		SOURCESDK::CSGO::ICallQueue * callQueue = m_Detours->GetCallQueue((DWORD *)m_Ctx);

		return AfxWrapCallQueue(callQueue);
	}

	SOURCESDK::IMeshEx_csgo* Hook_GetDynamicMeshEx(
		unsigned __int32 _unknown1,
		bool buffered,
		unsigned __int32 _unknown2,
		SOURCESDK::IMesh_csgo* pVertexOverride,
		SOURCESDK::IMesh_csgo* pIndexOverride,
		SOURCESDK::IMaterial_csgo *pAutoBind)
	{
		SOURCESDK::IMeshEx_csgo * iMesh = m_Detours->GetDynamicMeshEx((DWORD *)m_Ctx,
			_unknown1,
			buffered,
			_unknown2,
			pVertexOverride,
			pIndexOverride,
			DoOnMaterialHook(pAutoBind));

		return AfxWrapMesh(iMesh);
	}

	void Hook_DrawInstances(
		int nInstanceCount,
		const SOURCESDK::MeshInstanceData_t_csgo *pInstance)
	{
		IAfxContextHook * afxStream = Hook_get();

		if (afxStream)
		{

			afxStream->DrawInstances(nInstanceCount, pInstance);
		}
		else
		{
			m_Detours->DrawInstances((DWORD *)m_Ctx,
				nInstanceCount,
				pInstance);
		}
	}

private:
	static std::map<int *, CMatRenderContextDetours> m_VtableMap;

	SOURCESDK::IMatRenderContext_csgo * m_Ctx;
	CMatRenderContextDetours * m_Detours;
	IAfxContextHook * m_Hook;

	void HooKVtable(SOURCESDK::IMatRenderContext_csgo * orgCtx);

	SOURCESDK::IMeshEx_csgo * AfxWrapMesh(SOURCESDK::IMeshEx_csgo * mesh)
	{
		if (!mesh)
			return 0;

		CAfxMesh * afxMesh;

		g_MeshMap_csgo_Mutex.lock();

		std::map<SOURCESDK::IMeshEx_csgo *, CAfxMesh *>::iterator it = g_MeshMap_csgo.find(mesh);


		if (it != g_MeshMap_csgo.end())
		{
			//Tier0_Msg("Found known IMesh 0x%08x.\n", (DWORD)iMesh);
			afxMesh = it->second; // re-use
		}
		else
		{
			//Tier0_Msg("New IMesh 0x%08x.\n", (DWORD)iMesh);
			afxMesh = new CAfxMesh(mesh);

			g_MeshMap_csgo[mesh] = afxMesh; // track new mesh
			g_MeshMap_csgo[afxMesh] = afxMesh; // make sure we won't wrap ourself!
		}

		g_MeshMap_csgo_Mutex.unlock();

		afxMesh->AfxMatRenderContext_set(this); // tell it about us :-)

		return afxMesh;
	}

	SOURCESDK::CSGO::ICallQueue * AfxWrapCallQueue(SOURCESDK::CSGO::ICallQueue * callQueue)
	{
		if (!callQueue)
			return 0;

		CAfxCallQueue * afxCallQueue;

		g_CallQueueMap_csgo_Mutex.lock();

		std::map<SOURCESDK::CSGO::ICallQueue *, CAfxCallQueue *>::iterator it = g_CallQueueMap_csgo.find(afxCallQueue);


		if (it != g_CallQueueMap_csgo.end())
		{
			afxCallQueue = it->second; // re-use
		}
		else
		{
			afxCallQueue = new CAfxCallQueue(callQueue);

			g_CallQueueMap_csgo[callQueue] = afxCallQueue; // track new mesh
			g_CallQueueMap_csgo[afxCallQueue] = afxCallQueue; // make sure we won't wrap ourself!
		}

		g_CallQueueMap_csgo_Mutex.unlock();

		afxCallQueue->AfxMatRenderContext_set(this); // tell it about us :-)

		return afxCallQueue;
	}

	SOURCESDK::IMaterial_csgo * DoOnMaterialHook(SOURCESDK::IMaterial_csgo * value)
	{
		IAfxContextHook * afxStream = Hook_get();
		if (afxStream && value)
			return afxStream->MaterialHook(value);

		return value;
	}
};


std::map<SOURCESDK::IMatRenderContext_csgo *, CMatRenderContextHook *> CMatRenderContextHook::m_Map;
std::mutex CMatRenderContextHook::m_MapMutex;
std::map<int *, CMatRenderContextDetours> CMatRenderContextHook::m_VtableMap;


void __stdcall MatRenderContextHook_Bind(
	DWORD *this_ptr,
	SOURCESDK::IMaterial_csgo * material,
	void *proxyData)
{
	CMatRenderContextHook * ctxh = CMatRenderContextHook::GetMatRenderContextHook((SOURCESDK::IMatRenderContext_csgo *)this_ptr);
	return ctxh->Hook_Bind(material, proxyData);
}

SOURCESDK::IMeshEx_csgo* _stdcall MatRenderContextHook_GetDynamicMesh(
	DWORD *this_ptr,
	bool buffered,
	SOURCESDK::IMesh_csgo* pVertexOverride,
	SOURCESDK::IMesh_csgo* pIndexOverride,
	SOURCESDK::IMaterial_csgo *pAutoBind)
{
	CMatRenderContextHook * ctxh = CMatRenderContextHook::GetMatRenderContextHook((SOURCESDK::IMatRenderContext_csgo *)this_ptr);
	return ctxh->Hook_GetDynamicMesh(buffered, pVertexOverride, pIndexOverride, pAutoBind);
}

void _stdcall MatRenderContextHook_DrawScreenSpaceQuad(
	DWORD *this_ptr,
	SOURCESDK::IMaterial_csgo * pMaterial)
{
	CMatRenderContextHook * ctxh = CMatRenderContextHook::GetMatRenderContextHook((SOURCESDK::IMatRenderContext_csgo *)this_ptr);
	return ctxh->Hook_DrawScreenSpaceQuad(pMaterial);
}

void _stdcall MatRenderContextHook_DrawScreenSpaceRectangle(
	DWORD *this_ptr,
	SOURCESDK::IMaterial_csgo *pMaterial,
	int destx, int desty,
	int width, int height,
	float src_texture_x0, float src_texture_y0,
	float src_texture_x1, float src_texture_y1,
	int src_texture_width, int src_texture_height,
	void *pClientRenderable,
	int nXDice,
	int nYDice)
{
	CMatRenderContextHook * ctxh = CMatRenderContextHook::GetMatRenderContextHook((SOURCESDK::IMatRenderContext_csgo *)this_ptr);
	return ctxh->Hook_DrawScreenSpaceRectangle(
		pMaterial,
		destx, desty,
		width, height,
		src_texture_x0, src_texture_y0,
		src_texture_x1, src_texture_y1,
		src_texture_width, src_texture_height,
		pClientRenderable,
		nXDice,
		nYDice);
}

SOURCESDK::CSGO::ICallQueue * _stdcall MatRenderContextHook_GetCallQueue(
	DWORD *this_ptr)
{
	CMatRenderContextHook * ctxh = CMatRenderContextHook::GetMatRenderContextHook((SOURCESDK::IMatRenderContext_csgo *)this_ptr);
	return ctxh->Hook_GetCallQueue();
}

SOURCESDK::IMeshEx_csgo* _stdcall MatRenderContextHook_GetDynamicMeshEx(
	DWORD *this_ptr,
	unsigned __int32 _unknown1,
	bool buffered,
	unsigned __int32 _unknown2,
	SOURCESDK::IMesh_csgo* pVertexOverride,
	SOURCESDK::IMesh_csgo* pIndexOverride,
	SOURCESDK::IMaterial_csgo *pAutoBind)
{
	CMatRenderContextHook * ctxh = CMatRenderContextHook::GetMatRenderContextHook((SOURCESDK::IMatRenderContext_csgo *)this_ptr);
	return ctxh->Hook_GetDynamicMeshEx(
		_unknown1,
		buffered,
		_unknown2,
		pVertexOverride,
		pIndexOverride,
		pAutoBind);
}

void _stdcall MatRenderContextHook_DrawInstances(
	DWORD *this_ptr,
	int nInstanceCount,
	const SOURCESDK::MeshInstanceData_t_csgo *pInstance)
{
	CMatRenderContextHook * ctxh = CMatRenderContextHook::GetMatRenderContextHook((SOURCESDK::IMatRenderContext_csgo *)this_ptr);
	return ctxh->Hook_DrawInstances(
		nInstanceCount,
		pInstance);
}

void CMatRenderContextHook::HooKVtable(SOURCESDK::IMatRenderContext_csgo * orgCtx)
{
	int * vtable = *(int**)orgCtx;

	std::map<int *, CMatRenderContextDetours>::iterator it = m_VtableMap.find(vtable);

	if (it != m_VtableMap.end())
	{
		m_Detours = &(it->second);
		return;
	}

	m_Detours = &(m_VtableMap[vtable]);

	//OutputDebugString("HooKVtable DETOUR BEGIN\n");
	m_Detours->Bind = (MatRenderContextHook_Bind_t)DetourIfacePtr((DWORD *)&(vtable[9]), MatRenderContextHook_Bind);
	m_Detours->GetDynamicMesh = (MatRenderContextHook_GetDynamicMesh_t)DetourIfacePtr((DWORD *)&(vtable[62]), MatRenderContextHook_GetDynamicMesh);
	m_Detours->DrawScreenSpaceQuad = (MatRenderContextHook_DrawScreenSpaceQuad_t)DetourIfacePtr((DWORD *)&(vtable[81]), MatRenderContextHook_DrawScreenSpaceQuad);
	m_Detours->DrawScreenSpaceRectangle = (MatRenderContextHook_DrawScreenSpaceRectangle_t)DetourIfacePtr((DWORD *)&(vtable[113]), MatRenderContextHook_DrawScreenSpaceRectangle);
	m_Detours->GetCallQueue = (MatRenderContextHook_GetCallQueue_t)DetourIfacePtr((DWORD *)&(vtable[150]), MatRenderContextHook_GetCallQueue);
	m_Detours->GetDynamicMeshEx = (MatRenderContextHook_GetDynamicMeshEx_t)DetourIfacePtr((DWORD *)&(vtable[167]), MatRenderContextHook_GetDynamicMeshEx);
	m_Detours->DrawInstances = (MatRenderContextHook_DrawInstances_t)DetourIfacePtr((DWORD *)&(vtable[192]), MatRenderContextHook_DrawInstances);
	//OutputDebugString("HooKVtable DETOUR END\n");
}

IAfxMatRenderContext * MatRenderContextHook(SOURCESDK::IMaterialSystem_csgo * materialSystem)
{
	SOURCESDK::IMatRenderContext_csgo * ctx = materialSystem->GetRenderContext();
	ctx->Release(); // GetRenderContext calls AddRef on Returned.

	return CMatRenderContextHook::GetMatRenderContextHook(ctx);
}

void MatRenderContextHook_Shutdown(void)
{
	//TODO: not sure if good idea atm, might be used afterwards?
	for (std::map<SOURCESDK::IMeshEx_csgo *, CAfxMesh *>::iterator it = g_MeshMap_csgo.begin(); it != g_MeshMap_csgo.end(); ++it)
	{
		delete it->second;
	}

	//TODO: not sure if good idea atm, might be used afterwards?
	for (std::map<SOURCESDK::IMatRenderContext_csgo *, CMatRenderContextHook *>::iterator it = CMatRenderContextHook::m_Map.begin(); it != CMatRenderContextHook::m_Map.end(); ++it)
	{
		delete it->second;
	}
}
