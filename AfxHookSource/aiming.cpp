#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-04-15 dominik.matrixstorm.com
//
// First changes:
// 2016-03-31 dominik.matrixstorm.com

#include "aiming.h"

#include "SourceInterfaces.h"

#define _USE_MATH_DEFINES
#include <math.h>

Aiming g_Aiming;

void LookAnglesFromTo(Vector3 const &from, Vector3 const &to, double & outPitch, double & outYaw)
{
	Vector3 dir = to - from;

	// Store then zero height
	double dz = dir.Z;
	
	dir.Z = 0;

	// Need this for later
	double length = dir.Length();

	dir = dir.Normalize();

	// This is our forward angle
	Vector3 vForward(1.0, 0.0, 0.0);

	double dot_product = (dir.X * vForward.X) + (dir.Y * vForward.Y) + (dir.Z * vForward.Z);

	double angle = acos(dot_product) * 180.0f / (float)M_PI;

	if (dir.Y < 0)
		angle = 360.0 - angle;

	// This is our pitchup/down
	if (length == 0)
		length = 0.01;

	double pitch = atan(dz / length) * 180.0f / (float)M_PI;

	outYaw = angle;
	outPitch = -pitch;
}

Aiming::Aiming()
: Active(false)
, SoftDeactivate(true)
, EntityIndex(-1)
, OffSet(0.0,0.0,0.0)
, SnapTo(false)
, LimitVelocity(180)
, LimitAcceleration(1)
, LimitJerk(0.01)
, Origin(O_View)
, Angles(A_Net)
, Up(U_World)
, m_YPitchVelocity(0)
, m_YPitchAcceleration(0)
, m_ZYawVelocity(0)
, m_ZYawAcceleration(0)
, m_XRollVelocity(0)
, m_XRollAcceleration(0)
, LastTargetOrigin(0, 0, 0)
, LastYPitch(0)
, LastZYaw(0)
, LastXRoll(0)
, m_Deactivating(false)
{

}

bool Aiming::Aim(double deltaT, Vector3 const camOrigin, double & yPitch, double & zYaw, double & xRoll)
{
	if(deltaT <= 0)
	{
		return false;
	}

	bool camRotationChanged = false;

	m_Deactivating = Active || m_Deactivating && SoftDeactivate;

	double targetYPitch = yPitch;
	double targetZYaw = zYaw;
	double targetXRoll = xRoll;

	if(Active)
	{
		IClientEntity_csgo * ce = g_Entitylist_csgo->GetClientEntity(EntityIndex);
		C_BaseEntity_csgo * be = ce ? ce->GetBaseEntity() : 0;

		if(ce)
		{
			Vector o =  be && O_View == Origin ? be->EyePosition() : ce->GetAbsOrigin();
			QAngle a =  be && A_View == Angles ? be->EyeAngles() : ce->GetAbsAngles();

			double forward[3], right[3], up[3];

			MakeVectors(a.z, a.x, a.y, forward, right, up);

			LastTargetOrigin = Vector3(
				o.x +OffSet.X * forward[0] + OffSet.Y * right[0] +OffSet.Z * up[0],
				o.y +OffSet.X * forward[1] + OffSet.Y * right[1] +OffSet.Z * up[1],
				o.z +OffSet.X * forward[2] + OffSet.Y * right[2] +OffSet.Z * up[2]
			);

		}
	}
	else
	{
		LastTargetOrigin = camOrigin;
	}

	if(Active || m_Deactivating)
	{
		if(Active)
		{
			LookAnglesFromTo(camOrigin, LastTargetOrigin, targetYPitch, targetZYaw);
			targetXRoll = U_World == Up ? 0 : targetXRoll;
		}

		if(SnapTo)
		{
			yPitch = targetYPitch;
			zYaw = targetZYaw;
			xRoll = targetXRoll;
			camRotationChanged = true;
			m_Deactivating = false;
		}
		else
		{
			double reaimYPitch = targetYPitch -LastYPitch;
			double reaimZYaw = targetZYaw -LastZYaw;
			double reaimXRoll = targetXRoll -LastXRoll;

			// Force reaim angles to be in [-180°, 180°)

			reaimYPitch = fmod(reaimYPitch + 180.0, 360.0) -180.0;
			reaimZYaw = fmod(reaimZYaw + 180.0, 360.0) -180.0;
			reaimXRoll = fmod(reaimXRoll + 180.0, 360.0) -180.0;

			m_Deactivating = Active || abs(reaimYPitch) > AFX_MATH_EPS || abs(reaimZYaw) > AFX_MATH_EPS || abs(reaimXRoll) > AFX_MATH_EPS;

			// apply re-aiming:

			CalcSmooth(deltaT, LastYPitch +reaimYPitch, LastYPitch, m_YPitchVelocity, m_YPitchAcceleration);
			yPitch = LastYPitch;

			CalcSmooth(deltaT, LastZYaw +reaimZYaw, LastZYaw, m_ZYawVelocity, m_ZYawAcceleration);
			zYaw = LastZYaw;

			CalcSmooth(deltaT, LastXRoll +reaimXRoll, LastXRoll, m_XRollVelocity, m_XRollAcceleration);
			xRoll = LastXRoll;

			camRotationChanged = true;
		}
	}

	// Force remembered angels to be in [-180°, 180°)

	LastYPitch = fmod(yPitch +180.0, 360.0) -180.0;
	LastZYaw = fmod(zYaw +180.0, 360.0) -180.0;
	LastXRoll = fmod(xRoll +180.0, 360.0) -180.0;

	return camRotationChanged;
}

void Aiming::TargetPoint(Vector3 origin)
{
	EntityIndex = -1;
	LastTargetOrigin = origin;
}

void Aiming::TargetPointFromLast(Vector3 offset)
{
	TargetPoint(LastTargetOrigin +offset);
}

void Aiming::CalcSmooth(double deltaT, double targetPos, double & lastPos, double & lastVel, double & lastAccel)
{
	if(deltaT <= 0)
		return;

	lastPos = targetPos;
	lastVel = 0;
	lastAccel = 0;
	return;

	// not implemented yet!
	// .

	// Objective:
	//
	// For the _normal_ case
	// we build a third order motion profile
	// (sort of sinusoidal):
	//
	// phaseT_k is the (positive) phase time.
	//
	// jerk(k) = c_k
	// accel(k) = accel(k-1) +Int(jerk(k),0+eps..phaseT_k) = accel(k-1) +jerk(k)*phaseT_k
	// vel(k) = velo(k-1) +Int(accell(k),0+eps..phaseT_k) = velo(k-1) +accel(k-1) * phaseT_k +jerk(k)/2 * phaseT_k^2
	// pos(k) = pos(k-1) +Int(vel(k),0+eps..phaseT_k) = pos(k-1) +vel(k-1)*phaseT_k +accel(k-1)/2*phaseT_k^2 +jerk(k)/6*phaseT_k^3
	//
	// -LimitJerk <= jerk(k) <= LimitJerk
	// -LimitAccel <= accel(k) <= LimitAccel
	// -LimitVelocity <= vel(k) <= LimitVelocity
	//
	// Possible phases:
	// P1) acceleration (decelleration) build-up
	// P2) limit acceleration (decelleration)
	// P3) acceleration (decelleration) ramp-down
	// P4) limit speed
	// P5) deceleration (accelleration) build-up
	// P6) limit deceleration (accelleration)
	// P7) deceleration (accelleration) ramp-down
	//

	// Complete programming problem:
	//
	//
	// Input:
	//
	// LimitVelocity - absolute velocity limit, where 0 < LimitVelocity
	// LimitAcceleration - absolute acceleration limit, where 0 < LimitAcceleration
	// LimitJerk - absolute jerk limit, where 0 < LimitJerk
	// targetPos - target position
	// lastPos - last position
	// lastVel - last velocity, where -LimitVelocity <= lastVel <= LimitVelocity
	// lastAccel - last acceleration, where -LimitAcceleration <= lastAccel <= LimitAcceleration
	//
	//
	// Output:
	//
	// phase1T, phase2T, phase3T, phase4T, phase5T, phase5T, phase5T, phase6T - Phase times
	// , where 0 <= phase1T, 0 <= phase2T, 0 <= phase3T, 0 <= phase4T, 0 <= phase5T, 0 <= phase6T, 0 <= phase7T
	//
	// dir - base jerk direction, where dir \in {-1, 1}
	//
	//
	// Further Equations:
	//
	// jerk(1) = +dir * LimitJerk
	// jerk(2) = 0
	// jerk(3) = -dir * LimitJerk
	// jerk(4) = 0
	// jerk(5) = -dir * LimitJerk
	// jerk(6) = 0
	// jerk(7) = +dir * LimitJerk
	//
	// accel(0) = lastAccel
	// accel(1) = accel(0) +jerk(1) * phase1T
	// accel(2) = accel(1) +jerk(2) * phase2T
	// accel(3) = accel(2) +jerk(3) * phase3T
	// accel(4) = accel(3) +jerk(4) * phase4T
	// accel(5) = accel(4) +jerk(5) * phase5T
	// accel(6) = accel(5) +jerk(6) * phase6T
	// accel(7) = accel(6) +jerk(7) * phase7T
	// accel(7) = 0
	//
	// -LimitAccel <= accel(1), accel(2), accel(3), accel(4), accel(5), accel(6), accel(7) <= LimitAccel
	//
	// vel(0) = lastVel
	// vel(1) = vel(0) +accel(0) * phase1T +jerk(1)/2 * phase1T^2
	// vel(2) = vel(1) +accel(1) * phase2T +jerk(2)/2 * phase2T^2
	// vel(3) = vel(2) +accel(2) * phase3T +jerk(3)/2 * phase3T^2
	// vel(4) = vel(3) +accel(3) * phase4T +jerk(4)/2 * phase4T^2
	// vel(5) = vel(4) +accel(4) * phase5T +jerk(5)/2 * phase5T^2
	// vel(6) = vel(5) +accel(5) * phase6T +jerk(6)/2 * phase6T^2
	// vel(7) = vel(6) +accel(6) * phase7T +jerk(7)/2 * phase7T^2
	// vel(7) = 0
	//
	// -LimitVelocity <= vel(1), vel(2), vel(3), vel(4), vel(5), vel(6), vel(7) <= LimitVelocity
	// 
	// resultDeltaPos = 0
	// +Int(vel(1),0+eps,phase1T)
	// +Int(vel(2),0+eps,phase2T)
	// +Int(vel(3),0+eps,phase3T)
	// +Int(vel(4),0+eps,phase4T)
	// +Int(vel(5),0+eps,phase5T)
	// +Int(vel(6),0+eps,phase6T)
	// +Int(vel(7),0+eps,phase7T)
	// = 0
	// +vel(0) * phase1T +accel(0)/2 * phase1T^2 +jerk(1)/6 * phase1T^3
	// +vel(1) * phase2T +accel(1)/2 * phase2T^2 +jerk(2)/6 * phase2T^3
	// +vel(2) * phase3T +accel(2)/2 * phase3T^2 +jerk(3)/6 * phase3T^3
	// +vel(3) * phase4T +accel(3)/2 * phase4T^2 +jerk(4)/6 * phase4T^3
	// +vel(4) * phase5T +accel(4)/2 * phase5T^2 +jerk(5)/6 * phase5T^3
	// +vel(5) * phase6T +accel(5)/2 * phase6T^2 +jerk(6)/6 * phase6T^3
	// +vel(6) * phase7T +accel(6)/2 * phase7T^2 +jerk(7)/6 * phase7T^3
	//
	//
	// deltaPos = targetPos -lastPos
	//
	// Minimize:
	//
	// 1) Position error: abs(deltaPos -resultDeltaPos)
	// 2) Position time: phase1T +phase2T +phase3T +phase4T +phase5T +phase6T +phase7T

	double curT = 0;

	while(curT +AFX_MATH_EPS < deltaT)
	{
		double deltaPos = targetPos -lastPos;


	}
}
