#ifndef ZAC_MATH_H
#define ZAC_MATH_H

#include <stdint.h>
#include <stddef.h>

/*
 channel color space conversion
*/
uint8_t ZAC_ToSRGB_Uint(uint8_t c);
float ZAC_ToSRGB_Float(float normalized);
uint8_t ZAC_ToLinear_Uint(uint8_t c);
float ZAC_ToLinear_Float(float normalized);

/*
 round/align to next power of 2
*/
size_t ZAC_NextMultipleOf(size_t x, size_t n);
size_t ZAC_RoundToPowerOfTwo(size_t x);



static inline float ZAC_Abs(float x) {
 return x < 0.0f ? -x : x;
}



float ZAC_Nan(void);

float ZAC_Infinity(void);


float ZAC_Round(float x);

float ZAC_Ceil(float x);

float ZAC_Floor(float x);

/*
 NOTE : all functions right here are approximated.
 if precision is needed, just use math.h/cmath instead.
*/
float ZAC_Sqrt(float x);

float ZAC_InvSqrt(float x);

float ZAC_Sin(float x);

float ZAC_Cos(float x);

void ZAC_SinCos(float x, float *_s, float *_c);

float ZAC_Tan(float x);

float ZAC_Asin(float x);

float ZAC_Acos(float x);

float ZAC_Atan(float x);

float ZAC_Atan2(float y, float x);

float ZAC_Exp2(float x);

float ZAC_Log2(float x);

float ZAC_Pow(float x, float y);

float ZAC_Fmod(float x, float y);


#endif

