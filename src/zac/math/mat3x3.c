#include "zac/math/mat3x3.h"
#include "zac/math/math.h"
#include <assert.h>

const ZAC_Mat3x3 ZAC_Mat3x3_Identity = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f
};


ZAC_Mat3x3 ZAC_Mat3x3_FromQuaternion(ZAC_Vec4 q) {
 ZAC_Mat3x3 m;
 float tmp[9];

 tmp[0] = q.x * q.x;
 tmp[1] = q.x * q.y;
 tmp[2] = q.x * q.z;
 tmp[3] = q.x * q.w;

 tmp[4] = q.y * q.y;
 tmp[5] = q.y * q.z;
 tmp[6] = q.y * q.w;

 tmp[7] = q.z * q.z;
 tmp[8] = q.z * q.w;

 m.m_00 = 1.0f - 2.0f * (tmp[4] + tmp[7]);
 m.m_10 =     2.0f * (tmp[1] - tmp[8]);
 m.m_20 =     2.0f * (tmp[2] + tmp[6]);

 m.m_01 =     2.0f * (tmp[1] + tmp[8]);
 m.m_11 = 1.0f - 2.0f * (tmp[0] + tmp[7]);
 m.m_21 =     2.0f * (tmp[5] - tmp[3]);

 m.m_02 =     2.0f * (tmp[2] - tmp[6]);
 m.m_12 =     2.0f * (tmp[5] + tmp[3]);
 m.m_22 = 1.0f - 2.0f * (tmp[0] + tmp[4]);
 return m;
}


ZAC_Mat3x3 ZAC_Mat3x3_Mul(ZAC_Mat3x3 a, ZAC_Mat3x3 b) {
 ZAC_Mat3x3 m;

	m.m_00 = b.m_00 * a.m_00 + b.m_10 * a.m_01 + b.m_20 * a.m_02;
	m.m_10 = b.m_00 * a.m_10 + b.m_10 * a.m_11 + b.m_20 * a.m_12;
	m.m_20 = b.m_00 * a.m_20 + b.m_10 * a.m_21 + b.m_20 * a.m_22;

	m.m_01 = b.m_01 * a.m_00 + b.m_11 * a.m_01 + b.m_21 * a.m_02;
	m.m_11 = b.m_01 * a.m_10 + b.m_11 * a.m_11 + b.m_21 * a.m_12;
	m.m_21 = b.m_01 * a.m_20 + b.m_11 * a.m_21 + b.m_21 * a.m_22;

	m.m_02 = b.m_02 * a.m_00 + b.m_12 * a.m_01 + b.m_22 * a.m_02;
	m.m_12 = b.m_02 * a.m_10 + b.m_12 * a.m_11 + b.m_22 * a.m_12;
	m.m_22 = b.m_02 * a.m_20 + b.m_12 * a.m_21 + b.m_22 * a.m_22;

 return m;
}


ZAC_Mat3x3 ZAC_Mat3x3_Invert(ZAC_Mat3x3 a) {
 ZAC_Mat3x3 m;
 float temp[4];
 
 temp[0] = a.m_11 * a.m_22 - a.m_21 * a.m_12;
	temp[1] = a.m_10 * a.m_22 - a.m_12 * a.m_20;
	temp[2] = a.m_10 * a.m_21 - a.m_11 * a.m_20;

	float inv_det = (a.m_00 * temp[0] - a.m_01 * temp[1] + a.m_02 * temp[2]);

 assert(!(ZAC_Abs(inv_det) <= 1e-4f));

 inv_det = 1.0f / inv_det;
 
	m.m_00 = (temp[0]) * inv_det;
 m.m_01 = -(a.m_01 * a.m_22 - a.m_02 * a.m_21) * inv_det;
 m.m_02 = (a.m_01 * a.m_12 - a.m_02 * a.m_11) * inv_det;
 
 m.m_10 = -(temp[1]) * inv_det;
 m.m_11 = (a.m_00 * a.m_22 - a.m_02 * a.m_20) * inv_det;
 m.m_12 = -(a.m_00 * a.m_12 - a.m_02 * a.m_10) * inv_det;
 
 m.m_20 = (temp[2]) * inv_det;
 m.m_21 = -(a.m_00 * a.m_21 - a.m_02 * a.m_20) * inv_det;
 m.m_22 = (a.m_00 * a.m_11 - a.m_10 * a.m_01) * inv_det;

 return m;
}


ZAC_Mat3x3 ZAC_Mat3x3_Transpose(ZAC_Mat3x3 a) {
 ZAC_Mat3x3 m;
	m.m_00 = a.m_00;
	m.m_10 = a.m_01;
	m.m_20 = a.m_02;
	
	m.m_01 = a.m_10;
	m.m_11 = a.m_11;
	m.m_21 = a.m_12;
	
	m.m_02 = a.m_20;
	m.m_12 = a.m_21;
	m.m_22 = a.m_22;
	return m;
}


ZAC_Vec3 ZAC_Mat3x3_MulVec(ZAC_Mat3x3 a, ZAC_Vec3 b) {
 ZAC_Vec3 v;

	v.x = a.m_00 * b.x + a.m_01 * b.y + a.m_02 * b.z;
 v.y = a.m_10 * b.x + a.m_11 * b.y + a.m_12 * b.z;
	v.z = a.m_20 * b.x + a.m_21 * b.y + a.m_22 * b.z;

 return v;
}


