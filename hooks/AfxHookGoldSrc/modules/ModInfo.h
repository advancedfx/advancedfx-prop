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


// ModInfo //////////////////////////////////////////////////////////////////

class ModInfo {
public:
	ModInfo(){
		m_In_R_DrawEntitiesOnList = false;
		m_In_R_DrawParticles = false;
		m_In_R_DrawViewModel = false;
		m_In_R_RenderView = false;
		m_Recording = false;
	}

	void SetIn_R_DrawEntitiesOnList(bool value) { m_In_R_DrawEntitiesOnList = value; }
	void SetIn_R_DrawParticles(bool value) { m_In_R_DrawParticles = value; }
	void SetIn_R_DrawViewModel(bool value) { m_In_R_DrawViewModel = value; }
	void SetIn_R_Renderview(bool value) { m_In_R_RenderView = value; }
	void SetRecording(bool value) { m_Recording = value; }

	int GetCurrentEntityIndex();

	//
	// Properties:
	bool In_R_DrawEntitiesOnList_get() { return m_In_R_DrawEntitiesOnList; }
	bool In_R_DrawParticles_get() { return m_In_R_DrawParticles; }
	bool In_R_DrawViewModel_get() { return m_In_R_DrawViewModel; }
	bool In_R_Renderview_get() { return m_In_R_RenderView; }
	bool Recording_get() { return m_Recording; }

private:
	bool m_In_R_DrawEntitiesOnList;
	bool m_In_R_DrawParticles;
	bool m_In_R_DrawViewModel;
	bool m_In_R_RenderView;
	bool m_Recording;
};

extern ModInfo g_ModInfo;

