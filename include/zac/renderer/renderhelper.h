#ifndef ZAC_RENDERHELPER_H
#define ZAC_RENDERHELPER_H

#include "zac/renderer/ctxrender.h"
#include "volk.h"

VkFormat __ZAC_FindImageFormat(VkPhysicalDevice physical_device, VkFormat *formats, uint32_t format_count, VkImageTiling tiling, VkFormatFeatureFlags feature_flags);
uint32_t __ZAC_FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
VkBool32 __ZAC_CheckDeviceCapabilities(VkPhysicalDevice gpu, VkPhysicalDeviceFeatures gpu_features, ZAC_Ctxrender *ctx);
VkBool32 __ZAC_HasStencil(VkFormat format);
uint32_t __ZAC_FindMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t bits);
void __ZAC_TransitionColorImageLayout(VkCommandBuffer cmd_buffer, VkImage img, VkImageLayout src_layout, VkImageLayout dst_layout, VkAccessFlags src_access, VkAccessFlags dst_access, VkPipelineStageFlagBits src_stage, VkPipelineStageFlagBits dst_stage);


#endif


