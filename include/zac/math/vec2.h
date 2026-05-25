#ifndef ZAC_VEC2_H
#define ZAC_VEC2_H

typedef struct {
 float x, y;
} ZAC_Vec2;

static inline ZAC_Vec2 ZAC_Stv2(float x) {
 ZAC_Vec2 out;
 out.x = x;
 out.y = x;
 return out;
}

static inline ZAC_Vec2 ZAC_Neg2(ZAC_Vec2 a) {
 ZAC_Vec2 out;
 out.x = -a.x;
 out.y = -a.y;
 return out;
}


static inline ZAC_Vec2 ZAC_Add2(ZAC_Vec2 a, ZAC_Vec2 b) {
 ZAC_Vec2 out;
 out.x = a.x + b.x;
 out.y = a.y + b.y;
 return out;
}


static inline ZAC_Vec2 ZAC_Sub2(ZAC_Vec2 a, ZAC_Vec2 b) {
 ZAC_Vec2 out;
 out.x = a.x - b.x;
 out.y = a.y - b.y;
 return out;
}


static inline ZAC_Vec2 ZAC_Mul2(ZAC_Vec2 a, ZAC_Vec2 b) {
 ZAC_Vec2 out;
 out.x = a.x * b.x;
 out.y = a.y * b.y;
 return out;
}

static inline ZAC_Vec2 ZAC_Div2(ZAC_Vec2 a, ZAC_Vec2 b) {
 ZAC_Vec2 out;
 out.x = a.x / b.x;
 out.y = a.y / b.y;
 return out;
}


static inline float ZAC_Dot2(ZAC_Vec2 a, ZAC_Vec2 b) {
 return a.x * b.x + a.y * b.y;
}



#endif

