#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-06-26 dominik.matrixstorm.com
//
// First changes:
// 2015-06-26 dominik.matrixstorm.com

#include "AfxStreams.h"

#include "SourceInterfaces.h"

#include <shared/StringTools.h>

CAfxStreams g_AfxStreams;


// CAfxStreams /////////////////////////////////////////////////////////////////

CAfxStreams::CAfxStreams()
: m_RecordName("untitled")
, m_OnAfxBaseClientDll_Free(0)
{
}

CAfxStreams::~CAfxStreams()
{
	delete m_OnAfxBaseClientDll_Free;
}


void CAfxStreams::OnMaterialSystem(IMaterialSystem_csgo * value)
{
}

void CAfxStreams::OnAfxVRenderView(IAfxVRenderView * value)
{
}

void CAfxStreams::OnAfxBaseClientDll(IAfxBaseClientDll * value)
{
	if(m_OnAfxBaseClientDll_Free) delete m_OnAfxBaseClientDll_Free;
	m_OnAfxBaseClientDll_Free = new CFreeDelegate(value->GetFreeMaster(), this, &CAfxStreams::OnAfxBaseClientDll_Free);
}

void CAfxStreams::OnAfxBaseClientDll_Free(void)
{

}

void CAfxStreams::Console_RecordName_set(const char * value)
{
	if(StringIsEmpty(value))
	{
		Tier0_Msg("Error: String can not be emty.\n");
		return;
	}
	if(!StringIsAlNum(value))
	{
		Tier0_Msg("Error: String must be alphanumeric.\n");
		return;
	}

	m_RecordName.assign(value);
}

const char * CAfxStreams::Console_RecordName_get()
{
	return m_RecordName.c_str();
}


void CAfxStreams::Console_Record_Start()
{
	Tier0_Msg("Error: Not implemented yet.\n");
}

void CAfxStreams::Console_Record_End()
{
	Tier0_Msg("Error: Not implemented yet.\n");
}

void CAfxStreams::Console_AddMatteWorldStream(const char * streamName)
{
	Tier0_Msg("Error: Not implemented yet.\n");
}

void CAfxStreams::Console_AddMatteEntityStream(const char * streamName)
{
	Tier0_Msg("Error: Not implemented yet.\n");
}

void CAfxStreams::Console_PrintStreams()
{
	Tier0_Msg("Error: Not implemented yet.\n");
}

void CAfxStreams::Console_RemoveStream(int index)
{
	Tier0_Msg("Error: Not implemented yet.\n");
}
