#ifndef ZAC_VEC4_H
#define ZAC_VEC4_H

typedef struct {
 float x, y, z, w;
} ZAC_Vec4;

static inline ZAC_Vec4 ZAC_Stv4(float x) {
 ZAC_Vec4 out;
 out.x = x;
 out.y = x;
 out.z = x;
 out.w = x;
 return out;
}

static inline ZAC_Vec4 ZAC_Neg4(ZAC_Vec4 a) {
 ZAC_Vec4 out;
 out.x = -a.x;
 out.y = -a.y;
 out.z = -a.z;
 out.w = -a.w;
 return out;
}


static inline ZAC_Vec4 ZAC_Add4(ZAC_Vec4 a, ZAC_Vec4 b) {
 ZAC_Vec4 out;
 out.x = a.x + b.x;
 out.y = a.y + b.y;
 out.z = a.z + b.z;
 out.w = a.w + b.w;
 return out;
}


static inline ZAC_Vec4 ZAC_Sub4(ZAC_Vec4 a, ZAC_Vec4 b) {
 ZAC_Vec4 out;
 out.x = a.x - b.x;
 out.y = a.y - b.y;
 out.z = a.z - b.z;
 out.w = a.w - b.w;
 return out;
}


static inline ZAC_Vec4 ZAC_Mul4(ZAC_Vec4 a, ZAC_Vec4 b) {
 ZAC_Vec4 out;
 out.x = a.x * b.x;
 out.y = a.y * b.y;
 out.z = a.z * b.z;
 out.w = a.w * b.w;
 return out;
}

static inline ZAC_Vec4 ZAC_Div4(ZAC_Vec4 a, ZAC_Vec4 b) {
 ZAC_Vec4 out;
 out.x = a.x / b.x;
 out.y = a.y / b.y;
 out.z = a.z / b.z;
 out.w = a.w / b.w;
 return out;
}


static inline float ZAC_Dot4(ZAC_Vec4 a, ZAC_Vec4 b) {
 return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}



#endif

