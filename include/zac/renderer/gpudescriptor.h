#ifndef ZAC_GPUDESCRIPTOR_H
#define ZAC_GPUDESCRIPTOR_H

#include "zac/renderer/ctxrender.h"
#include "zac/renderer/gpualloc.h"

typedef struct ZAC_Gpudescriptor ZAC_Gpudescriptor;

#ifdef __ZAC_INTERNAL__

#include "volk.h"

struct ZAC_Gpudescriptor {
 VkDescriptorPool _textured_pool; 
 VkDescriptorPool _ubo_pool;
};

#endif

typedef enum {
 ZAC_GPUDESCRIPTOR_ALLOCATE_TEXTURES, 
 ZAC_GPUDESCRIPTOR_ALLOCATE_UBO,
} ZAC_GpudescriptorAllocateType;

typedef enum {
 ZAC_GPUDESCRIPTOR_SAMPLER_NONE,
 ZAC_GPUDESCRIPTOR_SAMPLER_LINEAR,
 ZAC_GPUDESCRIPTOR_SAMPLER_NEAREST,
} ZAC_GpudescriptorSamplerType;


ZAC_Gpudescriptor* ZAC_Gpudescriptor_Init(ZAC_Ctxrender *ctx, size_t max_textured, size_t max_ubo);
uintptr_t ZAC_Gpudescriptor_Allocate(ZAC_Ctxrender *ctx, ZAC_Gpudescriptor *d, ZAC_GpudescriptorAllocateType allocate_type);
void ZAC_Gpudescriptor_Deallocate(ZAC_Ctxrender *ctx, ZAC_Gpudescriptor *d, ZAC_GpudescriptorAllocateType allocate_type, uintptr_t ds);
void ZAC_Gpudescriptor_Destroy(ZAC_Ctxrender *ctx, ZAC_Gpudescriptor *d);
/* they are fixed to 8 elements array*/
void ZAC_Gpudescriptor_BindTextures(ZAC_Ctxrender *ctx, uintptr_t descriptor, ZAC_Texture **textures, ZAC_GpudescriptorSamplerType *samplers);
void ZAC_Gpudescriptor_BindUBO(ZAC_Ctxrender *ctx, uintptr_t descriptor, ZAC_Buffer *ubo);

#endif

