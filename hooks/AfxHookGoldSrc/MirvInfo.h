#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-16 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#include <windows.h>
#include <gl\gl.h>


// MirvInfo //////////////////////////////////////////////////////////////////

class MirvInfo {
public:
	MirvInfo(){
		m_In_R_DrawEntitiesOnList = false;
		m_In_R_DrawParticles = false;
		m_In_R_DrawViewModel = false;
		m_In_R_RenderView = false;
	}

	void SetIn_R_DrawEntitiesOnList(bool value) { m_In_R_DrawEntitiesOnList = value; }
	void SetIn_R_DrawParticles(bool value) { m_In_R_DrawParticles = value; }
	void SetIn_R_DrawViewModel(bool value) { m_In_R_DrawViewModel = value; }
	void SetIn_R_Renderview(bool value) { m_In_R_RenderView = value; }

	//
	// Properties:
	bool In_R_DrawEntitiesOnList_get() { return m_In_R_DrawEntitiesOnList; }
	bool In_R_DrawParticles_get() { return m_In_R_DrawParticles; }
	bool In_R_DrawViewModel_get() { return m_In_R_DrawViewModel; }
	bool In_R_Renderview_get() { return m_In_R_RenderView; }

private:
	bool m_In_R_DrawEntitiesOnList;
	bool m_In_R_DrawParticles;
	bool m_In_R_DrawViewModel;
	bool m_In_R_RenderView;
};

extern MirvInfo g_MirvInfo;

