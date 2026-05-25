#ifndef ZAC_FONT_H
#define ZAC_FONT_H

#include "zac/renderer/ctxrender.h"
#include "zac/renderer/pipelines.h"
#include "zac/renderer/gpualloc.h"
#include "zac/math/vec4.h"
#include "zac/math/vec2.h"
#include <stdalign.h>
#include <stdint.h>

/*
 FONT SYSTEM that only support ascii characters
*/


typedef struct ZAC_Font ZAC_Font;

typedef enum {
 ZAC_FONT_ITERATION_FLAG_NONE,
 ZAC_FONT_ITERATION_FLAG_CONTINUE,
 ZAC_FONT_ITERATION_FLAG_BREAK,
} ZAC_FontIterationFlag;

#ifdef __ZAC_INTERNAL__

typedef struct {
 float _rect[4];
 float _lsb;
 float _aw, _ah;
} ZAC_GlyphInfo;

struct ZAC_Font {
 /* raw image data*/
 uint8_t *_raw_data_bytes;
 
 /* glyph metrics LUT */
 ZAC_GlyphInfo *_glyph_info;
 size_t _glyph_info_size;
 
 uint32_t _resolution_width;
 uint32_t _resolution_height;
 uint8_t _filtered;
};

#endif


ZAC_Font* ZAC_Font_Init(const char *fpath, uint32_t quality);
ZAC_Texture* ZAC_Font_GetTexture(ZAC_Ctxrender *ctx, ZAC_Gpualloc *alloc, ZAC_Font *font);
ZAC_Font_Info ZAC_Font_GetMetricsOf(ZAC_Font *font, ZAC_Vec2 *advance_position, ZAC_FontIterationFlag *out_flag, float font_size, char c);
void ZAC_Font_Destroy(ZAC_Font *font);


#endif

