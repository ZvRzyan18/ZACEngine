#ifndef ZAC_PIPELINES_H
#define ZAC_PIPELINES_H

#include "zac/renderer/ctxrender.h"
#include "zac/math/vec2.h"
#include "zac/math/vec3.h"
#include <stdalign.h>

typedef struct ZAC_Pipelines ZAC_Pipelines;

#ifdef __ZAC_INTERNAL__

#include "volk.h"
/* collection of pre baked pipelines */
struct ZAC_Pipelines {
 VkPipeline gui_pannel_pipeline;
 VkPipelineLayout gui_pannel_pipeline_layout;

 VkPipeline gui_text_pipeline;
 VkPipelineLayout gui_text_pipeline_layout;

 VkPipeline gui_textured_pannel_pipeline;
 VkPipelineLayout gui_textured_pannel_pipeline_layout;


 VkPipeline textured_3d_pipeline;
 VkPipelineLayout textured_3d_pipeline_layout;
};

#endif


typedef struct {
 alignas(16) ZAC_Vec3 position;
 alignas(16) ZAC_Vec3 normal;
 alignas(8) ZAC_Vec2 uv;
} ZAC_Vertex;


typedef struct {
  alignas(16) ZAC_Mat4x4 transform;
  alignas(16) ZAC_Vec4 color;
} ZAC_GuiPC;

typedef struct {
 alignas(16) ZAC_Vec4 transform;
 alignas(16) ZAC_Vec4 uv_rect;
} ZAC_Font_Info;


ZAC_Pipelines* ZAC_Pipelines_Init(ZAC_Ctxrender *ctx, const char* dir);
void ZAC_Pipelines_Destroy(ZAC_Ctxrender *ctx, ZAC_Pipelines *p);

#endif

