#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-01-06 dominik.matrixstorm.com
//
// First changes:
// 2015-06-26 dominik.matrixstorm.com

#include "AfxStreams.h"

#include "SourceInterfaces.h"
#include "WrpVEngineClient.h"
#include "csgo_CSkyBoxView.h"
#include "csgo_view.h"

#include <shared/StringTools.h>
#include <shared/FileTools.h>
#include <shared/RawOutput.h>

#include <Windows.h>

#include <stdio.h>
#include <sstream>
#include <iomanip>

extern WrpVEngineClient * g_VEngineClient;

CAfxStreams g_AfxStreams;

////////////////////////////////////////////////////////////////////////////////

/* Doesn't work for some reason.
void DebugDepthFixDraw(IMesh_csgo * pMesh)
{
	MeshDesc_t_csgo meshDesc;
	VertexDesc_t_csgo vertexDesc;

	int nMaxVertexCount, nMaxIndexCount;
	nMaxVertexCount =  nMaxIndexCount = 4;

	pMesh->SetPrimitiveType( MATERIAL_POINTS );

	pMesh->LockMesh(nMaxVertexCount, nMaxIndexCount, meshDesc, 0);

	IIndexBuffer_csgo * m_pIndexBuffer = pMesh;
	int m_nIndexCount = 0;
	int m_nMaxIndexCount = nMaxIndexCount;

	int m_nIndexOffset = meshDesc.m_nFirstVertex;
	unsigned short * m_pIndices = meshDesc.m_pIndices;
	unsigned int m_nIndexSize = meshDesc.m_nIndexSize;
	int m_nCurrentIndex = 0;

	IVertexBuffer_csgo * m_pVertexBuffer = pMesh;
	memcpy( static_cast<VertexDesc_t_csgo*>( &vertexDesc ), static_cast<const VertexDesc_t_csgo*>( &meshDesc ), sizeof(VertexDesc_t_csgo) );
	int m_nMaxVertexCount = nMaxVertexCount;

	unsigned int m_nTotalVertexCount = 0;
	//unsigned int m_nBufferOffset = static_cast< const VertexDesc_t_csgo* >( &meshDesc )->m_nOffset;
	//unsigned int m_nBufferFirstVertex = meshDesc.m_nFirstVertex;

	int m_nVertexCount = 0;

	int m_nCurrentVertex = 0;

	float * m_pCurrPosition = vertexDesc.m_pPosition;
	float * m_pCurrNormal = vertexDesc.m_pNormal;
	float * m_pCurrTexCoord[VERTEX_MAX_TEXTURE_COORDINATES_csgo];
	for ( size_t i = 0; i < VERTEX_MAX_TEXTURE_COORDINATES_csgo; i++ )
	{
		m_pCurrTexCoord[i] = vertexDesc.m_pTexCoord[i];
	}
	unsigned char * m_pCurrColor = vertexDesc.m_pColor;
	// BEGIN actual "drawing":

	// Position:
	{
		float *pDst = m_pCurrPosition;
		*pDst++ = 0.0f;
		*pDst++ = 0.0f;
		*pDst = 0.0f;
	}

	// Color:
	if(false) {
		int r = 0;
		int g = 0;
		int b = 0;
		int a = 0;

		int col = b | (g << 8) | (r << 16) | (a << 24);

		*(int*)m_pCurrColor = col;
	}


	// Normal:
	{
		float *pDst = m_pCurrNormal;
		*pDst++ = 0.0f;
		*pDst++ = 0.0f;
		*pDst = 0.0f;
	}


	// TextCoord:
	{
		float *pDst = m_pCurrTexCoord[0];
		*pDst++ = 1.0f;
		*pDst++ = 0.0f;
		*pDst = 0.0f;
	}

	// AdvanceVertex:
	{
		if ( ++m_nCurrentVertex > m_nVertexCount )
		{
			m_nVertexCount = m_nCurrentVertex;
		}

		//m_pCurrPosition = reinterpret_cast<float*>( reinterpret_cast<unsigned char*>( m_pCurrPosition ) + vertexDesc.m_VertexSize_Position );
		//m_pCurrColor += vertexDesc.m_VertexSize_Color;
	}

	// End drawing:

	{
		int nIndexCount = 1;
		if(0 != m_nIndexSize)
		{
			int nMaxIndices = m_nMaxIndexCount - m_nCurrentIndex;
			nIndexCount = min( nMaxIndices, nIndexCount );
			if ( nIndexCount != 0 )
			{
				unsigned short *pIndices = &m_pIndices[m_nCurrentIndex];

				//GenerateSequentialIndexBuffer( pIndices, nIndexCount, m_nIndexOffset );
				// What about m_IndexOffset? -> dunno.

				*pIndices = m_nIndexOffset;
			}

			m_nCurrentIndex += nIndexCount * m_nIndexSize;
			if ( m_nCurrentIndex > m_nIndexCount )
			{
				m_nIndexCount = m_nCurrentIndex; 
			}
		}
	}
	
//	Tier0_Msg("Spew: ");
//	pMesh->Spew( m_nVertexCount, m_nIndexCount, meshDesc );

//	pMesh->ValidateData( m_nVertexCount ,m_nIndexCount, meshDesc );

	pMesh->UnlockMesh( m_nVertexCount, m_nIndexCount, meshDesc );

	// Draw!!!!
	pMesh->Draw();
}
*/

// CAfxFileTracker /////////////////////////////////////////////////////////////

void CAfxFileTracker::TrackFile(char const * filePath)
{
	std::string str(filePath);

	m_FilePaths.push(str);
}

void CAfxFileTracker::WaitForFiles(unsigned int maxUnfinishedFiles)
{
	while(m_FilePaths.size() > maxUnfinishedFiles)
	{
		FILE * file;

		//Tier0_Msg("Waiting for file \"%s\" .... ", m_FilePaths.front().c_str());

		do
		{
			file = fopen(m_FilePaths.front().c_str(), "rb+");
		}while(!file);

		fclose(file);

		//Tier0_Msg("done.\n");

		m_FilePaths.pop();
	}
}

// CAfxStream //////////////////////////////////////////////////////////////////

CAfxStream::CAfxStream()
{
}

CAfxStream::~CAfxStream()
{
}

void CAfxStream::LevelShutdown(IAfxStreams4Stream * streams)
{
}

// CAfxRenderViewStream ////////////////////////////////////////////////////////

CAfxRenderViewStream::CAfxRenderViewStream()
: CAfxStream()
, m_Streams(0)
, m_DrawViewModel(true)
, m_DrawHud(false)
, m_StreamRenderType(SRT_RenderView)
{
}

void CAfxRenderViewStream::StreamAttach(IAfxStreams4Stream * streams)
{
	m_Streams = streams;
}

void CAfxRenderViewStream::StreamDetach(IAfxStreams4Stream * streams)
{
	m_Streams = 0;
}

char const * CAfxRenderViewStream::AttachCommands_get(void)
{
	return m_AttachCommands.c_str();
}

void CAfxRenderViewStream::AttachCommands_set(char const * value)
{
	m_AttachCommands.assign(value);
}

char const * CAfxRenderViewStream::DetachCommands_get(void)
{
	return m_DetachCommands.c_str();
}

void CAfxRenderViewStream::DetachCommands_set(char const * value)
{
	m_DetachCommands.assign(value);
}

bool CAfxRenderViewStream::DrawHud_get(void)
{
	return m_DrawHud;
}

void CAfxRenderViewStream::DrawHud_set(bool value)
{
	m_DrawHud = value;
}

bool CAfxRenderViewStream::DrawViewModel_get(void)
{
	return m_DrawViewModel;
}

void CAfxRenderViewStream::DrawViewModel_set(bool value)
{
	m_DrawViewModel = value;
}

CAfxRenderViewStream::StreamRenderType CAfxRenderViewStream::StreamRenderType_get(void)
{
	return m_StreamRenderType;
}

void CAfxRenderViewStream::StreamRenderType_set(StreamRenderType value)
{
	m_StreamRenderType = value;
}

// CAfxRecordStream ////////////////////////////////////////////////////////////

CAfxRecordStream::CAfxRecordStream(char const * streamName)
: CAfxStream()
, m_StreamName(streamName)
, m_Record(true)
{
}

bool CAfxRecordStream::Record_get(void)
{
	return m_Record;
}

void CAfxRecordStream::Record_set(bool value)
{
	m_Record = value;
}

void CAfxRecordStream::RecordStart()
{
	m_TriedCreatePath = false;
	m_SucceededCreatePath = false;
}

bool CAfxRecordStream::CreateCapturePath(const std::wstring & takeDir, int frameNumber, bool isBmpAndNotTga, std::wstring &outPath)
{
	if(!m_TriedCreatePath)
	{
		m_TriedCreatePath = true;
		std::wstring wideStreamName;
		if(AnsiStringToWideString(m_StreamName.c_str(), wideStreamName))
		{
			m_CapturePath = takeDir;
			m_CapturePath.append(L"\\");
			m_CapturePath.append(wideStreamName);

			bool dirCreated = CreatePath(m_CapturePath.c_str(), m_CapturePath);
			if(dirCreated)
			{
				m_SucceededCreatePath = true;
			}
			else
			{
				Tier0_Warning("ERROR: could not create \"%s\"\n", m_CapturePath.c_str());
			}
		}
		else
		{
			Tier0_Warning("Error: Failed to convert stream name \"%s\" to a wide string.\n", m_StreamName.c_str());
		}
	}

	if(!m_SucceededCreatePath)
		return false;

	std::wostringstream os;
	os << m_CapturePath << L"\\" << std::setfill(L'0') << std::setw(5) << frameNumber << std::setw(0) << (isBmpAndNotTga ? L".bmp" : L".tga");

	outPath = os.str();

	return true;
}

void CAfxRecordStream::RecordEnd()
{

}

char const * CAfxRecordStream::StreamName_get(void)
{
	return m_StreamName.c_str();
}

// CAfxSingleStream ////////////////////////////////////////////////////////////

CAfxSingleStream::CAfxSingleStream(char const * streamName, CAfxRenderViewStream * stream)
: CAfxRecordStream(streamName)
, m_Stream(stream)
{
}

CAfxSingleStream::~CAfxSingleStream()
{
	delete m_Stream;
}

CAfxRenderViewStream * CAfxSingleStream::Stream_get(void)
{
	return m_Stream;
}

void CAfxSingleStream::LevelShutdown(IAfxStreams4Stream * streams)
{
	m_Stream->LevelShutdown(streams);
}

// CAfxTwinStream //////////////////////////////////////////////////////////////

CAfxTwinStream::CAfxTwinStream(char const * streamName, CAfxRenderViewStream * streamA, CAfxRenderViewStream * streamB, StreamCombineType streamCombineType)
: CAfxRecordStream(streamName)
, m_StreamA(streamA)
, m_StreamB(streamB)
, m_StreamCombineType(streamCombineType)
{
}

CAfxTwinStream::~CAfxTwinStream()
{
	delete m_StreamA;
	delete m_StreamB;
}

void CAfxTwinStream::LevelShutdown(IAfxStreams4Stream * streams)
{
	m_StreamA->LevelShutdown(streams);
	m_StreamB->LevelShutdown(streams);
}

CAfxRenderViewStream * CAfxTwinStream::StreamA_get()
{
	return m_StreamA;
}

CAfxRenderViewStream * CAfxTwinStream::StreamB_get()
{
	return m_StreamB;
}

CAfxTwinStream::StreamCombineType CAfxTwinStream::StreamCombineType_get(void)
{
	return m_StreamCombineType;
}

void CAfxTwinStream::StreamCombineType_set(StreamCombineType value)
{
	m_StreamCombineType = value;
}


// CAfxDeveloperStream /////////////////////////////////////////////////////////

CAfxDeveloperStream::CAfxDeveloperStream()
: CAfxRenderViewStream()
, m_ReplaceUpdate(false)
, m_Replace (false)
, m_ReplaceMaterial(0)
, m_ReplaceMaterialActive(false)
, m_BlockDraw(false)
{
}
	
CAfxDeveloperStream::~CAfxDeveloperStream()
{
	delete m_ReplaceMaterial;
}

void CAfxDeveloperStream::MatchName_set(char const * value)
{
	m_MatchName.assign(value);
}

char const * CAfxDeveloperStream::MatchName_get(void)
{
	return m_MatchName.c_str();
}

void CAfxDeveloperStream::MatchTextureGroupName_set(char const * value)
{
	m_MatchTextureGroupName.assign(value);
}

char const * CAfxDeveloperStream::MatchTextureGroupName_get(void)
{
	return m_MatchTextureGroupName.c_str();
}

void CAfxDeveloperStream::ReplaceName_set(char const * name)
{
	m_ReplaceName.assign(name);
	m_ReplaceUpdate = true;
}

char const * CAfxDeveloperStream::ReplaceName_get(void)
{
	return m_ReplaceName.c_str();
}

void CAfxDeveloperStream::BlockDraw_set(bool value)
{
	m_BlockDraw = value;
}

bool CAfxDeveloperStream::BlockDraw_get(void)
{
	return m_BlockDraw;
}

void CAfxDeveloperStream::StreamAttach(IAfxStreams4Stream * streams)
{
	CAfxRenderViewStream::StreamAttach(streams);

	if(m_ReplaceUpdate)
	{
		m_ReplaceUpdate = false;
		m_Replace = !m_ReplaceName.empty();

		if(m_Replace)
		{
			delete m_ReplaceMaterial; m_ReplaceMaterial = 0;
			m_ReplaceMaterial = new CAfxMaterial(streams->GetFreeMaster(), streams->GetMaterialSystem()->FindMaterial(m_ReplaceName.c_str(),NULL));
		}
	}

	if(m_Replace)
	{
		streams->OnMaterialHook_set(this);
		streams->OnDraw_set(this);
		streams->OnDraw_2_set(this);
		streams->OnDrawModulated_set(this);
		streams->OnDrawInstances_set(this);
	}
}

void CAfxDeveloperStream::StreamDetach(IAfxStreams4Stream * streams)
{
	streams->OnDrawInstances_set(0);
	streams->OnDrawModulated_set(0);
	streams->OnDraw_2_set(0);
	streams->OnDraw_set(0);
	streams->OnMaterialHook_set(0);

	CAfxRenderViewStream::StreamDetach(streams);
}

IMaterial_csgo * CAfxDeveloperStream::MaterialHook(IAfxMatRenderContext * ctx, IMaterial_csgo * material)
{
	bool replace =
		m_Replace
		&& !strcmp(material->GetTextureGroupName(), m_MatchTextureGroupName.c_str())
		&& !strcmp(material->GetName(), m_MatchName.c_str())
	;

	m_ReplaceMaterialActive = replace;

	return replace ? m_ReplaceMaterial->GetMaterial() : material;
}

void CAfxDeveloperStream::DrawInstances(IAfxMatRenderContext * ctx, int nInstanceCount, const MeshInstanceData_t_csgo *pInstance )
{
	//if(m_ReplaceMaterialActive) Tier0_Msg("CAfxDeveloperStream::DrawInstances\n");

	if(!(m_BlockDraw && m_ReplaceMaterialActive)) 
		ctx->GetParent()->DrawInstances(nInstanceCount, pInstance);
}

void CAfxDeveloperStream::Draw(IAfxMesh * am, int firstIndex, int numIndices)
{
	//if(m_ReplaceMaterialActive) Tier0_Msg("CAfxDeveloperStream::Draw\n");

	if(!(m_BlockDraw && m_ReplaceMaterialActive)) 
		am->GetParent()->Draw(firstIndex, numIndices);
	else
		am->GetParent()->MarkAsDrawn();
}

void CAfxDeveloperStream::Draw_2(IAfxMesh * am, CPrimList_csgo *pLists, int nLists)
{
	//if(m_ReplaceMaterialActive) Tier0_Msg("CAfxDeveloperStream::Draw_2\n");

	if(!(m_BlockDraw && m_ReplaceMaterialActive)) 
		am->GetParent()->Draw(pLists, nLists);
	else
		am->GetParent()->MarkAsDrawn();
}

void CAfxDeveloperStream::DrawModulated(IAfxMesh * am, const Vector4D_csgo &vecDiffuseModulation, int firstIndex, int numIndices)
{
	//if(m_ReplaceMaterialActive) Tier0_Msg("CAfxDeveloperStream::DrawModulated\n");

	if(!(m_BlockDraw && m_ReplaceMaterialActive)) 
		am->GetParent()->DrawModulated(vecDiffuseModulation, firstIndex, numIndices);
	else
		am->GetParent()->MarkAsDrawn();
}

// CAfxBaseFxStream ////////////////////////////////////////////////////////////

CAfxBaseFxStream::CAfxBaseFxStream()
: CAfxRenderViewStream()
, m_ClientEffectTexturesAction(AA_Draw)
, m_WorldTexturesAction(AA_Draw)
, m_SkyBoxTexturesAction(AA_Draw)
, m_StaticPropTexturesAction(AA_Draw)
, m_CableAction(AA_Draw)
, m_PlayerModelsAction(AA_Draw)
, m_WeaponModelsAction(AA_Draw)
, m_StattrackAction(AA_Draw)
, m_ShellModelsAction(AA_Draw)
, m_OtherModelsAction(AA_Draw)
, m_DecalTexturesAction(AA_Draw)
, m_EffectsAction(AA_Draw)
, m_ShellParticleAction(AA_Draw)
, m_OtherParticleAction(AA_Draw)
, m_StickerAction(AA_Draw)
, m_ErrorMaterialAction(AA_Draw)
, m_TestAction(false)
, m_DepthVal(1)
, m_DepthValMax(1024)
, m_CurrentAction(0)
, m_DepthAction(0)
, m_Depth24Action(0)
, m_MatteAction(0)
, m_PassthroughAction(0)
, m_InvisibleAction(0)
, m_NoDrawAction(0)
, m_BlackAction(0)
, m_WhiteAction(0)
, m_DebugDumpAction(0)
, m_StandardActionsInitialized(false)
, m_BoundAction(false)
, m_DebugPrint(false)
{
}

CAfxBaseFxStream::~CAfxBaseFxStream()
{
	InvalidateMap();
	InvalidateSpritecardHookActions();
	InvalidateVertexLitGenericHookActions();

	if(m_WhiteAction) m_WhiteAction->Release();
	if(m_BlackAction) m_BlackAction->Release();
	if(m_MatteAction) m_MatteAction->Release();
	if(m_Depth24Action) m_Depth24Action->Release();
	if(m_DepthAction) m_DepthAction->Release();
	if(m_DebugDumpAction) m_DebugDumpAction->Release();
	if(m_NoDrawAction) m_NoDrawAction->Release();
	if(m_InvisibleAction) m_InvisibleAction->Release();
	if(m_PassthroughAction) m_PassthroughAction->Release();
}

void CAfxBaseFxStream::LevelShutdown(IAfxStreams4Stream * streams)
{
	InvalidateMap();
	InvalidateSpritecardHookActions();
	InvalidateVertexLitGenericHookActions();

	CAfxStream::LevelShutdown(streams);
}

void CAfxBaseFxStream::StreamAttach(IAfxStreams4Stream * streams)
{
	CAfxRenderViewStream::StreamAttach(streams);

	if(!m_StandardActionsInitialized)
	{
		m_PassthroughAction = new CAction(this);
		m_PassthroughAction->AddRef();
		
		m_InvisibleAction = new CActionInvisible(this, streams->GetFreeMaster(), streams->GetMaterialSystem());
		m_InvisibleAction->AddRef();

		m_NoDrawAction = new CActionNoDraw(this);
		m_NoDrawAction->AddRef();

		m_DebugDumpAction = new CActionDebugDump(this);
		m_DebugDumpAction->AddRef();

		m_DepthAction = new CActionUnlitGenericFallback(
			this
			, CActionAfxVertexLitGenericHookKey(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::AFXMODE_0, 0.7f)
			, streams->GetFreeMaster(), streams->GetMaterialSystem()
			, "afx/white"
			);
		m_DepthAction->AddRef();

		m_Depth24Action = new CActionUnlitGenericFallback(
			this
			, CActionAfxVertexLitGenericHookKey(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::AFXMODE_1, 0.7f)
			, streams->GetFreeMaster(), streams->GetMaterialSystem()
			, "afx/white"
			);
		m_Depth24Action->AddRef();

		m_MatteAction = new CActionUnlitGenericFallback(
			this
			, CActionAfxVertexLitGenericHookKey(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::AFXMODE_2, 0.7f)
			, streams->GetFreeMaster(), streams->GetMaterialSystem()
			, "afx/white"
			);
		m_MatteAction->AddRef();
		
		m_BlackAction = new CActionUnlitGenericFallback(
			this
			, CActionAfxVertexLitGenericHookKey(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::AFXMODE_3, 0.7f)
			, streams->GetFreeMaster(), streams->GetMaterialSystem()
			, "afx/white"
			);
		m_BlackAction->AddRef();

		m_WhiteAction = new CActionUnlitGenericFallback(
			this
			, CActionAfxVertexLitGenericHookKey(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::AFXMODE_4, 0.7f)
			, streams->GetFreeMaster(), streams->GetMaterialSystem()
			, "afx/white"
			);
		m_WhiteAction->AddRef();

		m_StandardActionsInitialized = true;
	}

	// Set a default action, just in case:
	m_CurrentAction = m_PassthroughAction;

	streams->OnMaterialHook_set(this);
	streams->OnDrawInstances_set(this);
	streams->OnDraw_set(this);
	streams->OnDraw_2_set(this);
	streams->OnDrawModulated_set(this);
	streams->OnSetVertexShader_set(this);
	streams->OnSetPixelShader_set(this);
}

void CAfxBaseFxStream::StreamDetach(IAfxStreams4Stream * streams)
{
	if(m_BoundAction)
	{
		m_CurrentAction->AfxUnbind(streams->GetCurrentContext());
		m_BoundAction = false;
	}

	streams->OnSetPixelShader_set(0);
	streams->OnSetVertexShader_set(0);
	streams->OnDrawModulated_set(0);
	streams->OnDraw_2_set(0);
	streams->OnDraw_set(0);
	streams->OnDrawInstances_set(0);
	streams->OnMaterialHook_set(0);

	CAfxRenderViewStream::StreamDetach(streams);
}

IMaterial_csgo * CAfxBaseFxStream::MaterialHook(IAfxMatRenderContext * ctx, IMaterial_csgo * material)
{
	if(m_BoundAction)
	{
		m_CurrentAction->AfxUnbind(ctx);
		m_BoundAction = false;
	}

	CAfxMaterialKey key(material);

	std::map<CAfxMaterialKey, CAction *>::iterator it = m_Map.find(key);

	if(it != m_Map.end())
		m_CurrentAction = it->second;
	else
	{
		// determine current action and cache it.

		m_CurrentAction = GetAction(material);
		m_CurrentAction->AddRef();

		m_Map[key] = m_CurrentAction;

		if(m_DebugPrint) Tier0_Msg("\n");
	}

	m_BoundAction = true;

	return m_CurrentAction->MaterialHook(ctx, material);
}

CAfxBaseFxStream::CAction * CAfxBaseFxStream::GetAction(IMaterial_csgo * material)
{
	const char * groupName =  material->GetTextureGroupName();
	const char * name = material->GetName();
	const char * shaderName = material->GetShaderName();
	bool isErrorMaterial = material->IsErrorMaterial();

	if(m_DebugPrint)
		Tier0_Msg("Stream: GetAction: %s|%s|%s%s -> ", groupName, name, shaderName, isErrorMaterial ? "|isErrorMaterial" : "");

	if(isErrorMaterial)
		return GetAction(material, m_ErrorMaterialAction, true);
	else
	if(!strcmp("ClientEffect textures", groupName))
		return GetAction(material, m_ClientEffectTexturesAction, true);
	else
	if(!strcmp("Decal textures", groupName))
		return GetAction(material, m_DecalTexturesAction, true);
	else
	if(!strcmp("World textures", groupName))
	{
		return GetAction(material, m_WorldTexturesAction, false);
	}
	else
	if(!strcmp("SkyBox textures", groupName))
		return GetAction(material, m_SkyBoxTexturesAction, false);
	else
	if(!strcmp("StaticProp textures", groupName))
	{
		if(StringBeginsWith(name, "models/weapons/"))
			return GetAction(material, m_WeaponModelsAction, false);
		else
			return GetAction(material, m_StaticPropTexturesAction, false);
	}
	else
	if(!strcmp("Model textures", groupName))
	{
		if(StringBeginsWith(name, "models/player/"))
			return GetAction(material, m_PlayerModelsAction, false);
		else
		if(StringBeginsWith(name, "models/weapons/"))
		{
			if(StringBeginsWith(name, "models/weapons/stattrack/"))
				return GetAction(material, m_StattrackAction, false);
			else
				return GetAction(material, m_WeaponModelsAction, false);
		}
		else
		if(StringBeginsWith(name, "models/shells/"))
			return GetAction(material, m_ShellModelsAction, false);
		else
			return GetAction(material, m_OtherModelsAction, false);
	}
	else
	if(!strcmp("Other textures", groupName))
	{
		if(StringBeginsWith(name, "cable/"))
			return GetAction(material, m_CableAction, true);
		else
		if(StringBeginsWith(name, "cs_custom_material_"))
			return GetAction(material, m_WeaponModelsAction, false);
		else
		if(StringBeginsWith(name, "effects/"))
			return GetAction(material, m_EffectsAction, true);
		else
		if(StringBeginsWith(name, "particle/"))
		{
			if(StringBeginsWith(name, "particle/shells/"))
				return GetAction(material, m_ShellParticleAction, true);
			else
				return GetAction(material, m_OtherParticleAction, true);
		}
		else
		if(StringBeginsWith(name, "sticker_"))
				return GetAction(material, m_StickerAction, false);
		else
		{
			if(m_DebugPrint) Tier0_Msg("(unhandled)");
			return m_PassthroughAction;
		}
	}
	else
	if(!strcmp("Particle textures", groupName))
	{
		if(StringBeginsWith(name, "particle/shells/"))
			return GetAction(material, m_ShellParticleAction, true);
		else
			return GetAction(material, m_OtherParticleAction, true);
	}
	else
	if(!strcmp("Precached", groupName))
	{
		if(StringBeginsWith(name, "effects/"))
			return GetAction(material, m_EffectsAction, true);
		else
		if(StringBeginsWith(name, "particle/"))
		{
			if(StringBeginsWith(name, "particle/shells/"))
				return GetAction(material, m_ShellParticleAction, true);
			else
				return GetAction(material, m_OtherParticleAction, true);
		}
		else
		{
			if(m_DebugPrint) Tier0_Msg("(unhandled)");
			return m_PassthroughAction;
		}
	}

	if(m_DebugPrint) Tier0_Msg("(unhandled)");
	return m_PassthroughAction;
}

CAfxBaseFxStream::CAction * CAfxBaseFxStream::GetAction(IMaterial_csgo * material, AfxAction action, bool safeMode)
{
	const char * shaderName = material->GetShaderName();

	int numVars = material->ShaderParamCount();
	IMaterialVar_csgo ** orgParams = material->GetShaderParams();

	int flags = orgParams[FLAGS]->GetIntValue();
	bool isAlphatest = 0 != (flags & MATERIAL_VAR_ALPHATEST);
	bool isTranslucent = 0 != (flags & MATERIAL_VAR_TRANSLUCENT);
	bool isAdditive = 0 != (flags & MATERIAL_VAR_ADDITIVE);

	float alphaTestReference = 0.7f;
	bool alphaTestReferenceDefined = false;
	bool isPhong = false;
	bool isBump = false;

	{
		IMaterialVar_csgo ** params = orgParams;

		//Tier0_Msg("---- Params ----\n");

		for(int i=0; i<numVars; ++i)
		{
			//Tier0_Msg("Param: %s -> %s (%s,isTexture: %s)\n",params[0]->GetName(), params[0]->GetStringValue(), params[0]->IsDefined() ? "defined" : "UNDEFINED", params[0]->IsTexture() ? "Y" : "N");

			if(params[0]->IsDefined())
			{
				char const * varName = params[0]->GetName();

				if(!strcmp(varName,"$bumpmap"))
				{
					if(params[0]->IsTexture())
						isBump = true;
				}
				else
				if(!strcmp(varName,"$phong"))
				{
					if(params[0]->GetIntValue())
						isPhong = true;
				}
				else
				if(!strcmp(params[0]->GetName(),"$alphatestreference") && 0.0 < params[0]->GetFloatValue())
				{
					alphaTestReferenceDefined = true;
					alphaTestReference = params[0]->GetFloatValue();
					break;
				}
			}

			++params;
		}
	}

	if(!strcmp(shaderName, "Spritecard"))
	{
			switch(action)
			{
			case AA_DrawDepth:
				{
					if(m_DebugPrint) Tier0_Msg("drawDepth (Spritecard hook)");

					CActionAfxSpritecardHookKey key(
						ShaderCombo_afxHook_spritecard_ps20b::AFXMODE_0,
						alphaTestReferenceDefined ? alphaTestReference : 0.3f // go easy on them ;)
						);

					return GetSpritecardHookAction(key);
				}
			case AA_DrawDepth24:
				{
					if(m_DebugPrint) Tier0_Msg("drawDepth24 (Spritecardc hook)");

					CActionAfxSpritecardHookKey key(
						ShaderCombo_afxHook_spritecard_ps20b::AFXMODE_1,
						alphaTestReferenceDefined ? alphaTestReference : 0.3f // go easy on them ;)
						);

					return GetSpritecardHookAction(key);
				}
			case AA_GreenScreen:
				{
					if(m_DebugPrint) Tier0_Msg("mask (Spritecard hook)");

					CActionAfxSpritecardHookKey key(
						ShaderCombo_afxHook_spritecard_ps20b::AFXMODE_2,
						alphaTestReferenceDefined ? alphaTestReference : 0.3f // go easy on them ;)
						);

					return GetSpritecardHookAction(key);
				}
			case AA_Black:
				{
					if(m_DebugPrint) Tier0_Msg("black (Spritecard hook)");

					CActionAfxSpritecardHookKey key(
						ShaderCombo_afxHook_spritecard_ps20b::AFXMODE_3,
						alphaTestReferenceDefined ? alphaTestReference : 0.3f // go easy on them ;)
						);

					return GetSpritecardHookAction(key);
				}
			case AA_White:
				{
					if(m_DebugPrint) Tier0_Msg("white (Spritecard hook)");

					CActionAfxSpritecardHookKey key(
						ShaderCombo_afxHook_spritecard_ps20b::AFXMODE_4,
						alphaTestReferenceDefined ? alphaTestReference : 0.3f // go easy on them ;)
						);

					return GetSpritecardHookAction(key);
				}
			}
	}
	else
	if(!isAdditive && (!strcmp(shaderName, "VertexLitGeneric") || !strcmp(shaderName, "UnlitGeneric")))
	{
		if(!isPhong && !isBump)
		{
			switch(action)
			{
			case AA_DrawDepth:
				{
					if(m_DebugPrint) Tier0_Msg("drawDepth (VertexLitGeneric/UnlitGeneric hook)");

					CActionAfxVertexLitGenericHookKey key(
						ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::AFXMODE_0,
						alphaTestReference);

					return GetVertexLitGenericHookAction(key);
				}
			case AA_DrawDepth24:
				{
					if(m_DebugPrint) Tier0_Msg("drawDepth24 (VertexLitGeneric/UnlitGeneric hook)");

					CActionAfxVertexLitGenericHookKey key(
						ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::AFXMODE_1,
						alphaTestReference);

					return GetVertexLitGenericHookAction(key);
				}
			case AA_GreenScreen:
				{
					if(m_DebugPrint) Tier0_Msg("mask (VertexLitGeneric/UnlitGeneric hook)");

					CActionAfxVertexLitGenericHookKey key(
						ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::AFXMODE_2,
						alphaTestReference);

					return GetVertexLitGenericHookAction(key);
				}
			case AA_Black:
				{
					if(m_DebugPrint) Tier0_Msg("black (VertexLitGeneric/UnlitGeneric hook)");

					CActionAfxVertexLitGenericHookKey key(
						ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::AFXMODE_3,
						alphaTestReference);

					return GetVertexLitGenericHookAction(key);
				}
			case AA_White:
				{
					if(m_DebugPrint) Tier0_Msg("white (VertexLitGeneric/UnlitGeneric hook)");

					CActionAfxVertexLitGenericHookKey key(
						ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::AFXMODE_4,
						alphaTestReference);

					return GetVertexLitGenericHookAction(key);
				}
			}
		}
	}

	switch(action)
	{
	case AA_Draw:
		if(m_DebugPrint) Tier0_Msg("draw");
		return m_PassthroughAction;
	case AA_NoDraw:
		if(m_DebugPrint) Tier0_Msg("noDraw");
		return m_NoDrawAction;
	case AA_Invisible:
		if(safeMode)
		{
			if(m_DebugPrint) Tier0_Msg("invisible -> noDraw");
			return m_NoDrawAction;
		}
		else
		{
			if(m_DebugPrint) Tier0_Msg("invisible");
			return m_InvisibleAction;
		}
	case AA_DrawDepth:
		if(safeMode)
		{
			if(m_DebugPrint) Tier0_Msg("drawDepth -> noDraw");
			return m_NoDrawAction;
		}
		else if(isAdditive)
		{
			if(m_DebugPrint) Tier0_Msg("drawDepth -> invisbile");
			return m_InvisibleAction;
		}
		else
		{
			if(m_DebugPrint) Tier0_Msg("drawDepth");
			return m_DepthAction;
		}
	case AA_DrawDepth24:
		if(safeMode)
		{
			if(m_DebugPrint) Tier0_Msg("drawDepth24 -> noDraw");
			return m_NoDrawAction;
		}
		else if(isAdditive)
		{
			if(m_DebugPrint) Tier0_Msg("drawDepth24 -> invisbile");
			return m_InvisibleAction;
		}
		else
		{
			if(m_DebugPrint) Tier0_Msg("drawDepth24");
			return m_Depth24Action;
		}
	case AA_GreenScreen:
		if(safeMode)
		{
			if(m_DebugPrint) Tier0_Msg("mask -> noDraw");
			return m_NoDrawAction;
		}
		else if(isAdditive)
		{
			if(m_DebugPrint) Tier0_Msg("drawDepth -> invisbile");
			return m_InvisibleAction;
		}
		else
		{
			if(m_DebugPrint) Tier0_Msg("mask");
			return m_MatteAction;
		}
	case AA_Black:
		if(safeMode)
		{
			if(m_DebugPrint) Tier0_Msg("black -> noDraw");
			return m_NoDrawAction;
		}
		else if(isAdditive)
		{
			if(m_DebugPrint) Tier0_Msg("drawDepth -> invisbile");
			return m_InvisibleAction;
		}
		else
		{
			if(m_DebugPrint) Tier0_Msg("black");
			return m_BlackAction;
		}
	case AA_White:
		if(safeMode)
		{
			if(m_DebugPrint) Tier0_Msg("white -> noDraw");
			return m_NoDrawAction;
		}
		else if(isAdditive)
		{
			if(m_DebugPrint) Tier0_Msg("drawDepth -> invisbile");
			return m_InvisibleAction;
		}
		else
		{
			if(m_DebugPrint) Tier0_Msg("white");
			return m_WhiteAction;
		}
	case AA_DebugDump:
		if(m_DebugPrint) Tier0_Msg("debugDump");
		return m_DebugDumpAction;
	};

	if(m_DebugPrint) Tier0_Msg("draw");
	return m_PassthroughAction;
}

void CAfxBaseFxStream::DrawInstances(IAfxMatRenderContext * ctx, int nInstanceCount, const MeshInstanceData_t_csgo *pInstance )
{
	m_CurrentAction->DrawInstances(ctx, nInstanceCount, pInstance);
}

void CAfxBaseFxStream::Draw(IAfxMesh * am, int firstIndex, int numIndices)
{
	m_CurrentAction->Draw(am, firstIndex, numIndices);
}

void CAfxBaseFxStream::Draw_2(IAfxMesh * am, CPrimList_csgo *pLists, int nLists)
{
	m_CurrentAction->Draw_2(am, pLists, nLists);
}

void CAfxBaseFxStream::DrawModulated(IAfxMesh * am, const Vector4D_csgo &vecDiffuseModulation, int firstIndex, int numIndices)
{
	m_CurrentAction->DrawModulated(am, vecDiffuseModulation, firstIndex, numIndices);
}

void CAfxBaseFxStream::SetVertexShader(CAfx_csgo_ShaderState & state)
{
	m_CurrentAction->SetVertexShader(state);
}

void CAfxBaseFxStream::SetPixelShader(CAfx_csgo_ShaderState & state)
{
	m_CurrentAction->SetPixelShader(state);
}

CAfxBaseFxStream::AfxAction CAfxBaseFxStream::ClientEffectTexturesAction_get(void)
{
	return m_ClientEffectTexturesAction;
}

void CAfxBaseFxStream::ClientEffectTexturesAction_set(AfxAction value)
{
	InvalidateMap();

	m_ClientEffectTexturesAction = value;
}

CAfxBaseFxStream::AfxAction CAfxBaseFxStream::WorldTexturesAction_get(void)
{
	return m_WorldTexturesAction;
}

void CAfxBaseFxStream::WorldTexturesAction_set(AfxAction value)
{
	InvalidateMap();
	m_WorldTexturesAction = value;
}

CAfxBaseFxStream::AfxAction CAfxBaseFxStream::SkyBoxTexturesAction_get(void)
{
	return m_SkyBoxTexturesAction;
}

void CAfxBaseFxStream::SkyBoxTexturesAction_set(AfxAction value)
{
	InvalidateMap();
	m_SkyBoxTexturesAction = value;
}

CAfxBaseFxStream::AfxAction CAfxBaseFxStream::StaticPropTexturesAction_get(void)
{
	return m_StaticPropTexturesAction;
}

void CAfxBaseFxStream::StaticPropTexturesAction_set(AfxAction value)
{
	InvalidateMap();
	m_StaticPropTexturesAction = value;
}

CAfxBaseFxStream::AfxAction CAfxBaseFxStream::CableAction_get(void)
{
	return m_CableAction;
}

void CAfxBaseFxStream::CableAction_set(AfxAction value)
{
	InvalidateMap();
	m_CableAction = value;
}

CAfxBaseFxStream::AfxAction CAfxBaseFxStream::PlayerModelsAction_get(void)
{
	return m_PlayerModelsAction;
}

void CAfxBaseFxStream::PlayerModelsAction_set(AfxAction value)
{
	InvalidateMap();
	m_PlayerModelsAction = value;
}

CAfxBaseFxStream::AfxAction CAfxBaseFxStream::WeaponModelsAction_get(void)
{
	return m_WeaponModelsAction;
}

void CAfxBaseFxStream::WeaponModelsAction_set(AfxAction value)
{
	InvalidateMap();
	m_WeaponModelsAction = value;
}

CAfxBaseFxStream::AfxAction CAfxBaseFxStream::StattrackAction_get(void)
{
	return m_StattrackAction;
}

void CAfxBaseFxStream::StattrackAction_set(AfxAction value)
{
	InvalidateMap();
	m_StattrackAction = value;
}

CAfxBaseFxStream::AfxAction CAfxBaseFxStream::ShellModelsAction_get(void)
{
	return m_ShellModelsAction;
}

void CAfxBaseFxStream::ShellModelsAction_set(AfxAction value)
{
	InvalidateMap();
	m_ShellModelsAction = value;
}

CAfxBaseFxStream::AfxAction CAfxBaseFxStream::OtherModelsAction_get(void)
{
	return m_OtherModelsAction;
}

void CAfxBaseFxStream::OtherModelsAction_set(AfxAction value)
{
	InvalidateMap();
	m_OtherModelsAction = value;
}

CAfxBaseFxStream::AfxAction CAfxBaseFxStream::DecalTexturesAction_get(void)
{
	return m_DecalTexturesAction;
}


void CAfxBaseFxStream::DecalTexturesAction_set(AfxAction value)
{
	InvalidateMap();
	m_DecalTexturesAction = value;
}

CAfxBaseFxStream::AfxAction CAfxBaseFxStream::EffectsAction_get(void)
{
	return m_EffectsAction;
}

void CAfxBaseFxStream::EffectsAction_set(AfxAction value)
{
	InvalidateMap();
	m_EffectsAction = value;
}

CAfxBaseFxStream::AfxAction CAfxBaseFxStream::ShellParticleAction_get(void)
{
	return m_ShellParticleAction;
}

void CAfxBaseFxStream::ShellParticleAction_set(AfxAction value)
{
	InvalidateMap();
	m_ShellParticleAction = value;
}

CAfxBaseFxStream::AfxAction CAfxBaseFxStream::OtherParticleAction_get(void)
{
	return m_OtherParticleAction;
}

void CAfxBaseFxStream::OtherParticleAction_set(AfxAction value)
{
	InvalidateMap();
	m_OtherParticleAction = value;
}

CAfxBaseFxStream::AfxAction CAfxBaseFxStream::StickerAction_get(void)
{
	return m_StickerAction;
}

void CAfxBaseFxStream::StickerAction_set(AfxAction value)
{
	InvalidateMap();
	m_StickerAction = value;
}

bool CAfxBaseFxStream::TestAction_get(void)
{
	return m_TestAction;
}

void CAfxBaseFxStream::TestAction_set(bool value)
{
	InvalidateMap();
	m_TestAction = value;
}

float CAfxBaseFxStream::DepthVal_get(void)
{
	return m_DepthVal;
}

void CAfxBaseFxStream::DepthVal_set(float value)
{
	m_DepthVal = value;
}

float CAfxBaseFxStream::DepthValMax_get(void)
{
	return m_DepthValMax;
}

void CAfxBaseFxStream::DepthValMax_set(float value)
{
	m_DepthValMax = value;
}

bool CAfxBaseFxStream::DebugPrint_get(void)
{
	return m_DebugPrint;
}

void CAfxBaseFxStream::DebugPrint_set(bool value)
{
	m_DebugPrint = value;
}

void CAfxBaseFxStream::InvalidateMap(void)
{
	if(m_DebugPrint) Tier0_Msg("Stream: Invalidating material cache.\n");
	
	for(std::map<CAfxMaterialKey, CAction *>::iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		it->second->Release();
	}
	m_Map.clear();
}

CAfxBaseFxStream::CAction * CAfxBaseFxStream::GetSpritecardHookAction(CActionAfxSpritecardHookKey & key)
{
	std::map<CActionAfxSpritecardHookKey, CActionAfxSpritecardHook *>::iterator it = m_SpritecardHookActions.find(key);

	if(it != m_SpritecardHookActions.end())
	{
		return it->second;
	}

	CActionAfxSpritecardHook * action = new CActionAfxSpritecardHook(this, key);

	action->AddRef();
	m_SpritecardHookActions[key] = action;

	return action;
}

CAfxBaseFxStream::CAction * CAfxBaseFxStream::GetVertexLitGenericHookAction(CActionAfxVertexLitGenericHookKey & key)
{
	std::map<CActionAfxVertexLitGenericHookKey, CActionAfxVertexLitGenericHook *>::iterator it = m_VertexLitGenericHookActions.find(key);

	if(it != m_VertexLitGenericHookActions.end())
	{
		return it->second;
	}

	CActionAfxVertexLitGenericHook * action = new CActionAfxVertexLitGenericHook(this, key);

	action->AddRef();
	m_VertexLitGenericHookActions[key] = action;

	return action;
}

void CAfxBaseFxStream::InvalidateSpritecardHookActions()
{
	for(std::map<CActionAfxSpritecardHookKey, CActionAfxSpritecardHook *>::iterator it = m_SpritecardHookActions.begin();
		it != m_SpritecardHookActions.end();
		++it)
	{
		it->second->Release();
	}
	m_SpritecardHookActions.clear();
}

void CAfxBaseFxStream::InvalidateVertexLitGenericHookActions()
{
	for(std::map<CActionAfxVertexLitGenericHookKey, CActionAfxVertexLitGenericHook *>::iterator it = m_VertexLitGenericHookActions.begin();
		it != m_VertexLitGenericHookActions.end();
		++it)
	{
		it->second->Release();
	}
	m_VertexLitGenericHookActions.clear();
}

// CAfxBaseFxStream::CActionInvisible //////////////////////////////////////////

void CAfxBaseFxStream::CActionInvisible::AfxUnbind(IAfxMatRenderContext * ctx)
{
	//AfxD3D9OverrideEnd_D3DRS_ZWRITEENABLE();

	m_ParentStream->m_Streams->EndOverrideSetColorModulation();
	m_ParentStream->m_Streams->EndOverrideSetBlend();
}

IMaterial_csgo * CAfxBaseFxStream::CActionInvisible::MaterialHook(IAfxMatRenderContext * ctx, IMaterial_csgo * material)
{
	float color[3] = { 0.0f, 0.0f, 0.0f };
	m_ParentStream->m_Streams->OverrideSetBlend(0.0f);
	m_ParentStream->m_Streams->OverrideSetColorModulation(color);

	//AfxD3D9OverrideBegin_D3DRS_ZWRITEENABLE(FALSE);

	return m_InvisibleMaterial.GetMaterial();
}

// CAfxBaseFxStream::CActionAfxVertexLitGenericHook ////////////////////////////

csgo_Stdshader_dx9_Combos_vertexlit_and_unlit_generic_ps20 CAfxBaseFxStream::CActionAfxVertexLitGenericHook::m_Combos_ps20;
csgo_Stdshader_dx9_Combos_vertexlit_and_unlit_generic_ps20b CAfxBaseFxStream::CActionAfxVertexLitGenericHook::m_Combos_ps20b;
csgo_Stdshader_dx9_Combos_vertexlit_and_unlit_generic_ps30 CAfxBaseFxStream::CActionAfxVertexLitGenericHook::m_Combos_ps30;

CAfxBaseFxStream::CActionAfxVertexLitGenericHook::CActionAfxVertexLitGenericHook(CAfxBaseFxStream * parentStream, CActionAfxVertexLitGenericHookKey & key)
: CAction(parentStream)
, m_Key(key)
{
}

void CAfxBaseFxStream::CActionAfxVertexLitGenericHook::AfxUnbind(IAfxMatRenderContext * ctx)
{
	AfxD3D9_OverrideEnd_ps_c5();

	AfxD3D9OverrideEnd_D3DRS_SRGBWRITEENABLE();

	AfxD3D9_OverrideEnd_SetPixelShader();
}

IMaterial_csgo * CAfxBaseFxStream::CActionAfxVertexLitGenericHook::MaterialHook(IAfxMatRenderContext * ctx, IMaterial_csgo * material)
{
	// depth factors:

	float scale = g_bIn_csgo_CSkyBoxView_Draw ? csgo_CSkyBoxView_GetScale() : 1.0f;
	float flDepthFactor = scale * m_ParentStream->m_DepthVal;
	float flDepthFactorMax = scale * m_ParentStream->m_DepthValMax;

	//
	// Force SRGBWriteEnable to off:

	AfxD3D9OverrideBegin_D3DRS_SRGBWRITEENABLE(FALSE);

	// Fill in g_AfxConstants in shader:

	float mulFac = flDepthFactorMax -flDepthFactor;
	mulFac = !mulFac ? 0.0f : 1.0f / mulFac;

	float overFac[4] = { flDepthFactor, mulFac, m_Key.AlphaTestReference, 0.0f };

	AfxD3D9_OverrideBegin_ps_c5(overFac);

	// Bind normal material:
	return material;
}

void CAfxBaseFxStream::CActionAfxVertexLitGenericHook::SetPixelShader(CAfx_csgo_ShaderState & state)
{
	char const * shaderName = state.Static.SetPixelShader.pFileName.c_str();

	if(!strcmp(shaderName,"vertexlit_and_unlit_generic_ps20"))
	{
		static bool firstPass = true;
		if(firstPass)
		{
			firstPass = false;
			Tier0_Warning("AFXWARNING: You are using an untested code path in CAfxBaseFxStream::CActionAfxVertexLitGenericHook::SetPixelShader for %s.\n", shaderName);
		}

		m_Combos_ps20.CalcCombos(state.Static.SetPixelShader.nStaticPshIndex, state.Dynamic.SetPixelShaderIndex.pshIndex);

		if(0 < m_Combos_ps20.m_LIGHTNING_PREVIEW)
		{
			Tier0_Warning("AFXERROR: CAfxBaseFxStream::CActionAfxVertexLitGenericHook::SetPixelShader: 0 < m_LIGHTNING_PREVIEW not supported for %s.\n", shaderName);
			return;
		}

		int combo = ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps20::GetCombo(
			(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps20::AFXMODE_e)m_Key.AFXMODE,
			(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps20::DETAILTEXTURE_e)m_Combos_ps20.m_DETAILTEXTURE,
			!m_Combos_ps20.m_BASEALPHAENVMAPMASK && !m_Combos_ps20.m_SELFILLUM ? ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps20::NOT_BASEALPHAENVMAPMASK_AND_NOT_SELFILLUM_1 : ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps20::NOT_BASEALPHAENVMAPMASK_AND_NOT_SELFILLUM_0,
			(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps20::FLASHLIGHT_e)m_Combos_ps20.m_FLASHLIGHT,
			(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps20::DETAIL_BLEND_MODE_e)m_Combos_ps20.m_DETAIL_BLEND_MODE,
			(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps20::DESATURATEWITHBASEALPHA_e)m_Combos_ps20.m_DESATURATEWITHBASEALPHA,
			ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps20::NOT_LIGHTING_PREVIEW_ONLY_0
		);

		IAfxPixelShader * afxPixelShader = g_AfxShaders.GetAcsPixelShader("afxHook_vertexlit_and_unlit_generic_ps20.acs", combo);

		if(!afxPixelShader->GetPixelShader())
			Tier0_Warning("AFXERROR: CAfxBaseFxStream::CActionAfxVertexLitGenericHook::SetPixelShader: Replacement Shader combo %i for %s is null.\n", combo, shaderName);
		else
		{
			// Override shader:
			AfxD3D9_OverrideBegin_SetPixelShader(afxPixelShader->GetPixelShader());
		}

		afxPixelShader->Release();
	}
	else
	if(!strcmp(shaderName,"vertexlit_and_unlit_generic_ps20b"))
	{
		static bool firstPass = true;
		if(firstPass)
		{
			firstPass = false;
			Tier0_Warning("AFXWARNING: You are using an untested code path in CAfxBaseFxStream::CActionAfxVertexLitGenericHook::SetPixelShader for %s.\n", shaderName);
		}

		m_Combos_ps20b.CalcCombos(state.Static.SetPixelShader.nStaticPshIndex, state.Dynamic.SetPixelShaderIndex.pshIndex);

		if(0 < m_Combos_ps20b.m_LIGHTNING_PREVIEW)
		{
			Tier0_Warning("AFXERROR: CAfxBaseFxStream::CActionAfxVertexLitGenericHook::SetPixelShader: 0 < m_LIGHTNING_PREVIEW not supported for %s.\n", shaderName);
			return;
		}

		int combo = ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps20b::GetCombo(
			(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps20b::AFXMODE_e)m_Key.AFXMODE,
			(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps20b::DETAILTEXTURE_e)m_Combos_ps20b.m_DETAILTEXTURE,
			!m_Combos_ps20b.m_BASEALPHAENVMAPMASK && !m_Combos_ps20b.m_SELFILLUM ? ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps20b::NOT_BASEALPHAENVMAPMASK_AND_NOT_SELFILLUM_1 : ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps20b::NOT_BASEALPHAENVMAPMASK_AND_NOT_SELFILLUM_0,
			(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps20b::FLASHLIGHT_e)m_Combos_ps20b.m_FLASHLIGHT,
			(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps20b::DETAIL_BLEND_MODE_e)m_Combos_ps20b.m_DETAIL_BLEND_MODE,
			(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps20b::DESATURATEWITHBASEALPHA_e)m_Combos_ps20b.m_DESATURATEWITHBASEALPHA,
			ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps20b::NOT_LIGHTING_PREVIEW_ONLY_0
		);

		IAfxPixelShader * afxPixelShader = g_AfxShaders.GetAcsPixelShader("afxHook_vertexlit_and_unlit_generic_ps20b.acs", combo);

		if(!afxPixelShader->GetPixelShader())
			Tier0_Warning("AFXERROR: CAfxBaseFxStream::CActionAfxVertexLitGenericHook::SetPixelShader: Replacement Shader combo %i for %s is null.\n", combo, shaderName);
		else
		{
			// Override shader:
			AfxD3D9_OverrideBegin_SetPixelShader(afxPixelShader->GetPixelShader());
		}

		afxPixelShader->Release();
	}
	else
	if(!strcmp(shaderName,"vertexlit_and_unlit_generic_ps30"))
	{
		m_Combos_ps30.CalcCombos(state.Static.SetPixelShader.nStaticPshIndex, state.Dynamic.SetPixelShaderIndex.pshIndex);

		if(0 < m_Combos_ps30.m_LIGHTNING_PREVIEW)
		{
			Tier0_Warning("AFXERROR: CAfxBaseFxStream::CActionAfxVertexLitGenericHook::SetPixelShader: 0 < m_LIGHTNING_PREVIEW not supported for %s.\n", shaderName);
			return;
		}

		int combo = ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::GetCombo(
			m_Key.AFXMODE,
			(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::DETAILTEXTURE_e)m_Combos_ps30.m_DETAILTEXTURE,
			!m_Combos_ps30.m_BASEALPHAENVMAPMASK && !m_Combos_ps30.m_SELFILLUM ? ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::NOT_BASEALPHAENVMAPMASK_AND_NOT_SELFILLUM_1 : ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::NOT_BASEALPHAENVMAPMASK_AND_NOT_SELFILLUM_0,
			(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::FLASHLIGHT_e)m_Combos_ps30.m_FLASHLIGHT,
			(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::DETAIL_BLEND_MODE_e)m_Combos_ps30.m_DETAIL_BLEND_MODE,
			(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::DESATURATEWITHBASEALPHA_e)m_Combos_ps30.m_DESATURATEWITHBASEALPHA,
			ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::NOT_LIGHTING_PREVIEW_ONLY_0
		);

		/*
		Tier0_Msg("%s %i %i - %i - %i %i %i %i %i %i -> %i\n",
			shaderName,
			state.Static.SetPixelShader.nStaticPshIndex, state.Dynamic.SetPixelShaderIndex.pshIndex,
			m_Key.AFXMODE,
			(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::DETAILTEXTURE_e)m_Combos_ps30.m_DETAILTEXTURE,
			!m_Combos_ps30.m_BASEALPHAENVMAPMASK && !m_Combos_ps30.m_SELFILLUM ? ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::NOT_BASEALPHAENVMAPMASK_AND_NOT_SELFILLUM_1 : ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::NOT_BASEALPHAENVMAPMASK_AND_NOT_SELFILLUM_0,
			(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::FLASHLIGHT_e)m_Combos_ps30.m_FLASHLIGHT,
			(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::DETAIL_BLEND_MODE_e)m_Combos_ps30.m_DETAIL_BLEND_MODE,
			(ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::DESATURATEWITHBASEALPHA_e)m_Combos_ps30.m_DESATURATEWITHBASEALPHA,
			ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::NOT_LIGHTING_PREVIEW_ONLY_0,
			combo
			);
		*/

		IAfxPixelShader * afxPixelShader = g_AfxShaders.GetAcsPixelShader("afxHook_vertexlit_and_unlit_generic_ps30.acs", combo);

		if(!afxPixelShader->GetPixelShader())
			Tier0_Warning("AFXERROR: CAfxBaseFxStream::CActionAfxVertexLitGenericHook::SetPixelShader: Replacement Shader combo %i for %s is null.\n", combo, shaderName);
		else
		{
			// Override shader:
			AfxD3D9_OverrideBegin_SetPixelShader(afxPixelShader->GetPixelShader());
		}

		afxPixelShader->Release();
	}
	else
		Tier0_Warning("AFXERROR: CAfxBaseFxStream::CActionAfxVertexLitGenericHook::SetPixelShader: No replacement defined for %s.\n", shaderName);

}

// CAfxBaseFxStream::CActionAfxSpritecardHook ////////////////////////////

csgo_Stdshader_dx9_Combos_splinecard_vs20 CAfxBaseFxStream::CActionAfxSpritecardHook::m_Combos_splinecard_vs20;
csgo_Stdshader_dx9_Combos_spritecard_vs20 CAfxBaseFxStream::CActionAfxSpritecardHook::m_Combos_spritecard_vs20;
csgo_Stdshader_dx9_Combos_spritecard_ps20 CAfxBaseFxStream::CActionAfxSpritecardHook::m_Combos_ps20;
csgo_Stdshader_dx9_Combos_spritecard_ps20b CAfxBaseFxStream::CActionAfxSpritecardHook::m_Combos_ps20b;

CAfxBaseFxStream::CActionAfxSpritecardHook::CActionAfxSpritecardHook(CAfxBaseFxStream * parentStream, CActionAfxSpritecardHookKey & key)
: CAction(parentStream)
, m_Key(key)
{
}

void CAfxBaseFxStream::CActionAfxSpritecardHook::AfxUnbind(IAfxMatRenderContext * ctx)
{
	AfxD3D9_OverrideEnd_ps_c31();

	AfxD3D9OverrideEnd_D3DRS_SRGBWRITEENABLE();
	AfxD3D9OverrideEnd_D3DRS_DESTBLEND();
	AfxD3D9OverrideEnd_D3DRS_SRCBLEND();

	AfxD3D9_OverrideEnd_SetPixelShader();
	AfxD3D9_OverrideEnd_SetVertexShader();
}

IMaterial_csgo * CAfxBaseFxStream::CActionAfxSpritecardHook::MaterialHook(IAfxMatRenderContext * ctx, IMaterial_csgo * material)
{
	// depth factors:

	float scale = g_bIn_csgo_CSkyBoxView_Draw ? csgo_CSkyBoxView_GetScale() : 1.0f;
	float flDepthFactor = scale * m_ParentStream->m_DepthVal;
	float flDepthFactorMax = scale * m_ParentStream->m_DepthValMax;

	// Force wanted state:

	AfxD3D9OverrideBegin_D3DRS_SRCBLEND(D3DBLEND_SRCALPHA);
	AfxD3D9OverrideBegin_D3DRS_DESTBLEND(D3DBLEND_INVSRCALPHA);
	AfxD3D9OverrideBegin_D3DRS_SRGBWRITEENABLE(FALSE);

	// Fill in g_AfxConstants in shader:

	float mulFac = flDepthFactorMax -flDepthFactor;
	mulFac = !mulFac ? 0.0f : 1.0f / mulFac;

	float overFac[4] = { flDepthFactor, mulFac, m_Key.AlphaTestReference, 0.0f };

	AfxD3D9_OverrideBegin_ps_c31(overFac);

	// Bind normal material:
	return material;
}

void CAfxBaseFxStream::CActionAfxSpritecardHook::SetVertexShader(CAfx_csgo_ShaderState & state)
{
	char const * shaderName = state.Static.SetVertexShader.pFileName.c_str();

	if(!strcmp(shaderName,"splinecard_vs20"))
	{
		int remainder = m_Combos_splinecard_vs20.CalcCombos(state.Static.SetVertexShader.nStaticVshIndex, state.Dynamic.SetVertexShaderIndex.vshIndex);

		int combo = ShaderCombo_afxHook_splinecard_vs20::GetCombo(
			(ShaderCombo_afxHook_splinecard_vs20::ORIENTATION_e)m_Combos_splinecard_vs20.m_Orientation,
			(ShaderCombo_afxHook_splinecard_vs20::ADDBASETEXTURE2_e)m_Combos_splinecard_vs20.m_ADDBASETEXTURE2,
			(ShaderCombo_afxHook_splinecard_vs20::EXTRACTGREENALPHA_e)m_Combos_splinecard_vs20.m_EXTRACTGREENALPHA,
			(ShaderCombo_afxHook_splinecard_vs20::DUALSEQUENCE_e)m_Combos_splinecard_vs20.m_DUALSEQUENCE,
			(ShaderCombo_afxHook_splinecard_vs20::DEPTHBLEND_e)m_Combos_splinecard_vs20.m_DEPTHBLEND,
			(ShaderCombo_afxHook_splinecard_vs20::PACKED_INTERPOLATOR_e)m_Combos_splinecard_vs20.m_PACKED_INTERPOLATOR,
			(ShaderCombo_afxHook_splinecard_vs20::ANIMBLEND_OR_MAXLUMFRAMEBLEND1_e)m_Combos_splinecard_vs20.m_ANIMBLEND_OR_MAXLUMFRAMEBLEND1		
		);

		IAfxVertexShader * afxVertexShader = g_AfxShaders.GetAcsVertexShader("afxHook_splinecard_vs20.acs", combo);

		if(!afxVertexShader->GetVertexShader())
			Tier0_Warning("AFXERROR: CAfxBaseFxStream::CActionAfxSpritecardHook::SetVertexShader: Replacement Shader combo %i for %s is null.\n", combo, shaderName);
		else
		{
			// Override shader:
			AfxD3D9_OverrideBegin_SetVertexShader(afxVertexShader->GetVertexShader());
		}

		afxVertexShader->Release();

		return;
	}
	else
	if(!strcmp(shaderName,"spritecard_vs20"))
	{
		int remainder = m_Combos_spritecard_vs20.CalcCombos(state.Static.SetVertexShader.nStaticVshIndex, state.Dynamic.SetVertexShaderIndex.vshIndex);

		if(3 < m_Combos_spritecard_vs20.m_Orientation)
		{
			Tier0_Warning("AFXERROR: CAfxBaseFxStream::CActionAfxSpritecardHook::SetVertexShader: Orientation %i for %s is not supported.\n", m_Combos_spritecard_vs20.m_Orientation, shaderName);
			return;
		}

		int combo = ShaderCombo_afxHook_spritecard_vs20::GetCombo(
			(ShaderCombo_afxHook_spritecard_vs20::ORIENTATION_e)m_Combos_spritecard_vs20.m_Orientation,
			(ShaderCombo_afxHook_spritecard_vs20::ZOOM_ANIMATE_SEQ2_e)m_Combos_spritecard_vs20.m_ZOOM_ANIMATE_SEQ2,
			(ShaderCombo_afxHook_spritecard_vs20::DUALSEQUENCE_e)m_Combos_spritecard_vs20.m_DUALSEQUENCE,
			(ShaderCombo_afxHook_spritecard_vs20::ADDBASETEXTURE2_e)m_Combos_spritecard_vs20.m_ADDBASETEXTURE2,
			(ShaderCombo_afxHook_spritecard_vs20::EXTRACTGREENALPHA_e)m_Combos_spritecard_vs20.m_EXTRACTGREENALPHA,
			(ShaderCombo_afxHook_spritecard_vs20::DEPTHBLEND_e)m_Combos_spritecard_vs20.m_DEPTHBLEND,
			(ShaderCombo_afxHook_spritecard_vs20::ANIMBLEND_OR_MAXLUMFRAMEBLEND1_e)m_Combos_spritecard_vs20.m_ANIMBLEND_OR_MAXLUMFRAMEBLEND1,
			(ShaderCombo_afxHook_spritecard_vs20::CROP_e)m_Combos_spritecard_vs20.m_CROP,
			(ShaderCombo_afxHook_spritecard_vs20::PACKED_INTERPOLATOR_e)m_Combos_spritecard_vs20.m_PACKED_INTERPOLATOR,
			(ShaderCombo_afxHook_spritecard_vs20::SPRITECARDVERTEXFOG_e)m_Combos_spritecard_vs20.m_SPRITECARDVERTEXFOG,
			(ShaderCombo_afxHook_spritecard_vs20::HARDWAREFOGBLEND_e)m_Combos_spritecard_vs20.m_HARDWAREFOGBLEND,
			(ShaderCombo_afxHook_spritecard_vs20::PERPARTICLEOUTLINE_e)m_Combos_spritecard_vs20.m_PERPARTICLEOUTLINE
		);

		IAfxVertexShader * afxVertexShader = g_AfxShaders.GetAcsVertexShader("afxHook_spritecard_vs20.acs", combo);

		if(!afxVertexShader->GetVertexShader())
			Tier0_Warning("AFXERROR: CAfxBaseFxStream::CActionAfxSpritecardHook::SetVertexShader: Replacement Shader combo %i for %s is null.\n", combo, shaderName);
		else
		{
			// Override shader:
			AfxD3D9_OverrideBegin_SetVertexShader(afxVertexShader->GetVertexShader());
		}

		afxVertexShader->Release();

		return;
	}
	else
		Tier0_Warning("AFXERROR: CAfxBaseFxStream::CActionAfxSpritecardHook::SetVertexShader: No replacement defined for %s.\n", shaderName);
}

void CAfxBaseFxStream::CActionAfxSpritecardHook::SetPixelShader(CAfx_csgo_ShaderState & state)
{
	char const * shaderName = state.Static.SetPixelShader.pFileName.c_str();

	if(!strcmp(shaderName,"spritecard_ps20"))
	{
		static bool firstPass = true;
		if(firstPass)
		{
			firstPass = false;
			Tier0_Warning("AFXWARNING: You are using an untested code path in CAfxBaseFxStream::CActionAfxSpritecardHook::SetPixelShader for %s.\n", shaderName);
		}

		ShaderCombo_afxHook_spritecard_ps20::AFXORGBLENDMODE_e afxOrgBlendMode;

		if(state.Static.EnableBlending.bEnable)
		{
			if(SHADER_BLEND_DST_COLOR == state.Static.BlendFunc.srcFactor
				&& SHADER_BLEND_SRC_COLOR == state.Static.BlendFunc.dstFactor)
			{
				afxOrgBlendMode = ShaderCombo_afxHook_spritecard_ps20::AFXORGBLENDMODE_0;
			}
			else
			if(SHADER_BLEND_ONE == state.Static.BlendFunc.srcFactor
				&& SHADER_BLEND_ONE_MINUS_SRC_ALPHA == state.Static.BlendFunc.dstFactor)
			{
				afxOrgBlendMode = ShaderCombo_afxHook_spritecard_ps20::AFXORGBLENDMODE_1;
			}
			else
			if(SHADER_BLEND_SRC_ALPHA == state.Static.BlendFunc.srcFactor
				&& SHADER_BLEND_ONE == state.Static.BlendFunc.dstFactor)
			{
				afxOrgBlendMode = ShaderCombo_afxHook_spritecard_ps20::AFXORGBLENDMODE_2;
			}
			else
			if(SHADER_BLEND_SRC_ALPHA == state.Static.BlendFunc.srcFactor
				&& SHADER_BLEND_ONE_MINUS_SRC_ALPHA == state.Static.BlendFunc.dstFactor)
			{
				afxOrgBlendMode = ShaderCombo_afxHook_spritecard_ps20::AFXORGBLENDMODE_3;
			}
			else
			{
				Tier0_Warning("AFXERROR: CAfxBaseFxStream::CActionAfxSpritecardHook::SetPixelShader: current blend mode not supported for %s.\n", shaderName);
				return;
			}
		}
		else
		{
			Tier0_Warning("AFXERROR: CAfxBaseFxStream::CActionAfxSpritecardHook::SetPixelShader: non-blending mode not supported for %s.\n", shaderName);
			return;
		}

		int remainder = m_Combos_ps20.CalcCombos(state.Static.SetPixelShader.nStaticPshIndex, state.Dynamic.SetPixelShaderIndex.pshIndex);

		int combo = ShaderCombo_afxHook_spritecard_ps20::GetCombo(
			(ShaderCombo_afxHook_spritecard_ps20::AFXMODE_e)m_Key.AFXMODE,
			afxOrgBlendMode,
			(ShaderCombo_afxHook_spritecard_ps20::DUALSEQUENCE_e)m_Combos_ps20.m_DUALSEQUENCE,
			(ShaderCombo_afxHook_spritecard_ps20::SEQUENCE_BLEND_MODE_e)m_Combos_ps20.m_SEQUENCE_BLEND_MODE,
			(ShaderCombo_afxHook_spritecard_ps20::ADDBASETEXTURE2_e)m_Combos_ps20.m_ADDBASETEXTURE2,
			(ShaderCombo_afxHook_spritecard_ps20::MAXLUMFRAMEBLEND1_e)m_Combos_ps20.m_MAXLUMFRAMEBLEND1,
			(ShaderCombo_afxHook_spritecard_ps20::MAXLUMFRAMEBLEND2_e)m_Combos_ps20.m_MAXLUMFRAMEBLEND2,
			(ShaderCombo_afxHook_spritecard_ps20::EXTRACTGREENALPHA_e)m_Combos_ps20.m_EXTRACTGREENALPHA,
			(ShaderCombo_afxHook_spritecard_ps20::COLORRAMP_e)m_Combos_ps20.m_COLORRAMP,
			(ShaderCombo_afxHook_spritecard_ps20::ANIMBLEND_e)m_Combos_ps20.m_ANIMBLEND,
			(ShaderCombo_afxHook_spritecard_ps20::ADDSELF_e)m_Combos_ps20.m_ADDSELF,
			(ShaderCombo_afxHook_spritecard_ps20::MOD2X_e)m_Combos_ps20.m_MOD2X,
			(ShaderCombo_afxHook_spritecard_ps20::COLOR_LERP_PS_e)m_Combos_ps20.m_COLOR_LERP_PS,
			(ShaderCombo_afxHook_spritecard_ps20::PACKED_INTERPOLATOR_e)m_Combos_ps20.m_PACKED_INTERPOLATOR,
			(ShaderCombo_afxHook_spritecard_ps20::DISTANCEALPHA_e)m_Combos_ps20.m_DISTANCEALPHA,
			(ShaderCombo_afxHook_spritecard_ps20::SOFTEDGES_e)m_Combos_ps20.m_SOFTEDGES,
			(ShaderCombo_afxHook_spritecard_ps20::OUTLINE_e)m_Combos_ps20.m_OUTLINE,
			(ShaderCombo_afxHook_spritecard_ps20::MULOUTPUTBYALPHA_e)m_Combos_ps20.m_MULOUTPUTBYALPHA
		);

		IAfxPixelShader * afxPixelShader = g_AfxShaders.GetAcsPixelShader("afxHook_spritecard_ps20.acs", combo);

		if(!afxPixelShader->GetPixelShader())
			Tier0_Warning("AFXERROR: CAfxBaseFxStream::CActionAfxSpritecardHook::SetPixelShader: Replacement Shader combo %i for %s is null.\n", combo, shaderName);
		else
		{
			// Override shader:
			AfxD3D9_OverrideBegin_SetPixelShader(afxPixelShader->GetPixelShader());
		}

		afxPixelShader->Release();

		return;
	}
	else
	if(!strcmp(shaderName,"spritecard_ps20b"))
	{
		ShaderCombo_afxHook_spritecard_ps20b::AFXORGBLENDMODE_e afxOrgBlendMode;

		if(state.Static.EnableBlending.bEnable)
		{
			if(SHADER_BLEND_DST_COLOR == state.Static.BlendFunc.srcFactor
				&& SHADER_BLEND_SRC_COLOR == state.Static.BlendFunc.dstFactor)
			{
				afxOrgBlendMode = ShaderCombo_afxHook_spritecard_ps20b::AFXORGBLENDMODE_0;
			}
			else
			if(SHADER_BLEND_ONE == state.Static.BlendFunc.srcFactor
				&& SHADER_BLEND_ONE_MINUS_SRC_ALPHA == state.Static.BlendFunc.dstFactor)
			{
				afxOrgBlendMode = ShaderCombo_afxHook_spritecard_ps20b::AFXORGBLENDMODE_1;
			}
			else
			if(SHADER_BLEND_SRC_ALPHA == state.Static.BlendFunc.srcFactor
				&& SHADER_BLEND_ONE == state.Static.BlendFunc.dstFactor)
			{
				afxOrgBlendMode = ShaderCombo_afxHook_spritecard_ps20b::AFXORGBLENDMODE_2;
			}
			else
			if(SHADER_BLEND_SRC_ALPHA == state.Static.BlendFunc.srcFactor
				&& SHADER_BLEND_ONE_MINUS_SRC_ALPHA == state.Static.BlendFunc.dstFactor)
			{
				afxOrgBlendMode = ShaderCombo_afxHook_spritecard_ps20b::AFXORGBLENDMODE_3;
			}
			else
			{
				Tier0_Warning("AFXERROR: CAfxBaseFxStream::CActionAfxSpritecardHook::SetPixelShader: current blend mode not supported for %s.\n", shaderName);
				return;
			}
		}
		else
		{
			Tier0_Warning("AFXERROR: CAfxBaseFxStream::CActionAfxSpritecardHook::SetPixelShader: non-blending mode not supported for %s.\n", shaderName);
			return;
		}

		int remainder = m_Combos_ps20b.CalcCombos(state.Static.SetPixelShader.nStaticPshIndex, state.Dynamic.SetPixelShaderIndex.pshIndex);

		int combo = ShaderCombo_afxHook_spritecard_ps20b::GetCombo(
			(ShaderCombo_afxHook_spritecard_ps20b::AFXMODE_e)m_Key.AFXMODE,
			afxOrgBlendMode,
			(ShaderCombo_afxHook_spritecard_ps20b::DUALSEQUENCE_e)m_Combos_ps20b.m_DUALSEQUENCE,
			(ShaderCombo_afxHook_spritecard_ps20b::SEQUENCE_BLEND_MODE_e)m_Combos_ps20b.m_SEQUENCE_BLEND_MODE,
			(ShaderCombo_afxHook_spritecard_ps20b::ADDBASETEXTURE2_e)m_Combos_ps20b.m_ADDBASETEXTURE2,
			(ShaderCombo_afxHook_spritecard_ps20b::MAXLUMFRAMEBLEND1_e)m_Combos_ps20b.m_MAXLUMFRAMEBLEND1,
			(ShaderCombo_afxHook_spritecard_ps20b::MAXLUMFRAMEBLEND2_e)m_Combos_ps20b.m_MAXLUMFRAMEBLEND2,
			(ShaderCombo_afxHook_spritecard_ps20b::EXTRACTGREENALPHA_e)m_Combos_ps20b.m_EXTRACTGREENALPHA,
			(ShaderCombo_afxHook_spritecard_ps20b::COLORRAMP_e)m_Combos_ps20b.m_COLORRAMP,
			(ShaderCombo_afxHook_spritecard_ps20b::ANIMBLEND_e)m_Combos_ps20b.m_ANIMBLEND,
			(ShaderCombo_afxHook_spritecard_ps20b::ADDSELF_e)m_Combos_ps20b.m_ADDSELF,
			(ShaderCombo_afxHook_spritecard_ps20b::MOD2X_e)m_Combos_ps20b.m_MOD2X,
			(ShaderCombo_afxHook_spritecard_ps20b::COLOR_LERP_PS_e)m_Combos_ps20b.m_COLOR_LERP_PS,
			(ShaderCombo_afxHook_spritecard_ps20b::PACKED_INTERPOLATOR_e)m_Combos_ps20b.m_PACKED_INTERPOLATOR,
			(ShaderCombo_afxHook_spritecard_ps20b::DISTANCEALPHA_e)m_Combos_ps20b.m_DISTANCEALPHA,
			(ShaderCombo_afxHook_spritecard_ps20b::SOFTEDGES_e)m_Combos_ps20b.m_SOFTEDGES,
			(ShaderCombo_afxHook_spritecard_ps20b::OUTLINE_e)m_Combos_ps20b.m_OUTLINE,
			(ShaderCombo_afxHook_spritecard_ps20b::MULOUTPUTBYALPHA_e)m_Combos_ps20b.m_MULOUTPUTBYALPHA
		);

		/*
		Tier0_Msg(
			"%i -> %i %i | %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i (%i)\n",
			combo,
			(ShaderCombo_afxHook_spritecard_ps20b::AFXMODE_e)m_Key.AFXMODE,
			afxOrgBlendMode,
			(ShaderCombo_afxHook_spritecard_ps20b::DUALSEQUENCE_e)m_Combos_ps20b.m_DUALSEQUENCE,
			(ShaderCombo_afxHook_spritecard_ps20b::SEQUENCE_BLEND_MODE_e)m_Combos_ps20b.m_SEQUENCE_BLEND_MODE,
			(ShaderCombo_afxHook_spritecard_ps20b::ADDBASETEXTURE2_e)m_Combos_ps20b.m_ADDBASETEXTURE2,
			(ShaderCombo_afxHook_spritecard_ps20b::MAXLUMFRAMEBLEND1_e)m_Combos_ps20b.m_MAXLUMFRAMEBLEND1,
			(ShaderCombo_afxHook_spritecard_ps20b::MAXLUMFRAMEBLEND2_e)m_Combos_ps20b.m_MAXLUMFRAMEBLEND2,
			(ShaderCombo_afxHook_spritecard_ps20b::EXTRACTGREENALPHA_e)m_Combos_ps20b.m_EXTRACTGREENALPHA,
			(ShaderCombo_afxHook_spritecard_ps20b::COLORRAMP_e)m_Combos_ps20b.m_COLORRAMP,
			(ShaderCombo_afxHook_spritecard_ps20b::ANIMBLEND_e)m_Combos_ps20b.m_ANIMBLEND,
			(ShaderCombo_afxHook_spritecard_ps20b::ADDSELF_e)m_Combos_ps20b.m_ADDSELF,
			(ShaderCombo_afxHook_spritecard_ps20b::MOD2X_e)m_Combos_ps20b.m_MOD2X,
			(ShaderCombo_afxHook_spritecard_ps20b::COLOR_LERP_PS_e)m_Combos_ps20b.m_COLOR_LERP_PS,
			(ShaderCombo_afxHook_spritecard_ps20b::PACKED_INTERPOLATOR_e)m_Combos_ps20b.m_PACKED_INTERPOLATOR,
			(ShaderCombo_afxHook_spritecard_ps20b::DISTANCEALPHA_e)m_Combos_ps20b.m_DISTANCEALPHA,
			(ShaderCombo_afxHook_spritecard_ps20b::SOFTEDGES_e)m_Combos_ps20b.m_SOFTEDGES,
			(ShaderCombo_afxHook_spritecard_ps20b::OUTLINE_e)m_Combos_ps20b.m_OUTLINE,
			(ShaderCombo_afxHook_spritecard_ps20b::MULOUTPUTBYALPHA_e)m_Combos_ps20b.m_MULOUTPUTBYALPHA,
			remainder
			);
		*/

		IAfxPixelShader * afxPixelShader = g_AfxShaders.GetAcsPixelShader("afxHook_spritecard_ps20b.acs", combo);

		if(!afxPixelShader->GetPixelShader())
			Tier0_Warning("AFXERROR: CAfxBaseFxStream::CActionAfxSpritecardHook::SetPixelShader: Replacement Shader combo %i for %s is null.\n", combo, shaderName);
		else
		{
			// Override shader:
			AfxD3D9_OverrideBegin_SetPixelShader(afxPixelShader->GetPixelShader());
		}

		afxPixelShader->Release();
	}
	else
		Tier0_Warning("AFXERROR: CAfxBaseFxStream::CActionAfxSpritecardHook::SetPixelShader: No replacement defined for %s.\n", shaderName);

}

// CAfxBaseFxStream::CActionUnlitGenericFallback ///////////////////////////////

CAfxBaseFxStream::CActionUnlitGenericFallback::CActionUnlitGenericFallback(CAfxBaseFxStream * parentStream, CActionAfxVertexLitGenericHookKey & key, IAfxFreeMaster * freeMaster, IMaterialSystem_csgo * matSystem, char const * unlitGenericFallbackMaterialName)
: CActionAfxVertexLitGenericHook(parentStream, key)
, m_Material(freeMaster, matSystem->FindMaterial(unlitGenericFallbackMaterialName,NULL))
{
}

IMaterial_csgo * CAfxBaseFxStream::CActionUnlitGenericFallback::MaterialHook(IAfxMatRenderContext * ctx, IMaterial_csgo * material)
{
	return CActionAfxVertexLitGenericHook::MaterialHook(ctx, m_Material.GetMaterial());
}

// CAfxStreams /////////////////////////////////////////////////////////////////

CAfxStreams::CAfxStreams()
: m_RecordName("untitled_rec")
, m_OnAfxBaseClientDll_Free(0)
, m_MaterialSystem(0)
, m_VRenderView(0)
, m_AfxBaseClientDll(0)
, m_ShaderShadow(0)
, m_CurrentContext(0)
, m_PreviewStream(0)
, m_Recording(false)
, m_Frame(0)
, m_OnDraw(0)
, m_OnDraw_2(0)
, m_OnDrawModulated(0)
, m_OnSetVertexShader(0)
, m_OnSetPixelShader(0)
, m_MatQueueModeRef(0)
, m_MatPostProcessEnableRef(0)
, m_MatDynamicTonemappingRef(0)
, m_MatMotionBlurEnabledRef(0)
, m_ColorModulationOverride(false)
, m_BlendOverride(false)
, m_FormatBmpAndNotTga(false)
, m_Current_View_Render_ThreadId(0)
//, m_RgbaRenderTarget(0)
//, m_RenderTargetDummy(0)
//, m_RenderTargetDepth(0)
{
	m_OverrideColor[0] =
	m_OverrideColor[1] =
	m_OverrideColor[2] =
	m_OverrideColor[3] =
	m_OriginalColorModulation[0] =
	m_OriginalColorModulation[1] =
	m_OriginalColorModulation[2] =
	m_OriginalColorModulation[3] = 1;
}

CAfxStreams::~CAfxStreams()
{
	while(!m_Streams.empty())
	{
		delete m_Streams.front();
		m_Streams.pop_front();
	}

	delete m_OnAfxBaseClientDll_Free;

	delete m_MatQueueModeRef;
	delete m_MatPostProcessEnableRef;
}


void CAfxStreams::OnMaterialSystem(IMaterialSystem_csgo * value)
{
	m_MaterialSystem = value;

	CreateRenderTargets(value);
}

void CAfxStreams::OnAfxVRenderView(IAfxVRenderView * value)
{
	m_VRenderView = value;

	if(m_VRenderView) m_VRenderView->OnSetBlend_set(this);
	if(m_VRenderView) m_VRenderView->OnSetColorModulation_set(this);
}

void CAfxStreams::SetCurrent_View_Render_ThreadId(DWORD id)
{
	InterlockedExchange(&m_Current_View_Render_ThreadId, id);
}

DWORD CAfxStreams::GetCurrent_View_Render_ThreadId()
{
	return InterlockedCompareExchange(&m_Current_View_Render_ThreadId, -1, -1);
}


void CAfxStreams::OnAfxBaseClientDll(IAfxBaseClientDll * value)
{
	if(m_OnAfxBaseClientDll_Free) { delete m_OnAfxBaseClientDll_Free; m_OnAfxBaseClientDll_Free = 0; }
	m_AfxBaseClientDll = value;
	if(m_AfxBaseClientDll)
	{
		m_OnAfxBaseClientDll_Free = new CFreeDelegate(m_AfxBaseClientDll->GetFreeMaster(), this, &CAfxStreams::OnAfxBaseClientDll_Free);
		m_AfxBaseClientDll->OnLevelShutdown_set(this);
		m_AfxBaseClientDll->OnView_Render_set(this);
	}
}

void CAfxStreams::OnAfxBaseClientDll_Free(void)
{
	/*
	if(m_RenderTargetDepth)
	{
		m_RenderTargetDepth->DecrementReferenceCount();
		m_RenderTargetDepth = 0;
	}
	if(m_RenderTargetDummy)
	{
		m_RenderTargetDummy->DecrementReferenceCount();
		m_RenderTargetDummy = 0;
	}
	if(m_RgbaRenderTarget)
	{
		m_RgbaRenderTarget->DecrementReferenceCount();
		m_RgbaRenderTarget = 0;
	}
	*/

	if(m_AfxBaseClientDll)
	{
		m_AfxBaseClientDll->OnView_Render_set(0);
		m_AfxBaseClientDll->OnLevelShutdown_set(0);
		m_AfxBaseClientDll = 0;
	}
}

void CAfxStreams::OnShaderShadow(IShaderShadow_csgo * value)
{
	m_ShaderShadow = value;
}

void CAfxStreams::OnDraw(IAfxMesh * am, int firstIndex, int numIndices)
{
	if(m_OnDraw)
		m_OnDraw->Draw(am, firstIndex, numIndices);
	else
		am->GetParent()->Draw(firstIndex, numIndices);
}

void CAfxStreams::OnDraw_2(IAfxMesh * am, CPrimList_csgo *pLists, int nLists)
{
	if(m_OnDraw_2)
		m_OnDraw_2->Draw_2(am, pLists, nLists);
	else
		am->GetParent()->Draw(pLists, nLists);
}

void CAfxStreams::OnDrawModulated(IAfxMesh * am, const Vector4D_csgo &vecDiffuseModulation, int firstIndex, int numIndices)
{
	if(m_OnDrawModulated)
		m_OnDrawModulated->DrawModulated(am, vecDiffuseModulation, firstIndex, numIndices);
	else
		am->GetParent()->DrawModulated(vecDiffuseModulation, firstIndex, numIndices);
}

void CAfxStreams::OnSetVertexShader(CAfx_csgo_ShaderState & state)
{
	if(!(m_Recording || m_PreviewStream) || GetCurrent_View_Render_ThreadId() != GetCurrentThreadId())
		// If streams system is live, then the thread should be the View_Render thread,
		// because we run in mat_queue_mode 0,
		// so we quit if it's from a different thread.
		return;
	
	if(m_OnSetVertexShader)
		m_OnSetVertexShader->SetVertexShader(state);
}

void CAfxStreams::OnSetPixelShader(CAfx_csgo_ShaderState & state)
{
	if(!(m_Recording || m_PreviewStream) || GetCurrent_View_Render_ThreadId() != GetCurrentThreadId())
		// If streams system is live, then the thread should be the View_Render thread,
		// because we run in mat_queue_mode 0,
		// so we quit if it's from a different thread.
		return;
	
	if(m_OnSetPixelShader)
		m_OnSetPixelShader->SetPixelShader(state);
}

void CAfxStreams::SetBlend(IAfxVRenderView * rv, float blend )
{
	m_OriginalColorModulation[3] = blend;

	if(!m_BlendOverride)
	{
		m_OverrideColor[3] = blend;
	}

	rv->GetParent()->SetBlend(blend);

	if(m_ColorModulationOverride||m_BlendOverride) AfxD3D9SetModulationColorFix(m_OverrideColor);
}

void CAfxStreams::SetColorModulation(IAfxVRenderView * rv, float const* blend )
{
	if(blend)
	{
		m_OriginalColorModulation[0] = blend[0];
		m_OriginalColorModulation[1] = blend[1];
		m_OriginalColorModulation[2] = blend[2];

		if(!m_ColorModulationOverride)
		{
			m_OverrideColor[0] = blend[0];
			m_OverrideColor[1] = blend[1];
			m_OverrideColor[2] = blend[2];
		}
	}

	rv->GetParent()->SetColorModulation(blend);

	if(m_ColorModulationOverride||m_BlendOverride) AfxD3D9SetModulationColorFix(m_OverrideColor);
}

void CAfxStreams::GetBlend(float &outBlend)
{
	if(!m_VRenderView)
		outBlend = m_OriginalColorModulation[3];
	else
		outBlend = m_VRenderView->GetParent()->GetBlend();
}

void CAfxStreams::GetColorModulation(float (& outColor)[3])
{
	if(!m_VRenderView)
	{
		outColor[0] = m_OriginalColorModulation[0];
		outColor[1] = m_OriginalColorModulation[1];
		outColor[2] = m_OriginalColorModulation[2];
	}
	else
	{
		m_VRenderView->GetParent()->GetColorModulation(outColor);
	}

}

void CAfxStreams::OverrideSetColorModulation(float const color[3])
{
	m_ColorModulationOverride = true;

	m_OverrideColor[0] = color[0];
	m_OverrideColor[1] = color[1];
	m_OverrideColor[2] = color[2];

	AfxD3D9SetModulationColorFix(m_OverrideColor);
}

void CAfxStreams::EndOverrideSetColorModulation()
{
	if(m_ColorModulationOverride)
	{
		m_OverrideColor[0] = m_OriginalColorModulation[0];
		m_OverrideColor[1] = m_OriginalColorModulation[1];
		m_OverrideColor[2] = m_OriginalColorModulation[2];

		AfxD3D9SetModulationColorFix(m_OverrideColor);

		m_ColorModulationOverride = false;
	}
}

void CAfxStreams::OverrideSetBlend(float blend)
{
	m_BlendOverride = true;

	m_OverrideColor[3] = blend;

	AfxD3D9SetModulationColorFix(m_OverrideColor);

}

void CAfxStreams::EndOverrideSetBlend()
{
	if(m_BlendOverride)
	{
		m_OverrideColor[3] = m_OriginalColorModulation[3];

		AfxD3D9SetModulationColorFix(m_OverrideColor);

		m_BlendOverride = false;
	}
}

void CAfxStreams::Console_RecordName_set(const char * value)
{
	m_RecordName.assign(value);
}

const char * CAfxStreams::Console_RecordName_get()
{
	return m_RecordName.c_str();
}

void CAfxStreams::Console_RecordFormat_set(const char * value)
{
	if(!_stricmp(value, "bmp"))
		m_FormatBmpAndNotTga = true;
	else
	if(!_stricmp(value, "tga"))
		m_FormatBmpAndNotTga = false;
	else
		Tier0_Warning("Error: Invalid format %s\n.", value);
}

const char * CAfxStreams::Console_RecordFormat_get()
{
	return m_FormatBmpAndNotTga ? "bmp" : "tga";
}

void CAfxStreams::Console_Record_Start()
{
	Console_Record_End();

	Tier0_Msg("Starting recording ... ");
	
	if(AnsiStringToWideString(m_RecordName.c_str(), m_TakeDir)
		&& (m_TakeDir.append(L"\\take"), SuggestTakePath(m_TakeDir.c_str(), 4, m_TakeDir))
		&& CreatePath(m_TakeDir.c_str(), m_TakeDir)
	)
	{
		m_Recording = true;
		m_Frame = 0;

		BackUpMatVars();
		SetMatVarsForStreams();

		for(std::list<CAfxRecordStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
		{
			(*it)->RecordStart();
		}

		Tier0_Msg("done.\n");

		std::string ansiTakeDir;
		Tier0_Msg("Recording to \"%s\".\n", WideStringToAnsiString(m_TakeDir.c_str(), ansiTakeDir) ? ansiTakeDir.c_str() : "?");
	}
	else
	{
		Tier0_Msg("FAILED");
		Tier0_Warning("Error: Failed to create directories for \"%s\".\n", m_RecordName.c_str());
	}
}

void CAfxStreams::Console_Record_End()
{
	if(m_Recording)
	{
		Tier0_Msg("Finishing recording ... ");

		for(std::list<CAfxRecordStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
		{
			(*it)->RecordEnd();
		}

		RestoreMatVars();

		Tier0_Msg("done.\n");
	}

	m_Recording = false;
}

void CAfxStreams::Console_AddStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxSingleStream(streamName, new CAfxRenderViewStream()));
}

void CAfxStreams::Console_AddBaseFxStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxSingleStream(streamName, new CAfxBaseFxStream()));
}

void CAfxStreams::Console_AddDeveloperStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxSingleStream(streamName, new CAfxDeveloperStream()));
}

void CAfxStreams::Console_AddDepthStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxSingleStream(streamName, new CAfxDepthStream()));
}

void CAfxStreams::Console_AddMatteWorldStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxSingleStream(streamName, new CAfxMatteWorldStream()));
}

void CAfxStreams::Console_AddDepthWorldStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxSingleStream(streamName, new CAfxDepthWorldStream()));
}

void CAfxStreams::Console_AddMatteEntityStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxSingleStream(streamName, new CAfxMatteEntityStream()));
}

void CAfxStreams::Console_AddDepthEntityStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxSingleStream(streamName, new CAfxDepthEntityStream()));
}

void CAfxStreams::Console_AddAlphaMatteStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxSingleStream(streamName, new CAfxAlphaMatteStream()));
}

void CAfxStreams::Console_AddAlphaEntityStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxSingleStream(streamName, new CAfxAlphaEntityStream()));
}

void CAfxStreams::Console_AddAlphaWorldStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxSingleStream(streamName, new CAfxAlphaWorldStream()));
}

void CAfxStreams::Console_AddAlphaMatteEntityStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxTwinStream(streamName, new CAfxAlphaMatteStream(), new CAfxAlphaEntityStream(), CAfxTwinStream::SCT_ARedAsAlphaBColor));
}

void CAfxStreams::Console_PrintStreams()
{
	Tier0_Msg("index: name -> recorded?\n");
	int index = 0;
	for(std::list<CAfxRecordStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
	{
		Tier0_Msg("%i: %s -> %s\n", index, (*it)->StreamName_get(), (*it)->Record_get() ? "RECORD ON (1)" : "record off (0)");
		++index;
	}
	Tier0_Msg(
		"=== Total streams: %i ===\n",
		index
	);
}

void CAfxStreams::Console_RemoveStream(const char * streamName)
{
	for(std::list<CAfxRecordStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
	{
		if(!_stricmp(streamName, (*it)->StreamName_get()))
		{
			CAfxRecordStream * cur = *it;

			if(m_Recording) cur->RecordEnd();

			if(m_PreviewStream == cur) m_PreviewStream = 0;

			m_Streams.erase(it);

			delete cur;

			return;
		}
	}
	Tier0_Msg("Error: invalid streamName.\n");
}

void CAfxStreams::Console_PreviewStream(const char * streamName)
{
	if(StringIsEmpty(streamName))
	{
		if(m_PreviewStream)
		{
			if(!m_Recording) RestoreMatVars();
		}
		m_PreviewStream = 0;
		return;
	}

	for(std::list<CAfxRecordStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
	{
		if(!_stricmp(streamName, (*it)->StreamName_get()))
		{
			if(!(*it)->AsAfxSingleStream())
			{
				Tier0_Msg("Error: Only simple (single) streams can be previewed.\n");
				return;
			}

			CAfxRecordStream * cur = *it;
			m_PreviewStream = cur;
			if(!m_Recording) BackUpMatVars();
			SetMatVarsForStreams();
			return;
		}
	}
	Tier0_Msg("Error: invalid streamName.\n");
}

// debugDump action is not displayed, because we don't want users to use it:
#define CAFXBASEFXSTREAM_AFXACTIONS "draw|noDraw|invisible|drawDepth|drawDepth24|mask|black|white"

#define CAFXBASEFXSTREAM_STREAMCOMBINETYPES "aRedAsAlphaBColor|aColorBRedAsAlpha"
#define CAFXBASEFXSTREAM_STREAMRENDERTYPES "renderView|depth"

void CAfxStreams::Console_EditStream(const char * streamName, IWrpCommandArgs * args, int argcOffset, char const * cmdPrefix)
{
	for(std::list<CAfxRecordStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
	{
		if(!_stricmp(streamName, (*it)->StreamName_get()))
		{
			Console_EditStream((*it), args, argcOffset, cmdPrefix);
			return;
		}
	}
	Tier0_Msg("Error: invalid streamName.\n");
}

void CAfxStreams::Console_EditStream(CAfxStream * stream, IWrpCommandArgs * args, int argcOffset, char const * cmdPrefix)
{
	CAfxStream * cur = stream;

	CAfxRecordStream * curRecord = 0;
	CAfxSingleStream * curSingle = 0;
	CAfxTwinStream * curTwin = 0;
	CAfxRenderViewStream * curRenderView = 0;
	CAfxDeveloperStream * curDeveloper = 0;
	CAfxBaseFxStream * curBaseFx = 0;
	
	if(cur)
	{
		curRecord = cur->AsAfxRecordStream();
	
		if(curRecord)
		{
			curSingle = curRecord->AsAfxSingleStream();
			curTwin = curRecord->AsAfxTwinStream();

			if(curSingle)
			{
				curRenderView = curSingle->Stream_get();
			}
		}
		else
		{
			curRenderView = cur->AsAfxRenderViewStream();
		}
	}

	if(curRenderView)
	{
		curDeveloper = curRenderView->AsAfxDeveloperStream();
		curBaseFx = curRenderView->AsAfxBaseFxStream();
	}

	int argc = args->ArgC() -argcOffset;

	if(cur)
	{
	}

	if(curSingle)
	{
	}

	if(curTwin)
	{
		if(1 <= argc)
		{
			char const * cmd0 = args->ArgV(argcOffset +0);

			if(!_stricmp(cmd0, "streamA"))
			{
				std::string newPrefix(cmdPrefix);
				newPrefix.append(" streamA");
				Console_EditStream(curTwin->StreamA_get(), args, argcOffset+1, newPrefix.c_str());
				return;
			}
			else
			if(!_stricmp(cmd0, "streamB"))
			{
				std::string newPrefix(cmdPrefix);
				newPrefix.append(" streamB");
				Console_EditStream(curTwin->StreamB_get(), args, argcOffset+1, newPrefix.c_str());
				return;
			}
			else
			if(!_stricmp(cmd0, "streamCombineType"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);
					CAfxTwinStream::StreamCombineType value;

					if(Console_ToStreamCombineType(cmd1, value))
					{
						curTwin->StreamCombineType_set(value);
						return;
					}
				}

				Tier0_Msg(
					"%s streamCombineType " CAFXBASEFXSTREAM_STREAMCOMBINETYPES " - Set new combine type.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, Console_FromStreamCombineType(curTwin->StreamCombineType_get())
				);
				return;
			}
		}
	}

	if(curRecord)
	{
		if(1 <= argc)
		{
			char const * cmd0 = args->ArgV(argcOffset +0);

			if(!_stricmp(cmd0, "record"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);

					curRecord->Record_set(atoi(cmd1) != 0 ? true : false);

					return;
				}

				Tier0_Msg(
					"%s record 0|1 - Whether to record this stream with mirv_streams record - 0 = record off, 1 = RECORD ON.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, curRecord->Record_get() ? "1" : "0"
				);
				return;
			}
		}
	}

	if(curRenderView)
	{
		if(1 <= argc)
		{
			char const * cmd0 = args->ArgV(argcOffset +0);

			if(!_stricmp(cmd0, "attachCommands"))
			{
				if(2 <= argc)
				{
					std::string value;

					for(int i=argcOffset +1; i < args->ArgC(); ++i)
					{
						if(argcOffset +1 < i)
						{
							value.append(" ");
						}
						value.append(args->ArgV(i));
					}
					
					curRenderView->AttachCommands_set(value.c_str());
					return;
				}

				Tier0_Msg(
					"%s attachCommands <commandString1> [<commandString2>] ... [<commandStringN>] - Set command strings to be executed when stream is attached.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, curRenderView->AttachCommands_get()
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "detachCommands"))
			{
				if(2 <= argc)
				{
					std::string value;

					for(int i=argcOffset +1; i < args->ArgC(); ++i)
					{
						if(argcOffset +1 < i)
						{
							value.append(" ");
						}
						value.append(args->ArgV(i));
					}
					
					curRenderView->DetachCommands_set(value.c_str());
					return;
				}

				Tier0_Msg(
					"%s detachCommands <commandString1> [<commandString2>] ... [<commandStringN>] - Set command strings to be executed when stream is detached.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, curRenderView->DetachCommands_get()
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "drawHud"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);

					curRenderView->DrawHud_set(atoi(cmd1) != 0 ? true : false);

					return;
				}

				Tier0_Msg(
					"%s drawHud 0|1 - Whether to draw HUD for this stream - 0 = don't draw, 1 = draw.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, curRenderView->DrawHud_get() ? "1" : "0"
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "drawViewModel"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);

					curRenderView->DrawViewModel_set(atoi(cmd1) != 0 ? true : false);

					return;
				}

				Tier0_Msg(
					"%s drawViewModel 0|1 - Whether to draw view model (in-eye weapon) for this stream - 0 = don't draw, 1 = draw.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, curRenderView->DrawViewModel_get() ? "1" : "0"
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "renderType"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);
					CAfxRenderViewStream::StreamRenderType value;

					if(Console_ToStreamRenderType(cmd1, value))
					{
						curRenderView->StreamRenderType_set(value);
						return;
					}
				}

				Tier0_Msg(
					"%s renderType " CAFXBASEFXSTREAM_STREAMRENDERTYPES " - Set new render type.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, Console_FromStreamRenderType(curRenderView->StreamRenderType_get())
				);
				return;
			}
		}
	}

	if(curDeveloper)
	{
		if(1 <= argc)
		{
			char const * cmd0 = args->ArgV(argcOffset +0);

			if(!_stricmp(cmd0, "matchTextureGroupName"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);

					curDeveloper->MatchTextureGroupName_set(cmd1);

					return;
				}

				Tier0_Msg(
					"%s matchTextureGroupName <name> - Set new texture group name to match.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, curDeveloper->MatchTextureGroupName_get()
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "matchName"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);

					curDeveloper->MatchName_set(cmd1);

					return;
				}

				Tier0_Msg(
					"%s matchName <name> - Set new name to match.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, curDeveloper->MatchName_get()
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "replaceName"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);

					curDeveloper->ReplaceName_set(cmd1);

					return;
				}

				Tier0_Msg(
					"%s replaceName <name> - Set the name of the replacement material, set an empty string(\"\") to replace nothing.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, curDeveloper->ReplaceName_get()
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "blockDraw"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);

					curDeveloper->BlockDraw_set(0 != atoi(cmd1));

					return;
				}

				Tier0_Msg(
					"%s blockDraw 0|1 - Whether to block drawing when replaceMaterial is active.\n"
					"Current value: %i.\n"
					, cmdPrefix
					, curDeveloper->BlockDraw_get() ? 1L : 0L
				);
				return;
			}
		}
	}

	if(curBaseFx)
	{
		if(1 <= argc)
		{
			char const * cmd0 = args->ArgV(argcOffset +0);

			if(!_stricmp(cmd0, "clientEffectTexturesAction"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);
					CAfxBaseFxStream::AfxAction value;

					if(Console_ToAfxAction(cmd1, value))
					{
						curBaseFx->ClientEffectTexturesAction_set(value);
						return;
					}
				}

				Tier0_Msg(
					"%s clientEffectTexturesAction " CAFXBASEFXSTREAM_AFXACTIONS " - Set new action.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, Console_FromAfxAction(curBaseFx->ClientEffectTexturesAction_get())
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "worldTexturesAction"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);
					CAfxBaseFxStream::AfxAction value;

					if(Console_ToAfxAction(cmd1, value))
					{
						curBaseFx->WorldTexturesAction_set(value);
						return;
					}
				}

				Tier0_Msg(
					"%s worldTexturesAction " CAFXBASEFXSTREAM_AFXACTIONS " - Set new action.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, Console_FromAfxAction(curBaseFx->WorldTexturesAction_get())
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "skyBoxTexturesAction"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);
					CAfxBaseFxStream::AfxAction value;

					if(Console_ToAfxAction(cmd1, value))
					{
						curBaseFx->SkyBoxTexturesAction_set(value);
						return;
					}
				}

				Tier0_Msg(
					"%s skyBoxTexturesAction " CAFXBASEFXSTREAM_AFXACTIONS " - Set new action.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, Console_FromAfxAction(curBaseFx->SkyBoxTexturesAction_get())
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "staticPropTexturesAction"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);
					CAfxBaseFxStream::AfxAction value;

					if(Console_ToAfxAction(cmd1, value))
					{
						curBaseFx->StaticPropTexturesAction_set(value);
						return;
					}
				}

				Tier0_Msg(
					"%s staticPropTexturesAction " CAFXBASEFXSTREAM_AFXACTIONS " - Set new action.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, Console_FromAfxAction(curBaseFx->StaticPropTexturesAction_get())
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "cableAction"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);
					CAfxBaseFxStream::AfxAction value;

					if(Console_ToAfxAction(cmd1, value))
					{
						curBaseFx->CableAction_set(value);
						return;
					}
				}

				Tier0_Msg(
					"%s cableAction " CAFXBASEFXSTREAM_AFXACTIONS " - Set new action.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, Console_FromAfxAction(curBaseFx->CableAction_get())
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "playerModelsAction"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);
					CAfxBaseFxStream::AfxAction value;

					if(Console_ToAfxAction(cmd1, value))
					{
						curBaseFx->PlayerModelsAction_set(value);
						return;
					}
				}

				Tier0_Msg(
					"%s playerModelsAction " CAFXBASEFXSTREAM_AFXACTIONS " - Set new action.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, Console_FromAfxAction(curBaseFx->PlayerModelsAction_get())
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "weaponModelsAction"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);
					CAfxBaseFxStream::AfxAction value;

					if(Console_ToAfxAction(cmd1, value))
					{
						curBaseFx->WeaponModelsAction_set(value);
						return;
					}
				}

				Tier0_Msg(
					"%s weaponModelsAction " CAFXBASEFXSTREAM_AFXACTIONS " - Set new action.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, Console_FromAfxAction(curBaseFx->WeaponModelsAction_get())
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "stattrackAction"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);
					CAfxBaseFxStream::AfxAction value;

					if(Console_ToAfxAction(cmd1, value))
					{
						curBaseFx->StattrackAction_set(value);
						return;
					}
				}

				Tier0_Msg(
					"%s stattrackAction " CAFXBASEFXSTREAM_AFXACTIONS " - Set new action.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, Console_FromAfxAction(curBaseFx->StattrackAction_get())
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "shellModelsAction"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);
					CAfxBaseFxStream::AfxAction value;

					if(Console_ToAfxAction(cmd1, value))
					{
						curBaseFx->ShellModelsAction_set(value);
						return;
					}
				}

				Tier0_Msg(
					"%s shellModelsAction " CAFXBASEFXSTREAM_AFXACTIONS " - Set new action.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, Console_FromAfxAction(curBaseFx->ShellModelsAction_get())
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "otherModelsAction"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);
					CAfxBaseFxStream::AfxAction value;

					if(Console_ToAfxAction(cmd1, value))
					{
						curBaseFx->OtherModelsAction_set(value);
						return;
					}
				}

				Tier0_Msg(
					"%s otherModelsAction " CAFXBASEFXSTREAM_AFXACTIONS " - Set new action.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, Console_FromAfxAction(curBaseFx->OtherModelsAction_get())
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "decalTexturesAction"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);
					CAfxBaseFxStream::AfxAction value;

					if(Console_ToAfxAction(cmd1, value))
					{
						curBaseFx->DecalTexturesAction_set(value);
						return;
					}
				}

				Tier0_Msg(
					"%s decalTexturesAction " CAFXBASEFXSTREAM_AFXACTIONS " - Set new action.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, Console_FromAfxAction(curBaseFx->DecalTexturesAction_get())
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "effectsAction"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);
					CAfxBaseFxStream::AfxAction value;

					if(Console_ToAfxAction(cmd1, value))
					{
						curBaseFx->EffectsAction_set(value);
						return;
					}
				}

				Tier0_Msg(
					"%s effectsAction " CAFXBASEFXSTREAM_AFXACTIONS " - Set new action.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, Console_FromAfxAction(curBaseFx->EffectsAction_get())
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "shellParticleAction"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);
					CAfxBaseFxStream::AfxAction value;

					if(Console_ToAfxAction(cmd1, value))
					{
						curBaseFx->ShellParticleAction_set(value);
						return;
					}
				}

				Tier0_Msg(
					"%s shellParticleAction " CAFXBASEFXSTREAM_AFXACTIONS " - Set new action.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, Console_FromAfxAction(curBaseFx->ShellParticleAction_get())
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "otherParticleAction"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);
					CAfxBaseFxStream::AfxAction value;

					if(Console_ToAfxAction(cmd1, value))
					{
						curBaseFx->OtherParticleAction_set(value);
						return;
					}
				}

				Tier0_Msg(
					"%s otherParticleAction " CAFXBASEFXSTREAM_AFXACTIONS " - Set new action.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, Console_FromAfxAction(curBaseFx->OtherParticleAction_get())
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "stickerAction"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);
					CAfxBaseFxStream::AfxAction value;

					if(Console_ToAfxAction(cmd1, value))
					{
						curBaseFx->StickerAction_set(value);
						return;
					}
				}

				Tier0_Msg(
					"%s stickerAction " CAFXBASEFXSTREAM_AFXACTIONS " - Set new action.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, Console_FromAfxAction(curBaseFx->StickerAction_get())
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "depthVal"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);
					curBaseFx->DepthVal_set((float)atof(cmd1));
					return;
				}

				Tier0_Msg(
					"%s depthVal <fValue> - Set new miniumum depth floating point value <fValue>.\n"
					"Current value: %f.\n"
					, cmdPrefix
					, curBaseFx->DepthVal_get()
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "depthValMax"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);
					curBaseFx->DepthValMax_set((float)atof(cmd1));
					return;
				}

				Tier0_Msg(
					"%s depthValMax <fValue> - Set new maximum depth floating point value <fValue>.\n"
					"Current value: %f.\n"
					, cmdPrefix
					, curBaseFx->DepthValMax_get()
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "debugPrint"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);

					curBaseFx->DebugPrint_set(0 != atoi(cmd1) ? true : false);
					return;
				}

				Tier0_Msg(
					"%s debugPrint 0|1 - Disable / enable debug console output.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, curBaseFx->DebugPrint_get() ? "1" : "0"
				);
				return;
			}
			else
			if(!_stricmp(cmd0, "invalidateMap"))
			{
				curBaseFx->InvalidateMap();
				return;
			}
			else
			if(!_stricmp(cmd0, "testAction"))
			{
				if(2 <= argc)
				{
					char const * cmd1 = args->ArgV(argcOffset +1);

					curBaseFx->TestAction_set(0 != atoi(cmd1) ? true : false);
					return;
				}

				Tier0_Msg(
					"%s testAction 0|1 - Disable / enable action for devloper testing purposes.\n"
					"Current value: %s.\n"
					, cmdPrefix
					, curBaseFx->TestAction_get() ? "1" : "0"
				);
				return;
			}
		}
	}

	if(cur)
	{
	}

	if(curRecord)
	{
		Tier0_Msg("%s record [...] - Controlls whether or not this stream is recorded with mirv_streams record.\n", cmdPrefix);
	}

	if(curSingle)
	{
	}

	if(curTwin)
	{
		Tier0_Msg("%s streamA [...] - Edit sub stream A.\n", cmdPrefix);
		Tier0_Msg("%s streamB [...] - Edit sub stream B.\n", cmdPrefix);
		Tier0_Msg("%s streamCombineType [...] - Controlls how streams are combined.\n", cmdPrefix);
	}

	if(curRenderView)
	{
		Tier0_Msg("%s attachCommands [...] - Commands to be executed when stream is attached. WARNING. Use at your own risk, game may crash!\n", cmdPrefix);
		Tier0_Msg("%s detachCommands [...] - Commands to be executed when stream is detached. WARNING. Use at your own risk, game may crash!\n", cmdPrefix);
		Tier0_Msg("%s drawHud [...] - Controlls whether or not HUD is drawn for this stream.\n", cmdPrefix);
		Tier0_Msg("%s drawViewModel [...] - Controlls whether or not view model (in-eye weapon) is drawn for this stream.\n", cmdPrefix);
		// renderType options is not displayed, because we don't want users to use it.
		// Tier0_Msg("%s renderType [...] - Stream render type.\n", cmdPrefix);
	}
			
	if(curDeveloper)
	{
		Tier0_Msg("%s matchTextureGroupName [...]\n", cmdPrefix);
		Tier0_Msg("%s matchName [...]\n", cmdPrefix);
		Tier0_Msg("%s replaceName [...]\n", cmdPrefix);
		Tier0_Msg("%s blockDraw [...]\n", cmdPrefix);
	}

	if(curBaseFx)
	{
		Tier0_Msg("%s clientEffectTexturesAction [...]\n", cmdPrefix);
		Tier0_Msg("%s worldTexturesAction [...]\n", cmdPrefix);
		Tier0_Msg("%s skyBoxTexturesAction [...]\n", cmdPrefix);
		Tier0_Msg("%s staticPropTexturesAction [...]\n", cmdPrefix);
		Tier0_Msg("%s cableAction [...]\n", cmdPrefix);
		Tier0_Msg("%s playerModelsAction [...]\n", cmdPrefix);
		Tier0_Msg("%s weaponModelsAction [...]\n", cmdPrefix);
		Tier0_Msg("%s stattrackAction [...]\n", cmdPrefix);
		Tier0_Msg("%s shellModelsAction [...]\n", cmdPrefix);
		Tier0_Msg("%s otherModelsAction [...]\n", cmdPrefix);
		Tier0_Msg("%s decalTexturesAction [...]\n", cmdPrefix);
		Tier0_Msg("%s effectsAction [...]\n", cmdPrefix);
		Tier0_Msg("%s shellParticleAction [...]\n", cmdPrefix);
		Tier0_Msg("%s otherParticleAction [...]\n", cmdPrefix);
		Tier0_Msg("%s stickerAction [...]\n", cmdPrefix);
		Tier0_Msg("%s depthVal [...]\n", cmdPrefix);
		Tier0_Msg("%s depthValMax [...]\n", cmdPrefix);
		Tier0_Msg("%s debugPrint [...]\n", cmdPrefix);
		Tier0_Msg("%s invalidateMap - invaldiates the material map.\n", cmdPrefix);
		// testAction options is not displayed, because we don't want users to use it.
		// Tier0_Msg("%s testAction [...]\n", cmdPrefix);
	}

	Tier0_Msg("No further options for this stream.\n");
}

IMaterialSystem_csgo * CAfxStreams::GetMaterialSystem(void)
{
	return m_MaterialSystem;
}

IAfxFreeMaster * CAfxStreams::GetFreeMaster(void)
{
	if(m_AfxBaseClientDll) return m_AfxBaseClientDll->GetFreeMaster();
	return 0;
}

IAfxMatRenderContext * CAfxStreams::GetCurrentContext(void)
{
	return m_CurrentContext;
}

IShaderShadow_csgo * CAfxStreams::GetShaderShadow(void)
{
	return m_ShaderShadow;
}

std::wstring CAfxStreams::GetTakeDir(void)
{
	return m_TakeDir;
}

void CAfxStreams::OnMaterialHook_set(IAfxMatRenderContextMaterialHook * value)
{
	if(m_CurrentContext) m_CurrentContext->OnMaterialHook_set(value);
}

void CAfxStreams::OnDrawInstances_set(IAfxMatRenderContextDrawInstances * value)
{
	if(m_CurrentContext) m_CurrentContext->OnDrawInstances_set(value);
}

void CAfxStreams::OnDraw_set(IAfxMeshDraw * value)
{
	m_OnDraw = value;
}

void CAfxStreams::OnDraw_2_set(IAfxMeshDraw_2 * value)
{
	m_OnDraw_2 = value;
}

void CAfxStreams::OnDrawModulated_set(IAfxMeshDrawModulated * value)
{
	m_OnDrawModulated = value;
}

void CAfxStreams::OnSetVertexShader_set(IAfxSetVertexShader * value)
{
	m_OnSetVertexShader = value;
}

void CAfxStreams::OnSetPixelShader_set(IAfxSetPixelShader * value)
{
	m_OnSetPixelShader = value;
}

void CAfxStreams::LevelShutdown(IAfxBaseClientDll * cl)
{
	for(std::list<CAfxRecordStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
	{
		(*it)->LevelShutdown(this);
	}
}

extern bool g_bD3D9DebugPrint;

void CAfxStreams::DebugDump()
{
	bool isRgba = true;

	int width = 1280;
	int height = 720;

	if(m_BufferA.AutoRealloc(isRgba ? m_BufferA.IBPF_BGRA : m_BufferA.IBPF_BGR, width, height))
	{
		m_CurrentContext->GetParent()->ReadPixels(
			0, 0,
			width, height,
			(unsigned char*)m_BufferA.Buffer,
			isRgba ? IMAGE_FORMAT_RGBA8888 : IMAGE_FORMAT_RGB888
		);

		// (back) transform to MDT native format:
		{
			int lastLine = height >> 1;
			if(height & 0x1) ++lastLine;

			for(int y=0;y<lastLine;++y)
			{
				int srcLine = y;
				int dstLine = height -1 -y;

				if(isRgba)
				{
					for(int x=0;x<width;++x)
					{
						unsigned char r = ((unsigned char *)m_BufferA.Buffer)[dstLine*m_BufferA.ImagePitch +4*x +0];
						unsigned char g = ((unsigned char *)m_BufferA.Buffer)[dstLine*m_BufferA.ImagePitch +4*x +1];
						unsigned char b = ((unsigned char *)m_BufferA.Buffer)[dstLine*m_BufferA.ImagePitch +4*x +2];
						unsigned char a = ((unsigned char *)m_BufferA.Buffer)[dstLine*m_BufferA.ImagePitch +4*x +3];
									
						((unsigned char *)m_BufferA.Buffer)[dstLine*m_BufferA.ImagePitch +4*x +0] = ((unsigned char *)m_BufferA.Buffer)[srcLine*m_BufferA.ImagePitch +4*x +2];
						((unsigned char *)m_BufferA.Buffer)[dstLine*m_BufferA.ImagePitch +4*x +1] = ((unsigned char *)m_BufferA.Buffer)[srcLine*m_BufferA.ImagePitch +4*x +1];
						((unsigned char *)m_BufferA.Buffer)[dstLine*m_BufferA.ImagePitch +4*x +2] = ((unsigned char *)m_BufferA.Buffer)[srcLine*m_BufferA.ImagePitch +4*x +0];
						((unsigned char *)m_BufferA.Buffer)[dstLine*m_BufferA.ImagePitch +4*x +3] = ((unsigned char *)m_BufferA.Buffer)[srcLine*m_BufferA.ImagePitch +4*x +3];

						((unsigned char *)m_BufferA.Buffer)[srcLine*m_BufferA.ImagePitch +4*x +0] = b;
						((unsigned char *)m_BufferA.Buffer)[srcLine*m_BufferA.ImagePitch +4*x +1] = g;
						((unsigned char *)m_BufferA.Buffer)[srcLine*m_BufferA.ImagePitch +4*x +2] = r;
						((unsigned char *)m_BufferA.Buffer)[srcLine*m_BufferA.ImagePitch +4*x +3] = a;
					}
				}
				else
				{
					for(int x=0;x<width;++x)
					{
						unsigned char r = ((unsigned char *)m_BufferA.Buffer)[dstLine*m_BufferA.ImagePitch +3*x +0];
						unsigned char g = ((unsigned char *)m_BufferA.Buffer)[dstLine*m_BufferA.ImagePitch +3*x +1];
						unsigned char b = ((unsigned char *)m_BufferA.Buffer)[dstLine*m_BufferA.ImagePitch +3*x +2];
									
						((unsigned char *)m_BufferA.Buffer)[dstLine*m_BufferA.ImagePitch +3*x +0] = ((unsigned char *)m_BufferA.Buffer)[srcLine*m_BufferA.ImagePitch +3*x +2];
						((unsigned char *)m_BufferA.Buffer)[dstLine*m_BufferA.ImagePitch +3*x +1] = ((unsigned char *)m_BufferA.Buffer)[srcLine*m_BufferA.ImagePitch +3*x +1];
						((unsigned char *)m_BufferA.Buffer)[dstLine*m_BufferA.ImagePitch +3*x +2] = ((unsigned char *)m_BufferA.Buffer)[srcLine*m_BufferA.ImagePitch +3*x +0];
									
						((unsigned char *)m_BufferA.Buffer)[srcLine*m_BufferA.ImagePitch +3*x +0] = b;
						((unsigned char *)m_BufferA.Buffer)[srcLine*m_BufferA.ImagePitch +3*x +1] = g;
						((unsigned char *)m_BufferA.Buffer)[srcLine*m_BufferA.ImagePitch +3*x +2] = r;
					}
				}
			}
		}

		// Write to disk:
		{
			std::wstring path = L"debug.tga";
			if(!WriteBufferToFile(m_BufferA, path))
			{
				Tier0_Warning("CAfxStreams::DebugDump:Failed writing image for frame #%i\n.", m_Frame);
			}
		}
	}
	else
	{
		Tier0_Warning("CAfxStreams::DebugDump: Failed to realloc m_BufferA.\n");
	}

}

void CAfxStreams::View_Render(IAfxBaseClientDll * cl, IAfxMatRenderContext * cx, vrect_t_csgo *rect)
{
	SetCurrent_View_Render_ThreadId(GetCurrentThreadId());
	m_CurrentContext = cx;

	bool canFeed = CheckCanFeedStreams();

	CAfxRenderViewStream * previewStream = 0;
	if(m_PreviewStream)
	{
		if(CAfxSingleStream * singleStream = m_PreviewStream->AsAfxSingleStream())
		{
			previewStream = singleStream->Stream_get();
		}
	}

	if(previewStream)
	{
		if(!canFeed)
			Tier0_Warning("Error: Cannot preview stream %s due to missing dependencies!\n", m_PreviewStream->StreamName_get());
		else
		{
			SetMatVarsForStreams(); // keep them set in case a mofo resets them.

			previewStream->StreamAttach(this);

			if(0 < strlen(previewStream->AttachCommands_get())) g_VEngineClient->ExecuteClientCmd(previewStream->AttachCommands_get());

			cx->GetParent()->ClearColor4ub(0,0,0,0);
			cx->GetParent()->ClearBuffers(true,false,false);
		}
	}

	cl->GetParent()->View_Render(rect);

	if(previewStream && canFeed)
	{
		if(0 < strlen(previewStream->DetachCommands_get())) g_VEngineClient->ExecuteClientCmd(previewStream->DetachCommands_get());

		previewStream->StreamDetach(this);
	}

	if(m_Recording)
	{
		if(!canFeed)
		{
			Tier0_Warning("Error: Cannot record streams due to missing dependencies!\n");
		}
		else
		{
			for(std::list<CAfxRecordStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
			{
				if(!(*it)->Record_get()) continue;

				//
				// Record the stream:

				CAfxRenderViewStream * streamA = 0;
				CAfxRenderViewStream * streamB = 0;
				bool streamAOk = false;
				bool streamBOk = false;

				CAfxSingleStream * curSingle = (*it)->AsAfxSingleStream();
				CAfxTwinStream * curTwin = (*it)->AsAfxTwinStream();

				if(curSingle)
				{
					streamA = curSingle->Stream_get();
				}
				else
				if(curTwin)
				{
					CAfxTwinStream::StreamCombineType streamCombineType = curTwin->StreamCombineType_get();

					if(CAfxTwinStream::SCT_ARedAsAlphaBColor == streamCombineType)
					{
						streamA = curTwin->StreamB_get();
						streamB = curTwin->StreamA_get();
					}
					else
					if(CAfxTwinStream::SCT_AColorBRedAsAlpha == streamCombineType)
					{
						streamA = curTwin->StreamA_get();
						streamB = curTwin->StreamB_get();
					}
				}

				if(streamA)
				{
					streamAOk = CaptureStreamToBuffer(streamA, m_BufferA, cx);
				}

				if(streamB)
				{
					streamBOk = CaptureStreamToBuffer(streamB, m_BufferB, cx);
				}

				if(streamA && streamB)
				{
					streamAOk = streamAOk && streamBOk
						&& m_BufferA.Width == m_BufferB.Width
						&& m_BufferA.Height == m_BufferB.Height
						&& m_BufferA.PixelFormat == m_BufferA.IBPF_BGR
						&& m_BufferA.PixelFormat == m_BufferB.PixelFormat
						&& m_BufferA.ImagePitch == m_BufferB.ImagePitch
						&& m_BufferA.AutoRealloc(m_BufferA.IBPF_BGRA, m_BufferA.Width, m_BufferA.Height)
					;

					if(streamAOk)
					{
						// interleave B as alpha into A:

						for(int y = m_BufferA.Height-1;y>=0;--y)
						{
							for(int x=m_BufferA.Width-1;x>=0;--x)
							{
								unsigned char b = ((unsigned char *)m_BufferA.Buffer)[y*m_BufferB.ImagePitch+x*3+0];
								unsigned char g = ((unsigned char *)m_BufferA.Buffer)[y*m_BufferB.ImagePitch+x*3+1];
								unsigned char r = ((unsigned char *)m_BufferA.Buffer)[y*m_BufferB.ImagePitch+x*3+2];
								unsigned char a = ((unsigned char *)m_BufferB.Buffer)[y*m_BufferB.ImagePitch+x*3+0];

								((unsigned char *)m_BufferA.Buffer)[y*m_BufferA.ImagePitch+x*4+0] = b;
								((unsigned char *)m_BufferA.Buffer)[y*m_BufferA.ImagePitch+x*4+1] = g;
								((unsigned char *)m_BufferA.Buffer)[y*m_BufferA.ImagePitch+x*4+2] = r;
								((unsigned char *)m_BufferA.Buffer)[y*m_BufferA.ImagePitch+x*4+3] = a;
							}
						}
					}
					else
					{
						Tier0_Warning("CAfxStreams::View_Render: Combining streams failed.\n");
					}
				}

				// Write to disk:
				if(streamAOk)
				{
					std::wstring path;
					if((*it)->CreateCapturePath(m_TakeDir, m_Frame, m_FormatBmpAndNotTga, path))
					{
						if(!WriteBufferToFile(m_BufferA, path))
						{
							Tier0_Warning("Failed writing image #%i for stream %s\n.", m_Frame, (*it)->StreamName_get());
						}
					}
				}

			}

		}

		++m_Frame;
	}

	m_CurrentContext = 0;
	SetCurrent_View_Render_ThreadId(0);
}

bool CAfxStreams::CaptureStreamToBuffer(CAfxRenderViewStream * stream, CImageBuffer & buffer, IAfxMatRenderContext * cx)
{
	bool bOk = false;

	SetMatVarsForStreams(); // keep them set in case a mofo resets them.

	m_MaterialSystem->SwapBuffers();

	stream->StreamAttach(this);

	if(0 < strlen(stream->AttachCommands_get())) g_VEngineClient->ExecuteClientCmd(stream->AttachCommands_get());

	IViewRender_csgo * view = GetView_csgo();

	const CViewSetup_csgo * viewSetup = view->GetViewSetup();

	int whatToDraw = RENDERVIEW_UNSPECIFIED;

	if(stream->DrawHud_get()) whatToDraw |= RENDERVIEW_DRAWHUD;
	if(stream->DrawViewModel_get()) whatToDraw |= RENDERVIEW_DRAWVIEWMODEL;

	//if(stream->SRT_Depth == stream->StreamRenderType_get())
	//	cx->GetParent()->PushRenderTargetAndViewport(m_RgbaRenderTarget);

	cx->GetParent()->ClearColor4ub(0,0,0,0);
	cx->GetParent()->ClearBuffers(true,false,false);

	if(stream->SRT_Depth == stream->StreamRenderType_get())
		; // do nothing for now, just in case // view->UpdateShadowDepthTexture(0,0,*viewSetup);
	else
		view->RenderView(*viewSetup, *viewSetup, VIEW_CLEAR_STENCIL|VIEW_CLEAR_DEPTH, whatToDraw);

	if(stream->SRT_RenderView == stream->StreamRenderType_get())
	{
		if(buffer.AutoRealloc(CImageBuffer::IBPF_BGR, viewSetup->m_nUnscaledWidth, viewSetup->m_nUnscaledHeight))
		{
			cx->GetParent()->ReadPixels(
				viewSetup->m_nUnscaledX, viewSetup->m_nUnscaledY,
				buffer.Width, buffer.Height,
				(unsigned char*)buffer.Buffer,
				IMAGE_FORMAT_RGB888
			);

			// (back) transform to MDT native format:
			{
				int lastLine = buffer.Height >> 1;
				if(buffer.Height & 0x1) ++lastLine;

				for(int y=0;y<lastLine;++y)
				{
					int srcLine = y;
					int dstLine = buffer.Height -1 -y;

					for(int x=0;x<buffer.Width;++x)
					{
						unsigned char r = ((unsigned char *)buffer.Buffer)[dstLine*buffer.ImagePitch +3*x +0];
						unsigned char g = ((unsigned char *)buffer.Buffer)[dstLine*buffer.ImagePitch +3*x +1];
						unsigned char b = ((unsigned char *)buffer.Buffer)[dstLine*buffer.ImagePitch +3*x +2];
									
						((unsigned char *)buffer.Buffer)[dstLine*buffer.ImagePitch +3*x +0] = ((unsigned char *)buffer.Buffer)[srcLine*buffer.ImagePitch +3*x +2];
						((unsigned char *)buffer.Buffer)[dstLine*buffer.ImagePitch +3*x +1] = ((unsigned char *)buffer.Buffer)[srcLine*buffer.ImagePitch +3*x +1];
						((unsigned char *)buffer.Buffer)[dstLine*buffer.ImagePitch +3*x +2] = ((unsigned char *)buffer.Buffer)[srcLine*buffer.ImagePitch +3*x +0];
									
						((unsigned char *)buffer.Buffer)[srcLine*buffer.ImagePitch +3*x +0] = b;
						((unsigned char *)buffer.Buffer)[srcLine*buffer.ImagePitch +3*x +1] = g;
						((unsigned char *)buffer.Buffer)[srcLine*buffer.ImagePitch +3*x +2] = r;
					}
				}
			}

			bOk = true;
		}
		else
		{
			Tier0_Warning("CAfxStreams::CaptureStreamToBuffer: Failed to realloc buffer.\n");
		}
	}
	else if(stream->SRT_Depth == stream->StreamRenderType_get())
	{
		if(buffer.AutoRealloc(CImageBuffer::IBPF_A, viewSetup->m_nUnscaledWidth, viewSetup->m_nUnscaledHeight))
		{
			cx->GetParent()->ReadPixels(
				viewSetup->m_nUnscaledX, viewSetup->m_nUnscaledY,
				buffer.Width, buffer.Height,
				(unsigned char*)buffer.Buffer,
				IMAGE_FORMAT_A8
			);

			// (back) transform to MDT native format:
			{
				int lastLine = buffer.Height >> 1;
				if(buffer.Height & 0x1) ++lastLine;

				for(int y=0;y<lastLine;++y)
				{
					int srcLine = y;
					int dstLine = buffer.Height -1 -y;

					for(int x=0;x<buffer.Width;++x)
					{
						unsigned char a = ((unsigned char *)buffer.Buffer)[dstLine*buffer.ImagePitch +1*x +0];
									
						((unsigned char *)buffer.Buffer)[dstLine*buffer.ImagePitch +1*x +0] = ((unsigned char *)buffer.Buffer)[srcLine*buffer.ImagePitch +1*x +0];
									
						((unsigned char *)buffer.Buffer)[srcLine*buffer.ImagePitch +1*x +0] = a;
					}
				}
			}

			bOk = true;
		}
		else
		{
			Tier0_Warning("CAfxStreams::CaptureStreamToBuffer: Failed to realloc buffer.\n");
		}
	}

	//if(stream->SRT_Depth == stream->StreamRenderType_get())
	//	cx->GetParent()->PopRenderTargetAndViewport();

	if(0 < strlen(stream->DetachCommands_get())) g_VEngineClient->ExecuteClientCmd(stream->DetachCommands_get());

	stream->StreamDetach(this);

	return bOk;
}

bool CAfxStreams::WriteBufferToFile(const CImageBuffer & buffer, const std::wstring & path)
{
	if(buffer.IBPF_A == buffer.PixelFormat)
	{
		return m_FormatBmpAndNotTga
			? WriteRawBitmap((unsigned char*)buffer.Buffer, path.c_str(), buffer.Width, buffer.Height, 8, buffer.ImagePitch)
			: WriteRawTarga((unsigned char*)buffer.Buffer, path.c_str(), buffer.Width, buffer.Height, 8, true, buffer.ImagePitch, 0)
		;
	}

	bool isBgra = buffer.IBPF_BGRA == buffer.PixelFormat;

	return m_FormatBmpAndNotTga && !isBgra
		? WriteRawBitmap((unsigned char*)buffer.Buffer, path.c_str(), buffer.Width, buffer.Height, 24, buffer.ImagePitch)
		: WriteRawTarga((unsigned char*)buffer.Buffer, path.c_str(), buffer.Width, buffer.Height, isBgra ? 32 : 24, false, buffer.ImagePitch, isBgra ? 8 : 0)
	;
}

bool CAfxStreams::Console_CheckStreamName(char const * value)
{
	if(StringIsEmpty(value))
	{
		Tier0_Msg("Error: Stream name can not be emty.\n");
		return false;
	}
	if(!StringIsAlNum(value))
	{
		Tier0_Msg("Error: Stream name must be alphanumeric.\n");
		return false;
	}

	// Check if name is unique:
	{
		int index = 0;
		for(std::list<CAfxRecordStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
		{
			if(!_stricmp((*it)->StreamName_get(), value))
			{
				Tier0_Msg("Error: Stream name must be unique, \"%s\" is already in use by stream with index %i.\n", value, index);
				return false;
			}

			++index;
		}
	}

	return true;
}

bool CAfxStreams::Console_ToAfxAction(char const * value, CAfxBaseFxStream::AfxAction & action)
{
	if(!_stricmp(value, "draw"))
	{
		action = CAfxBaseFxStream::AA_Draw;
		return true;
	}
	else
	if(!_stricmp(value, "noDraw"))
	{
		action = CAfxBaseFxStream::AA_NoDraw;
		return true;
	}
	else
	if(!_stricmp(value, "invisible"))
	{
		action = CAfxBaseFxStream::AA_Invisible;
		return true;
	}
	else
	if(!_stricmp(value, "drawDepth"))
	{
		action = CAfxBaseFxStream::AA_DrawDepth;
		return true;
	}
	else
	if(!_stricmp(value, "drawDepth24"))
	{
		action = CAfxBaseFxStream::AA_DrawDepth24;
		return true;
	}
	else
	if(!_stricmp(value, "mask"))
	{
		action = CAfxBaseFxStream::AA_GreenScreen;
		return true;
	}
	else
	if(!_stricmp(value, "black"))
	{
		action = CAfxBaseFxStream::AA_Black;
		return true;
	}
	else
	if(!_stricmp(value, "white"))
	{
		action = CAfxBaseFxStream::AA_White;
		return true;
	}
	else
	if(!_stricmp(value, "debugDump"))
	{
		action = CAfxBaseFxStream::AA_DebugDump;
		return true;
	}

	return false;
}

char const * CAfxStreams::Console_FromAfxAction(CAfxBaseFxStream::AfxAction action)
{
	switch(action)
	{
	case CAfxBaseFxStream::AA_Draw:
		return "draw";
	case CAfxBaseFxStream::AA_NoDraw:
		return "noDraw";
	case CAfxBaseFxStream::AA_Invisible:
		return "invisible";
	case CAfxBaseFxStream::AA_DrawDepth:
		return "drawDepth";
	case CAfxBaseFxStream::AA_DrawDepth24:
		return "drawDepth24";
	case CAfxBaseFxStream::AA_GreenScreen:
		return "mask";
	case CAfxBaseFxStream::AA_Black:
		return "black";
	case CAfxBaseFxStream::AA_White:
		return "white";
	case CAfxBaseFxStream::AA_DebugDump:
		return "debugDump";
	}

	return "[unknown]";
}

bool CAfxStreams::Console_ToStreamCombineType(char const * value, CAfxTwinStream::StreamCombineType & streamCombineType)
{
	if(!_stricmp(value, "aRedAsAlphaBColor"))
	{
		streamCombineType = CAfxTwinStream::SCT_ARedAsAlphaBColor;
		return true;
	}
	else
	if(!_stricmp(value, "aColorBRedAsAlpha"))
	{
		streamCombineType = CAfxTwinStream::SCT_AColorBRedAsAlpha;
		return true;
	}

	return false;
}

char const * CAfxStreams::Console_FromStreamCombineType(CAfxTwinStream::StreamCombineType streamCombineType)
{
	switch(streamCombineType)
	{
	case CAfxTwinStream::SCT_ARedAsAlphaBColor:
		return "aRedAsAlphaBColor";
	case CAfxTwinStream::SCT_AColorBRedAsAlpha:
		return "aColorBRedAsAlpha";
	}

	return "[unkown]";
}

bool CAfxStreams::Console_ToStreamRenderType(char const * value, CAfxRenderViewStream::StreamRenderType & streamRenderType)
{
	if(!_stricmp(value, "renderView"))
	{
		streamRenderType = CAfxRenderViewStream::SRT_RenderView;
		return true;
	}
	else
	if(!_stricmp(value, "depth"))
	{
		streamRenderType = CAfxRenderViewStream::SRT_Depth;
		return true;
	}

	return false;
}

char const * CAfxStreams::Console_FromStreamRenderType(CAfxRenderViewStream::StreamRenderType streamRenderType)
{
	switch(streamRenderType)
	{
	case CAfxRenderViewStream::SRT_RenderView:
		return "renderView";
	case CAfxRenderViewStream::SRT_Depth:
		return "depth";
	}

	return "[unkown]";

}

bool CAfxStreams::CheckCanFeedStreams(void)
{
	return 0 != GetView_csgo()
		&& 0 != m_MaterialSystem
		&& 0 != m_VRenderView
		&& 0 != m_AfxBaseClientDll
		&& 0 != m_ShaderShadow
		&& 0 != m_CurrentContext
	;
}

void CAfxStreams::BackUpMatVars()
{
	EnsureMatVars();

	m_OldMatQueueMode = m_MatQueueModeRef->GetInt();
	m_OldMatPostProcessEnable = m_MatPostProcessEnableRef->GetInt();
	m_OldMatDynamicTonemapping = m_MatDynamicTonemappingRef->GetInt();
	m_OldMatMotionBlurEnabled = m_MatMotionBlurEnabledRef->GetInt();
}

void CAfxStreams::SetMatVarsForStreams()
{
	EnsureMatVars();

	m_MatQueueModeRef->SetValue(0.0f);
	m_MatPostProcessEnableRef->SetValue(0.0f);
	m_MatDynamicTonemappingRef->SetValue(0.0f);
	m_MatMotionBlurEnabledRef->SetValue(0.0f);
}

void CAfxStreams::RestoreMatVars()
{
	EnsureMatVars();

	m_MatQueueModeRef->SetValue((float)m_OldMatQueueMode);
	m_MatPostProcessEnableRef->SetValue((float)m_OldMatPostProcessEnable);
	m_MatDynamicTonemappingRef->SetValue((float)m_OldMatDynamicTonemapping);
	m_MatMotionBlurEnabledRef->SetValue((float)m_OldMatMotionBlurEnabled);
}

void CAfxStreams::EnsureMatVars()
{
	if(!m_MatQueueModeRef) m_MatQueueModeRef = new WrpConVarRef("mat_queue_mode");
	if(!m_MatPostProcessEnableRef) m_MatPostProcessEnableRef = new WrpConVarRef("mat_postprocess_enable");
	if(!m_MatDynamicTonemappingRef) m_MatDynamicTonemappingRef = new WrpConVarRef("mat_dynamic_tonemapping");
	if(!m_MatMotionBlurEnabledRef) m_MatMotionBlurEnabledRef = new WrpConVarRef("mat_motion_blur_enabled");
}

void CAfxStreams::AddStream(CAfxRecordStream * stream)
{
	m_Streams.push_back(stream);

	if(m_Recording) stream->RecordStart();
}

void CAfxStreams::CreateRenderTargets(IMaterialSystem_csgo * materialSystem)
{
	//materialSystem->BeginRenderTargetAllocation();

/*
	m_RgbaRenderTarget = materialSystem->CreateRenderTargetTexture(0,0,RT_SIZE_FULL_FRAME_BUFFER,IMAGE_FORMAT_RGBA8888);
	if(m_RgbaRenderTarget)
	{
		m_RgbaRenderTarget->IncrementReferenceCount();
	}
	else
	{
		Tier0_Warning("AFXERROR: CAfxStreams::CreateRenderTargets no m_RgbaRenderTarget (affects rgba captures)!\n");
	}
*/

/*	
	m_RenderTargetDummy = materialSystem->CreateRenderTargetTexture(0,0,RT_SIZE_FULL_FRAME_BUFFER,IMAGE_FORMAT_NULL);
	if(m_RenderTargetDummy)
	{
		m_RenderTargetDummy->IncrementReferenceCount();
	}
	else
	{
		Tier0_Warning("AFXERROR: CAfxStreams::CreateRenderTargets no m_RenderTargetDummy (affects depth captures)!\n");
	}

	m_RenderTargetDepth = materialSystem->CreateRenderTargetTexture(0,0,RT_SIZE_FULL_FRAME_BUFFER,IMAGE_FORMAT_A8,MATERIAL_RT_DEPTH_NONE);
	if(m_RenderTargetDepth)
	{
		m_RenderTargetDepth->IncrementReferenceCount();
	}
	else
	{
		Tier0_Warning("AFXERROR: CAfxStreams::CreateRenderTargets no m_RenderTargetDepth (affects depth captures)!\n");
	}
*/

	//materialSystem->EndRenderTargetAllocation();

}

// CAfxStreams::CImageBuffer ///////////////////////////////////////////////////

CAfxStreams::CImageBuffer::CImageBuffer()
: Buffer(0)
, m_BufferBytesAllocated(0)
{
}

CAfxStreams::CImageBuffer::~CImageBuffer()
{
	free(Buffer);
}

bool CAfxStreams::CImageBuffer::AutoRealloc(ImageBufferPixelFormat pixelFormat, int width, int height)
{
	size_t pitch = width;

	switch(pixelFormat)
	{
	case IBPF_BGR:
		pitch *= 3;
		break;
	case IBPF_BGRA:
		pitch *= 4;
		break;
	case IBPF_A:
		pitch *= 1;
		break;
	default:
		Tier0_Warning("CAfxStreams::CImageBuffer::AutoRealloc: Unsupported pixelFormat\n");
		return false;
	}

	size_t imageBytes = pitch * height;

	if( !Buffer || m_BufferBytesAllocated < imageBytes)
	{
		Buffer = realloc(Buffer, imageBytes);
		if(Buffer)
		{
			m_BufferBytesAllocated = imageBytes;
		}
	}

	m_BufferBytesAllocated = imageBytes;
	PixelFormat = pixelFormat;
	Width = width;
	Height = height;
	ImagePitch = pitch;
	ImageBytes = imageBytes;

	return 0 != Buffer;
}
