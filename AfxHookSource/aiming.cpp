#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-04-14 dominik.matrixstorm.com
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
, m_YPitchVelocity(0)
, m_YPitchAcceleration(0)
, m_ZYawVelocity(0)
, m_ZYawAcceleration(0)
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
			LookAnglesFromTo(camOrigin, LastTargetOrigin, targetYPitch, targetZYaw);

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

			m_Deactivating = Active || reaimYPitch > AFX_MATH_EPS || reaimZYaw > AFX_MATH_EPS;

			// For when angles are on the 359..0 crossover
			if (reaimYPitch > 180.0) reaimYPitch -= 360.0;
			else if (reaimYPitch < -180.0) reaimYPitch += 360.0;
			if (reaimZYaw > 180.0) reaimZYaw -= 360.0;
			else if (reaimZYaw < -180.0) reaimZYaw += 360.0;

			// apply re-aiming:

			CalcSmooth(deltaT, LastYPitch +reaimYPitch, LastYPitch, m_YPitchVelocity, m_YPitchAcceleration);
			yPitch = LastYPitch;

			CalcSmooth(deltaT, LastZYaw +reaimZYaw, LastZYaw, m_ZYawVelocity, m_ZYawAcceleration);
			zYaw = LastZYaw;

			camRotationChanged = true;
		}
	}

	LastYPitch = yPitch;
	LastZYaw = zYaw;
	LastXRoll = xRoll;

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


	//
	// We build a third order motion profile
	// (sort of sinusoidal):
	
	// phaseT is the (positive) phase time.
	//
	// jerk(k) = c_k
	// accel(k) = accel(k-1) +Int(jerk(k),0+eps..phaseT) = accel(k-1) +jerk(k)*phaseT
	// vel(k) = velo(k-1) +Int(accell(k),0+eps..phaseT) = velo(k-1) +accel(k-1) * phaseT +jerk(k)/2 * phaseT^2
	// pos(k) = pos(k-1) +Int(vel(k),0+eps..phaseT) = pos(k-1) +vel(k-1)*phaseT +accel(k-1)/2*phaseT^2 +jerk(k)/6*phaseT^3
	//
	// -LimitJerk <= jerk(k) <= LimitJerk
	// -LimitAccel <= accel(k) <= LimitAccel
	// -LimitVelocity <= vel(k) <= LimitVelocity
	//
	// posErr(k) = targetPos(k) -pos(k)
	//
	// We are making the following (potentially wrong) assumption: 
	// targetPos remains constant from k onwards.
	//
	// We want to minimize (in this order of priority):
	// 1) Total position changes
	// 2) Total velocity changes
	// 3) Total acceleration changes
	//
	// Possible phases:
	// P1) acceleration (decelleration for negative deltaPos) build-up
	// P2) limit acceleration (decelleration for negative deltaPos)
	// P3) acceleration (decelleration for negative deltaPos) ramp-down
	// P4) limit speed
	// P5) deceleration (accelleration for negative deltaPos) build-up
	// P6) limit deceleration (accelleration for negative deltaPos)
	// P7) deceleration (accelleration for negative deltaPos) ramp-down
	//
	// If we are doomed to overshoot the position,
	// then we start operate throughout P4 - P7 with maximum Jerk changes
	// until we are fully stopped, after that normally again.

	double curT = 0;

	while(curT +AFX_MATH_EPS < deltaT)
	{
		double deltaPos = targetPos -lastPos;

		// 0 <= phase1T, 0 <= phase2T, 0 <= phase3T, 0 <= phase4T, 0 <= phase5T, 0 <= phase6T, 0 <= phase7T
		// dirA \in {-1,1}
		// dirB \in {-1,1}
		// 
		// deltaPos
		// = 0
		// +Int(vel(1),0+eps,phase1T)
		// +Int(vel(2),0+eps,phase2T)
		// +Int(vel(3),0+eps,phase3T)
		// +Int(vel(4),0+eps,phase4T)
		// +Int(vel(5),0+eps,phase5T)
		// +Int(vel(6),0+eps,phase6T)
		// +Int(vel(7),0+eps,phase7T)
		// = 
		// +lastVel*phase1T +lastAccel/2*phase1T^2 +dirA*LimitJerk/6*phase1T^3
		// +vel(1)*phase2T +accel(1)/2*phase2T^2
		// +vel(2)*phase3T +accel(1)/2*phase3T^2 -dirA*LimitJerk/6*phase3T^3
		// +vel(3)*phase4T
		// +vel(3)*phase5T -dirB*LimitJerk/6*phase5T^3
		// +vel(5)*phase6T +accel(5)/2*phase6T^2
		// +vel(6)*phase7T +accel(5)/2*phase7T^2 +dirB*LimitJerk/6*phase7T^3
		//
		// vel(1) = lastVel +lastAccel * phase1T +dirA*LimitJerk/2 * phase1T^2
		// vel(2) = vel(1) +accel(1) * phase2T
		// vel(3) = vel(2) +accel(1) * phase3T -dirA*LimitJerk/2 * phase3T^2
		// vel(5) = vel(3) -dirB*LimitJerk/2 * phase5T^2
		// vel(6) = vel(5) +accel(5) * phase6T
		//
		// -LimitVelocity <= vel(1), vel(2), vel(3), vel(4), vel(5), vel(6) <= LimitVelocity
		//
		// accel(1) = lastVel +dirA*LimitJerk*phase1T
		// accel(5) = -dirB*LimitJerk/6*phase5T^3
		//
		// -LimitAccel <= accel(1), accel(5) <= LimitAccel
		//
		// Minimize:
		// phase1T +phase2T +phase3T +phase4T +phase5T +phase6T +phase7T

	}
}
