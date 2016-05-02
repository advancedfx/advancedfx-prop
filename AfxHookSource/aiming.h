#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-04-15 dominik.matrixstorm.com
//
// First changes:
// 2016-03-31 dominik.matrixstorm.com

#include <shared/AfxMath.h>

using namespace Afx::Math;

class Aiming
{
public:
	Aiming();

	bool Aim(double deltaT, Vector3 const camOrigin, double & yPitch, double & zYaw, double & xRoll);

	void TargetPoint(Vector3 origin);
	void TargetPointFromLast(Vector3 offset);

	bool Active;
	bool SoftDeactivate;

	int EntityIndex;

	Vector3 OffSet;

	bool SnapTo;
	double LimitVelocity;
	double LimitAcceleration;
	//double LimitJerk;

	enum Origin_e {
		O_Net,
		O_View
	} Origin;

	enum Angles_e {
		A_Net,
		A_View
	} Angles;

	enum Up_e {
		U_Input,
		U_World
	} Up;

private:
	Vector3 LastTargetOrigin;
	double m_YPitchVelocity;
	double m_ZYawVelocity;
	double m_XRollVelocity;
	double LastYPitch;
	double LastZYaw;
	double LastXRoll;
	bool m_Deactivating;

	void CalcSmooth(double deltaT, double targetPos, double & lastPos, double & lastVel);
};

extern Aiming g_Aiming;
