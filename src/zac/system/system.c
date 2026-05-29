#include "zac/system/system.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdatomic.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "stb_image.h"

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



void ZAC_System_ReadImage(const char* file, void **pixels, uint16_t *w, uint16_t *h, uint8_t *channels) {
 int c, ww, hh, manual_copy;
 uint8_t *data = (uint8_t*)stbi_load(file, &ww, &hh, &c, 0);
 
 manual_copy = 0;
 if(c == 1) {
  c = 1;
  manual_copy = 0;
 } else if(c == 3) {
  c = 4;
  manual_copy = 1;
 } else if(c == 4) {
  c = 4;
  manual_copy = 0;
 }
 *pixels = ZAC_System_AllocateMemory(ww * hh * c);

 if(manual_copy) {
  for(int i = 0; i < (ww * hh); i++) {
   uint8_t *pp = (uint8_t*)*pixels;
   
   pp[i * 4 + 0] = data[i * 3 + 0];
   pp[i * 4 + 1] = data[i * 3 + 1];
   pp[i * 4 + 2] = data[i * 3 + 2];
   pp[i * 4 + 3] = 0xFF;
  }
 } else {
  memcpy(*pixels, data, ww * hh * c);
 }
 
 *w = (uint16_t)ww;
 *h = (uint16_t)hh;
 
 if(channels != NULL)
  *channels = (uint8_t)c;

 stbi_image_free(data);
}

