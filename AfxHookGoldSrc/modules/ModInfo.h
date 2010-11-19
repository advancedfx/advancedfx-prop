#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-17 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#include <windows.h>
#include <gl\gl.h>


// ModInfo //////////////////////////////////////////////////////////////////

class ModInfo {
public:
	ModInfo(){
		m_In_R_DrawEntitiesOnList = false;
		m_In_R_DrawParticles = false;
		m_In_R_DrawViewModel = false;
	}

	void SetIn_R_DrawEntitiesOnList(bool value) { m_In_R_DrawEntitiesOnList = value; }
	void SetIn_R_DrawParticles(bool value) { m_In_R_DrawParticles = value; }
	void SetIn_R_DrawViewModel(bool value) { m_In_R_DrawViewModel = value; }

	//
	// Properties:
	bool In_R_DrawEntitiesOnList_get() { return m_In_R_DrawEntitiesOnList; }
	bool In_R_DrawParticles_get() { return m_In_R_DrawParticles; }
	bool In_R_DrawViewModel_get() { return m_In_R_DrawViewModel; }

private:
	bool m_In_R_DrawEntitiesOnList;
	bool m_In_R_DrawParticles;
	bool m_In_R_DrawViewModel;
};

extern ModInfo g_ModInfo;

