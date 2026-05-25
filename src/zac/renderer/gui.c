#define __ZAC_INTERNAL__

#include "zac/renderer/gui.h"
#include "zac/math/geometry.h"
#include "zac/math/math.h"
#include "zac/system/system.h"
#include <stdalign.h>
#include <string.h>
#include <assert.h>

/*
 gui pannel 
*/

void ZAC_GuiPannel_Draw(ZAC_Ctxrender *ctx, ZAC_Pipelines *p, ZAC_GuiPannel *gui, const ZAC_Mat4x4 *proj) {
 VkCommandBuffer cmd = __ZAC_Ctxrenderer_AquireCmdBuffer(ctx);
 
 ZAC_GuiPC _data_pc;

	_data_pc.color = gui->color;
	_data_pc.transform = ZAC_Mat4x4_Mul(*proj, gui->transform);
	
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, p->gui_panel_pipeline);
 vkCmdPushConstants(cmd, p->gui_panel_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(_data_pc), &_data_pc);

	vkCmdSetViewport(cmd, 0, 1, &ctx->_viewport);
 
 if(gui->clipper.x == gui->clipper.x) {
  ZAC_Vec2 size = {(float)ctx->_screen_size.width, (float)ctx->_screen_size.height};
  ZAC_Vec4 v = ZAC_Mat4x4_UnprojectRect(gui->clipper, ctx->_projection_2d, size);
  VkRect2D scissor;
  scissor.offset.x = (uint32_t)ZAC_Round(v.x);
  scissor.offset.y = (uint32_t)ZAC_Round(v.y);
  scissor.extent.width = (uint32_t)ZAC_Round(v.z);
  scissor.extent.height = (uint32_t)ZAC_Round(v.w);

  vkCmdSetScissor(cmd, 0, 1, &scissor);
 } else {
  vkCmdSetScissor(cmd, 0, 1, &ctx->_scissor);
 }
 vkCmdDraw(cmd, 6, 1, 0, 0);
}




/*
 gui text
*/



void ZAC_GuiText_Init(ZAC_Ctxrender *ctx, ZAC_GuiText *txt, ZAC_Gpualloc *alloc, size_t max_chars, uintptr_t descriptor) {
 memset(txt, 0, sizeof(ZAC_GuiText));

 txt->max_chars = max_chars;
 txt->descriptor = descriptor;
 txt->buffer[0] = ZAC_Gpualloc_PushBindBuffer(ctx, alloc, sizeof(ZAC_Font_Info) * max_chars, NULL);
 txt->buffer[1] = ZAC_Gpualloc_PushBindBuffer(ctx, alloc, sizeof(ZAC_Font_Info) * max_chars, NULL);

 txt->transform = ZAC_Mat4x4_Identity;
 txt->font_size = 3.0f;
 txt->line_spacing = 10;
 txt->clipper = ZAC_Stv4(ZAC_Nan());
}


void ZAC_GuiText_Destroy(ZAC_Ctxrender *ctx, ZAC_GuiText *txt) {
 ZAC_Buffer_Destroy(ctx, txt->buffer[0]);
 ZAC_Buffer_Destroy(ctx, txt->buffer[1]);
}


void ZAC_GuiText_Draw(ZAC_Ctxrender *ctx, ZAC_Pipelines *p, ZAC_GuiText *txt, const ZAC_Mat4x4 *proj, ZAC_Font *fnt, const char* str, size_t str_len) {
/*
 str_len is included so we dont have to use strlen everyframe 
*/
 VkCommandBuffer cmd = __ZAC_Ctxrenderer_AquireCmdBuffer(ctx);

 ZAC_FontIterationFlag flag;
 ZAC_Vec2 advance_position;
 
 advance_position.x = 0.0f;
 advance_position.y = 0.0f;


 ZAC_Buffer* current_buffer = txt->buffer[ctx->_current_frame];
 ZAC_Gpuallocation allocation = ZAC_Buffer_GetAllocation(current_buffer);
 ZAC_Font_Info *vertex_info = (ZAC_Font_Info*)ZAC_Gpualloc_Map(allocation);

 size_t final_size = 0;
 for(int i = 0; i < str_len; i++) {
  ZAC_Font_Info info = ZAC_Font_GetMetricsOf(fnt, &advance_position, &flag, txt->font_size, str[i]);
  
  if(flag == ZAC_FONT_ITERATION_FLAG_CONTINUE) {
   continue;
  }
  
  if(flag == ZAC_FONT_ITERATION_FLAG_BREAK) {
   break;
  }
  
  memcpy(&vertex_info[final_size++], &info, sizeof(ZAC_Font_Info)); 
 }
 
 ZAC_Gpualloc_Flush(ctx, allocation);
 
 ZAC_GuiPC pc;
 pc.color = txt->color;
	pc.transform = ZAC_Mat4x4_Mul(*proj, txt->transform);

 const VkDescriptorSet dc = (VkDescriptorSet)txt->descriptor;
 
 uint32_t _set = 0;
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, p->gui_text_pipeline);
 vkCmdPushConstants(cmd, p->gui_text_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ZAC_GuiPC), &pc);
 vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, p->gui_text_pipeline_layout, _set, 1, &dc, 0, NULL);

 VkBuffer buffers = current_buffer->_buffer;
 VkDeviceSize offsets = 0;
 vkCmdBindVertexBuffers(cmd, 0, 1, &buffers, &offsets);

 if(txt->clipper.x == txt->clipper.x) {
  ZAC_Vec2 size = {(float)ctx->_screen_size.width, (float)ctx->_screen_size.height};
  ZAC_Vec4 v = ZAC_Mat4x4_UnprojectRect(txt->clipper, ctx->_projection_2d, size);
  VkRect2D scissor;
  scissor.offset.x = (uint32_t)ZAC_Round(v.x);
  scissor.offset.y = (uint32_t)ZAC_Round(v.y);
  scissor.extent.width = (uint32_t)ZAC_Round(v.z);
  scissor.extent.height = (uint32_t)ZAC_Round(v.w);

  vkCmdSetScissor(cmd, 0, 1, &scissor);
 } else {
  vkCmdSetScissor(cmd, 0, 1, &ctx->_scissor);
 }
	vkCmdSetViewport(cmd, 0, 1, &ctx->_viewport);
 vkCmdDraw(cmd, 6, final_size, 0, 0);

}



