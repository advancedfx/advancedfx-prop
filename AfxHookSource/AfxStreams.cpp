#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-07-27 dominik.matrixstorm.com
//
// First changes:
// 2015-06-26 dominik.matrixstorm.com

#include "AfxStreams.h"

#include "SourceInterfaces.h"
#include "WrpVEngineClient.h"
#include "d3d9Hooks.h"
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

CAfxStream::CAfxStream(char const * streamName)
: m_StreamName(streamName)
, m_Streams(0)
, m_Record(true)
, m_DrawViewModel(true)
, m_DrawHud(false)
{
}

CAfxStream::~CAfxStream()
{
}

bool CAfxStream::DrawHud_get(void)
{
	return m_DrawHud;
}

void CAfxStream::DrawHud_set(bool value)
{
	m_DrawHud = value;
}

bool CAfxStream::DrawViewModel_get(void)
{
	return m_DrawViewModel;
}

void CAfxStream::DrawViewModel_set(bool value)
{
	m_DrawViewModel = value;
}

bool CAfxStream::Record_get(void)
{
	return m_Record;
}

void CAfxStream::Record_set(bool value)
{
	m_Record = value;
}

void CAfxStream::RecordStart()
{
	m_TriedCreatePath = false;
	m_SucceededCreatePath = false;
}

bool CAfxStream::CreateCapturePath(int frameNumber, bool isBmpAndNotTga, std::wstring &outPath)
{
	if(!m_TriedCreatePath)
	{
		m_TriedCreatePath = true;
		std::wstring wideStreamName;
		if(AnsiStringToWideString(GetStreamName(), wideStreamName))
		{
			m_CapturePath = m_Streams->GetTakeDir();
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
			Tier0_Warning("Error: Failed to convert stream name \"%s\" to a wide string.\n", GetStreamName());
		}
	}

	if(!m_SucceededCreatePath)
		return false;

	std::wostringstream os;
	os << m_CapturePath << L"\\" << std::setfill(L'0') << std::setw(5) << frameNumber << std::setw(0) << (isBmpAndNotTga ? L".bmp" : L".tga");

	outPath = os.str();

	return true;
}

void CAfxStream::RecordEnd()
{

}

char const * CAfxStream::GetStreamName(void)
{
	return m_StreamName.c_str();
}

void CAfxStream::LevelShutdown(IAfxStreams4Stream * streams)
{
}

void CAfxStream::StreamAttach(IAfxStreams4Stream * streams)
{
	m_Streams = streams;
}

void CAfxStream::StreamDetach(IAfxStreams4Stream * streams)
{
	m_Streams = 0;
}

// CAfxDeveloperStream /////////////////////////////////////////////////////////

CAfxDeveloperStream::CAfxDeveloperStream(char const * streamName)
: CAfxStream(streamName)
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
	CAfxStream::StreamAttach(streams);

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
		streams->OnBind_set(this);
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
	streams->OnBind_set(0);

	CAfxStream::StreamDetach(streams);
}

void CAfxDeveloperStream::Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData)
{
	//Tier0_Msg("CAfxDeveloperStream::Bind\n");

	bool replace =
		m_Replace
		&& !strcmp(material->GetTextureGroupName(), m_MatchTextureGroupName.c_str())
		&& !strcmp(material->GetName(), m_MatchName.c_str())
	;

	m_ReplaceMaterialActive = replace;

	//if(replace)	Tier0_Msg("Replaced %s|%s with %s\n", material->GetTextureGroupName(), material->GetName(), m_ReplaceName.c_str());

	ctx->GetParent()->Bind(replace ? m_ReplaceMaterial->GetMaterial() : material, proxyData);
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

CAfxBaseFxStream::CAfxBaseFxStream(char const * streamName)
: CAfxStream(streamName)
, m_ClientEffectTexturesAction(HA_Draw)
, m_WorldTexturesAction(MA_Draw)
, m_SkyBoxTexturesAction(MA_Draw)
, m_StaticPropTexturesAction(MA_Draw)
, m_CableAction(HA_Draw)
, m_PlayerModelsAction(MA_Draw)
, m_WeaponModelsAction(MA_Draw)
, m_ShellModelsAction(MA_Draw)
, m_OtherModelsAction(MA_Draw)
, m_DecalTexturesAction(HA_Draw)
, m_EffectsAction(HA_Draw)
, m_ShellParticleAction(HA_Draw)
, m_OtherParticleAction(HA_Draw)
, m_StickerAction(MA_Draw)
, m_ErrorMaterialAction(HA_Draw)
, m_DepthVal(1)
, m_DepthValMax(1024)
, m_CurrentAction(0)
, m_DepthAction(0)
, m_MatteAction(0)
, m_PassthroughAction(0)
, m_InvisibleAction(0)
, m_NoDrawAction(0)
, m_BlackAction(0)
, m_WhiteAction(0)
, m_BoundAction(false)
, m_DebugPrint(false)
{
}

CAfxBaseFxStream::~CAfxBaseFxStream()
{
	delete m_NoDrawAction;
	delete m_InvisibleAction;
	delete m_MatteAction;
	delete m_DepthAction;
	delete m_PassthroughAction;
}

void CAfxBaseFxStream::LevelShutdown(IAfxStreams4Stream * streams)
{
	InvalidateCache();

	CAfxStream::LevelShutdown(streams);
}

void CAfxBaseFxStream::StreamAttach(IAfxStreams4Stream * streams)
{
	CAfxStream::StreamAttach(streams);

	if(!m_PassthroughAction) m_PassthroughAction = new CAction(this);
	if(!m_DepthAction) m_DepthAction = new CActionDepth(this, streams->GetFreeMaster(), streams->GetMaterialSystem());
	if(!m_MatteAction) m_MatteAction = new CActionMatte(this, streams->GetFreeMaster(), streams->GetMaterialSystem());
	if(!m_InvisibleAction) m_InvisibleAction = new CActionInvisible(this, streams->GetFreeMaster(), streams->GetMaterialSystem());
	if(!m_NoDrawAction) m_NoDrawAction = new CActionNoDraw(this);
	if(!m_BlackAction) m_BlackAction = new CActionBlack(this, streams->GetFreeMaster(), streams->GetMaterialSystem());
	if(!m_WhiteAction) m_WhiteAction = new CActionWhite(this, streams->GetFreeMaster(), streams->GetMaterialSystem());

	// Set a default action, just in case:
	m_CurrentAction = m_PassthroughAction;

	streams->OnBind_set(this);
	streams->OnDrawInstances_set(this);
	streams->OnDraw_set(this);
	streams->OnDraw_2_set(this);
	streams->OnDrawModulated_set(this);
}

void CAfxBaseFxStream::StreamDetach(IAfxStreams4Stream * streams)
{
	if(m_BoundAction)
	{
		m_CurrentAction->AfxUnbind(streams->GetCurrentContext());
		m_BoundAction = false;
	}

	streams->OnDrawModulated_set(0);
	streams->OnDraw_2_set(0);
	streams->OnDraw_set(0);
	streams->OnDrawInstances_set(0);
	streams->OnBind_set(0);

	CAfxStream::StreamDetach(streams);
}

void CAfxBaseFxStream::Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData )
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

		const char * groupName =  material->GetTextureGroupName();
		const char * name = material->GetName();
		const char * shaderName = material->GetShaderName();
		bool isErrorMaterial = material->IsErrorMaterial();

		if(m_DebugPrint) Tier0_Msg("Stream %s: Caching Material: %s|%s|%s%s -> ", GetStreamName(), groupName, name, shaderName, isErrorMaterial ? "|isErrorMaterial" : "");

		if(isErrorMaterial)
			m_CurrentAction = GetAction(m_ErrorMaterialAction);
		else
		if(!strcmp("ClientEffect textures", groupName))
			m_CurrentAction = GetAction(m_ClientEffectTexturesAction);
		else
		if(!strcmp("Decal textures", groupName))
			m_CurrentAction = GetAction(m_DecalTexturesAction);
		else
		if(!strcmp("World textures", groupName))
		{
			m_CurrentAction = GetAction(m_WorldTexturesAction);
		}
		else
		if(!strcmp("SkyBox textures", groupName))
			m_CurrentAction = GetAction(m_SkyBoxTexturesAction);
		else
		if(!strcmp("StaticProp textures", groupName))
		{
			if(StringBeginsWith(name, "models/weapons/"))
				m_CurrentAction = GetAction(m_WeaponModelsAction);
			else
				m_CurrentAction = GetAction(m_StaticPropTexturesAction);
		}
		else
		if(!strcmp("Model textures", groupName))
		{
			if(StringBeginsWith(name, "models/player/"))
				m_CurrentAction = GetAction(m_PlayerModelsAction);
			else
			if(StringBeginsWith(name, "models/weapons/"))
				m_CurrentAction = GetAction(m_WeaponModelsAction);
			else
			if(StringBeginsWith(name, "models/shells/"))
				m_CurrentAction = GetAction(m_ShellModelsAction);
			else
				m_CurrentAction = GetAction(m_OtherModelsAction);
		}
		else
		if(!strcmp("Other textures", groupName))
		{
			if(StringBeginsWith(name, "cable/"))
				m_CurrentAction = GetAction(m_CableAction);
			else
			if(StringBeginsWith(name, "cs_custom_material_"))
				m_CurrentAction = GetAction(m_WeaponModelsAction);
			else
			if(StringBeginsWith(name, "effects/"))
				m_CurrentAction = GetAction(m_EffectsAction);
			else
			if(StringBeginsWith(name, "particle/"))
			{
				if(StringBeginsWith(name, "particle/shells/"))
					m_CurrentAction = GetAction(m_ShellParticleAction);
				else
					m_CurrentAction = GetAction(m_OtherParticleAction);
			}
			else
			if(StringBeginsWith(name, "sticker_"))
					m_CurrentAction = GetAction(m_StickerAction);
			else
			{
				if(m_DebugPrint) Tier0_Msg("draw (unhandled)");
				m_CurrentAction = m_PassthroughAction;
			}
		}
		else
		if(!strcmp("Particle textures", groupName))
		{
			if(StringBeginsWith(name, "particle/shells/"))
				m_CurrentAction = GetAction(m_ShellParticleAction);
			else
				m_CurrentAction = GetAction(m_OtherParticleAction);
		}
		else
		if(!strcmp("Precached", groupName))
		{
			if(StringBeginsWith(name, "effects/"))
				m_CurrentAction = GetAction(m_EffectsAction);
			else
			if(StringBeginsWith(name, "particle/"))
			{
				if(StringBeginsWith(name, "particle/shells/"))
					m_CurrentAction = GetAction(m_ShellParticleAction);
				else
					m_CurrentAction = GetAction(m_OtherParticleAction);
			}
			else
			{
				if(m_DebugPrint) Tier0_Msg("draw (unhandled)");
				m_CurrentAction = m_PassthroughAction;
			}
		}
		else
		{
			if(m_DebugPrint) Tier0_Msg("draw (unhandled)");
			m_CurrentAction = m_PassthroughAction;
		}

		m_Map[key] = m_CurrentAction;

		if(m_DebugPrint) Tier0_Msg("\n");
	}

	m_CurrentAction->Bind(ctx, material, proxyData);

	m_BoundAction = true;
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

CAfxBaseFxStream::HideableAction CAfxBaseFxStream::ClientEffectTexturesAction_get(void)
{
	return m_ClientEffectTexturesAction;
}

void CAfxBaseFxStream::ClientEffectTexturesAction_set(HideableAction value)
{
	InvalidateCache();

	m_ClientEffectTexturesAction = value;
}

CAfxBaseFxStream::MaskableAction CAfxBaseFxStream::WorldTexturesAction_get(void)
{
	return m_WorldTexturesAction;
}

void CAfxBaseFxStream::WorldTexturesAction_set(MaskableAction value)
{
	InvalidateCache();
	m_WorldTexturesAction = value;
}

CAfxBaseFxStream::MaskableAction CAfxBaseFxStream::SkyBoxTexturesAction_get(void)
{
	return m_SkyBoxTexturesAction;
}

void CAfxBaseFxStream::SkyBoxTexturesAction_set(MaskableAction value)
{
	InvalidateCache();
	m_SkyBoxTexturesAction = value;
}

CAfxBaseFxStream::MaskableAction CAfxBaseFxStream::StaticPropTexturesAction_get(void)
{
	return m_StaticPropTexturesAction;
}

void CAfxBaseFxStream::StaticPropTexturesAction_set(MaskableAction value)
{
	InvalidateCache();
	m_StaticPropTexturesAction = value;
}

CAfxBaseFxStream::HideableAction CAfxBaseFxStream::CableAction_get(void)
{
	return m_CableAction;
}

void CAfxBaseFxStream::CableAction_set(HideableAction value)
{
	InvalidateCache();
	m_CableAction = value;
}

CAfxBaseFxStream::MaskableAction CAfxBaseFxStream::PlayerModelsAction_get(void)
{
	return m_PlayerModelsAction;
}

void CAfxBaseFxStream::PlayerModelsAction_set(MaskableAction value)
{
	InvalidateCache();
	m_PlayerModelsAction = value;
}

CAfxBaseFxStream::MaskableAction CAfxBaseFxStream::WeaponModelsAction_get(void)
{
	return m_WeaponModelsAction;
}

void CAfxBaseFxStream::WeaponModelsAction_set(MaskableAction value)
{
	InvalidateCache();
	m_WeaponModelsAction = value;
}

CAfxBaseFxStream::MaskableAction CAfxBaseFxStream::ShellModelsAction_get(void)
{
	return m_ShellModelsAction;
}

void CAfxBaseFxStream::ShellModelsAction_set(MaskableAction value)
{
	InvalidateCache();
	m_ShellModelsAction = value;
}

CAfxBaseFxStream::MaskableAction CAfxBaseFxStream::OtherModelsAction_get(void)
{
	return m_OtherModelsAction;
}

void CAfxBaseFxStream::OtherModelsAction_set(MaskableAction value)
{
	InvalidateCache();
	m_OtherModelsAction = value;
}

CAfxBaseFxStream::HideableAction CAfxBaseFxStream::DecalTexturesAction_get(void)
{
	return m_DecalTexturesAction;
}


void CAfxBaseFxStream::DecalTexturesAction_set(HideableAction value)
{
	InvalidateCache();
	m_DecalTexturesAction = value;
}

CAfxBaseFxStream::HideableAction CAfxBaseFxStream::EffectsAction_get(void)
{
	return m_EffectsAction;
}

void CAfxBaseFxStream::EffectsAction_set(HideableAction value)
{
	InvalidateCache();
	m_EffectsAction = value;
}

CAfxBaseFxStream::HideableAction CAfxBaseFxStream::ShellParticleAction_get(void)
{
	return m_ShellParticleAction;
}

void CAfxBaseFxStream::ShellParticleAction_set(HideableAction value)
{
	InvalidateCache();
	m_ShellParticleAction = value;
}

CAfxBaseFxStream::HideableAction CAfxBaseFxStream::OtherParticleAction_get(void)
{
	return m_OtherParticleAction;
}

void CAfxBaseFxStream::OtherParticleAction_set(HideableAction value)
{
	InvalidateCache();
	m_OtherParticleAction = value;
}

CAfxBaseFxStream::MaskableAction CAfxBaseFxStream::StickerAction_get(void)
{
	return m_StickerAction;
}

void CAfxBaseFxStream::StickerAction_set(MaskableAction value)
{
	InvalidateCache();
	m_StickerAction = value;
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

void CAfxBaseFxStream::InvalidateCache(void)
{
	if(m_DebugPrint) Tier0_Msg("Stream %s: Invalidating material cache.\n", GetStreamName());
	m_Map.clear();
}

CAfxBaseFxStream::CAction * CAfxBaseFxStream::GetAction(MaskableAction value)
{
	switch(value)
	{
	case MA_DrawDepth:
		if(m_DebugPrint) Tier0_Msg("drawDepth");
		return m_DepthAction;
	case MA_Mask:
		if(m_DebugPrint) Tier0_Msg("mask");
		return m_MatteAction;
	case MA_Invisible:
		if(m_DebugPrint) Tier0_Msg("invisible");
		return m_InvisibleAction;
	case MA_Black:
		if(m_DebugPrint) Tier0_Msg("black");
		return m_BlackAction;
	case MA_White:
		if(m_DebugPrint) Tier0_Msg("white");
		return m_WhiteAction;

	};

	if(m_DebugPrint) Tier0_Msg("draw");
	return m_PassthroughAction;
}

CAfxBaseFxStream::CAction * CAfxBaseFxStream::GetAction(HideableAction value)
{
	switch(value)
	{
	case HA_NoDraw:
		if(m_DebugPrint) Tier0_Msg("noDraw");
		return m_NoDrawAction;
	};

	if(m_DebugPrint) Tier0_Msg("draw");
	return m_PassthroughAction;
}

// CAfxBaseFxStream::CActionMatte //////////////////////////////////////////////

void CAfxBaseFxStream::CActionMatte::AfxUnbind(IAfxMatRenderContext * ctx)
{
	m_ParentStream->m_Streams->EndOverrideSetColorModulation();
}

void CAfxBaseFxStream::CActionMatte::Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData)
{
	ctx->GetParent()->Bind(m_MatteMaterial.GetMaterial(), proxyData);

	float color[3] = { 1.0f, 1.0f, 1.0f };
	m_ParentStream->m_Streams->OverrideSetColorModulation(color);
}

// CAfxBaseFxStream::CActionBlack //////////////////////////////////////////////


void CAfxBaseFxStream::CActionBlack::AfxUnbind(IAfxMatRenderContext * ctx)
{
	m_ParentStream->m_Streams->EndOverrideSetColorModulation();
}

void CAfxBaseFxStream::CActionBlack::Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData )
{
	ctx->GetParent()->Bind(m_Material.GetMaterial(), proxyData);

	float color[3] = { 0.0f, 0.0f, 0.0f };
	m_ParentStream->m_Streams->OverrideSetColorModulation(color);
}

// CAfxBaseFxStream::CActionWhite //////////////////////////////////////////////

void CAfxBaseFxStream::CActionWhite::AfxUnbind(IAfxMatRenderContext * ctx)
{
	m_ParentStream->m_Streams->EndOverrideSetColorModulation();
}

void CAfxBaseFxStream::CActionWhite::Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData )
{
	ctx->GetParent()->Bind(m_Material.GetMaterial(), proxyData);

	float color[3] = { 1.0f, 1.0f, 1.0f };
	m_ParentStream->m_Streams->OverrideSetColorModulation(color);
}


// CAfxBaseFxStream::CActionDepth //////////////////////////////////////////////

void CAfxBaseFxStream::CActionDepth::AfxUnbind(IAfxMatRenderContext * ctx)
{
}

void CAfxBaseFxStream::CActionDepth::Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData)
{
	// set-up debudepth material cvars accordingly:

	float scale = g_bIn_csgo_CSkyBoxView_Draw ? csgo_CSkyBoxView_GetScale() : 1.0f;
	float flDepthFactor = scale * m_ParentStream->m_DepthVal;
	float flDepthFactorMax = scale * m_ParentStream->m_DepthValMax;
	if ( flDepthFactor == 0 ) 
	{ 
		flDepthFactor = 1.0f; 
	} 

	m_DepthValRef.SetValueFastHack(flDepthFactor);
	//if(flDepthFactor != m_DepthValRef.GetFloat()) Tier0_Msg("CAfxBaseFxStream::CActionDepth::Bind: Error with m_DepthValRef: Expected %f got %f.\n", flDepthFactor, m_DepthValRef.GetFloat());
	m_DepthValMaxRef.SetValueFastHack(flDepthFactorMax);
	//if(flDepthFactorMax != m_DepthValMaxRef.GetFloat()) Tier0_Msg("CAfxBaseFxStream::CActionDepth::Bind: Error with m_DepthValMaxRef: Expected %f got %f.\n", flDepthFactorMax, m_DepthValMaxRef.GetFloat());

	// Bind our material:

	ctx->GetParent()->Bind(m_DepthMaterial.GetMaterial(), proxyData);
	
	// fix-up shader constants, because those are updated quite randomly
	// which would cause problems:

	//float vecZFactor[4] = { (flDepthFactorMax - flDepthFactor), flDepthFactor, 1 ,1};
	//AfxD3D9SetVertexShaderConstantF(48, vecZFactor, 1);

	// Force SRGBWriteEnable to off (Engine doesn't do this, otherwise it would be random):
	m_ParentStream->m_Streams->GetShaderShadow()->EnableSRGBWrite(false);
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
, m_MatQueueModeRef(0)
, m_MatPostProcessEnableRef(0)
, m_MatDynamicTonemappingRef(0)
, m_ColorModulationOverride(false)
, m_BlendOverride(false)
, m_TempBuffer(0)
, m_TempBufferBytesAllocated(0)
, m_FormatBmpAndNotTga(false)
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

	free(m_TempBuffer); m_TempBuffer = 0;
}


void CAfxStreams::OnMaterialSystem(IMaterialSystem_csgo * value)
{
	m_MaterialSystem = value;
}

void CAfxStreams::OnAfxVRenderView(IAfxVRenderView * value)
{
	m_VRenderView = value;

	if(m_VRenderView) m_VRenderView->OnSetBlend_set(this);
	if(m_VRenderView) m_VRenderView->OnSetColorModulation_set(this);
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

		for(std::list<CAfxStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
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

		for(std::list<CAfxStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
		{
			(*it)->RecordEnd();
		}

		m_FileTracker.WaitForFiles(0);
		RestoreMatVars();

		Tier0_Msg("done.\n");
	}

	m_Recording = false;
}

void CAfxStreams::Console_AddStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxStream(streamName));
}

void CAfxStreams::Console_AddBaseFxStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxBaseFxStream(streamName));
}

void CAfxStreams::Console_AddDeveloperStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxDeveloperStream(streamName));
}

void CAfxStreams::Console_AddDepthStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxDepthStream(streamName));
}

void CAfxStreams::Console_AddMatteWorldStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxMatteWorldStream(streamName));
}

void CAfxStreams::Console_AddDepthWorldStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxDepthWorldStream(streamName));
}

void CAfxStreams::Console_AddMatteEntityStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxMatteEntityStream(streamName));
}

void CAfxStreams::Console_AddDepthEntityStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxDepthEntityStream(streamName));
}

void CAfxStreams::Console_AddAlphaMatteStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxAlphaMatteStream(streamName));
}

void CAfxStreams::Console_AddAlphaEntityStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxAlphaEntityStream(streamName));
}

void CAfxStreams::Console_AddAlphaWorldStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	AddStream(new CAfxAlphaWorldStream(streamName));
}

void CAfxStreams::Console_PrintStreams()
{
	Tier0_Msg("index: name -> recorded?\n");
	int index = 0;
	for(std::list<CAfxStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
	{
		Tier0_Msg("%i: %s -> %s\n", index, (*it)->GetStreamName(), (*it)->Record_get() ? "RECORD ON (1)" : "record off (0)");
		++index;
	}
	Tier0_Msg(
		"=== Total streams: %i ===\n",
		index
	);
}

void CAfxStreams::Console_RemoveStream(const char * streamName)
{
	for(std::list<CAfxStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
	{
		if(!_stricmp(streamName, (*it)->GetStreamName()))
		{
			CAfxStream * cur = *it;

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

	for(std::list<CAfxStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
	{
		if(!_stricmp(streamName, (*it)->GetStreamName()))
		{
			CAfxStream * cur = *it;
			m_PreviewStream = cur;
			if(!m_Recording) BackUpMatVars();
			SetMatVarsForStreams();
			return;
		}
	}
	Tier0_Msg("Error: invalid streamName.\n");
}

#define CAFXBASEFXSTREAM_MASKABLEACTIONS "draw|drawDepth|mask|invisible|black|white"
#define CAFXBASEFXSTREAM_HIDEABLEACTIONS "draw|noDraw"

void CAfxStreams::Console_EditStream(const char * streamName, IWrpCommandArgs * args, int argcOffset, char const * cmdPrefix)
{
	for(std::list<CAfxStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
	{
		if(!_stricmp(streamName, (*it)->GetStreamName()))
		{
			CAfxStream * cur = *it;
			CAfxBaseFxStream * curBaseFx = cur->AsAfxBaseFxStream();
			CAfxDeveloperStream * curDeveloper = cur->AsAfxDeveloperStream();

			int argc = args->ArgC() -argcOffset;

			if(cur)
			{
				if(1 <= argc)
				{
					char const * cmd0 = args->ArgV(argcOffset +0);

					if(!_stricmp(cmd0, "record"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);

							cur->Record_set(atoi(cmd1) != 0 ? true : false);

							return;
						}

						Tier0_Msg(
							"%s record 0|1 - Whether to record this stream with mirv_streams record - 0 = record off, 1 = RECORD ON.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, cur->Record_get() ? "1" : "0"
						);
						return;
					}

					if(!_stricmp(cmd0, "drawHud"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);

							cur->DrawHud_set(atoi(cmd1) != 0 ? true : false);

							return;
						}

						Tier0_Msg(
							"%s drawHud 0|1 - Whether to draw HUD for this stream - 0 = don't draw, 1 = draw.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, cur->DrawHud_get() ? "1" : "0"
						);
						return;
					}

					if(!_stricmp(cmd0, "drawViewModel"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);

							cur->DrawViewModel_set(atoi(cmd1) != 0 ? true : false);

							return;
						}

						Tier0_Msg(
							"%s drawViewModel 0|1 - Whether to draw view model (in-eye weapon) for this stream - 0 = don't draw, 1 = draw.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, cur->DrawViewModel_get() ? "1" : "0"
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
							CAfxBaseFxStream::HideableAction value;

							if(Console_ToHideableAction(cmd1, value))
							{
								curBaseFx->ClientEffectTexturesAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s clientEffectTexturesAction " CAFXBASEFXSTREAM_HIDEABLEACTIONS " - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromHideableAction(curBaseFx->ClientEffectTexturesAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "worldTexturesAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxBaseFxStream::MaskableAction value;

							if(Console_ToMaskableAction(cmd1, value))
							{
								curBaseFx->WorldTexturesAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s worldTexturesAction " CAFXBASEFXSTREAM_MASKABLEACTIONS " - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromMaskableAction(curBaseFx->WorldTexturesAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "skyBoxTexturesAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxBaseFxStream::MaskableAction value;

							if(Console_ToMaskableAction(cmd1, value))
							{
								curBaseFx->SkyBoxTexturesAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s skyBoxTexturesAction " CAFXBASEFXSTREAM_MASKABLEACTIONS " - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromMaskableAction(curBaseFx->SkyBoxTexturesAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "staticPropTexturesAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxBaseFxStream::MaskableAction value;

							if(Console_ToMaskableAction(cmd1, value))
							{
								curBaseFx->StaticPropTexturesAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s staticPropTexturesAction " CAFXBASEFXSTREAM_MASKABLEACTIONS " - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromMaskableAction(curBaseFx->StaticPropTexturesAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "cableAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxBaseFxStream::HideableAction value;

							if(Console_ToHideableAction(cmd1, value))
							{
								curBaseFx->CableAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s cableAction " CAFXBASEFXSTREAM_HIDEABLEACTIONS " - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromHideableAction(curBaseFx->CableAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "playerModelsAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxBaseFxStream::MaskableAction value;

							if(Console_ToMaskableAction(cmd1, value))
							{
								curBaseFx->PlayerModelsAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s playerModelsAction " CAFXBASEFXSTREAM_MASKABLEACTIONS " - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromMaskableAction(curBaseFx->PlayerModelsAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "weaponModelsAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxBaseFxStream::MaskableAction value;

							if(Console_ToMaskableAction(cmd1, value))
							{
								curBaseFx->WeaponModelsAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s weaponModelsAction " CAFXBASEFXSTREAM_MASKABLEACTIONS " - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromMaskableAction(curBaseFx->WeaponModelsAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "shellModelsAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxBaseFxStream::MaskableAction value;

							if(Console_ToMaskableAction(cmd1, value))
							{
								curBaseFx->ShellModelsAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s shellModelsAction " CAFXBASEFXSTREAM_MASKABLEACTIONS " - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromMaskableAction(curBaseFx->ShellModelsAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "otherModelsAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxBaseFxStream::MaskableAction value;

							if(Console_ToMaskableAction(cmd1, value))
							{
								curBaseFx->OtherModelsAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s otherModelsAction " CAFXBASEFXSTREAM_MASKABLEACTIONS " - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromMaskableAction(curBaseFx->OtherModelsAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "decalTexturesAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxBaseFxStream::HideableAction value;

							if(Console_ToHideableAction(cmd1, value))
							{
								curBaseFx->DecalTexturesAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s decalTexturesAction " CAFXBASEFXSTREAM_HIDEABLEACTIONS " - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromHideableAction(curBaseFx->DecalTexturesAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "effectsAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxBaseFxStream::HideableAction value;

							if(Console_ToHideableAction(cmd1, value))
							{
								curBaseFx->EffectsAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s effectsAction " CAFXBASEFXSTREAM_HIDEABLEACTIONS " - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromHideableAction(curBaseFx->EffectsAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "shellParticleAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxBaseFxStream::HideableAction value;

							if(Console_ToHideableAction(cmd1, value))
							{
								curBaseFx->ShellParticleAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s shellParticleAction " CAFXBASEFXSTREAM_HIDEABLEACTIONS " - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromHideableAction(curBaseFx->ShellParticleAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "otherParticleAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxBaseFxStream::HideableAction value;

							if(Console_ToHideableAction(cmd1, value))
							{
								curBaseFx->OtherParticleAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s otherParticleAction " CAFXBASEFXSTREAM_HIDEABLEACTIONS " - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromHideableAction(curBaseFx->OtherParticleAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "stickerAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxBaseFxStream::MaskableAction value;

							if(Console_ToMaskableAction(cmd1, value))
							{
								curBaseFx->StickerAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s stickerAction " CAFXBASEFXSTREAM_MASKABLEACTIONS " - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromMaskableAction(curBaseFx->StickerAction_get())
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
					if(!_stricmp(cmd0, "invalidateCache"))
					{
						curBaseFx->InvalidateCache();
						return;
					}
				}
			}

			if(cur)
			{
				Tier0_Msg("%s record [...] - Controlls whether or not this stream is recorded with mirv_streams record.\n", cmdPrefix);
				Tier0_Msg("%s drawHud [...] - Controlls whether or not HUD is drawn for this stream.\n", cmdPrefix);
				Tier0_Msg("%s drawViewModel [...] - Controlls whether or not view model (in-eye weapon) is drawn for this stream.\n", cmdPrefix);
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
				Tier0_Msg("%s invalidateCache - invaldiates the material cache.\n", cmdPrefix);
			}

			Tier0_Msg("No further options for this stream.\n");
			return;
		}
	}
	Tier0_Msg("Error: invalid streamName.\n");
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

void CAfxStreams::OnBind_set(IAfxMatRenderContextBind * value)
{
	if(m_CurrentContext) m_CurrentContext->OnBind_set(value);
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

void CAfxStreams::LevelShutdown(IAfxBaseClientDll * cl)
{
	for(std::list<CAfxStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
	{
		(*it)->LevelShutdown(this);
	}
}

void CAfxStreams::View_Render(IAfxBaseClientDll * cl, IAfxMatRenderContext * cx, vrect_t_csgo *rect)
{
	m_CurrentContext = cx;

	bool canFeed = CheckCanFeedStreams();

	if(m_PreviewStream)
	{
		if(!canFeed)
			Tier0_Warning("Error: Cannot preview stream %s due to missing dependencies!\n", m_PreviewStream->GetStreamName());
		else
		{
			SetMatVarsForStreams(); // keep them set in case a mofo resets them.

			m_PreviewStream->StreamAttach(this);
		}
	}

	cl->GetParent()->View_Render(rect);

	if(m_PreviewStream && canFeed)
	{
		m_PreviewStream->StreamDetach(this);
	}

	if(m_Recording)
	{
		if(!canFeed)
		{
			Tier0_Warning("Error: Cannot record streams due to missing dependencies!\n");
		}
		else
		{
			for(std::list<CAfxStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
			{
				if(!(*it)->Record_get()) continue;

				//std::ostringstream oss;
	
				//oss << m_RecordName << "_"
				//	<< (*it)->GetStreamName() << "_"
				//	<< std::setfill('0') << std::setw(5) << m_Frame
				//	<< ".tga"
				//;

				//std::string filePath(g_VEngineClient->GetGameDirectory());
				//filePath.append("\\");
				//filePath.append(oss.str());

				//
				// Delete file if it already exists, so tracking (waiting for it) works:

				//DeleteFileA(filePath.c_str()); // Todo: Use wide character version maybe, otherwise I hope MAX_PATH will do.

				//
				// Record the stream to a file:

				m_MaterialSystem->SwapBuffers();

				SetMatVarsForStreams(); // keep them set in case a mofo resets them.

				(*it)->StreamAttach(this);

				//cl->GetParent()->WriteSaveGameScreenshotOfSize(oss.str().c_str(), rect->width, rect->height);

				IViewRender_csgo * view = GetView_csgo();

				const CViewSetup_csgo * viewSetup = view->GetViewSetup();

				int whatToDraw = RENDERVIEW_UNSPECIFIED;

				if((*it)->DrawHud_get()) whatToDraw |= RENDERVIEW_DRAWHUD;
				if((*it)->DrawViewModel_get()) whatToDraw |= RENDERVIEW_DRAWVIEWMODEL;

				view->RenderView(*viewSetup, *viewSetup, VIEW_CLEAR_STENCIL|VIEW_CLEAR_DEPTH|VIEW_CLEAR_COLOR, whatToDraw);

				int imagePitch = viewSetup->m_nUnscaledWidth * 3;
				size_t imageBytes = imagePitch * viewSetup->m_nUnscaledHeight;

				if( !m_TempBuffer || m_TempBufferBytesAllocated < imageBytes)
				{
					m_TempBuffer = realloc(m_TempBuffer, imageBytes);
					if(m_TempBuffer) m_TempBufferBytesAllocated = imageBytes;
				}

				if(m_TempBuffer)
				{
					cx->GetParent()->ReadPixels(
						viewSetup->m_nUnscaledX, viewSetup->m_nUnscaledY,
						viewSetup->m_nUnscaledWidth, viewSetup->m_nUnscaledHeight,
						(unsigned char*)m_TempBuffer,
						IMAGE_FORMAT_RGB888
					);

					// (back) transform to MDT native format:
					{
						int lastLine = viewSetup->m_nUnscaledHeight >> 1;
						if(viewSetup->m_nUnscaledHeight & 0x1) ++lastLine;

						for(int y=0;y<lastLine;++y)
						{
							int srcLine = y;
							int dstLine = viewSetup->m_nUnscaledHeight -1 -y;

							for(int x=0;x<viewSetup->m_nUnscaledWidth;++x)
							{
								unsigned char r = ((unsigned char *)m_TempBuffer)[dstLine*imagePitch +3*x +0];
								unsigned char g = ((unsigned char *)m_TempBuffer)[dstLine*imagePitch +3*x +1];
								unsigned char b = ((unsigned char *)m_TempBuffer)[dstLine*imagePitch +3*x +2];
								((unsigned char *)m_TempBuffer)[dstLine*imagePitch +3*x +0] = ((unsigned char *)m_TempBuffer)[srcLine*imagePitch +3*x +2];
								((unsigned char *)m_TempBuffer)[dstLine*imagePitch +3*x +1] = ((unsigned char *)m_TempBuffer)[srcLine*imagePitch +3*x +1];
								((unsigned char *)m_TempBuffer)[dstLine*imagePitch +3*x +2] = ((unsigned char *)m_TempBuffer)[srcLine*imagePitch +3*x +0];
								((unsigned char *)m_TempBuffer)[srcLine*imagePitch +3*x +0] = b;
								((unsigned char *)m_TempBuffer)[srcLine*imagePitch +3*x +1] = g;
								((unsigned char *)m_TempBuffer)[srcLine*imagePitch +3*x +2] = r;
							}
						}
					}

					// Write to disk:
					{
						std::wstring path;
						if((*it)->CreateCapturePath(m_Frame, m_FormatBmpAndNotTga, path))
						{
							bool success = m_FormatBmpAndNotTga
								? WriteRawBitmap((unsigned char*)m_TempBuffer, path.c_str(), viewSetup->m_nUnscaledWidth, viewSetup->m_nUnscaledHeight, 24, imagePitch)
								: WriteRawTarga((unsigned char*)m_TempBuffer, path.c_str(), viewSetup->m_nUnscaledWidth, viewSetup->m_nUnscaledHeight, 24, false, imagePitch)
							;
							if(!success)
							{
								Tier0_Warning("Failed writing image #%i for stream %s\n.", m_Frame, (*it)->GetStreamName());
							}
						}
					}
				}
				else
				{
					Tier0_Warning("CAfxStreams::View_Render: Failed to realoc m_TempBuffer.\n");
				}

				(*it)->StreamDetach(this);

				//
				// Make sure to wait for files to be written (and thus memory to be freed), otherwise we
				// will run out of memory on most systems:

				//m_FileTracker.TrackFile(filePath.c_str());

				m_FileTracker.WaitForFiles(2);
			}

		}

		++m_Frame;
	}

	m_CurrentContext = 0;
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
		for(std::list<CAfxStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
		{
			if(!_stricmp((*it)->GetStreamName(), value))
			{
				Tier0_Msg("Error: Stream name must be unique, \"%s\" is already in use by stream with index %i.\n", value, index);
				return false;
			}

			++index;
		}
	}

	return true;
}

bool CAfxStreams::Console_ToMaskableAction(char const * value, CAfxBaseFxStream::MaskableAction & maskableAction)
{
	if(!_stricmp(value, "draw"))
	{
		maskableAction = CAfxBaseFxStream::MA_Draw;
		return true;
	}
	else
	if(!_stricmp(value, "drawDepth"))
	{
		maskableAction = CAfxBaseFxStream::MA_DrawDepth;
		return true;
	}
	else
	if(!_stricmp(value, "mask"))
	{
		maskableAction = CAfxBaseFxStream::MA_Mask;
		return true;
	}
	else
	if(!_stricmp(value, "invisible"))
	{
		maskableAction = CAfxBaseFxStream::MA_Invisible;
		return true;
	}
	else
	if(!_stricmp(value, "black"))
	{
		maskableAction = CAfxBaseFxStream::MA_Black;
		return true;
	}
	else
	if(!_stricmp(value, "white"))
	{
		maskableAction = CAfxBaseFxStream::MA_White;
		return true;
	}

	return false;
}

bool CAfxStreams::Console_ToHideableAction(char const * value, CAfxBaseFxStream::HideableAction & hideableAction)
{
	if(!_stricmp(value, "draw"))
	{
		hideableAction = CAfxBaseFxStream::HA_Draw;
		return true;
	}
	else
	if(!_stricmp(value, "noDraw"))
	{
		hideableAction = CAfxBaseFxStream::HA_NoDraw;
		return true;
	}

	return false;
}

char const * CAfxStreams::Console_FromMaskableAction(CAfxBaseFxStream::MaskableAction maskableAction)
{
	switch(maskableAction)
	{
	case CAfxBaseFxStream::MA_Draw:
		return "draw";
	case CAfxBaseFxStream::MA_DrawDepth:
		return "drawDepth";
	case CAfxBaseFxStream::MA_Mask:
		return "mask";
	case CAfxBaseFxStream::MA_Invisible:
		return "invisible";
	case CAfxBaseFxStream::MA_Black:
		return "black";
	case CAfxBaseFxStream::MA_White:
		return "white";
	}

	return "[unknown]";
}

char const * CAfxStreams::Console_FromHideableAction(CAfxBaseFxStream::HideableAction hideableAction)
{
	switch(hideableAction)
	{
	case CAfxBaseFxStream::HA_Draw:
		return "draw";
	case CAfxBaseFxStream::HA_NoDraw:
		return "noDraw";
	}

	return "[unknown]";
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
}

void CAfxStreams::SetMatVarsForStreams()
{
	EnsureMatVars();

	m_MatQueueModeRef->SetValue(0.0f);
	m_MatPostProcessEnableRef->SetValue(0.0f);
	m_MatDynamicTonemappingRef->SetValue(0.0f);
}

void CAfxStreams::RestoreMatVars()
{
	EnsureMatVars();

	m_MatQueueModeRef->SetValue((float)m_OldMatQueueMode);
	m_MatPostProcessEnableRef->SetValue((float)m_OldMatPostProcessEnable);
	m_MatDynamicTonemappingRef->SetValue((float)m_OldMatDynamicTonemapping);
}

void CAfxStreams::EnsureMatVars()
{
	if(!m_MatQueueModeRef) m_MatQueueModeRef = new WrpConVarRef("mat_queue_mode");
	if(!m_MatPostProcessEnableRef) m_MatPostProcessEnableRef = new WrpConVarRef("mat_postprocess_enable");
	if(!m_MatDynamicTonemappingRef) m_MatDynamicTonemappingRef = new WrpConVarRef("mat_dynamic_tonemapping");
}

void CAfxStreams::AddStream(CAfxStream * stream)
{
	m_Streams.push_back(stream);

	if(m_Recording) stream->RecordStart();
}
