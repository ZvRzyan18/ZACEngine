#ifndef ZAC_GUI_TEXT_PIPELINE_H
#define ZAC_GUI_TEXT_PIPELINE_H

#include "zac/renderer/ctxrender.h"
#include "zac/renderer/pipelines.h"


void __ZAC_CreateGuiTextPipeline(ZAC_Ctxrender *ctx, ZAC_Pipelines *p, uintptr_t shaders[2], size_t shader_size[2], size_t pc_size, size_t offsets[2], size_t vertex_stride);

#endif

