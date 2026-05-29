#ifndef ZAC_SYSTEM_H
#define ZAC_SYSTEM_H

#include <stddef.h>
#include <stdint.h>

void ZAC_System_Panic(const char *fmt, ...);
void ZAC_System_TerminateSuccess(void);
void ZAC_System_TerminateFailure(void);

void* ZAC_System_AllocateMemory(size_t s);
void ZAC_System_FreeMemory(void *p);
void ZAC_System_CheckForLeak(void);


void* ZAC_System_ReadDataFromFile(const char* file, size_t *out_size);
void ZAC_System_ReadImage(const char* file, void **pixels, uint16_t *w, uint16_t *h, uint8_t *channels);

#endif

