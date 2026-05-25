#include "zac/math/math.h"

/*
 Glsl functions
*/
static float Step(float edge, float x) {
 return (x < edge) ? 0.0f : 1.0f;
}


static float Mix(float x, float y, float a) {
 return x * (1.0f - a) + y * a;
}

/*
 color channel convertion  sRGB <-> Linear
*/
uint8_t ZAC_ToSRGB_Uint(uint8_t c) {
 float normalized = ((float)c) / 255.0f;
 normalized = Mix(12.92f * normalized, 1.055f * ZAC_Pow(normalized, 1.0f/2.4f) - 0.055f, Step(0.0031308, normalized));
 normalized *= 255.0f;
 return (uint8_t)(normalized < 0.0f ? 0.0f : (normalized > 255.0f ? 255.0f : normalized));
}


float ZAC_ToSRGB_Float(float normalized) {
 return Mix(12.92f * normalized, 1.055f * ZAC_Pow(normalized, 1.0f/2.4f) - 0.055f, Step(0.0031308, normalized));
}


uint8_t ZAC_ToLinear_Uint(uint8_t c) {
 float normalized = ((float)c) / 255.0f;
 normalized = Mix(normalized / 12.92f, ZAC_Pow((normalized + 0.055f) / 1.055f, 2.4), Step(0.04045f, normalized));
 normalized *= 255.0f;
 return (uint8_t)(normalized < 0.0f ? 0.0f : (normalized > 255.0f ? 255.0f : normalized));
}


float ZAC_ToLinear_Float(float normalized) {
 return Mix(normalized / 12.92f, ZAC_Pow((normalized + 0.055f) / 1.055f, 2.4), Step(0.04045f, normalized));
}




/*
 Integer Rounding and alignment
*/

size_t ZAC_NextMultipleOf(size_t x, size_t n) {
 return (x + n - 1) & ~(n - 1);
}


size_t ZAC_RoundToPowerOfTwo(size_t x) {
 x--;
 x |= x >> 1;
 x |= x >> 2;
 x |= x >> 4;
 x |= x >> 8;
 x |= x >> 16;
 return x + 1;
}



typedef union {
 uint32_t i;
 float f;
} float_bits;


/*
 manually using float representation of inf, nan
*/
float ZAC_Nan(void) {
 float_bits bits;
 bits.i = 0x7F800001;
 return bits.f;
}



float ZAC_Infinity(void) {
 float_bits bits;
 bits.i = 0x7F800000;
 return bits.f;
}



/*
 2^23, large enough to completly round off fractionals
*/
static float HUGE_NUM[4] = {
 8388608.0f, -8388608.0f,
 -8388608.0f, 8388608.0f,
};


float ZAC_Round(float x) {
	float_bits bits;
	bits.f = x;
	
	float* huge = HUGE_NUM + (bits.i >> 31) * 2;
	return (x + huge[0]) + huge[1];
}


float ZAC_Ceil(float x) {
 int i = (int)x;
 return (float)(x > 0 && x != (float)i) ? i + 1 : i;
}



float ZAC_Floor(float x) {
 int i = (int)x;
 return (float)(x < 0 && x != (float)i) ? i - 1 : i;
}


/*
 Quake III Fast Inv sqrt
*/
float ZAC_Sqrt(float x) {
  float f, mx, x_half;
  float_bits bits, x_bits;
 
  x_bits.f = x;
  bits.i =   0x5F378171 - (x_bits.i >> 1);
  mx =       bits.f;
  x_half =   x * 0.5f;
  f =        mx;
  f =        (f * (1.5f - (x_half * f * f)));
  return x * f;
}


float ZAC_InvSqrt(float x) {
  float f, mx, x_half;
  float_bits bits, x_bits;
 
  x_bits.f = x;
  bits.i =   0x5F378171 - (x_bits.i >> 1);
  mx =       bits.f;
  x_half =   x * 0.5f;
  f =        mx;
  f =        (f * (1.5f - (x_half * f * f)));
  return f;
}





/*
 coeffs polynomial with lowest degree
 just acceptable enough for rough estimation
*/
static const float COSINE[3] = {
/* original coeffs */
/*
  3.515696e-02f,
 -4.909662e-01f,
  1.00000000e-00f,
*/

/*  scaled coeffs */
 2.97037352e-18f,
-4.51285593e-09f,
 1.00000000e-00f,
};

/*
 scaled by the 
 COSINE[0] *= powf(M_PI*2 / 65536.0, 4)
 COSINE[1] *= powf(M_PI*2 / 65536.0, 2)

*/

/*-------------------------------------------------------------------*/

/*

               point in unit circle
               *
              / |
             /  |
  radius -> /   |
           /    | -> sin
          /     |
         / |    |
 origin *------ *
       angle  | -> cos

  cos(theta) = x-axis of theta angle
  sin(theta) = y-axis of theta angle
*/ 


float ZAC_Sin(float x) {
  uint16_t tx = 0x3FFF - (uint16_t)(x * 10430.378350f);
  int16_t ix = tx;
  uint8_t fp = (tx >= 0x3FFF) && (tx <= 0xBFFF);
  ix = fp ? 0x7FFF - ix : ix;
  float t = ((float)ix);
  float x2 = t * t;
  float mx = (COSINE[0] * x2 + COSINE[1]) * x2 + COSINE[2];
  return fp ? -mx : mx;
}

float ZAC_Cos(float x) {
  uint16_t tx = (uint16_t)(x * 10430.378350f);
  int16_t ix = tx;
  uint8_t fp = (tx >= 0x3FFF) && (tx <= 0xBFFF);
  ix = fp ? 0x7FFF - ix : ix;
  float t = ((float)ix);
  float x2 = t * t;
  float mx = (COSINE[0] * x2 + COSINE[1]) * x2 + COSINE[2];
  return fp ? -mx : mx;
}


void ZAC_SinCos(float x, float *_s, float *_c) {
  /*
   convert to fixed point of (65536 / TAU)
   and use intentional overflow so we dont have to
   use remainder operation.
  */
  uint16_t tx = (uint16_t)(ZAC_Abs(x) * 10430.378350f);
  int16_t ix = (int16_t)tx;
  uint8_t fp = (tx >= 0x3FFF) && (tx <= 0xBFFF);
  ix = fp ? 0x7FFF - ix : ix;
  /*
   convert it back
   np_real t = ((np_real)ix) * 0.0000958738f;
  */
  float t = ((float)ix);

  float x2 = t * t;
  float mx = (COSINE[0] * x2 + COSINE[1]) * x2 + COSINE[2];
  float mx1 = ZAC_Sqrt(1.0f - mx * mx);
  *_c = fp ? -mx : mx;
  *_s = ((x < 0.0f) ^ (tx > 0x7FFF)) ? -mx1 : mx1;
}


float ZAC_Tan(float x) {
 float s, c;
 ZAC_SinCos(x, &s, &c);
 return s / c;
}


/*
 arc sine
          • (x, y) = normalized value
         /|
        / |
       /  |
      /   |
     /__  |
    /   | |
   •-------
   ^
  angle
 
  angle = asin(y)
*/

/*
 approximation

 acos(x) = acos(x)/sqrt(1.0 - x), interval : [0, 1]
 asin(x) = pi/2 - acos(x)
*/
static const float ASIN[3] = {
  0.46707077e-01f,
 -0.20212058e-00f,
  1.57021169e-00f,
};


float ZAC_Asin(float x) {
 float mx, mx1;
 mx = ZAC_Abs(x);
 
 if(mx > 1.0f) {
  return x < 0.0f ? -1.570796f : 1.570796f;
 }
 mx1 = ASIN[0] * mx + ASIN[1];
 mx1 = mx1 * mx + ASIN[2];
 mx1 = mx1 * ZAC_Sqrt(1.0f - mx);
 return (1.570796f - ((x < 0.0f) ? 3.14159f - mx1 : mx1));
}




/*
 arc cosine
          • (x, y) = normalized value
         /|
        / |
       /  |
      /   |
     /__  |
    /   | |
   •-------
   ^
  angle
 
  angle = acos(x)
*/
/*
 approxiamtion

 acos(x) = acos(x)/sqrt(1.0 - x), interval : [0, 1]
*/

static const float ACOS[2] = {
 -0.15658276e-00f,
  1.56496682e-00f,
};

float ZAC_Acos(float x) {
 float mx, mx1;
 mx = ZAC_Abs(x);
 
 if(mx > 1.0f) {
  return x < 0.0f ? 3.14159f : 0.0f;
 }
 mx1 = ACOS[0] * mx + ACOS[1];
 mx1 = mx1 * ZAC_Sqrt(1.0f - mx);
 return ((x < 0.0f) ? 3.14159f - mx1 : mx1);
}


/*
 arc tangent
          • (x, y) normalized value
         /|
        / |
       /  |
      /   |
     /__  |
    /   | |
   •-------
     ^
    angle
 
  angle = atan(y/x)
*/

static const float ATAN[2] = {
/*
 -6.038020e-02f,
  1.770501e-01f,
 -3.323025e-01f,
*/
  1.187314e-01f,
 -3.257921e-01f
};

/*
 # arc tangent
                     ________
 atan(x) = asin(x / √1 + x*x ) 

 • alternative (only works on positive x):
                     ________
 atan(x) = acos(1 / √1 + x*x )
*/

/*
 implementation
 
 for |x| <= 1.0
 
 atanPoly(x) = c0 * x + c1 * x² + c2 * x³....
 
 for |x| >= 1.0
 
 atan(x) pi_half - atanPoly(1.0 / x)
*/
float ZAC_Atan(float x) {
 float x2, mx, out;
 uint8_t hi;
 
 mx = ZAC_Abs(x);
 hi = mx > 1.0f;
 if(!hi) {
  x2 = mx * mx;
  mx = mx + (mx * x2) * (ATAN[0] * x2 + ATAN[1]);
  return x < 0.0f ? -mx : mx;
 }
 mx = 1.0f / mx;
 x2 = mx * mx;
 out = mx + (mx * x2) * (ATAN[0] * x2 + ATAN[1]);
 out = 1.570796f - out;
 return x < 0.0f ? -out : out;
}


float ZAC_Atan2(float y, float x) {
 if(x > 0.0f) {
  return ZAC_Atan(y / x);
 } else if(x < 0.0f) {
  return ZAC_Atan(y / x) + ((y >= 0.0f) ? 3.14159f : -3.14159f);
 } else {
  return (y > 0.0f) ? 1.570796f : -1.570796f;
 }
 /* unreachable */
 return 0.0f;
}



/*-------------------------------------------------------------------*/
/*
 exponential base 2 coeffs
 accurate enough for at lest 2-3 decimal digits.
*/
static const float EXP2[5] = {
 0.96857114e-02f,
 0.55922025e-01f,
 0.24021865e-00f,
 0.69312103e-00f,
 1.00000015e-00f,
};

/*

 
                exponential growth

                        /
                       |
                      /   
                     |
                    /
                ---*  <- point exactly to one
              __
    _______---      <- negative values are always below one

    exp2(x) = exp(x * ln(2))
*/
float ZAC_Exp2(float x) {
 float mx, a;
	float_bits bits;
 int32_t whole;

 if(ZAC_Abs(x) < 0.5f) {
 	a = x;
  return ((((EXP2[0] * a + EXP2[1]) * a + EXP2[2]) * a + EXP2[3]) * a + EXP2[4]);
 }
	a = x;
 whole = (int32_t)(a - 0.5f);
 a = a - ((float)whole);
 bits.i = (uint32_t)((int32_t)(127 + whole) << 23);
 mx = ((((EXP2[0] * a + EXP2[1]) * a + EXP2[2]) * a + EXP2[3]) * a + EXP2[4]);
 return bits.f * mx;
}

/*-------------------------------------------------------------------*/
/*
 log base 2 coeffs
 accurate enough for at lest 2-3 decimal digits.
*/
static const float LOG2[5] = {
 -8.161580e-02f,
  6.451423e-01f,
 -2.120675e-00f,
  4.070090e-00f,
 -2.512854e-00f,
};
/*

 
                                        ______
                                    __ -
  logarithmic growth ->         __ -
                            _ -
                           /
                          -
                         /
    udefined negatives  |
      -------|-------|--}* -> point exactly to one
                       /
                       | -> below 1.0 are negatives
                       /
   
-> gradually goes to zero with negative sign


  log2(x) = ln(x) / ln(2)
 
*/
float ZAC_Log2(float x) {
 float_bits mantissa;
 float_bits bits;
 float ma, mx;
 
 if(x < 1.0f) {
 	bits.f = 1.0f / x;
  mantissa.i = 1065353216U | (bits.i & 0x007FFFFF);
  ma = mantissa.f;
  mx = ((((LOG2[0] * ma + LOG2[1]) * ma + LOG2[2]) * ma + LOG2[3]) * ma + LOG2[4]);
  mx = (float)((int32_t)(bits.i >> 23) - 127) + mx;
  return -mx;
 } else if(x >= 1.0f && x < 2.0f) {
  ma = x;
  return ((((LOG2[0] * ma + LOG2[1]) * ma + LOG2[2]) * ma + LOG2[3]) * ma + LOG2[4]);
 }
 
 bits.f = x;
 mantissa.i = 1065353216U | (bits.i & 0x007FFFFF);
 ma = mantissa.f;
 mx = ((((LOG2[0] * ma + LOG2[1]) * ma + LOG2[2]) * ma + LOG2[3]) * ma + LOG2[4]);
 mx = (float)((int32_t)(bits.i >> 23) - 127) + mx;
 return mx;
}



float ZAC_Pow(float x, float y) {
 return ZAC_Exp2(y * ZAC_Log2(x));
}


float ZAC_Fmod(float x, float y) {
 float fdiv = (float) (int)(x / y);
 return x - fdiv * y;
}

