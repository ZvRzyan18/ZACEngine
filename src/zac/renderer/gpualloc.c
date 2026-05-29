#define __ZAC_INTERNAL__

#include "zac/renderer/gpualloc.h"
#include "zac/renderer/ctxrender.h"
#include "zac/renderer/renderhelper.h"
#include "zac/system/system.h"
#include "zac/math/math.h"
#include <string.h>
#include <assert.h>


/*
 stack based gpu allocator
*/


/*
 static functions, or some boilerplate wrappers
*/

static VkBuffer create_buffer(ZAC_Ctxrender *ctx, size_t buff_size) {
 VkBufferCreateInfo buffer_create_info;
 memset(&buffer_create_info, 0, sizeof(VkBufferCreateInfo));
 buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
 buffer_create_info.size = buff_size;
 buffer_create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
 buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
 VkBuffer buffer;
 if(vkCreateBuffer(ctx->_device, &buffer_create_info, NULL, &buffer) != VK_SUCCESS) {
  ZAC_System_Panic("Failed to create buffer!");
 }
 return buffer;
}


static VkImage create_image(ZAC_Ctxrender *ctx, uint16_t w, uint16_t h, ZAC_TextureFormat format, VkFormat img_format) {
 VkImageCreateInfo image_create_info;
 memset(&image_create_info, 0, sizeof(VkImageCreateInfo));
 image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
 image_create_info.imageType = VK_IMAGE_TYPE_2D;
 image_create_info.extent.width = w;
 image_create_info.extent.height = h;
 image_create_info.extent.depth = 1;
 image_create_info.mipLevels = 1;
 image_create_info.arrayLayers = 1;
 image_create_info.format = img_format;
 
 image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;

 image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
 image_create_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
 image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
 image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
 image_create_info.flags = 0;

 VkImage image;
 if(vkCreateImage(ctx->_device, &image_create_info, NULL, &image) != VK_SUCCESS) {
  ZAC_System_Panic("create_image() : failed swapchain depth image view creation ");
 }
 return image;
}


static VkImageView create_image_view(ZAC_Ctxrender *ctx, VkImage image, ZAC_TextureFormat format, VkFormat img_format) {
 VkImageViewCreateInfo image_view_create_info;
 memset(&image_view_create_info, 0, sizeof(VkImageViewCreateInfo));
 image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
 image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
 image_view_create_info.format = img_format;
 
 image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
 image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
 image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
 image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

 image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
 image_view_create_info.subresourceRange.baseMipLevel = 0;
 image_view_create_info.subresourceRange.levelCount = 1;
 image_view_create_info.subresourceRange.baseArrayLayer = 0;
 image_view_create_info.subresourceRange.layerCount = 1;
  
 VkImageView image_view;
 image_view_create_info.image = image;
 if(vkCreateImageView(ctx->_device, &image_view_create_info, NULL, &image_view) != VK_SUCCESS) {
  ZAC_System_Panic("create_image_view() : failed swapchain depth image view creation ");
 }
 return image_view;
}



static void buffer_staging(ZAC_Ctxrender *ctx, ZAC_Gpuallocation allocation, VkBuffer dst, void *data, size_t buff_size) {
 switch(allocation._memory->_type) {
  case ZAC_GPUMEMORY_LOCAL:
  {

   VkBufferCreateInfo buffer_create_info;
   memset(&buffer_create_info, 0, sizeof(VkBufferCreateInfo));
   buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
   buffer_create_info.size = buff_size;
   buffer_create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
   buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
   VkBuffer tmp_buffer;
   if(vkCreateBuffer(ctx->_device, &buffer_create_info, NULL, &tmp_buffer) != VK_SUCCESS) {
    ZAC_System_Panic("Failed to create buffer!");
   }

 
   VkMemoryRequirements requirements;
   vkGetBufferMemoryRequirements(ctx->_device, tmp_buffer, &requirements);

   assert(requirements.size < ctx->_staging_memory_size);
   vkBindBufferMemory(ctx->_device, tmp_buffer, ctx->_staging_memory, 0);
  
   memcpy(ctx->_mapped_staging_memory, data, buff_size);
   
   VkMappedMemoryRange range;
   range.pNext = NULL;
   range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
   range.memory = ctx->_staging_memory;
   range.offset = 0;
   range.size = ZAC_NextMultipleOf(requirements.size, ctx->_physical_device_properties.limits.nonCoherentAtomSize);
   vkFlushMappedMemoryRanges(ctx->_device, 1, &range); 

   if(vkResetCommandBuffer(ctx->_immediate_cmd_buffer, 0) != VK_SUCCESS) {
    ZAC_System_Panic("ZAC_Ctxrender_BeginUpdate() : failed.");
   }

   VkCommandBufferBeginInfo begin_info;
   memset(&begin_info, 0, sizeof(VkCommandBufferBeginInfo));
   begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
   begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

   if(vkBeginCommandBuffer(ctx->_immediate_cmd_buffer, &begin_info) != VK_SUCCESS)
    ZAC_System_Panic("begin command buffer failed.");


   VkBufferCopy copy_region;
   memset(&copy_region, 0, sizeof(VkBufferCopy));
   copy_region.size = buff_size;
   vkCmdCopyBuffer(ctx->_immediate_cmd_buffer, tmp_buffer, dst, 1, &copy_region);


   vkEndCommandBuffer(ctx->_immediate_cmd_buffer);

   VkSubmitInfo submit_info;
   memset(&submit_info, 0, sizeof(VkSubmitInfo));
   submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
   submit_info.commandBufferCount = 1;
   submit_info.pCommandBuffers = &ctx->_immediate_cmd_buffer;

   vkQueueSubmit(ctx->_graphics_queue, 1, &submit_info, ctx->_staging_fence);
   vkWaitForFences(ctx->_device, 1, &ctx->_staging_fence, VK_TRUE, UINT64_MAX);
   vkResetFences(ctx->_device, 1, &ctx->_staging_fence);

   vkDestroyBuffer(ctx->_device, tmp_buffer, NULL);
  }
  break;
  case ZAC_GPUMEMORY_SHARED:
  {
   uint8_t *bytes = (uint8_t*)allocation._memory->_mapped;
   bytes += allocation._offset;
   memcpy(bytes, data, buff_size);

   VkMappedMemoryRange range;
   range.pNext = NULL;
   range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
   range.memory = allocation._memory->_memory;
   range.offset = allocation._offset;
   range.size = allocation._non_coherent_size;
   vkFlushMappedMemoryRanges(ctx->_device, 1, &range); 
  }
  break;
 }
}


static void image_staging(ZAC_Ctxrender *ctx, ZAC_Gpuallocation allocation, VkImage dst, void *data, uint16_t w, uint16_t h, ZAC_TextureFormat texture_format) {
 VkBufferCreateInfo buffer_create_info;
 memset(&buffer_create_info, 0, sizeof(VkBufferCreateInfo));
 buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
   
 switch(texture_format) {
 	case ZAC_TEXTURE_FORMAT_RGBA:
   buffer_create_info.size = w * h * 4;
 	break;
  case ZAC_TEXTURE_FORMAT_R:
   buffer_create_info.size = w * h;
  break;
 }
   
 buffer_create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
 buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
 VkBuffer tmp_buffer;
 if(vkCreateBuffer(ctx->_device, &buffer_create_info, NULL, &tmp_buffer) != VK_SUCCESS) {
  ZAC_System_Panic("Failed to create buffer!");
 }

 
 VkMemoryRequirements requirements;
 vkGetBufferMemoryRequirements(ctx->_device, tmp_buffer, &requirements);

 assert(requirements.size < ctx->_staging_memory_size);
 vkBindBufferMemory(ctx->_device, tmp_buffer, ctx->_staging_memory, 0);
  
 memcpy(ctx->_mapped_staging_memory, data, buffer_create_info.size);
   
 VkMappedMemoryRange range;
 range.pNext = NULL;
 range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
 range.memory = ctx->_staging_memory;
 range.offset = 0;
 range.size = ZAC_NextMultipleOf(requirements.size, ctx->_physical_device_properties.limits.nonCoherentAtomSize);
 vkFlushMappedMemoryRanges(ctx->_device, 1, &range); 

 if(vkResetCommandBuffer(ctx->_immediate_cmd_buffer, 0) != VK_SUCCESS) {
  ZAC_System_Panic("ZAC_Ctxrender_BeginUpdate() : failed.");
 }

 VkCommandBufferBeginInfo begin_info;
 memset(&begin_info, 0, sizeof(VkCommandBufferBeginInfo));
 begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
 begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

 if(vkBeginCommandBuffer(ctx->_immediate_cmd_buffer, &begin_info) != VK_SUCCESS)
  ZAC_System_Panic("begin command buffer failed.");


 __ZAC_TransitionColorImageLayout(ctx->_immediate_cmd_buffer, dst, 
  VK_IMAGE_LAYOUT_UNDEFINED, 
  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
  0, 
  VK_ACCESS_TRANSFER_WRITE_BIT, 
  VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
  VK_PIPELINE_STAGE_TRANSFER_BIT
  );



 VkBufferImageCopy region;
 memset(&region, 0, sizeof(VkBufferImageCopy));
 region.bufferOffset = 0;
 region.bufferRowLength = 0;
 region.bufferImageHeight = 0;

 region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
 region.imageSubresource.mipLevel = 0;
 region.imageSubresource.baseArrayLayer = 0;
 region.imageSubresource.layerCount = 1;

 region.imageExtent.width = w;
 region.imageExtent.height = h;
 region.imageExtent.depth = 1;


 vkCmdCopyBufferToImage(
  ctx->_immediate_cmd_buffer,
  tmp_buffer,
  dst,
  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
  1,
  &region
 );


 __ZAC_TransitionColorImageLayout(ctx->_immediate_cmd_buffer, dst, 
  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
  VK_ACCESS_TRANSFER_WRITE_BIT, 
  VK_ACCESS_SHADER_READ_BIT, 
  VK_PIPELINE_STAGE_TRANSFER_BIT,
  VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
  );


 vkEndCommandBuffer(ctx->_immediate_cmd_buffer);

 VkSubmitInfo submit_info;
 memset(&submit_info, 0, sizeof(VkSubmitInfo));
 submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
 submit_info.commandBufferCount = 1;
 submit_info.pCommandBuffers = &ctx->_immediate_cmd_buffer;

 vkQueueSubmit(ctx->_graphics_queue, 1, &submit_info, ctx->_staging_fence);
 vkWaitForFences(ctx->_device, 1, &ctx->_staging_fence, VK_TRUE, UINT64_MAX);
 vkResetFences(ctx->_device, 1, &ctx->_staging_fence);

 vkDestroyBuffer(ctx->_device, tmp_buffer, NULL);
}



/*
 functions
*/

ZAC_Gpualloc* ZAC_Gpualloc_Init(ZAC_Ctxrender *ctx, size_t page_size, ZAC_GpuMemoryType memtype) {
 ZAC_Gpualloc *allocator = (ZAC_Gpualloc*)ZAC_System_AllocateMemory(sizeof(ZAC_Gpualloc));
 memset(allocator, 0, sizeof(ZAC_Gpualloc));
 allocator->_max_size = page_size;
 allocator->_type = memtype;
 
 VkMemoryAllocateInfo allocate_info;
 memset(&allocate_info, 0, sizeof(VkMemoryAllocateInfo));
 allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
 allocate_info.allocationSize = page_size;


 switch(memtype) {
 	case ZAC_GPUMEMORY_LOCAL:
   allocate_info.memoryTypeIndex = __ZAC_FindMemoryType(
    ctx->_physical_device,
    ctx->_local_memory_requirements,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
   );
 
   allocator->_mapped = NULL;
  break;
  case ZAC_GPUMEMORY_SHARED:
   allocate_info.memoryTypeIndex = __ZAC_FindMemoryType(
    ctx->_physical_device,
    ctx->_shared_memory_requirements,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
   );
  break;
 }
 
 if(vkAllocateMemory(ctx->_device, &allocate_info, NULL, &allocator->_memory) != VK_SUCCESS) {
  ZAC_System_Panic("Failed to allocate memory!.");
 }

 if(memtype == ZAC_GPUMEMORY_SHARED)
  vkMapMemory(ctx->_device, allocator->_memory, 0, page_size, 0, &allocator->_mapped);


 return allocator;
}



void ZAC_Gpualloc_Invalidate(ZAC_Ctxrender *ctx, ZAC_Gpuallocation alloc) {
 assert(alloc._memory->_mapped != NULL);

 VkMappedMemoryRange range;
 range.pNext = NULL;
 range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
 range.memory = alloc._memory->_memory;
 range.offset = alloc._offset;
 range.size = alloc._non_coherent_size;
 vkInvalidateMappedMemoryRanges(ctx->_device, 1, &range); 
}


void ZAC_Gpualloc_Flush(ZAC_Ctxrender *ctx, ZAC_Gpuallocation alloc) {
 assert(alloc._memory->_mapped != NULL);

 VkMappedMemoryRange range;
 range.pNext = NULL;
 range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
 range.memory = alloc._memory->_memory;
 range.offset = alloc._offset;
 range.size = alloc._non_coherent_size;
 vkFlushMappedMemoryRanges(ctx->_device, 1, &range); 
}


void* ZAC_Gpualloc_Map(ZAC_Gpuallocation alloc) {
 ZAC_Gpualloc *gpualloc = alloc._memory;
 return (void*) (((uint8_t*)gpualloc->_mapped) + alloc._offset);
}


/*
 reset the entire allocation size
 
 NOTE : Make sure that all handles binded must be destroyed to make sure no undefined behaviour
*/
void ZAC_Gpualloc_PopInto(ZAC_Gpualloc *gpualloc, size_t n) {
 assert((gpualloc->_max_size < n ));
 gpualloc->_current_size = n;
}


/*
 picking highest number for alignment will satisfy also
 the lower number of aligments, since they are always power of 2
*/

ZAC_Buffer* ZAC_Gpualloc_PushBindBuffer(ZAC_Ctxrender *ctx, ZAC_Gpualloc *gpualloc, size_t buff_size, void *data) {
 ZAC_Buffer *buffer = (ZAC_Buffer*)ZAC_System_AllocateMemory(sizeof(ZAC_Buffer));
  
 size_t coherent_atom_size = 0;
 size_t min_mapping_alignment = 0;
 
 switch(gpualloc->_type) {
  case ZAC_GPUMEMORY_SHARED:
   coherent_atom_size = ctx->_physical_device_properties.limits.nonCoherentAtomSize;
   min_mapping_alignment = ctx->_physical_device_properties.limits.minMemoryMapAlignment;
  break;
  case ZAC_GPUMEMORY_LOCAL:
   coherent_atom_size = 1;
   min_mapping_alignment = 1;
  break;
 }
 
 
 buffer->_buffer = create_buffer(ctx, buff_size);
 
 VkMemoryRequirements requirements;
 vkGetBufferMemoryRequirements(ctx->_device, buffer->_buffer, &requirements);
 
 size_t final_size_padding = ctx->_physical_device_properties.limits.bufferImageGranularity;
 if(final_size_padding == 1 || final_size_padding == 0)
  final_size_padding = 0;
 
 size_t final_alignment = (requirements.alignment > min_mapping_alignment ? requirements.alignment : min_mapping_alignment);


 buffer->_allocation._memory = gpualloc;
 buffer->_allocation._offset = ZAC_NextMultipleOf(gpualloc->_current_size, final_alignment);
 buffer->_allocation._non_coherent_size = ZAC_NextMultipleOf(requirements.size, coherent_atom_size);
 
 buffer->_allocation._memory = gpualloc;
 buffer->_allocation._size = requirements.size;

 vkBindBufferMemory(ctx->_device, buffer->_buffer, gpualloc->_memory, buffer->_allocation._offset);

 gpualloc->_current_size += buffer->_allocation._non_coherent_size + final_size_padding;

 assert(!((buffer->_allocation._offset % final_alignment) != 0));
 assert(!((buffer->_allocation._non_coherent_size % coherent_atom_size) != 0));

 assert(!(gpualloc->_max_size < gpualloc->_current_size ));

 if(data != NULL) {
 	/* TODO : staging here */
  buffer_staging(ctx, buffer->_allocation, buffer->_buffer, data, buff_size);
 }

 return buffer;
}



ZAC_Texture* ZAC_Gpualloc_PushBindTexture(ZAC_Ctxrender *ctx, ZAC_Gpualloc *gpualloc, uint16_t w, uint16_t h, ZAC_TextureFormat format, void *data) {
 ZAC_Texture *texture = (ZAC_Texture*)ZAC_System_AllocateMemory(sizeof(ZAC_Texture));
  
 VkFormat img_format;
 
 if(ctx->_is_unorm) {
  switch(format) {
  	case ZAC_TEXTURE_FORMAT_RGBA:
    img_format = VK_FORMAT_R8G8B8A8_UNORM;
  	break;
  	case ZAC_TEXTURE_FORMAT_R:
    img_format = VK_FORMAT_R8_UNORM;
  	break;
  }
 } else {
  switch(format) {
  	case ZAC_TEXTURE_FORMAT_RGBA:
    img_format = VK_FORMAT_R8G8B8A8_SRGB;
  	break;
  	case ZAC_TEXTURE_FORMAT_R:
    img_format = VK_FORMAT_R8_SRGB;
  	break;
  }
 }

 
 texture->_image = create_image(ctx, w, h, format, img_format);
 
 VkMemoryRequirements requirements;
 vkGetImageMemoryRequirements(ctx->_device, texture->_image, &requirements);
 
 size_t final_size_padding = ctx->_physical_device_properties.limits.bufferImageGranularity;
 if(final_size_padding == 1 || final_size_padding == 0)
  final_size_padding = 0;
 
 texture->_allocation._memory = gpualloc;
 texture->_allocation._offset = ZAC_NextMultipleOf(gpualloc->_current_size, requirements.alignment);
 texture->_allocation._non_coherent_size = requirements.size;
 
 texture->_allocation._memory = gpualloc;
 texture->_allocation._size = requirements.size;


 vkBindImageMemory(ctx->_device, texture->_image, gpualloc->_memory, texture->_allocation._offset);

 gpualloc->_current_size += texture->_allocation._non_coherent_size + final_size_padding;
 assert(gpualloc->_type != ZAC_GPUMEMORY_SHARED && "image texture should not be shared.");


 texture->_image_view = create_image_view(ctx, texture->_image, format, img_format);

 assert(!(gpualloc->_max_size < gpualloc->_current_size ));
 
 if(data != NULL) {
 	/* TODO : staging here */
 	image_staging(ctx, texture->_allocation, texture->_image, data, w, h, format);
 }

 return texture;
}




void ZAC_Gpualloc_Destroy(ZAC_Ctxrender *ctx, ZAC_Gpualloc *gpualloc) {
 if(gpualloc->_mapped != NULL) {
  vkUnmapMemory(ctx->_device, gpualloc->_memory);
 }
 vkFreeMemory(ctx->_device, gpualloc->_memory, NULL);
 ZAC_System_FreeMemory(gpualloc);
}




ZAC_Gpuallocation ZAC_Texture_GetAllocation(ZAC_Texture *texture) {
 return texture->_allocation;
}

ZAC_Gpuallocation ZAC_Buffer_GetAllocation(ZAC_Buffer *buffer) {
 return buffer->_allocation;
}



void ZAC_Texture_Destroy(ZAC_Ctxrender *ctx, ZAC_Texture *texture) {
 vkDestroyImageView(ctx->_device, texture->_image_view, NULL);
 vkDestroyImage(ctx->_device, texture->_image, NULL);
 ZAC_System_FreeMemory(texture);
}


void ZAC_Buffer_Destroy(ZAC_Ctxrender *ctx, ZAC_Buffer *buffer) {
 vkDestroyBuffer(ctx->_device, buffer->_buffer, NULL);
 ZAC_System_FreeMemory(buffer);
}


