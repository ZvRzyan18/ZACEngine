#include "zac/math/mat4x4.h"
#include "zac/math/math.h"
#include <assert.h>
#include <string.h>

const ZAC_Mat4x4 ZAC_Mat4x4_Identity = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f,
};


ZAC_Mat4x4 ZAC_Mat4x4_FromQuaternion(ZAC_Vec4 q) {
 ZAC_Mat4x4 m;
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
 m.m_30 = 0.0f;

 m.m_01 =     2.0f * (tmp[1] + tmp[8]);
 m.m_11 = 1.0f - 2.0f * (tmp[0] + tmp[7]);
 m.m_21 =     2.0f * (tmp[5] - tmp[3]);
 m.m_31 = 0.0f;

 m.m_02 =     2.0f * (tmp[2] - tmp[6]);
 m.m_12 =     2.0f * (tmp[5] + tmp[3]);
 m.m_22 = 1.0f - 2.0f * (tmp[0] + tmp[4]);
 m.m_32 = 0.0f;
 
 m.m_03 = 0.0f;
 m.m_13 = 0.0f;
 m.m_23 = 0.0f;
 m.m_33 = 1.0f;
 return m;
}


ZAC_Mat4x4 ZAC_Mat4x4_Mul(ZAC_Mat4x4 a, ZAC_Mat4x4 b) {
	ZAC_Mat4x4 m;

	m.m_00 = b.m_00 * a.m_00 + b.m_10 * a.m_01 + b.m_20 * a.m_02 + b.m_30 * a.m_03;
	m.m_10 = b.m_00 * a.m_10 + b.m_10 * a.m_11 + b.m_20 * a.m_12 + b.m_30 * a.m_13;
	m.m_20 = b.m_00 * a.m_20 + b.m_10 * a.m_21 + b.m_20 * a.m_22 + b.m_30 * a.m_23;
	m.m_30 = b.m_00 * a.m_30 + b.m_10 * a.m_31 + b.m_20 * a.m_32 + b.m_30 * a.m_33;
	
	m.m_01 = b.m_01 * a.m_00 + b.m_11 * a.m_01 + b.m_21 * a.m_02 + b.m_31 * a.m_03;
	m.m_11 = b.m_01 * a.m_10 + b.m_11 * a.m_11 + b.m_21 * a.m_12 + b.m_31 * a.m_13;
	m.m_21 = b.m_01 * a.m_20 + b.m_11 * a.m_21 + b.m_21 * a.m_22 + b.m_31 * a.m_23;
	m.m_31 = b.m_01 * a.m_30 + b.m_11 * a.m_31 + b.m_21 * a.m_32 + b.m_31 * a.m_33;

	m.m_02 = b.m_02 * a.m_00 + b.m_12 * a.m_01 + b.m_22 * a.m_02 + b.m_32 * a.m_03;
	m.m_12 = b.m_02 * a.m_10 + b.m_12 * a.m_11 + b.m_22 * a.m_12 + b.m_32 * a.m_13;
	m.m_22 = b.m_02 * a.m_20 + b.m_12 * a.m_21 + b.m_22 * a.m_22 + b.m_32 * a.m_23;
	m.m_32 = b.m_02 * a.m_30 + b.m_12 * a.m_31 + b.m_22 * a.m_32 + b.m_32 * a.m_33;

	m.m_03 = b.m_03 * a.m_00 + b.m_13 * a.m_01 + b.m_23 * a.m_02 + b.m_33 * a.m_03;
	m.m_13 = b.m_03 * a.m_10 + b.m_13 * a.m_11 + b.m_23 * a.m_12 + b.m_33 * a.m_13;
	m.m_23 = b.m_03 * a.m_20 + b.m_13 * a.m_21 + b.m_23 * a.m_22 + b.m_33 * a.m_23;
	m.m_33 = b.m_03 * a.m_30 + b.m_13 * a.m_31 + b.m_23 * a.m_32 + b.m_33 * a.m_33;

 return m;
}


ZAC_Mat4x4 ZAC_Mat4x4_Invert(ZAC_Mat4x4 a) {
 ZAC_Mat4x4 m;
 float temp[12];

	temp[0] = a.m_00 * a.m_11 - a.m_10 * a.m_01;
	temp[1] = a.m_00 * a.m_12 - a.m_10 * a.m_02;
	temp[2] = a.m_00 * a.m_13 - a.m_10 * a.m_03;
	temp[3] = a.m_01 * a.m_12 - a.m_11 * a.m_02;
	
	temp[4] = a.m_01 * a.m_13 - a.m_11 * a.m_03;
	temp[5] = a.m_02 * a.m_13 - a.m_12 * a.m_03;
	temp[6] = a.m_22 * a.m_33 - a.m_32 * a.m_23;
	temp[7] = a.m_21 * a.m_33 - a.m_31 * a.m_23;
	
	temp[8] = a.m_21 * a.m_32 - a.m_31 * a.m_22;
	temp[9] = a.m_20 * a.m_33 - a.m_30 * a.m_23;
	temp[10] =a.m_20 * a.m_32 - a.m_30 * a.m_22;
	temp[11] =a.m_20 * a.m_31 - a.m_30 * a.m_21;

 float inv_det = (temp[0] * temp[6] - temp[1] * temp[7] + temp[2] * temp[8] + temp[3] * temp[9] - temp[4] * temp[10] + temp[5] * temp[11]);

 assert(!(ZAC_Abs(inv_det) <= 1e-4f));

 inv_det = 1.0f / inv_det;
 
 m.m_00 = (a.m_11 * temp[6] - a.m_12 * temp[7] + a.m_13 * temp[8]) * inv_det;
 m.m_01 = (-a.m_01 * temp[6] + a.m_02 * temp[7] - a.m_03 * temp[8]) * inv_det;
 m.m_02 = (a.m_31 * temp[5] - a.m_32 * temp[4] + a.m_33 * temp[3]) * inv_det;
 m.m_03 = (-a.m_21 * temp[5] + a.m_22 * temp[4] - a.m_23 * temp[3]) * inv_det;

 m.m_10 = (-a.m_10 * temp[6] + a.m_12 * temp[9] - a.m_13 * temp[10]) * inv_det;
 m.m_11 = (a.m_00 * temp[6] - a.m_02 * temp[9] + a.m_03 * temp[10]) * inv_det;
 m.m_12 = (-a.m_30 * temp[5] + a.m_32 * temp[2] - a.m_33 * temp[1]) * inv_det;
 m.m_13 = (a.m_20 * temp[5] - a.m_22 * temp[2] + a.m_23 * temp[1]) * inv_det;

 m.m_20 = (a.m_10 * temp[7] - a.m_11 * temp[9] + a.m_13 * temp[11]) * inv_det;
 m.m_21 = (-a.m_00 * temp[7] + a.m_01 * temp[9] - a.m_03 * temp[11]) * inv_det;
 m.m_22 = (a.m_30 * temp[4] - a.m_31 * temp[2] + a.m_33 * temp[0]) * inv_det;
 m.m_23 = (-a.m_20 * temp[4] + a.m_21 * temp[2] - a.m_23 * temp[0]) * inv_det;

 m.m_30 = (-a.m_10 * temp[8] + a.m_11 * temp[10] - a.m_12 * temp[11]) * inv_det;
 m.m_31 = (a.m_00 * temp[8] - a.m_01 * temp[10] + a.m_02 * temp[11]) * inv_det;
 m.m_32 = (-a.m_30 * temp[3] + a.m_31 * temp[1] - a.m_32 * temp[0]) * inv_det;
 m.m_33 = (a.m_20 * temp[3] - a.m_21 * temp[1] + a.m_22 * temp[0]) * inv_det;

 return m;
}


ZAC_Mat4x4 ZAC_Mat4x4_Transpose(ZAC_Mat4x4 a) {
 ZAC_Mat4x4 m;
	m.m_00 = a.m_00;
	m.m_10 = a.m_01;
	m.m_20 = a.m_02;
	m.m_30 = a.m_03;
	
	m.m_01 = a.m_10;
	m.m_11 = a.m_11;
	m.m_21 = a.m_12;
	m.m_31 = a.m_13;
	
	m.m_02 = a.m_20;
	m.m_12 = a.m_21;
	m.m_22 = a.m_22;
	m.m_32 = a.m_23;
	
	m.m_03 = a.m_30;
	m.m_13 = a.m_31;
	m.m_23 = a.m_32;
	m.m_33 = a.m_33;
	return m;
}


ZAC_Vec4 ZAC_Mat4x4_MulVec(ZAC_Mat4x4 a, ZAC_Vec4 b) {
 ZAC_Vec4 v;

	v.x = a.m_00 * b.x + a.m_01 * b.y + a.m_02 * b.z + a.m_03 * b.w;
 v.y = a.m_10 * b.x + a.m_11 * b.y + a.m_12 * b.z + a.m_13 * b.w;
	v.z = a.m_20 * b.x + a.m_21 * b.y + a.m_22 * b.z + a.m_23 * b.w;
	v.w = a.m_30 * b.x + a.m_31 * b.y + a.m_32 * b.z + a.m_33 * b.w;

 return v;
}


