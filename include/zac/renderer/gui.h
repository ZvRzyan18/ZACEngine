#ifndef ZAC_GUI_H
#define ZAC_GUI_H

#include "zac/renderer/pipelines.h"
#include "zac/renderer/gpualloc.h"
#include "zac/math/mat4x4.h"
#include "zac/system/font.h"
#include <stdint.h>


typedef struct {
 ZAC_Mat4x4 transform;
 ZAC_Vec4 color;
 ZAC_Vec4 clipper;
} ZAC_GuiPannel;

void ZAC_GuiPannel_Draw(ZAC_Ctxrender *ctx, ZAC_Pipelines *p, ZAC_GuiPannel *gui, const ZAC_Mat4x4 *proj);


/*
 TODO : Use multi instance for drawing text to minimize the cpu overhead
*/
typedef struct {
 ZAC_Mat4x4 transform;
 ZAC_Vec4 color;
 ZAC_Vec4 clipper;
 
 ZAC_Buffer *buffer[2];
 size_t max_chars;
 uintptr_t descriptor;
 float font_size;
 float line_spacing;
} ZAC_GuiText;

void ZAC_GuiText_Init(ZAC_Ctxrender *ctx, ZAC_GuiText *txt, ZAC_Gpualloc *alloc, size_t max_chars, uintptr_t texture_desc);
void ZAC_GuiText_Destroy(ZAC_Ctxrender *ctx, ZAC_GuiText *txt);
void ZAC_GuiText_Draw(ZAC_Ctxrender *ctx, ZAC_Pipelines *p, ZAC_GuiText *txt, const ZAC_Mat4x4 *proj, ZAC_Font *fnt, const char* str, size_t str_len);

#endif

