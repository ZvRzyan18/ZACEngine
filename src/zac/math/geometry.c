#include "zac/math/geometry.h"
#include "zac/math/math.h"
#include <string.h>


ZAC_Mat4x4 ZAC_Mat4x4_Ortho(float l, float r, float b, float t, float n, float f) {
 ZAC_Mat4x4 m;
 
 m.m_00 = 2.0f / (r - l);
 m.m_10 = 0.0f;
 m.m_20 = 0.0f;
 m.m_30 = 0.0f;
 
 m.m_01 = 0.0f;
 m.m_11 = -2.0f / (t - b);
 m.m_21 = 0.0f;
 m.m_31 = 0.0f;
 
 m.m_02 = 0.0f;
 m.m_12 = 0.0f;
 m.m_22 = 1 / (f - n);
 m.m_32 = 0.0f;
 
 m.m_03 = -(r + l) / (r - l);
 m.m_13 = -(t + b) / (t - b);
 m.m_23 = -n / (f - n);
 m.m_33 = 1.0f;
 return m;
}



ZAC_Mat4x4 ZAC_Mat4x4_Perspective(float fov, float aspect, float z_near, float z_far) { 
 ZAC_Mat4x4 m;
 float f  = 1.0f / ZAC_Tan(fov * 0.5f);
 float fn = 1.0f / (z_near - z_far);
 memset(&m, 0, sizeof(ZAC_Mat4x4));
 m.m_00 = f / aspect;
 m.m_11 = f;
 m.m_22 =-z_far * fn;
 m.m_32 = 1.0f;
 m.m_23 = z_near * z_far * fn;
 return m;
}



ZAC_Vec4 ZAC_Mat4x4_UnprojectRect(ZAC_Vec4 v, ZAC_Mat4x4 proj, ZAC_Vec2 size) {
 ZAC_Vec4 min_v = {v.x, v.y, 0.0f, 1.0f};
 ZAC_Vec4 max_v = {v.z, v.w, 0.0f, 1.0f};
 
 ZAC_Vec4 min_w = ZAC_Mat4x4_MulVec(proj, min_v);
 ZAC_Vec4 max_w = ZAC_Mat4x4_MulVec(proj, max_v);

 min_w.x = (min_w.x + 1.0f);
 min_w.y = (min_w.y + 1.0f);

 max_w.x = (max_w.x + 1.0f);
 max_w.y = (max_w.y + 1.0f);
 
 ZAC_Vec4 out;
 out.x = min_w.x * size.x;
 out.y = min_w.y * size.y;
 out.z = max_w.x * size.x;
 out.w = max_w.y * size.y; 

 out = ZAC_Mul4(out, ZAC_Stv4(0.5f));

 /* vulkan is inverted-y, so swap y,width is needed.*/
 float tmp = out.w;
 out.w = out.y;
 out.y = tmp;
 return out;
}

