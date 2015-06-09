#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-06-08 dominik.matrixstorm.com
//
// First changes:
// 2015-06-08 dominik.matrixstorm.com

#include <windows.h>

class AfxHookSourceInput
{
public:
	enum KeyState
	{
		KS_DOWN,
		KS_UP
	};

	AfxHookSourceInput();

	bool GetCamResetView(void);
	double GetCamDForward(void);
	double GetCamDLeft(void);
	double GetCamDUp(void);
	double GetCamDPitch(void);
	double GetCamDYaw(void);
	double GetCamDRoll(void);
	double GetCamDFov(void);

	bool GetCameraControlMode(void);
	void SetCameraControlMode(bool enable);

	double GetKeyboardSensitivty(void);
	void SetKeyboardSensitivity(double value);

	double GetMouseSensitivty(void);
	void SetMouseSensitivity(double value);

	bool Supply_CharEvent(WPARAM wParam, LPARAM lParam);
	bool Supply_KeyEvent(KeyState keyState, WPARAM wParam, LPARAM lParam);
	bool Supply_RawMouseMotion(int dX, int dY);
	void Supply_GetCursorPos(LPPOINT lpPoint);
	void Supply_SetCursorPos(int x, int y);
	void Supply_MouseFrameEnd(void);

private:
	static const double m_CamSpeedFacMove;
	static const double m_CamSpeedFacRotate;
	static const double m_CamSpeedFacZoom;
	
	double m_FirstGetCursorPos;
	double m_MouseSens;
	double m_KeyboardSens;

	bool m_CamResetView;
	double m_CamForward;
	double m_CamForwardI;
	double m_CamLeft;
	double m_CamLeftI;
	double m_CamUp;
	double m_CamUpI;
	double m_CamFov;
	double m_CamFovI;
	double m_CamPitch;
	double m_CamPitchI;
	double m_CamPitchM;
	double m_CamYaw;
	double m_CamYawI;
	double m_CamYawM;
	double m_CamRoll;
	double m_CamRollI;
	double m_CamSpeed;
	bool m_CameraControlMode;
	bool m_IgnoreKeyUp;
	bool m_IgnoreNextKey;
	LONG m_LastCursorX;
	LONG m_LastCursorY;

	bool GetConsoleOpen(void);

	void DoCamSpeedDecrease(void);
	void DoCamSpeedIncrease(void);
};

extern AfxHookSourceInput g_AfxHookSourceInput;
