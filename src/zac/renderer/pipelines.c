#define __ZAC_INTERNAL__

#include "zac/renderer/pipelines.h"
#include "zac/pipelines/gui_pannel_pipeline.h"
#include "zac/pipelines/gui_text_pipeline.h"
#include "zac/pipelines/gui_textured_pannel_pipeline.h"
#include "zac/system/system.h"
#include <string.h>


static void* ReadShaderData(const char* dir, const char* assets_path, size_t *size) {
 char tmp_buff[512];
 /* NOTE : UNSAFE !!!!! */
 /* migh cause overflow */
 strcpy(tmp_buff, dir);
 strcat(tmp_buff, assets_path);
 return ZAC_System_ReadDataFromFile(tmp_buff, size);
}




ZAC_Pipelines* ZAC_Pipelines_Init(ZAC_Ctxrender *ctx, const char *dir) {
 ZAC_Pipelines *pipelines = (ZAC_Pipelines*)ZAC_System_AllocateMemory(sizeof(ZAC_Pipelines));
 memset(pipelines, 0, sizeof(ZAC_Pipelines));
 
 uintptr_t shaders[2];
 size_t shader_size[2];



 {
  shaders[0] = (uintptr_t)ReadShaderData(dir, "shaders/gui_pannel.vert.spv", &shader_size[0]);
  shaders[1] = (uintptr_t)ReadShaderData(dir, "shaders/gui_pannel.frag.spv", &shader_size[1]);
 
  __ZAC_CreateGuiPannelPipeline(ctx, pipelines, shaders, shader_size, sizeof(ZAC_GuiPC));
 
  ZAC_System_FreeMemory((void*)shaders[0]);
  ZAC_System_FreeMemory((void*)shaders[1]);
 }



 {
  shaders[0] = (uintptr_t)ReadShaderData(dir, "shaders/gui_text.vert.spv", &shader_size[0]);
  shaders[1] = (uintptr_t)ReadShaderData(dir, "shaders/gui_text.frag.spv", &shader_size[1]);
 
  size_t offsets[2];
  offsets[0] = offsetof(ZAC_Font_Info, transform);
  offsets[1] = offsetof(ZAC_Font_Info, uv_rect);
  __ZAC_CreateGuiTextPipeline(ctx, pipelines, shaders, shader_size, sizeof(ZAC_GuiPC), 
  offsets, sizeof(ZAC_Font_Info));
 
  ZAC_System_FreeMemory((void*)shaders[0]);
  ZAC_System_FreeMemory((void*)shaders[1]);
 }

 {

  shaders[0] = (uintptr_t)ReadShaderData(dir, "shaders/gui_textured_pannel.vert.spv", &shader_size[0]);
  shaders[1] = (uintptr_t)ReadShaderData(dir, "shaders/gui_textured_pannel.frag.spv", &shader_size[1]);

  __ZAC_CreateTexturedPannelPipeline(ctx, pipelines, shaders, shader_size, sizeof(ZAC_GuiPC));

  ZAC_System_FreeMemory((void*)shaders[0]);
  ZAC_System_FreeMemory((void*)shaders[1]);
 }
 return pipelines;
}




void ZAC_Pipelines_Destroy(ZAC_Ctxrender *ctx, ZAC_Pipelines *p) {

 vkDestroyPipeline(ctx->_device, p->gui_textured_pannel_pipeline, NULL);
 vkDestroyPipelineLayout(ctx->_device, p->gui_textured_pannel_pipeline_layout, NULL);

 vkDestroyPipeline(ctx->_device, p->gui_text_pipeline, NULL);
 vkDestroyPipelineLayout(ctx->_device, p->gui_text_pipeline_layout, NULL);

 vkDestroyPipeline(ctx->_device, p->gui_pannel_pipeline, NULL);
 vkDestroyPipelineLayout(ctx->_device, p->gui_pannel_pipeline_layout, NULL);
 
 ZAC_System_FreeMemory(p);
}
