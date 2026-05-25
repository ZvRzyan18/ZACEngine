#include "zac/math/quat4.h"
#include "zac/math/math.h"


const ZAC_Vec4 ZAC_Quat4_Identity = {
 0.0f, 0.0f, 0.0f, 1.0f
};


/*
 multiply two quaternions
*/
ZAC_Vec4 ZAC_Quat4_Mul(ZAC_Vec4 a, ZAC_Vec4 b) {
 ZAC_Vec4 q;
 q.x = a.x * b.w + a.y * b.z - a.z * b.y + a.w * b.x;
 q.y = -a.x * b.z + a.y * b.w + a.z * b.x + a.w * b.y;
 q.z = a.x * b.y - a.y * b.x + a.z * b.w + a.w * b.z;
 q.w = -a.x * b.x - a.y * b.y - a.z * b.z + a.w * b.w;
 return q;
}

/*
 convert euler angle to quaternion 
*/
ZAC_Vec4 ZAC_Quat4_FromEulerAngle(const float roll, const float pitch, const float yaw) {
 ZAC_Vec4 q;
 float cy, sy;
 float cp, sp;
 float cr, sr;

 ZAC_SinCos(yaw * 0.5f, &sy, &cy);
 ZAC_SinCos(pitch * 0.5f, &sp, &cp);
 ZAC_SinCos(roll * 0.5f, &sr, &cr);
 
 const float crcp = cr*cp;
 const float crsp = cr*sp;
 const float srcp = sr*cp;
 const float srsp = sr*sp;
 
 q.w = crcp * cy + srsp * sy;
 q.x = srcp * cy - crsp * sy;
 q.y = crsp * cy + srcp * sy;
 q.z = crcp * sy - srsp * cy;
 return q;
}

/*
 convert quaternion to euler angle 
*/
ZAC_Vec3 ZAC_Quat4_ToEulerAngle(ZAC_Vec4 q) {
 ZAC_Vec3 out;
 const float y2 = q.y * q.y;
 float mpitch = 2.0f * (q.w * q.y - q.z * q.x);
 if(mpitch >= 1.0f) {
  out.y =  mpitch < 0.0f ? -1.570796f : 1.570796f;
 } else
  out.y = ZAC_Asin(mpitch);
 /* TODO : use vector instruction in atan2 */
 out.x = ZAC_Atan2(2.0f * (q.w * q.x + q.y * q.z), 1.0f - 2.0f * (q.x * q.x + y2));
 out.z = ZAC_Atan2(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (y2 + q.z * q.z));
 return out;
}

/*
 convert axis angle to quaternion
*/
ZAC_Vec4 ZAC_Quat4_FromAxisAngle(const float r, const float x, const float y, const float z) {
 ZAC_Vec4 q;
 float s;
 ZAC_SinCos(r * 0.5f, &s, &q.w);
	q.x = x * s;
 q.y = y * s;
	q.z = z * s;
	return q;
}

/*
 convert quaternion to axis angle
*/
ZAC_Vec4 ZAC_Quat4_ToAxisAngle(ZAC_Vec4 x) {
 ZAC_Vec4 a;
 a.w = ZAC_Acos(x.w);
 a.w += a.w;
 const float b = ZAC_InvSqrt(1.0f - x.w * x.w);
 a.x = x.x * b;
 a.y = x.y * b;
 a.z = x.z * b;
 return a;
}

/*
 linear interpolation
*/
ZAC_Vec4 ZAC_Quat4_Lerp(ZAC_Vec4 a, ZAC_Vec4 b, const float t) {
 ZAC_Vec4 q;
	if(ZAC_Dot4(a, b) < 0.0f) {
  q.x = a.x - t * (a.x - (-b.x));
  q.y = a.y - t * (a.y - (-b.y));
  q.z = a.z - t * (a.z - (-b.z));
  q.w = a.w - t * (a.w - (-b.w));
	} else {
  q.x = a.x - t * (a.x - (b.x));
  q.y = a.y - t * (a.y - (b.y));
  q.z = a.z - t * (a.z - (b.z));
  q.w = a.w - t * (a.w - (b.w));
	}
	return q;
}


/*
 spherical linear interpolation
*/
ZAC_Vec4 ZAC_Quat4_Slerp(ZAC_Vec4 a, ZAC_Vec4 b, const float t) {
 ZAC_Vec4 q;
 float s1, s2, db, omega, rs_omega;
	db = ZAC_Dot4(a, b);
	if(db < 0.0f) {
	 db = ZAC_Abs(db);
   if(db > (1.0f - 1e-4f)) { //db > 0.999900, or db > (1.0 - 1e-4)
    s1 = 1.0f - t;
   	s2 = -t;
   } else {
    omega = ZAC_Acos(db);
    rs_omega = 1.0f / ZAC_Sin(omega);
    /* TODO : use vector instruction here in sin */
    s1 = ZAC_Sin((1.0f - t) * omega) * rs_omega;
    s2 = -ZAC_Sin(t * omega) * rs_omega;
   }
 	} else {
   if(db > (1.0f - 1e-4f)) { //db > 0.999900, or db > (1.0 - 1e-4)
    s1 = 1.0f - t;
   	s2 = t;
   } else {
    omega = ZAC_Acos(db);
    rs_omega = 1.0f / ZAC_Sin(omega);
    /* TODO : use vector instruction here in sin */
    s1 = ZAC_Sin((1.0f - t) * omega) * rs_omega;
    s2 = ZAC_Sin(t * omega) * rs_omega;
  }
	}
 q.x = s1 * a.x + s2 * b.x;
 q.y = s1 * a.y + s2 * b.y;
 q.z = s1 * a.z + s2 * b.z;
 q.w = s1 * a.w + s2 * b.w;
 return q;
}


/*
 inverse, can be used to divide quaternion
 div(q1, q2) = q1 * q2 ^ -1 
*/
ZAC_Vec4 ZAC_Quat4_Invert(ZAC_Vec4 a) {
 ZAC_Vec4 q;
 const float rl = ZAC_InvSqrt(a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w);
 q.w *= rl;
	q.x *= (-rl);
	q.y *= (-rl);
	q.z *= (-rl);
	return q;
}


/*
 negate vector3 (x, y, z), and w component is still the same
*/
ZAC_Vec4 ZAC_Quat4_Conjugate(ZAC_Vec4 a) {
 ZAC_Vec4 q;
 q.w = a.w;
	q.x = -a.x;
	q.y = -a.y;
	q.z = -a.z;
	return q;
}


