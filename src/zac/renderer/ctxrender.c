#define __ZAC_INTERNAL__

#include "zac/renderer/ctxrender.h"
#include "zac/renderer/ctxinternal.h"
#include "zac/system/system.h"
#include "zac/math/math.h"
#include <string.h>

/*
 get specific components from the context object
 just displays here so it can reuse multiple times.
*/
void* ZAC_Ctxrender_Get(ZAC_Ctxrender *ctx, const char *name) {
	if(strcmp(name, "VkInstance") == 0) {
	 return ctx->_instance;
	}
	ZAC_System_Panic("ERROR : invalid string value \'%s\'", name);
	return NULL;
}




ZAC_Ctxrender* ZAC_Ctxrender_Init() {
 ZAC_Ctxrender *allocated_renderer = (ZAC_Ctxrender*)ZAC_System_AllocateMemory(sizeof(ZAC_Ctxrender));
 if(!allocated_renderer) {
  ZAC_System_Panic("ZAC_Ctxrender_Init() : failed allocation of ZAC_Ctxrender.");
 }
 memset(allocated_renderer, 0, sizeof(ZAC_Ctxrender));
 __ZAC_CreateInstance(allocated_renderer);
 return allocated_renderer;
}




void ZAC_Ctxrender_Stall(ZAC_Ctxrender *ctx) {
 vkDeviceWaitIdle(ctx->_device);
}



void ZAC_Ctxrender_Destroy(ZAC_Ctxrender *ctx) {
 vkDestroySampler(ctx->_device, ctx->_nearest_sample, NULL);
 vkDestroySampler(ctx->_device, ctx->_linear_sample, NULL);

 vkDestroyDescriptorSetLayout(ctx->_device, ctx->_textured_descriptor_layout, NULL);
 vkDestroyDescriptorSetLayout(ctx->_device, ctx->_ubo_descriptor_layout, NULL);


 vkFreeMemory(ctx->_device, ctx->_staging_memory, NULL);
 vkFreeMemory(ctx->_device, ctx->_dummy_device_memory, NULL);
 vkDestroyImageView(ctx->_device, ctx->_dummy_image_view, NULL);
 vkDestroyImage(ctx->_device, ctx->_dummy_image, NULL);
 vkDestroyBuffer(ctx->_device, ctx->_dummy_buffer, NULL);

 vkDestroyFence(ctx->_device, ctx->_staging_fence, NULL);
 
 for(uint32_t i = 0; i < ctx->_max_swapchain_images; i++) {
  vkDestroySemaphore(ctx->_device, ctx->_submit_semaphores[i], NULL);
 }
 ZAC_System_FreeMemory(ctx->_submit_semaphores);
 
 for(uint32_t i = 0; i < ctx->_max_in_flight_frames; i++) {
  vkDestroyFence(ctx->_device, ctx->_synchronizers[i]._fence, NULL);
  vkDestroySemaphore(ctx->_device, ctx->_synchronizers[i]._semaphore, NULL);
 }
  
 ZAC_System_FreeMemory(ctx->_synchronizers);
 
 
 ZAC_System_FreeMemory(ctx->_command_buffers);
 vkDestroyCommandPool(ctx->_device, ctx->_command_pool, NULL);
 vkDestroyRenderPass(ctx->_device, ctx->_render_pass, NULL);

 for(uint32_t i = 0; i < ctx->_max_swapchain_images * ctx->_max_in_flight_frames; i++) {
  vkDestroyFramebuffer(ctx->_device, ctx->_swapchain_framebuffers[i], NULL);
 }
 
 ZAC_System_FreeMemory(ctx->_swapchain_framebuffers);
 
 for(uint32_t i = 0; i < ctx->_max_swapchain_images; i++) {
  vkDestroyImageView(ctx->_device, ctx->_swapchain_images[i]._image_view, NULL);
 }
 for(uint32_t i = 0; i < ctx->_max_in_flight_frames; i++) {
  vkDestroyImageView(ctx->_device, ctx->_swapchain_depth_images[i]._image_view, NULL);
  vkDestroyImage(ctx->_device, ctx->_swapchain_depth_images[i]._image, NULL);
 }
 vkFreeMemory(ctx->_device, ctx->_depth_memory, NULL);
 ZAC_System_FreeMemory(ctx->_swapchain_images);
 ZAC_System_FreeMemory(ctx->_swapchain_depth_images);
 
 vkDestroySwapchainKHR(ctx->_device, ctx->_swapchain, NULL);
 vkDestroyDevice(ctx->_device, NULL);
 vkDestroySurfaceKHR(ctx->_instance, ctx->_surface, NULL);

 PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(ctx->_instance, "vkDestroyDebugUtilsMessengerEXT");
 if(func) {
  func(ctx->_instance, ctx->_debug_messenger, NULL);
 }
 vkDestroyInstance(ctx->_instance, NULL);
 ZAC_System_FreeMemory(ctx);
}





void ZAC_Ctxrender_SetDisplaySurface(ZAC_Ctxrender *ctx, void *surface) {
 ctx->_surface = (VkSurfaceKHR)surface;
 __ZAC_QueryPhysicalDevice(ctx);
 __ZAC_CreateDeviceAndQueues(ctx);
 __ZAC_QuerySurfaceCapabilities(ctx);
 __ZAC_CreateSwapchain(ctx);
 __ZAC_CreateSwapchainImages(ctx);
 __ZAC_CreateSwapchainRenderpass(ctx);
 __ZAC_CreateSwapchainFramebuffer(ctx);
 __ZAC_CreateCommandPool(ctx);
 __ZAC_DepthBufferImageTransition(ctx);
 __ZAC_CreateDummyObjects(ctx);
 __ZAC_CreateStagingMemory(ctx);
 __ZAC_CreateDescriptorSetLayouts(ctx);
 __ZAC_CreateSamplers(ctx);
}




uint32_t ZAC_Ctxrender_BeginUpdate(ZAC_Ctxrender *ctx) {
 VkResult result;
 ZAC_SwapchainSynchronizer *synchronizer = ctx->_synchronizers + ctx->_current_frame;
 
 vkWaitForFences(ctx->_device, 1, &synchronizer->_fence, VK_TRUE, UINT64_MAX);
 vkResetFences(ctx->_device, 1, &synchronizer->_fence);

 result = vkAcquireNextImageKHR(ctx->_device, ctx->_swapchain, UINT64_MAX, synchronizer->_semaphore, VK_NULL_HANDLE, &ctx->_current_image_frame);
 if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
  /*recreate swapchain */
  __ZAC_RecreateSwapchain(ctx);
  return -1;
 } else if(result != VK_SUCCESS) {
  ZAC_System_Panic("ZAC_Ctxrender_BeginUpdate() : failed.");
  return -1;
 }


 VkCommandBuffer cmd = ctx->_command_buffers[ctx->_current_frame];

 if(vkResetCommandBuffer(cmd, 0) != VK_SUCCESS) {
  ZAC_System_Panic("ZAC_Ctxrender_BeginUpdate() : failed.");
  return -1;
 }


 VkCommandBufferBeginInfo command_buffer_begin_info;
 command_buffer_begin_info.pNext = NULL;
 command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
 command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
 command_buffer_begin_info.pInheritanceInfo = VK_NULL_HANDLE;

 if(vkBeginCommandBuffer(cmd, &command_buffer_begin_info) != VK_SUCCESS) {
  ZAC_System_Panic("ZAC_Ctxrender_BeginUpdate() : failed.");
  return -1;
 }
  
 return 0;
}




uint32_t ZAC_Ctxrender_EndUpdate(ZAC_Ctxrender *ctx) {
 VkResult result;

 VkCommandBuffer cmd = ctx->_command_buffers[ctx->_current_frame];

 ZAC_SwapchainSynchronizer *synchronizer = ctx->_synchronizers + ctx->_current_frame;
 VkSemaphore *submit_semaphore = ctx->_submit_semaphores + ctx->_current_image_frame;

 result = vkEndCommandBuffer(cmd);
 if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
  /* recreate */
  __ZAC_RecreateSwapchain(ctx);
  return -1;
 } else if(result != VK_SUCCESS) {
  ZAC_System_Panic("ZAC_Ctxrender_BeginUpdate() : failed.");
  return -1;
 }

 VkPipelineStageFlags waitStages[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
 
 VkSubmitInfo submit_info;
 submit_info.pNext = NULL;
 submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
 submit_info.waitSemaphoreCount = 1;
 submit_info.pWaitSemaphores = &synchronizer->_semaphore;
 submit_info.pWaitDstStageMask = waitStages;
 submit_info.commandBufferCount = 1;
 submit_info.pCommandBuffers = &cmd;
 submit_info.signalSemaphoreCount = 1;
 submit_info.pSignalSemaphores = submit_semaphore;

 result = vkQueueSubmit(ctx->_graphics_queue, 1, &submit_info, synchronizer->_fence);

 if(result != VK_SUCCESS) {
  ZAC_System_Panic("ZAC_Ctxrender_BeginUpdate() : failed.");
  return -1;
 }
 
 VkPresentInfoKHR present_info;
 present_info.pNext = NULL;
 present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
 present_info.waitSemaphoreCount = 1;
 present_info.pWaitSemaphores = submit_semaphore;
 present_info.swapchainCount = 1;
 present_info.pSwapchains = &ctx->_swapchain;
 present_info.pImageIndices = &ctx->_current_image_frame;
 present_info.pResults = NULL;

 result = vkQueuePresentKHR(ctx->_present_queue, &present_info);
 if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
  /* recrrate */
  __ZAC_RecreateSwapchain(ctx);
  return -1;
 } else if(result != VK_SUCCESS) {
  ZAC_System_Panic("ZAC_Ctxrender_BeginUpdate() : failed.");
  return -1;
 }
 
 ctx->_current_frame = (ctx->_current_frame + 1) % ctx->_max_in_flight_frames;
 return 0;
}




void ZAC_Ctxrender_BeginFramebuffer(ZAC_Ctxrender *ctx) {
 const uint32_t index = ctx->_current_image_frame * ctx->_max_in_flight_frames + ctx->_current_frame;

 VkCommandBuffer cmd = ctx->_command_buffers[ctx->_current_frame];

 VkRenderPassBeginInfo render_pass_begin_info;
 render_pass_begin_info.pNext = NULL;
 render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
 render_pass_begin_info.renderPass = ctx->_render_pass;
 render_pass_begin_info.framebuffer = ctx->_swapchain_framebuffers[index];
 render_pass_begin_info.renderArea.offset.x = 0;
 render_pass_begin_info.renderArea.offset.y = 0;
 render_pass_begin_info.renderArea.extent = ctx->_screen_size;
 render_pass_begin_info.clearValueCount = 2;
 render_pass_begin_info.pClearValues = ctx->_clear_value;

 vkCmdBeginRenderPass(cmd, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}




void ZAC_Ctxrender_EndFramebuffer(ZAC_Ctxrender *ctx) {
 VkCommandBuffer cmd = ctx->_command_buffers[ctx->_current_frame];
 vkCmdEndRenderPass(cmd);
}



void ZAC_Ctxrender_SetClearValue(ZAC_Ctxrender *ctx, float r, float g, float b, float a) {
 memset(ctx->_clear_value, 0, sizeof(VkClearValue) * 2);
 if(ctx->_is_unorm) {
  ctx->_clear_value[0].color.float32[0] = r;
  ctx->_clear_value[0].color.float32[1] = g;
  ctx->_clear_value[0].color.float32[2] = b;
  ctx->_clear_value[0].color.float32[3] = a;
 } else {
  ctx->_clear_value[0].color.float32[0] = ZAC_ToLinear_Float(r);
  ctx->_clear_value[0].color.float32[1] = ZAC_ToLinear_Float(g);
  ctx->_clear_value[0].color.float32[2] = ZAC_ToLinear_Float(b);
  ctx->_clear_value[0].color.float32[3] = ZAC_ToLinear_Float(a);
 }
 ctx->_clear_value[1].depthStencil.depth = 1.0f;
 ctx->_clear_value[1].depthStencil.stencil = 0;
}



const ZAC_Mat4x4* ZAC_Ctxrender_Get2DProjection(ZAC_Ctxrender *ctx) {
 return &ctx->_projection_2d;
}


const ZAC_Mat4x4* ZAC_Ctxrender_Get3DProjection(ZAC_Ctxrender *ctx) {
 return &ctx->_projection_3d;
}


VkCommandBuffer __ZAC_Ctxrenderer_AcquireCmdBuffer(ZAC_Ctxrender *ctx) {
 return ctx->_command_buffers[ctx->_current_frame];
}
