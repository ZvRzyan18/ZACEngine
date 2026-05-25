#define __ZAC_INTERNAL__

#include "zac/system/font.h"
#include "zac/system/system.h"
#include "zac/math/math.h"
#include <string.h>
#include <assert.h>

#include "stb_truetype.h"


ZAC_Font* ZAC_Font_Init(const char *fpath, uint32_t quality) {
 ZAC_Font *font = (ZAC_Font*)ZAC_System_AllocateMemory(sizeof(ZAC_Font));
 memset(font, 0, sizeof(ZAC_Font));
 
 assert(quality != 0);
 
 
 font->_glyph_info = (ZAC_GlyphInfo*)ZAC_System_AllocateMemory(sizeof(ZAC_GlyphInfo) * 95);
 font->_glyph_info_size = 95;


 size_t fsize;
 uint8_t *font_buffer = ZAC_System_ReadDataFromFile(fpath, &fsize);
 
 stbtt_fontinfo info;
 if(!stbtt_InitFont(&info, font_buffer, stbtt_GetFontOffsetForIndex(font_buffer, 0))) {
  ZAC_System_Panic("ZAC_Font_Init() : failed init of font info");
 }

 float f_scale = stbtt_ScaleForPixelHeight(&info, quality);
 int ascent, descent, lineGap;
 stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
 

 float tmp_width = 0;
 
 for(int i = 0; i < 95; i++) {
  ZAC_GlyphInfo fa;
  int char_index = i + 33;

  int c_x1, c_y1, c_x2, c_y2;
  stbtt_GetCodepointBitmapBox(&info, char_index, f_scale, f_scale, &c_x1, &c_y1, &c_x2, &c_y2);

  int ax; int lsb;
  stbtt_GetCodepointHMetrics(&info, char_index, &ax, &lsb);

  font->_resolution_width += (uint32_t) ( ZAC_Abs((float)(c_x2 - c_x1)) + 1.0f);
   
  fa._rect[2] = ZAC_Abs((float)(c_x2 - c_x1)) + 1.0f;
  fa._rect[3] = ZAC_Abs((float)(c_y2 - c_y1)) + 1.0f;
  fa._rect[0] = tmp_width;
  fa._rect[1] = ZAC_Ceil((float)(ascent) * f_scale + (float)(c_y2 + c_y1));
   
  fa._lsb = ZAC_Ceil(lsb * f_scale);
  fa._aw = ZAC_Ceil(ax * f_scale);  
  fa._ah = ZAC_Ceil(ZAC_Abs((float)(descent - ascent)) * f_scale);

  font->_glyph_info[i] = fa;   
  tmp_width += ZAC_Abs((float)(c_x2 - c_x1)) + 1.0f;
 }

 font->_resolution_height = quality;
 font->_resolution_width = (uint32_t)ZAC_Round(tmp_width);
 
 font->_raw_data_bytes = (uint8_t*)ZAC_System_AllocateMemory(font->_resolution_width * font->_resolution_height);



 stbtt_packedchar* stb_pack = (stbtt_packedchar*)ZAC_System_AllocateMemory(sizeof(stbtt_packedchar) * info.numGlyphs); 

 stbtt_pack_context stb_ctx;
 stbtt_PackBegin(&stb_ctx, font->_raw_data_bytes, font->_resolution_width, font->_resolution_height, 0, 1, NULL);
 stbtt_PackFontRange(&stb_ctx, font_buffer, 0, font->_resolution_height, 33, 95, stb_pack);
 stbtt_PackEnd(&stb_ctx);
  
 ZAC_System_FreeMemory(stb_pack);
 ZAC_System_FreeMemory(font_buffer);

 return font;
}




ZAC_Texture* ZAC_Font_GetTexture(ZAC_Ctxrender *ctx, ZAC_Gpualloc *alloc, ZAC_Font *font) {
/* if(ctx->_is_unorm && !font->_filtered) {
  for(uint32_t i = 0; i < (font->_resolution_width * font->_resolution_height); i++) {
   font->_raw_data_bytes[i] = ZAC_ToLinear_Uint(font->_raw_data_bytes[i]);
  }
  font->_filtered = 1;
 } else if(!ctx->_is_unorm && !font->_filtered) {
  for(uint32_t i = 0; i < (font->_resolution_width * font->_resolution_height); i++) {
   font->_raw_data_bytes[i] = ZAC_ToSRGB_Uint(font->_raw_data_bytes[i]);
  }
  font->_filtered = 1;
 }*/
 
 if(!font->_filtered) {
  for(uint32_t i = 0; i < (font->_resolution_width * font->_resolution_height); i++) {
   font->_raw_data_bytes[i] = ZAC_ToSRGB_Uint(font->_raw_data_bytes[i]);
  }
  font->_filtered = 1;
 }
 return ZAC_Gpualloc_PushBindTexture(ctx, alloc, font->_resolution_width, font->_resolution_height, ZAC_TEXTURE_FORMAT_R, font->_raw_data_bytes);
}



ZAC_Font_Info ZAC_Font_GetMetricsOf(ZAC_Font *font, ZAC_Vec2 *advance_position, ZAC_FontIterationFlag *out_flag, float font_size, char c) {
 ZAC_Font_Info font_info;
 memset(&font_info, 0, sizeof(ZAC_Font_Info));
 
 if(c == '\0') {
  *out_flag = ZAC_FONT_ITERATION_FLAG_BREAK;
  return font_info;
 }
 
 if(c == ' ') {
  *out_flag = ZAC_FONT_ITERATION_FLAG_CONTINUE;
  advance_position->x += font->_glyph_info[3]._aw;
  return font_info;
 }
  
 if(c == '\n') {
  *out_flag = ZAC_FONT_ITERATION_FLAG_CONTINUE;
  advance_position->x = 0.0f;
  advance_position->y -= font->_glyph_info[0]._ah * 2; // + line spacing
  return font_info;
 }
 
 *out_flag = ZAC_FONT_ITERATION_FLAG_NONE;
 
 size_t glyph_index = ((size_t)c) - 33;
 
 assert(glyph_index < 95);
 ZAC_GlyphInfo *metrics = font->_glyph_info + glyph_index;
 
 font_info.uv_rect.x = metrics->_rect[0] / (float)(font->_resolution_width);
 font_info.uv_rect.y = 0.0f;
 font_info.uv_rect.z = metrics->_rect[2] / (float)(font->_resolution_width);
 font_info.uv_rect.w = metrics->_rect[3] / (float)(font->_resolution_height);

 float fscale = font_size / (float)(font->_resolution_height);

 ZAC_Vec2 transformed_scale = {metrics->_rect[2] * fscale, metrics->_rect[3] * fscale};

 font_info.transform.z = transformed_scale.x;
 font_info.transform.w = transformed_scale.y;

 ZAC_Vec2 transformed_position;
 transformed_position.x = (advance_position->x + metrics->_rect[2] + metrics->_lsb) * fscale;
 transformed_position.y = (advance_position->y - metrics->_rect[1]) * fscale; 
 
 font_info.transform.x = transformed_position.x;
 font_info.transform.y = transformed_position.y;
 advance_position->x += metrics->_aw + metrics->_rect[2];

 return font_info;
}



void ZAC_Font_Destroy(ZAC_Font *font) {
 ZAC_System_FreeMemory(font->_glyph_info);
 ZAC_System_FreeMemory(font->_raw_data_bytes);
 ZAC_System_FreeMemory(font);
}


