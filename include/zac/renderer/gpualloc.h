#ifndef ZAC_GPUALLOC_H
#define ZAC_GPUALLOC_H

#include "zac/renderer/ctxrender.h"

typedef enum {
 ZAC_GPUMEMORY_LOCAL,
 ZAC_GPUMEMORY_SHARED,
} ZAC_GpuMemoryType;


typedef enum {
 ZAC_TEXTURE_FORMAT_RGBA,
 ZAC_TEXTURE_FORMAT_R,
} ZAC_TextureFormat;


typedef struct ZAC_Gpualloc ZAC_Gpualloc;
typedef struct ZAC_Texture ZAC_Texture;
typedef struct ZAC_Buffer ZAC_Buffer;


typedef struct {
 ZAC_Gpualloc *_memory;
 size_t _offset;
 size_t _size;
 size_t _non_coherent_size;
} ZAC_Gpuallocation;


#ifdef __ZAC_INTERNAL__

#include "volk.h"

/* stack based allocator */
struct ZAC_Gpualloc {
 VkDeviceMemory _memory;
 void *_mapped;
 size_t _max_size;
 size_t _current_size;
 ZAC_GpuMemoryType _type;
};

struct ZAC_Texture {
 VkImage _image;
 VkImageView _image_view;
 ZAC_Gpuallocation _allocation;
};

struct ZAC_Buffer {
 VkBuffer _buffer;
 ZAC_Gpuallocation _allocation;
};

#endif

ZAC_Gpualloc* ZAC_Gpualloc_Init(ZAC_Ctxrender *ctx, size_t page_size, ZAC_GpuMemoryType memtype);
void ZAC_Gpualloc_Invalidate(ZAC_Ctxrender *ctx, ZAC_Gpuallocation alloc);
void ZAC_Gpualloc_Flush(ZAC_Ctxrender *ctx, ZAC_Gpuallocation alloc);
void* ZAC_Gpualloc_Map(ZAC_Gpuallocation alloc);
void ZAC_Gpualloc_PopInto(ZAC_Gpualloc *gpualloc, size_t n);
ZAC_Buffer* ZAC_Gpualloc_PushBindBuffer(ZAC_Ctxrender *ctx, ZAC_Gpualloc *gpualloc, size_t buff_size, void *data);
ZAC_Texture* ZAC_Gpualloc_PushBindTexture(ZAC_Ctxrender *ctx, ZAC_Gpualloc *gpualloc, uint16_t w, uint16_t h, ZAC_TextureFormat format, void* data);

void ZAC_Gpualloc_Destroy(ZAC_Ctxrender *ctx, ZAC_Gpualloc *gpualloc);

ZAC_Gpuallocation ZAC_Texture_GetAllocation(ZAC_Texture *texture);
ZAC_Gpuallocation ZAC_Buffer_GetAllocation(ZAC_Buffer *buffer);

void ZAC_Texture_Destroy(ZAC_Ctxrender *ctx, ZAC_Texture *texture);
void ZAC_Buffer_Destroy(ZAC_Ctxrender *ctx, ZAC_Buffer *buffer);

#endif

