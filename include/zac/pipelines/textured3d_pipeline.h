#ifndef ZAC_TEXTURED3D_PIPELINE_H
#define ZAC_TEXTURED3D_PIPELINE_H

#include "zac/renderer/pipelines.h"

void __ZAC_CreateTextured3DPipeline(ZAC_Ctxrender *ctx, ZAC_Pipelines *p, uintptr_t shaders[2], size_t shader_size[2], size_t vertex_size, size_t vertex_offsets[3]);


#endif

