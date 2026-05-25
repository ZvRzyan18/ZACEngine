#ifndef ZAC_STRING_H
#define ZAC_STRING_H

#include <stdint.h>
#include <stddef.h>

void ZAC_Itoa(char *str, int num);
void ZAC_Ftoa(char *res, float n, uint8_t dec);

void ZAC_PrintSize(char *str, size_t s);


#endif


