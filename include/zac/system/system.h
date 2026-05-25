#ifndef ZAC_SYSTEM_H
#define ZAC_SYSTEM_H

#include <stddef.h>

void ZAC_System_Panic(const char *fmt, ...);
void ZAC_System_TerminateSuccess(void);
void ZAC_System_TerminateFailure(void);

void* ZAC_System_AllocateMemory(size_t s);
void ZAC_System_FreeMemory(void *p);
void ZAC_System_CheckForLeak(void);


void* ZAC_System_ReadDataFromFile(const char* file, size_t *out_size);

#endif

