#ifndef ZAC_GUI_PANNEL_PIPELINE_H
#define ZAC_GUI_PANNEL_PIPELINE_H

#include "zac/renderer/ctxrender.h"
#include "zac/renderer/pipelines.h"

void __ZAC_CreateGuiPannelPipeline(ZAC_Ctxrender *ctx, ZAC_Pipelines *p, uintptr_t shaders[2], size_t shader_size[2], size_t pc_size);

#endif


