#ifndef ZAC_MAT4X4_H
#define ZAC_MAT4X4_H

#include "zac/math/vec4.h"

typedef struct {
	float m_00, m_10, m_20, m_30;
	float m_01, m_11, m_21, m_31;
	float m_02, m_12, m_22, m_32;
	float m_03, m_13, m_23, m_33;
} ZAC_Mat4x4;

extern const ZAC_Mat4x4 ZAC_Mat4x4_Identity;

ZAC_Mat4x4 ZAC_Mat4x4_FromQuaternion(ZAC_Vec4 q);
ZAC_Mat4x4 ZAC_Mat4x4_Mul(ZAC_Mat4x4 a, ZAC_Mat4x4 b);
ZAC_Mat4x4 ZAC_Mat4x4_Invert(ZAC_Mat4x4 a);
ZAC_Mat4x4 ZAC_Mat4x4_Transpose(ZAC_Mat4x4 a);
ZAC_Vec4 ZAC_Mat4x4_MulVec(ZAC_Mat4x4 a, ZAC_Vec4 b);


#endif

