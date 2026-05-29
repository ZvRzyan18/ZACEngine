#ifndef ZAC_CTXRENDER_H
#define ZAC_CTXRENDER_H

#include "zac/math/mat4x4.h"
#include <stdint.h>
/*
 main renderer context
*/
typedef struct ZAC_Ctxrender ZAC_Ctxrender;

#ifdef __ZAC_INTERNAL__

#include "volk.h"

typedef struct {
 VkImage _image;
 VkImageView _image_view;
} ZAC_SwapchainImage;

typedef struct {
 VkSemaphore _semaphore;
 VkFence _fence;
} ZAC_SwapchainSynchronizer;


struct ZAC_Ctxrender {
 uint32_t _min_swapchain_image_count;
 uint32_t _max_swapchain_images;
 uint32_t _max_in_flight_frames;
 uint32_t _current_frame;
 uint32_t _current_image_frame;
 VkDebugUtilsMessengerEXT _debug_messenger;
 VkInstance _instance;
 VkSurfaceKHR _surface;
 VkPhysicalDevice _physical_device;
 VkPhysicalDeviceFeatures _physical_device_features;
 VkPhysicalDeviceProperties _physical_device_properties;
 VkQueueFamilyProperties _queue_family_properties;
 uint32_t _queue_family_indices[2];
 VkDevice _device;
 VkQueue _graphics_queue;
 VkQueue _present_queue;
 
 VkSurfaceCapabilitiesKHR _surface_capabilities;
 VkSurfaceFormatKHR _surface_format;
 VkPresentModeKHR _present_mode;
 VkFormat _depth_format;
 VkBool32 _is_unorm;
 VkExtent2D _screen_size;
 VkViewport _viewport;
 VkClearValue _clear_value[2];
 VkRect2D _scissor;
 VkSwapchainKHR _swapchain;
 ZAC_SwapchainImage *_swapchain_images;
 ZAC_SwapchainImage *_swapchain_depth_images;
 VkDeviceMemory _depth_memory;
 VkRenderPass _render_pass;
 VkFramebuffer *_swapchain_framebuffers;
 
 VkFence _staging_fence;
 VkSemaphore *_submit_semaphores;
 ZAC_SwapchainSynchronizer *_synchronizers;
 
 VkCommandPool _command_pool;
 VkCommandBuffer *_command_buffers;
 VkCommandBuffer _immediate_cmd_buffer;
 
 VkDeviceMemory _dummy_device_memory;
 VkImage _dummy_image;
 VkImageView _dummy_image_view;
 VkBuffer _dummy_buffer;
 
 VkDeviceMemory _staging_memory;
 size_t _staging_memory_size;
 void* _mapped_staging_memory;
 uint32_t _shared_memory_requirements;
 uint32_t _local_memory_requirements;
 uint32_t _staging_memory_requirements;
 
 VkDescriptorSetLayout _textured_descriptor_layout;
 VkDescriptorSetLayout _ubo_descriptor_layout;

 VkSampler _nearest_sample;
 VkSampler _linear_sample;
 
 ZAC_Mat4x4 _projection_2d;
 ZAC_Mat4x4 _projection_3d;

};

VkCommandBuffer __ZAC_Ctxrenderer_AcquireCmdBuffer(ZAC_Ctxrender *ctx);

#endif

void* ZAC_Ctxrender_Get(ZAC_Ctxrender *ctx, const char *name);
ZAC_Ctxrender* ZAC_Ctxrender_Init();
void ZAC_Ctxrender_Stall(ZAC_Ctxrender *ctx);
void ZAC_Ctxrender_Destroy(ZAC_Ctxrender *ctx);
void ZAC_Ctxrender_SetDisplaySurface(ZAC_Ctxrender *ctx, void *surface);
uint32_t ZAC_Ctxrender_BeginUpdate(ZAC_Ctxrender *ctx);
uint32_t ZAC_Ctxrender_EndUpdate(ZAC_Ctxrender *ctx);
void ZAC_Ctxrender_BeginFramebuffer(ZAC_Ctxrender *ctx);
void ZAC_Ctxrender_EndFramebuffer(ZAC_Ctxrender *ctx);
void ZAC_Ctxrender_SetClearValue(ZAC_Ctxrender *ctx, float r, float g, float b, float a);

const ZAC_Mat4x4* ZAC_Ctxrender_Get2DProjection(ZAC_Ctxrender *ctx);
const ZAC_Mat4x4* ZAC_Ctxrender_Get3DProjection(ZAC_Ctxrender *ctx);

#endif

