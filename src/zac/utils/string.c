#include "zac/utils/string.h"
#include <assert.h>
#include <string.h>


static const char *units_lut[4] = {
 "b",
 "kb",
 "mb",
 "gb",
};

static const float decimal_lut[10] = {
 1.0f,
 10.0f,
 100.0f,
 1000.0f,
 10000.0f,
 100000.0f,
 1000000.0f,
 10000000.0f,
 100000000.0f,
 1000000000.0f
};

/*
 unsafe strings, for simplicity, it just follows the other c string functions
 and rely on null terminations.
*/

static int str_to_int(char *str, int num, int d) {
 int i, sign, start, end;
 
 sign = num;
 i = 0;
 start = 0;
 
 if(num < 0) num = -num;
 do {
  str[i++] = (num % 10) + '0';
  num /= 10;
 } while(num > 0);


 while(i < d)
  str[i++] = '0';

 if(sign < 0)
  str[i++] = '-';

 
 end = i - 1;
 while(start < end) {
  char temp = str[start];
  str[start] = str[end];
  str[end] = temp;
  start++;
  end--;
 }

 str[i] = '\0';

 return i;
}



void ZAC_Itoa(char *str, int num) {
 (void)str_to_int(str, num, 0);
}




void ZAC_Ftoa(char *res, float n, uint8_t dec) {
 int ipart = (int)n;
 float fpart = n - (float)ipart;
 
 fpart = fpart < 0.0f ? -fpart : fpart;
 int i = str_to_int(res, ipart, 0);
 if(dec) {
  res[i] = '.';
  assert(dec <= 10);
  fpart = fpart * decimal_lut[dec];
  (void)str_to_int(res + i + 1, (int)fpart, dec);
 }
}



void ZAC_PrintSize(char *str, size_t s) {
 float fval = (float)s;
 int i = 0;
 
 while(fval > 1024) {
  fval /= 1024;
  i++;
 }
 
 ZAC_Ftoa(str, fval, 2);
 assert(i <= 4);
 strcat(str, units_lut[i]);
}


