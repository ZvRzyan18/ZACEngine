#ifndef ZAC_GEOMETRY_H
#define ZAC_GEOMETRY_H

#include "zac/math/mat4x4.h"
#include "zac/math/vec2.h"

ZAC_Mat4x4 ZAC_Mat4x4_Ortho(float l, float r, float b, float t, float n, float f);
ZAC_Mat4x4 ZAC_Mat4x4_Perspective(float fov, float aspect, float z_near, float z_far) ;

ZAC_Vec4 ZAC_Mat4x4_UnprojectRect(ZAC_Vec4 v, ZAC_Mat4x4 proj, ZAC_Vec2 size);

#endif


