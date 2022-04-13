#include "stdafx.h"

#include "SourceSdkShared.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace SOURCESDK {

SOURCESDK::vec_t DotProduct(const SOURCESDK::vec_t * v1, const SOURCESDK::vec_t * v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void VectorAngles( const SOURCESDK::Vector& forward, SOURCESDK::QAngle &angles )
{
	float	tmp, yaw, pitch;
	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = (atan2f(forward[1], forward[0]) * 180 / (float)M_PI);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrtf(forward[0]*forward[0] + forward[1]*forward[1]);
		pitch = (atan2f(-forward[2], tmp) * 180 / (float)M_PI);
		if (pitch < 0)
			pitch += 360;
	}
	
	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

void VectorTransform(const SOURCESDK::Vector & in1, const SOURCESDK::matrix3x4_t & in2, SOURCESDK::Vector & out)
{
	out[0] = DotProduct(in1.Base(), in2[0]) + in2[0][3];
	out[1] = DotProduct(in1.Base(), in2[1]) + in2[1][3];
	out[2] = DotProduct(in1.Base(), in2[2]) + in2[2][3];
}

void MatrixAngles(const SOURCESDK::matrix3x4_t & matrix, SOURCESDK::QAngle & angles)
{
	float forward[3];
	float left[3];
	float up[3];

	forward[0] = matrix[0][0];
	forward[1] = matrix[1][0];
	forward[2] = matrix[2][0];
	left[0] = matrix[0][1];
	left[1] = matrix[1][1];
	left[2] = matrix[2][1];
	up[2] = matrix[2][2];

	float xyDist = sqrtf(forward[0] * forward[0] + forward[1] * forward[1]);

	if (xyDist > 0.001f)
	{
		angles[1] = (atan2f(forward[1], forward[0])) * 180.0f / (float)M_PI;
		angles[0] = (atan2f(-forward[2], xyDist)) * 180.0f / (float)M_PI;
		angles[2] = (atan2f(left[2], up[2])) * 180.0f / (float)M_PI;
	}
	else
	{
		angles[1] = (atan2f(-left[0], left[1])) * 180.0f / (float)M_PI;
		angles[0] = (atan2f(-forward[2], xyDist)) * 180.0f / (float)M_PI;
		angles[2] = 0;
	}
}

void AngleMatrix( const SOURCESDK::QAngle &angles,  SOURCESDK::matrix3x4_t& matrix )
{
	float sy = sinf(angles[1] * (float)M_PI / 180.0);
	float cy = cosf(angles[1] * (float)M_PI / 180.0);

	float sp = sinf(angles[0] * (float)M_PI / 180.0);
	float cp = cosf(angles[0] * (float)M_PI / 180.0);

	float sr = sinf(angles[2] * (float)M_PI / 180.0);
	float cr = cosf(angles[2] * (float)M_PI / 180.0);

	matrix[0][0] = cp*cy;
	matrix[1][0] = cp*sy;
	matrix[2][0] = -sp;

	matrix[0][1] = sr*sp*cy+cr*-sy;
	matrix[1][1] = sr*sp*sy+cr*cy;
	matrix[2][1] = sr*cp;
	matrix[0][2] = (cr*sp*cy+-sr*-sy);
	matrix[1][2] = (cr*sp*sy+-sr*cy);
	matrix[2][2] = cr*cp;

	matrix[0][3] = 0.0f;
	matrix[1][3] = 0.0f;
	matrix[2][3] = 0.0f;
}

void AngleMatrix( const QAngle &angles, const Vector &position, matrix3x4_t& matrix )
{
    AngleMatrix(angles, matrix);
    matrix[0][3] = position[0];
    matrix[1][3] = position[1];
    matrix[2][3] = position[2];
}

void R_ConcatTransforms (const SOURCESDK::matrix3x4_t & in1, const SOURCESDK::matrix3x4_t & in2, SOURCESDK::matrix3x4_t & out)
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
				in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
				in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
				in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
				in1[0][2] * in2[2][3] + in1[0][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
				in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
				in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
				in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
				in1[1][2] * in2[2][3] + in1[1][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
				in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
				in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
				in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
				in1[2][2] * in2[2][3] + in1[2][3];
}

} // namespace SOURCESDK {
