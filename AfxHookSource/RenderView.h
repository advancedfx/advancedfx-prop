#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2014-04-30 by dominik.matrixstorm.com
//
// First changes:
// 2009-11-01 by dominik.matrixstorm.com


#include "WrpGlobals.h"
#include <shared/CamPath.h>

class Hook_VClient_RenderView;

// global singelton instance:
extern Hook_VClient_RenderView g_Hook_VClient_RenderView;


// Hook_VClient_RenderView /////////////////////////////////////////////////////

class Hook_VClient_RenderView
{
public:
	CamPath m_CamPath;
	double LastCameraOrigin[3];
	double LastCameraAngles[3];
	double LastCameraFov;

	Hook_VClient_RenderView();
	~Hook_VClient_RenderView();

	bool ExportBegin(wchar_t const *fileName, double frameTime);
	void ExportEnd();

	void FovOverride(double value);
	void FovDefault();

	float GetCurTime();

	float GetImportBasteTime();

	bool ImportBegin(wchar_t const *fileName);
	void ImportEnd();

	void Install(WrpGlobals * globals);
	WrpGlobals * GetGlobals();

	bool IsInstalled(void);

	void OnViewOverride(float &Tx, float &Ty, float &Tz, float &Rx, float &Ry, float &Rz, float &Fov);

	void SetImportBaseTime(float value);

private:
	bool m_Export;
	bool m_FovOverride;
	double m_FovValue;
	WrpGlobals * m_Globals;
	bool m_Import;
	float m_ImportBaseTime;
	bool m_IsInstalled;

	void Install_cstrike(void);
	void Install_ep2(void);
};

