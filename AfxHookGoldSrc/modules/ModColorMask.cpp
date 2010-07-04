#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-06-29 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#include "ModColorMask.h"

ModColorMask g_ModColorMask;

// ModColorMask /////////////////////////////////////////////////////////////////

ModColorMask::ModColorMask()
{
	m_Active = false;
	m_Enabled = false;
}


void ModColorMask::OnGlBegin(GLenum mode)
{
	m_Active = m_Enabled;
	
	if(!m_Active) return;

	m_ColorMask.OnGlBegin(mode);
}

void ModColorMask::OnGlEnd()
{
	if(!m_Active) return;
	m_Active = false;

	m_ColorMask.OnGlEnd();
}


