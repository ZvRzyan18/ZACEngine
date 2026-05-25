#define __ZAC_INTERNAL__

#include "zac/renderer/gpudescriptor.h"
#include "zac/system/system.h"
#include <string.h>
#include <assert.h>



ZAC_Gpudescriptor* ZAC_Gpudescriptor_Init(ZAC_Ctxrender *ctx, size_t max_textured, size_t max_ubo) {
 ZAC_Gpudescriptor *descriptor = ZAC_System_AllocateMemory(sizeof(ZAC_Gpudescriptor));

 VkDescriptorPoolSize pool_size;
 memset(&pool_size, 0, sizeof(VkDescriptorPoolSize));
 pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
 pool_size.descriptorCount = max_textured * 8;
 
 
 VkDescriptorPoolCreateInfo pool_info;
 memset(&pool_info, 0, sizeof(VkDescriptorPoolCreateInfo));
 pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
 pool_info.poolSizeCount = 1;
 pool_info.pPoolSizes = &pool_size;
 pool_info.maxSets = max_textured;
 pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
 
 if(vkCreateDescriptorPool(ctx->_device, &pool_info, NULL, &descriptor->_textured_pool) != VK_SUCCESS) {
  ZAC_System_Panic(" could not create a descriptor pool.");
 }

 memset(&pool_size, 0, sizeof(VkDescriptorPoolSize));
 pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
 pool_size.descriptorCount = max_ubo;

 memset(&pool_info, 0, sizeof(VkDescriptorPoolCreateInfo));
 pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
 pool_info.poolSizeCount = 1;
 pool_info.pPoolSizes = &pool_size;
 pool_info.maxSets = max_ubo;
 pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
 
 if(vkCreateDescriptorPool(ctx->_device, &pool_info, NULL, &descriptor->_ubo_pool) != VK_SUCCESS) {
  ZAC_System_Panic(" could not create a descriptor pool.");
 }
 
 return descriptor;
}



uintptr_t ZAC_Gpudescriptor_Allocate(ZAC_Ctxrender *ctx, ZAC_Gpudescriptor *d, ZAC_GpudescriptorAllocateType allocate_type) {
 VkDescriptorPool pool = VK_NULL_HANDLE;
 VkDescriptorSetLayout layout = VK_NULL_HANDLE;
 
 switch(allocate_type) {
  case ZAC_GPUDESCRIPTOR_ALLOCATE_TEXTURES:
   pool = d->_textured_pool;
   layout = ctx->_textured_descriptor_layout;
  break;
  case ZAC_GPUDESCRIPTOR_ALLOCATE_UBO:
   pool = d->_ubo_pool;
   layout = ctx->_ubo_descriptor_layout;
  break;
 }

 VkDescriptorSetAllocateInfo decsriptor_set_alloc_info;
 memset(&decsriptor_set_alloc_info, 0, sizeof(VkDescriptorSetAllocateInfo));
 decsriptor_set_alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
 decsriptor_set_alloc_info.descriptorPool = pool;
 decsriptor_set_alloc_info.descriptorSetCount = 1;
 decsriptor_set_alloc_info.pSetLayouts = &layout;

 VkDescriptorSet ds;
 if(vkAllocateDescriptorSets(ctx->_device, &decsriptor_set_alloc_info, &ds) != VK_SUCCESS) {
 	ZAC_System_Panic("failed allocation of descriptor set");
 }

 return (uintptr_t)ds;
}



void ZAC_Gpudescriptor_Deallocate(ZAC_Ctxrender *ctx, ZAC_Gpudescriptor *d, ZAC_GpudescriptorAllocateType allocate_type, uintptr_t ds) {
 VkDescriptorPool pool = VK_NULL_HANDLE;
 
 switch(allocate_type) {
  case ZAC_GPUDESCRIPTOR_ALLOCATE_TEXTURES:
   pool = d->_textured_pool;
  break;
  case ZAC_GPUDESCRIPTOR_ALLOCATE_UBO:
   pool = d->_ubo_pool;
  break;
 }

 if(vkFreeDescriptorSets(ctx->_device, pool, 1, (const VkDescriptorSet*)&ds) != VK_SUCCESS) {
  ZAC_System_Panic("failed freeing descriptor set.");
 }
}



void ZAC_Gpudescriptor_Destroy(ZAC_Ctxrender *ctx, ZAC_Gpudescriptor *d) {
 vkDestroyDescriptorPool(ctx->_device, d->_textured_pool, NULL);
 vkDestroyDescriptorPool(ctx->_device, d->_ubo_pool, NULL); 
 ZAC_System_FreeMemory(d);
}




void ZAC_Gpudescriptor_BindTextures(ZAC_Ctxrender *ctx, uintptr_t descriptor, ZAC_Texture **textures, ZAC_GpudescriptorSamplerType *samplers) {
 ZAC_Texture **texture = textures;

 assert(texture != NULL && "textures are not optional.");
 assert(samplers != NULL && "samplers are not optional.");

 VkDescriptorImageInfo image_info[8];
 memset(image_info, 0, sizeof(VkDescriptorImageInfo) * 8);
 
 for(int i = 0; i < 8; i++) {
  
  VkSampler sampler;
  switch(samplers[i]) {
   case ZAC_GPUDESCRIPTOR_SAMPLER_LINEAR:
    sampler = ctx->_linear_sample;
   break;
   case ZAC_GPUDESCRIPTOR_SAMPLER_NEAREST:
    sampler = ctx->_nearest_sample;
   break;
   case ZAC_GPUDESCRIPTOR_SAMPLER_NONE:
    sampler = ctx->_linear_sample; 
   break;
   default:
    sampler = ctx->_linear_sample;
  }
  
  
  image_info[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  
  /* other texture slot are just optional 
   just point it into dummy texture to avoid UB.
  */
  if(texture[i] == NULL)
   image_info[i].imageView = ctx->_dummy_image_view;
  else 
   image_info[i].imageView = texture[i]->_image_view;
  image_info[i].sampler = sampler;

 }
 
 VkWriteDescriptorSet descriptor_write;
 memset(&descriptor_write, 0, sizeof(VkWriteDescriptorSet));
 
 descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
 descriptor_write.dstSet = (VkDescriptorSet)descriptor;
 descriptor_write.dstBinding = 0;
 descriptor_write.dstArrayElement = 0;
 descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
 descriptor_write.descriptorCount = 8;
 descriptor_write.pImageInfo = image_info;
 vkUpdateDescriptorSets(ctx->_device, 1, &descriptor_write, 0, NULL);
}




void ZAC_Gpudescriptor_BindUBO(ZAC_Ctxrender *ctx, uintptr_t descriptor, ZAC_Buffer *ubo) {
 ZAC_Buffer *buffer = ubo;
 
 assert(buffer != NULL && "buffer is not optional.");
 ZAC_Gpuallocation allocation = ZAC_Buffer_GetAllocation(buffer);
 
 VkDescriptorBufferInfo buffer_info;
 memset(&buffer_info, 0, sizeof(VkDescriptorBufferInfo));
 buffer_info.buffer = buffer->_buffer;
 buffer_info.offset = 0;
 buffer_info.range = allocation._size;
 
 VkWriteDescriptorSet descriptor_write;
 memset(&descriptor_write, 0, sizeof(VkWriteDescriptorSet));
 descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
 descriptor_write.dstSet = (VkDescriptorSet)descriptor;
 descriptor_write.dstBinding = 0;
 descriptor_write.dstArrayElement = 0;
 descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
 descriptor_write.descriptorCount = 1;
 descriptor_write.pBufferInfo = &buffer_info;
 
 vkUpdateDescriptorSets(ctx->_device, 1, &descriptor_write, 0, NULL);
}



