#define __ZAC_INTERNAL__

#include "zac/renderer/renderhelper.h"
#include "zac/renderer/ctxinternal.h"
#include "zac/system/system.h"
#include <string.h>


VkFormat __ZAC_FindImageFormat(VkPhysicalDevice physical_device, VkFormat *formats, uint32_t format_count, VkImageTiling tiling, VkFormatFeatureFlags feature_flags) {
	for(uint32_t i = 0; i < format_count; i++) {
		VkFormatProperties properties;
  vkGetPhysicalDeviceFormatProperties(physical_device, formats[i], &properties);

  if(tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & feature_flags) == feature_flags) {
   return formats[i];
  } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & feature_flags) == feature_flags) {
   return formats[i];
  }
	}
	return (VkFormat)0xFFFFFFFF;
}



uint32_t __ZAC_FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
 VkPhysicalDeviceMemoryProperties memProperties;
 vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

 for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
  if((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
   return i;
  }
 }
 ZAC_System_Panic("no available memory type");
 return 0;
}



VkBool32 __ZAC_CheckDeviceCapabilities(VkPhysicalDevice gpu, VkPhysicalDeviceFeatures gpu_features, ZAC_Ctxrender *ctx) {
 VkQueueFamilyProperties *queue_family_properties;
 uint32_t queue_family_properties_count;
 vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_properties_count, VK_NULL_HANDLE);
 
 queue_family_properties = (VkQueueFamilyProperties*)ZAC_System_AllocateMemory(sizeof(VkQueueFamilyProperties) * queue_family_properties_count);
 vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_properties_count, queue_family_properties);

 VkBool32 has_graphics = VK_FALSE;
 VkBool32 has_present = VK_FALSE;
 
 for(uint32_t i = 0; i < queue_family_properties_count; i++) {
  if(queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
   ctx->_queue_family_properties = queue_family_properties[i];
   ctx->_queue_family_indices[0] = (uint32_t)i;
   has_graphics = VK_TRUE;
   break;
  }
 }
 
 for(uint32_t i = 0; i < queue_family_properties_count; i++) {
  vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, ctx->_surface, &has_present);
  if(has_present == VK_TRUE) {
   ctx->_queue_family_indices[1] = (uint32_t)i;
   break;
  }
 }
  
 ZAC_System_FreeMemory(queue_family_properties);
 return gpu_features.geometryShader && has_graphics && has_present;
}
 



VkBool32 __ZAC_HasStencil(VkFormat format) {
 return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}




uint32_t __ZAC_FindMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t bits) {
 VkPhysicalDeviceMemoryProperties memProperties;
 vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

 for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
  if(memProperties.memoryTypes[i].propertyFlags & bits) {
   return i; 
  }
 }
 return 0x7FFFFFFF;
}



void __ZAC_TransitionColorImageLayout(VkCommandBuffer cmd_buffer, VkImage img, VkImageLayout src_layout, VkImageLayout dst_layout, VkAccessFlags src_access, VkAccessFlags dst_access, VkPipelineStageFlagBits src_stage, VkPipelineStageFlagBits dst_stage) {
 VkImageMemoryBarrier image_memory_barrier;
 memset(&image_memory_barrier, 0, sizeof(VkImageMemoryBarrier));
 image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
 image_memory_barrier.oldLayout = src_layout;
 image_memory_barrier.newLayout = dst_layout;

 image_memory_barrier.srcAccessMask = src_access;
 image_memory_barrier.dstAccessMask = dst_access;
 
 image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
 image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

 image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

 image_memory_barrier.subresourceRange.baseMipLevel = 0;
 image_memory_barrier.subresourceRange.levelCount = 1;
 image_memory_barrier.subresourceRange.baseArrayLayer = 0;
 image_memory_barrier.subresourceRange.layerCount = 1;

 image_memory_barrier.image = img;
 
 vkCmdPipelineBarrier(cmd_buffer, src_stage, dst_stage, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1, &image_memory_barrier);
}


