#ifndef ZAC_VEC3_H
#define ZAC_VEC3_H

typedef struct {
 float x, y, z, _pad;
} ZAC_Vec3;

static inline ZAC_Vec3 ZAC_Stv3(float x) {
 ZAC_Vec3 out;
 out.x = x;
 out.y = x;
 out.z = x;
 return out;
}

static inline ZAC_Vec3 ZAC_Neg3(ZAC_Vec3 a) {
 ZAC_Vec3 out;
 out.x = -a.x;
 out.y = -a.y;
 out.z = -a.z;
 return out;
}


static inline ZAC_Vec3 ZAC_Add3(ZAC_Vec3 a, ZAC_Vec3 b) {
 ZAC_Vec3 out;
 out.x = a.x + b.x;
 out.y = a.y + b.y;
 out.z = a.z + b.z;
 return out;
}


static inline ZAC_Vec3 ZAC_Sub3(ZAC_Vec3 a, ZAC_Vec3 b) {
 ZAC_Vec3 out;
 out.x = a.x - b.x;
 out.y = a.y - b.y;
 out.z = a.z - b.z;
 return out;
}


static inline ZAC_Vec3 ZAC_Mul3(ZAC_Vec3 a, ZAC_Vec3 b) {
 ZAC_Vec3 out;
 out.x = a.x * b.x;
 out.y = a.y * b.y;
 out.z = a.z * b.z;
 return out;
}

static inline ZAC_Vec3 ZAC_Div3(ZAC_Vec3 a, ZAC_Vec3 b) {
 ZAC_Vec3 out;
 out.x = a.x / b.x;
 out.y = a.y / b.y;
 out.z = a.z / b.z;
 return out;
}


static inline float ZAC_Dot3(ZAC_Vec3 a, ZAC_Vec3 b) {
 return a.x * b.x + a.y * b.y + a.z * b.z;
}



#endif

