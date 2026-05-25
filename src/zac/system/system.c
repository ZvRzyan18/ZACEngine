#include "zac/system/system.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdatomic.h>
#include <stdio.h>
#include <errno.h>

static atomic_int memory_counter = ATOMIC_VAR_INIT(0);

void ZAC_System_Panic(const char *fmt, ...) {
 va_list args;
 va_start(args, fmt);
 vfprintf(stderr, fmt, args);
 va_end(args);
 ZAC_System_TerminateFailure();
}


void ZAC_System_TerminateSuccess(void) {
 exit(EXIT_SUCCESS);
}


void ZAC_System_TerminateFailure(void) {
 exit(EXIT_FAILURE);
}



void* ZAC_System_AllocateMemory(size_t s) {
 if(s == 0)
  return NULL;
 atomic_fetch_add(&memory_counter, 1);
 return malloc(s);
}


void ZAC_System_FreeMemory(void *p) {
 if(p == NULL)
  return;
 atomic_fetch_sub(&memory_counter, 1);
 free(p);
}


void ZAC_System_CheckForLeak(void) {
 int i = atomic_load(&memory_counter);
 if(i > 0) {
  ZAC_System_Panic("Memory Leak [%i].", i);
 } else if(i < 0) {
  ZAC_System_Panic("Multi Free [%i].", i);
 }
}



void* ZAC_System_ReadDataFromFile(const char* file, size_t *out_size) {
 FILE *fp = fopen(file, "rb");

 if(fp == NULL) {
 switch (errno) {
  case ENOENT:
   ZAC_System_Panic("file \'%s\' not found.", file);
  break;
  case EACCES:
   ZAC_System_Panic("could not open \'%s\', permission denied.", file);
  break;
  default:
   ZAC_System_Panic("could not open \'%s\'.", file);
  }
 }
 
 fseek(fp, 0, SEEK_END);
 size_t fsize = ftell(fp);
 fseek(fp, 0, SEEK_SET);
 
 void *buffer = ZAC_System_AllocateMemory(fsize);
 
 fread(buffer, 1, fsize, fp);
 fclose(fp);
 
 if(out_size != NULL)
  *out_size = fsize;
 
 return buffer;
}
