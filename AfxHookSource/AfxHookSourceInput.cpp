#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-06-08 dominik.matrixstorm.com
//
// First changes:
// 2015-06-08 dominik.matrixstorm.com

#include "AfxHookSourceInput.h"

#include "WrpVEngineClient.h"


extern WrpVEngineClient * g_VEngineClient;

AfxHookSourceInput g_AfxHookSourceInput;

const double AfxHookSourceInput::m_CamSpeedFacMove = 320.0;
const double AfxHookSourceInput::m_CamSpeedFacRotate = 180.0;
const double AfxHookSourceInput::m_CamSpeedFacZoom = 10.0;

AfxHookSourceInput::AfxHookSourceInput()
: m_CameraControlMode(false)
, m_CamResetView(false)
, m_CamForward(0.0)
, m_CamForwardI(0.0)
, m_CamLeft(0.0)
, m_CamLeftI(0.0)
, m_CamUp(0.0)
, m_CamUpI(0.0)
, m_CamFov(0.0)
, m_CamFovI(0.0)
, m_CamPitch(0.0)
, m_CamPitchI(0.0)
, m_CamPitchM(0.0)
, m_CamYaw(0.0)
, m_CamYawI(0.0)
, m_CamYawM(0.0)
, m_CamRoll(0.0)
, m_CamRollI(0.0)
, m_CamSpeed(1.0)
, m_IgnoreKeyUp(false)
, m_IgnoreNextKey(false)
, m_MouseSens(1.0/40)
, m_KeyboardSens(1.0)
, m_FirstGetCursorPos(true)
, m_LastCursorX(0)
, m_LastCursorY(0)
{
}

void AfxHookSourceInput::DoCamSpeedDecrease(void)
{
	m_CamSpeed = m_CamSpeed / 2;
	if(m_CamSpeed < 1.0/256) m_CamSpeed = 1.0/256;
}

void AfxHookSourceInput::DoCamSpeedIncrease(void)
{
	m_CamSpeed = m_CamSpeed * 2;
	if(m_CamSpeed > 256) m_CamSpeed = 256;
}

bool AfxHookSourceInput::GetCamResetView(void)
{
	bool result = m_CamResetView;
	m_CamResetView = false;
	return result;
}

double AfxHookSourceInput::GetCamDForward(void)
{
	return m_CamSpeed * m_CamSpeedFacMove * (m_CamForward -m_CamForwardI);
}

double AfxHookSourceInput::GetCamDLeft(void)
{
	return m_CamSpeed * m_CamSpeedFacMove * (m_CamLeft -m_CamLeftI);
}

double AfxHookSourceInput::GetCamDUp(void)
{
	return m_CamSpeed * m_CamSpeedFacMove * (m_CamUp -m_CamUpI);
}

double AfxHookSourceInput::GetCamDPitch(void)
{
	return m_CamSpeed * m_CamSpeedFacRotate * (m_CamPitch -m_CamPitchI +m_CamPitchM);
}

double AfxHookSourceInput::GetCamDYaw(void)
{
	return m_CamSpeed * m_CamSpeedFacRotate * (m_CamYaw -m_CamYawI +m_CamYawM);
}

double AfxHookSourceInput::GetCamDRoll(void)
{
	return m_CamSpeed * m_CamSpeedFacRotate * (m_CamRoll -m_CamRollI);
}

double AfxHookSourceInput::GetCamDFov(void)
{
	return m_CamSpeed * m_CamSpeedFacZoom * (m_CamFov -m_CamFovI);
}

bool AfxHookSourceInput::GetCameraControlMode(void)
{
	return m_CameraControlMode;
}

void AfxHookSourceInput::SetCameraControlMode(bool enable)
{
	m_CameraControlMode = enable;
}

double AfxHookSourceInput::GetKeyboardSensitivty(void)
{
	return m_KeyboardSens;
}

void AfxHookSourceInput::SetKeyboardSensitivity(double value)
{
	m_KeyboardSens = value;
}

double AfxHookSourceInput::GetMouseSensitivty(void)
{
	return m_MouseSens;
}

void AfxHookSourceInput::SetMouseSensitivity(double value)
{
	m_MouseSens = value;
}

bool AfxHookSourceInput::Supply_CharEvent(WPARAM wParam, LPARAM lParam)
{
	if(GetConsoleOpen())
		return false;

	if(m_IgnoreNextKey)
	{
		return false;
	}

	if(m_CameraControlMode)
	{
		switch(wParam)
		{
		case '+':
			{
				for(int i=0; i<=(lParam&0xFFFF); i++) DoCamSpeedIncrease();
			}
			return true;
		case '-':
			{
				for(int i=0; i<=(lParam&0xFFFF); i++) DoCamSpeedDecrease();
			}
			return true;
		}
		return true;
	}

	return false;
}

bool AfxHookSourceInput::Supply_KeyEvent(KeyState keyState, WPARAM wParam, LPARAM lParam)
{
	if(GetConsoleOpen())
	{
		m_IgnoreKeyUp = KS_DOWN == keyState;
		return false;
	}

	if(m_IgnoreKeyUp && KS_UP == keyState)
	{
		m_IgnoreKeyUp = false;
		return false;
	}

	if(m_IgnoreNextKey)
	{
		if(KS_UP == keyState)
			m_IgnoreNextKey = false;

		return false;
	}

	if(m_CameraControlMode)
	{
		switch(wParam)
		{
		case VK_ESCAPE:
			if(KS_UP == keyState) m_CameraControlMode = false;
			return true;
		case VK_CONTROL:
			if(KS_UP == keyState) m_IgnoreNextKey = true;
			return true;
		case VK_HOME:
		case VK_NUMPAD5:
			if(KS_DOWN == keyState)
			{
				m_CamResetView = true;
				m_CamSpeed = 1.0;
			}
			return true;
		case 0x57: // W key
		case VK_NUMPAD8:
			m_CamForward = KS_DOWN == keyState ? m_KeyboardSens : 0.0;
			return true;
		case 0x53: // S key
		case VK_NUMPAD2:
			m_CamForwardI = KS_DOWN == keyState ? m_KeyboardSens : 0.0;
			return true;
		case 0x41: // A key
		case VK_NUMPAD4:
			m_CamLeft = KS_DOWN == keyState ? m_KeyboardSens : 0.0;
			return true;
		case 0x44: // D key
		case VK_NUMPAD6:
			m_CamLeftI = KS_DOWN == keyState ? m_KeyboardSens : 0.0;
			return true;
		case 0x52: // R key
		case VK_NUMPAD9:
			m_CamUp = KS_DOWN == keyState ? m_KeyboardSens : 0.0;
			return true;
		case 0x46: // F key
		case VK_NUMPAD3:
			m_CamUpI = KS_DOWN == keyState ? m_KeyboardSens : 0.0;
			return true;
		case VK_NUMPAD1:
		case VK_NEXT:
			m_CamFov = KS_DOWN == keyState ? m_KeyboardSens : 0.0;
			return true;
		case VK_NUMPAD7:
		case VK_PRIOR:
			m_CamFovI = KS_DOWN == keyState ? m_KeyboardSens : 0.0;
			return true;
		case 0x58: // X key
		case VK_DECIMAL:
			m_CamRoll = KS_DOWN == keyState ? m_KeyboardSens : 0.0;
			return true;
		case 0x5A: // Z key
		case VK_NUMPAD0:
			m_CamRollI = KS_DOWN == keyState ? m_KeyboardSens : 0.0;
			return true;
		case VK_DOWN:
			m_CamPitch = KS_DOWN == keyState ? m_KeyboardSens : 0.0;
			return true;
		case VK_UP:
			m_CamPitchI = KS_DOWN == keyState ? m_KeyboardSens : 0.0;
			return true;
		case VK_LEFT:
			m_CamYaw = KS_DOWN == keyState ? m_KeyboardSens : 0.0;
			return true;
		case VK_RIGHT:
			m_CamYawI = KS_DOWN == keyState ? m_KeyboardSens : 0.0;
			return true;
		}
	}

	return false;
}

bool AfxHookSourceInput::Supply_RawMouseMotion(int dX, int dY)
{
	if(GetConsoleOpen())
		return false;

	if(m_CameraControlMode)
	{
		m_CamYawM += -m_MouseSens * dX;
		m_CamPitchM += m_MouseSens * dY;

		return true;
	}

	return false;
}

void AfxHookSourceInput::Supply_GetCursorPos(LPPOINT lpPoint)
{
	if(!lpPoint)
		return;

	if(m_CameraControlMode)
	{
		if(m_FirstGetCursorPos)
		{
			m_FirstGetCursorPos = false;

			// Clear anything from raw input, since we override it with CursorPos method:
			m_CamYawM = 0.0;
			m_CamPitchM = 0.0;
		}

		// this will not work correctly if there was no SetCursorPos call prior to this call:
		LONG dX = lpPoint->x -m_LastCursorX;
		LONG dY = lpPoint->y -m_LastCursorY;

		m_CamYawM += -m_MouseSens * dX;
		m_CamPitchM += m_MouseSens * dY;

		// pretend we didn't move from last SetCursorPos call:
		lpPoint->x = m_LastCursorX;
		lpPoint->y = m_LastCursorY;
	}
}

void AfxHookSourceInput::Supply_SetCursorPos(int x, int y)
{
	m_LastCursorX = x;
	m_LastCursorY = y;
}

void AfxHookSourceInput::Supply_MouseFrameEnd(void)
{
	if(GetConsoleOpen())
		return;

	if(m_CameraControlMode)
	{
		if(m_FirstGetCursorPos)
		{
			// TODO: HACK: In POV-Demos only rawinput is available, because
			// they don't call SetCursorPos / GetCursorPos to read player mouse
			// input. We make room to move here (so the mouse won't leave
			// the window).
			SetCursorPos(m_LastCursorX, m_LastCursorY);
		}

		m_CamYawM = 0.0;
		m_CamPitchM = 0.0;
		m_FirstGetCursorPos = true;
	}
}


bool AfxHookSourceInput::GetConsoleOpen(void)
{
	return
		g_VEngineClient
		&& g_VEngineClient->Con_IsVisible()
	;
}
