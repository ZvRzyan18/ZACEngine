#ifndef ZAC_MAT3X3_H
#define ZAC_MAT3X3_H

#include "zac/math/vec3.h"
#include "zac/math/vec4.h"

typedef struct {
	float m_00, m_10, m_20, __pad1;
	float m_01, m_11, m_21, __pad2;
	float m_02, m_12, m_22, __pad3;
} ZAC_Mat3x3;

extern const ZAC_Mat3x3 ZAC_Mat3x3_Identity;

ZAC_Mat3x3 ZAC_Mat3x3_FromQuaternion(ZAC_Vec4 q);
ZAC_Mat3x3 ZAC_Mat3x3_Mul(ZAC_Mat3x3 a, ZAC_Mat3x3 b);
ZAC_Mat3x3 ZAC_Mat3x3_Invert(ZAC_Mat3x3 a);
ZAC_Mat3x3 ZAC_Mat3x3_Transpose(ZAC_Mat3x3 a);
ZAC_Vec3 ZAC_Mat3x3_MulVec(ZAC_Mat3x3 a, ZAC_Vec3 b);

#endif

