#ifndef ZAC_QUAT4_H
#define ZAC_QUAT4_H

#include "zac/math/vec4.h"
#include "zac/math/vec3.h"

extern const ZAC_Vec4 ZAC_Quat4_Identity;

ZAC_Vec4 ZAC_Quat4_Mul(ZAC_Vec4 a, ZAC_Vec4 b);
ZAC_Vec4 ZAC_Quat4_FromEulerAngle(const float roll, const float pitch, const float yaw);
ZAC_Vec3 ZAC_Quat4_ToEulerAngle(ZAC_Vec4 q);
ZAC_Vec4 ZAC_Quat4_FromAxisAngle(const float r, const float x, const float y, const float z);
ZAC_Vec4 ZAC_Quat4_ToAxisAngle(ZAC_Vec4 x);
ZAC_Vec4 ZAC_Quat4_Lerp(ZAC_Vec4 a, ZAC_Vec4 b, const float t);
ZAC_Vec4 ZAC_Quat4_Slerp(ZAC_Vec4 a, ZAC_Vec4 b, const float t);
ZAC_Vec4 ZAC_Quat4_Invert(ZAC_Vec4 a);
ZAC_Vec4 ZAC_Quat4_Conjugate(ZAC_Vec4 a);

#endif

