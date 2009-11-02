#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2009-11-01 by dominik.matrixstorm.com
//
// First changes:
// 2009-11-01 by dominik.matrixstorm.com


#include "SourceInterfaces.h"


class Hook_VClient_RenderView;


// global singelton instance:
extern Hook_VClient_RenderView g_Hook_VClient_RenderView;


// Hook_VClient_RenderView /////////////////////////////////////////////////////

class Hook_VClient_RenderView
{
public:
	Hook_VClient_RenderView();
	~Hook_VClient_RenderView();

	bool ExportBegin(char const *fileName, float frameTime);
	void ExportEnd();

	float GetCurTime();

	float GetImportBasteTime();

	bool ImportBegin(char const *fileName);
	void ImportEnd();

	void Install_cstrike(void);

	bool IsInstalled(void);

	void OnViewOverride(float &Tx, float &Ty, float &Tz, float &Rx, float &Ry, float &Rz);

	void SetImportBaseTime(float value);

private:
	bool m_Export;
	float m_FrameTime;
	bool m_Import;
	float m_ImportBaseTime;
	bool m_IsInstalled;
	float m_LastTime;
	float m_SubTime;
};

