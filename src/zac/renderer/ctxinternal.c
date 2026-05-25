#define __ZAC_INTERNAL__

#include "zac/renderer/ctxinternal.h"
#include "zac/renderer/renderhelper.h"
#include "zac/system/system.h"
#include "zac/math/geometry.h"
#include "zac/math/quat4.h"

#include <string.h>
#include <assert.h>


static uint32_t required_device_extensions_count = 1;
static const char* required_device_extensions[1] = {
 "VK_KHR_swapchain",
};

static const char* platform_instance_extensions[7] = {
 "VK_KHR_win32_surface", /* windows */
 "VK_KHR_xlib_surface", /* linux x11 */
 "VK_KHR_wayland_surface", /* linux wayland */
 "VK_KHR_xcb_surface", /* linux xcb */
 "VK_EXT_metal_surface", /* molten vk */
 "VK_KHR_android_surface", /* android */
 "VK_EXT_headless_surface", /* surface nondisplay */
};



static int any_of_platform_surface(const char *str) {
 return
  strcmp(str, platform_instance_extensions[0]) == 0 || 
  strcmp(str, platform_instance_extensions[1]) == 0 ||
  strcmp(str, platform_instance_extensions[2]) == 0 ||
  strcmp(str, platform_instance_extensions[3]) == 0 ||
  strcmp(str, platform_instance_extensions[4]) == 0 ||
  strcmp(str, platform_instance_extensions[5]) == 0 ||
  strcmp(str, platform_instance_extensions[6]) == 0;
}


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) { 
 ZAC_System_Panic("[validation] : %s\n\n\n", pCallbackData->pMessage);
 return VK_FALSE; // VK_TRUE would abort Vulkan call
}
 



/*
 vulkan instance creation
*/
void __ZAC_CreateInstance(ZAC_Ctxrender *ctx) {
 ctx->_min_swapchain_image_count = 2;
 ctx->_max_in_flight_frames = 2;
 
 if(volkInitialize() != VK_SUCCESS) {
  ZAC_System_Panic("__ZAC_CreateInstance() failed initializing vulkan.");
 }
 
 
 
 VkLayerProperties *layer_properties;
 uint32_t layer_properties_count;

 VkExtensionProperties *extension_properties;
 uint32_t extension_count, extension_names_count;
 const char** extension_names;


 vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);
 extension_names = (const char**)ZAC_System_AllocateMemory(sizeof(const char*) * extension_count);
 extension_properties = (VkExtensionProperties*)ZAC_System_AllocateMemory(sizeof(VkExtensionProperties) * extension_count);
 
 vkEnumerateInstanceExtensionProperties(NULL, &extension_count, extension_properties);

 extension_names_count = 0;
 for(uint32_t i = 0; i < extension_count; i++) {
  const char* ext_name = extension_properties[i].extensionName;
  if(strcmp(ext_name, "VK_KHR_surface") == 0 || any_of_platform_surface(ext_name)) {
   extension_names[extension_names_count++] = ext_name;
  }
 }
 
 VkBool32 has_validation_layer = VK_FALSE;
 const char* validation_layer = "VK_LAYER_KHRONOS_validation";
 vkEnumerateInstanceLayerProperties(&layer_properties_count, NULL);
 layer_properties = (VkLayerProperties*)ZAC_System_AllocateMemory(sizeof(VkLayerProperties) * layer_properties_count);
 vkEnumerateInstanceLayerProperties(&layer_properties_count, layer_properties);

 for(uint32_t i = 0; i < layer_properties_count; i++)
  if(strcmp(layer_properties[i].layerName, validation_layer) == 0)
   has_validation_layer = VK_TRUE;

 VkBool32 is_debug = VK_TRUE;
#ifdef NDEBUG
 is_debug = VK_FALSE;
#endif

 if(is_debug) {
  extension_names[extension_names_count++] = "VK_EXT_debug_utils";
 }


 VkApplicationInfo app_info;
 memset(&app_info, 0, sizeof(VkApplicationInfo));
 app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
 app_info.pApplicationName = "micro_engine_app";
 app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
 app_info.pEngineName = "micro_engine";
 app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
 app_info.apiVersion = VK_MAKE_VERSION(1, 0, 0);

 VkInstanceCreateInfo create_info;
 memset(&create_info, 0, sizeof(VkInstanceCreateInfo));
 create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
 create_info.pApplicationInfo = &app_info;
 

 if(is_debug) {
  create_info.enabledLayerCount = 1;
  create_info.ppEnabledLayerNames = &validation_layer;
 }

 create_info.enabledExtensionCount = extension_names_count;
 create_info.ppEnabledExtensionNames = extension_names;

 if(vkCreateInstance(&create_info, NULL, &ctx->_instance) != VK_SUCCESS)
  ZAC_System_Panic("__ZAC_CreateInstance() failed.");
 
 ZAC_System_FreeMemory(extension_properties);
 ZAC_System_FreeMemory(extension_names);
 ZAC_System_FreeMemory(layer_properties);
 
 volkLoadInstance(ctx->_instance);

 if(is_debug) {

 VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
 memset(&debug_create_info, 0, sizeof(VkDebugUtilsMessengerCreateInfoEXT));
 debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
 debug_create_info.messageSeverity =
  VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
 debug_create_info.messageType =
  VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
 debug_create_info.pfnUserCallback = debugCallback;

 PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(ctx->_instance, "vkCreateDebugUtilsMessengerEXT");
 if(func) {
  if(func(ctx->_instance, &debug_create_info, NULL, &ctx->_debug_messenger) != VK_SUCCESS)
   ZAC_System_Panic("__ZAC_CreateInstance() : failed to create debug utils messenger.");
 }
 }
}



void __ZAC_QueryPhysicalDevice(ZAC_Ctxrender *ctx) {
 VkPhysicalDevice *equiped_gpu;
 VkPhysicalDeviceFeatures *gpu_features;
 VkPhysicalDeviceProperties *gpu_properties;
 
 uint32_t physical_device_count, selected_index;

 vkEnumeratePhysicalDevices(ctx->_instance, &physical_device_count, NULL);
 
 if(physical_device_count == 0) {
  ZAC_System_Panic("__ZAC_QueryPhysicalDevice() : error no device count.");
 }
 
 gpu_features = (VkPhysicalDeviceFeatures*)ZAC_System_AllocateMemory(sizeof(VkPhysicalDeviceFeatures) * physical_device_count);
 gpu_properties = (VkPhysicalDeviceProperties*)ZAC_System_AllocateMemory(sizeof(VkPhysicalDeviceProperties) * physical_device_count);
 equiped_gpu = (VkPhysicalDevice*)ZAC_System_AllocateMemory(sizeof(VkPhysicalDevice) * physical_device_count);
 vkEnumeratePhysicalDevices(ctx->_instance, &physical_device_count, equiped_gpu);

 for(uint32_t i = 0; i < physical_device_count; i++) {
  vkGetPhysicalDeviceFeatures(equiped_gpu[i], &gpu_features[i]);
  vkGetPhysicalDeviceProperties(equiped_gpu[i], &gpu_properties[i]);
 }
 
 selected_index = 0xFFFFFFFF;
  
 /*select a feature */
 for(uint32_t i = 0; i < physical_device_count; i++) {
  if(__ZAC_CheckDeviceCapabilities(equiped_gpu[i], gpu_features[i], ctx)) {
   selected_index = i;
   break;
  }
 }
 
 if(selected_index == 0xFFFFFFFF) {
  ZAC_System_Panic("__ZAC_QueryPhysicalDevice() : no selected physical devices.");
 }
 
 ctx->_physical_device = equiped_gpu[selected_index];
 ctx->_physical_device_features = gpu_features[selected_index];
 ctx->_physical_device_properties = gpu_properties[selected_index];

 ZAC_System_FreeMemory(equiped_gpu);
 ZAC_System_FreeMemory(gpu_features);
 ZAC_System_FreeMemory(gpu_properties);
}


void __ZAC_CreateDeviceAndQueues(ZAC_Ctxrender *ctx) {
 float queue_priority = 1.0f; /* single queue */
 
 VkExtensionProperties *extension_properties;
 uint32_t extension_properties_size, extension_name_selected;
 const char** extension_names;
 
 vkEnumerateDeviceExtensionProperties(ctx->_physical_device, NULL, &extension_properties_size, NULL);
 extension_properties = (VkExtensionProperties*)ZAC_System_AllocateMemory(sizeof(VkExtensionProperties) * extension_properties_size);
 extension_names = (const char**)ZAC_System_AllocateMemory(sizeof(const char*) * extension_properties_size);
 vkEnumerateDeviceExtensionProperties(ctx->_physical_device, NULL, &extension_properties_size, extension_properties);

 extension_name_selected = 0;
 for(uint32_t i = 0; i < extension_properties_size; i++) {
  const char *ext_name = extension_properties[i].extensionName;
  //printf("%s\n", ext_name);
  for(uint32_t j = 0; j < required_device_extensions_count; j++) {
   if(strcmp(ext_name, required_device_extensions[j]) == 0)
    extension_names[extension_name_selected++] = ext_name;
  }
 }
 
 
 if(required_device_extensions_count != extension_name_selected) {
  ZAC_System_Panic("__ZAC_CreateDeviceAndQueues() : there are no selected extensions");
 }

 
 VkDeviceQueueCreateInfo device_queue_create_info[2];
 
 memset(device_queue_create_info, 0, sizeof(VkDeviceQueueCreateInfo) << 1);
 
 device_queue_create_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
 device_queue_create_info[0].queueFamilyIndex = ctx->_queue_family_indices[0];
 device_queue_create_info[0].queueCount = 1;
 device_queue_create_info[0].pQueuePriorities = &queue_priority;

 device_queue_create_info[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
 device_queue_create_info[1].queueFamilyIndex = ctx->_queue_family_indices[1];
 device_queue_create_info[1].queueCount = 1;
 device_queue_create_info[1].pQueuePriorities = &queue_priority;


 VkDeviceCreateInfo device_create_info;
 memset(&device_create_info, 0, sizeof(VkDeviceCreateInfo));
 device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
 device_create_info.pQueueCreateInfos = device_queue_create_info;
 device_create_info.queueCreateInfoCount = (ctx->_queue_family_indices[0] != ctx->_queue_family_indices[1]) ? 2 : 1;

 device_create_info.pEnabledFeatures = &ctx->_physical_device_features;

 device_create_info.enabledExtensionCount = extension_name_selected;
 device_create_info.ppEnabledExtensionNames = extension_names;

 if(vkCreateDevice(ctx->_physical_device, &device_create_info, NULL, &ctx->_device) != VK_SUCCESS) {
  ZAC_System_Panic("__ZAC_CreateDeviceAndQueues() : failed device creation");
 }
 
 volkLoadDevice(ctx->_device);

 vkGetDeviceQueue(ctx->_device, ctx->_queue_family_indices[0], 0, &ctx->_graphics_queue);
 vkGetDeviceQueue(ctx->_device, ctx->_queue_family_indices[1], 0, &ctx->_present_queue);

 ZAC_System_FreeMemory(extension_properties);
 ZAC_System_FreeMemory(extension_names);
 
}


void __ZAC_QuerySurfaceCapabilities(ZAC_Ctxrender *ctx) {
 vkGetPhysicalDeviceSurfaceCapabilitiesKHR(ctx->_physical_device, ctx->_surface, &ctx->_surface_capabilities);

 uint32_t surface_format_count, present_mode_count;
 VkSurfaceFormatKHR *surface_format;
 VkPresentModeKHR *present_mode;
 
 vkGetPhysicalDeviceSurfaceFormatsKHR(ctx->_physical_device, ctx->_surface, &surface_format_count, NULL);
 surface_format = (VkSurfaceFormatKHR*)ZAC_System_AllocateMemory(sizeof(VkSurfaceFormatKHR) * surface_format_count);
 vkGetPhysicalDeviceSurfaceFormatsKHR(ctx->_physical_device, ctx->_surface, &surface_format_count, surface_format);
 
 vkGetPhysicalDeviceSurfacePresentModesKHR(ctx->_physical_device, ctx->_surface, &present_mode_count, NULL);
 present_mode = (VkPresentModeKHR*)ZAC_System_AllocateMemory(sizeof(VkPresentModeKHR) * present_mode_count);
 vkGetPhysicalDeviceSurfacePresentModesKHR(ctx->_physical_device, ctx->_surface, &present_mode_count, present_mode);

 ctx->_surface_format = surface_format[0];


 VkBool32 has_chosen = VK_FALSE;
 //priority (standardized)
 for(uint32_t i = 0; i < surface_format_count; i++) {
  if((surface_format[i].format == VK_FORMAT_B8G8R8A8_SRGB && surface_format[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
  || (surface_format[i].format == VK_FORMAT_R8G8B8A8_SRGB && surface_format[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
  ) {
   ctx->_is_unorm = VK_FALSE;
   ctx->_surface_format = surface_format[i];
   has_chosen = VK_TRUE;
   break;
  }
 }
 /*
 //fallback
 for(uint32_t i = 0; (i < surface_format_count) && !has_chosen; i++) {
  if((surface_format[i].format == VK_FORMAT_B8G8R8A8_UNORM && surface_format[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
  || (surface_format[i].format == VK_FORMAT_R8G8B8A8_UNORM && surface_format[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
  || (surface_format[i].format == VK_FORMAT_B8G8R8A8_UNORM && surface_format[i].colorSpace == VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT)
  ) {
   ctx->_is_unorm = VK_TRUE;
   ctx->_surface_format = surface_format[i];
   has_chosen = VK_TRUE;
   break;
  }
 }
 */
 if(!has_chosen)
  ZAC_System_Panic("__ZAC_QuerySurfaceCapabilities() : no image format specified.");
 
 /*its guaranteed, no need checks */
 ctx->_present_mode = VK_PRESENT_MODE_FIFO_KHR;
/*
 for(uint32_t i = 0; i < present_mode_count; i++) {
  if(present_mode[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
   ctx->_present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
   break;
  }
 }*/
 ZAC_System_FreeMemory(surface_format);
 ZAC_System_FreeMemory(present_mode);
}


void __ZAC_CreateSwapchain(ZAC_Ctxrender *ctx) {

 vkGetPhysicalDeviceSurfaceCapabilitiesKHR(ctx->_physical_device, ctx->_surface, &ctx->_surface_capabilities);
 ctx->_min_swapchain_image_count = ctx->_surface_capabilities.minImageCount;

 ctx->_screen_size = ctx->_surface_capabilities.currentExtent;


 float aspect_ratio = (float)ctx->_screen_size.width / (float)ctx->_screen_size.height;
 if(ctx->_screen_size.height >= ctx->_screen_size.width) {
  /* portait */
  ctx->_projection_3d = ZAC_Mat4x4_Perspective(-1.57079632f / 2.0f, 1.0f / aspect_ratio, 1e-3f, 100.0f);
  ctx->_projection_2d = ZAC_Mat4x4_Ortho(-1.0f, 1.0f, -1.0f / aspect_ratio, 1.0f / aspect_ratio, -1.0f, 1.0f);
 } else {
  /* landscape */
  ctx->_projection_3d = ZAC_Mat4x4_Perspective(-1.57079632f / 2.0f, aspect_ratio, 1e-3f, 100.0f);
  ctx->_projection_2d = ZAC_Mat4x4_Ortho(-aspect_ratio, aspect_ratio, -1.0f, 1.0f, -1.0f, 1.0f);
 }

 
 if(ctx->_surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR) {
  /*270*/
  ZAC_Mat4x4 rot = ZAC_Mat4x4_FromQuaternion(ZAC_Quat4_FromEulerAngle(0.0f, 0.0f, -1.57079632f));
  ctx->_projection_2d = ZAC_Mat4x4_Mul(rot, ctx->_projection_2d);
  ctx->_projection_3d = ZAC_Mat4x4_Mul(rot, ctx->_projection_3d);
 } else if(ctx->_surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) {
  /*90*/
  ZAC_Mat4x4 rot = ZAC_Mat4x4_FromQuaternion(ZAC_Quat4_FromEulerAngle(0.0f, 0.0f, 1.57079632f));
  ctx->_projection_2d = ZAC_Mat4x4_Mul(rot, ctx->_projection_2d);
  ctx->_projection_3d = ZAC_Mat4x4_Mul(rot, ctx->_projection_3d);
 }


 memset(&ctx->_viewport, 0, sizeof(VkViewport));
 ctx->_viewport.width = (float)ctx->_screen_size.width;
 ctx->_viewport.height = (float)ctx->_screen_size.height;
 ctx->_viewport.minDepth = 0.0f;
 ctx->_viewport.maxDepth = 1.0f;

 memset(&ctx->_scissor, 0, sizeof(VkRect2D));
 ctx->_scissor.offset.x = 0;
 ctx->_scissor.offset.y = 0;
 ctx->_scissor.extent = ctx->_screen_size;
 
 VkSwapchainCreateInfoKHR swapchain_create_info;
 memset(&swapchain_create_info, 0, sizeof(VkSwapchainCreateInfoKHR));
 swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
 swapchain_create_info.surface = ctx->_surface;
 
 swapchain_create_info.minImageCount = ctx->_min_swapchain_image_count;
 swapchain_create_info.imageFormat = ctx->_surface_format.format;
 swapchain_create_info.imageColorSpace = ctx->_surface_format.colorSpace;
 swapchain_create_info.imageExtent = ctx->_screen_size;
 swapchain_create_info.imageArrayLayers = 1;
 swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
 
 if(ctx->_queue_family_indices[0] != ctx->_queue_family_indices[1]) {
  swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
  swapchain_create_info.queueFamilyIndexCount = 2;
  swapchain_create_info.pQueueFamilyIndices = ctx->_queue_family_indices;
 } else {
  swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapchain_create_info.queueFamilyIndexCount = 0; 
  swapchain_create_info.pQueueFamilyIndices = VK_NULL_HANDLE; 
 }
 
 swapchain_create_info.preTransform = ctx->_surface_capabilities.currentTransform;

 if(ctx->_surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) {
  swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
 } else if(ctx->_surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) {
  swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
 } else if(ctx->_surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) {
  swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
 } else if(ctx->_surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) {
  swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
 } else {
  ZAC_System_Panic("__ZAC_QuerySurfaceCapabilities() :  no composite alpha support");
 }

 swapchain_create_info.presentMode = ctx->_present_mode;
 swapchain_create_info.clipped = VK_TRUE;
 swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

 if(vkCreateSwapchainKHR(ctx->_device, &swapchain_create_info, NULL, &ctx->_swapchain) != VK_SUCCESS) {
  ZAC_System_Panic("__ZAC_QuerySurfaceCapabilities() : failed swapchain handle creation ");
 }
}



void __ZAC_CreateSwapchainImages(ZAC_Ctxrender *ctx) {
 VkImage *images_tmp;
 vkGetSwapchainImagesKHR(ctx->_device, ctx->_swapchain, &ctx->_max_swapchain_images, NULL);

 ctx->_swapchain_images = (ZAC_SwapchainImage*)ZAC_System_AllocateMemory(sizeof(ZAC_SwapchainImage) * ctx->_max_swapchain_images);

 images_tmp = (VkImage*)ZAC_System_AllocateMemory(sizeof(VkImage) * ctx->_max_swapchain_images);
 vkGetSwapchainImagesKHR(ctx->_device, ctx->_swapchain, &ctx->_max_swapchain_images, images_tmp);

 for(uint32_t i = 0; i < ctx->_max_swapchain_images; i++) {
  ctx->_swapchain_images[i]._image = images_tmp[i];
 }
 ZAC_System_FreeMemory(images_tmp);

  VkImageViewCreateInfo image_view_create_info;
  memset(&image_view_create_info, 0, sizeof(VkImageViewCreateInfo));
  image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  image_view_create_info.format = ctx->_surface_format.format;
  
  image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

  image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  image_view_create_info.subresourceRange.baseMipLevel = 0;
  image_view_create_info.subresourceRange.levelCount = 1;
  image_view_create_info.subresourceRange.baseArrayLayer = 0;
  image_view_create_info.subresourceRange.layerCount = 1;
  
 for(uint32_t i = 0; i < ctx->_max_swapchain_images; i++) {
  image_view_create_info.image = ctx->_swapchain_images[i]._image;

  if(vkCreateImageView(ctx->_device, &image_view_create_info, NULL, &ctx->_swapchain_images[i]._image_view) != VK_SUCCESS) {
   ZAC_System_Panic("__ZAC_CreateSwapchainImages() : failed swapchain image view creation ");
  }
 }
 
 
 /*depth image*/
 VkFormat formats[3] = {
 	VK_FORMAT_D32_SFLOAT_S8_UINT,
 	VK_FORMAT_D24_UNORM_S8_UINT,
 	VK_FORMAT_D32_SFLOAT,
 };
 
 ctx->_depth_format = __ZAC_FindImageFormat(ctx->_physical_device, formats, 3, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);


 ctx->_swapchain_depth_images = (ZAC_SwapchainImage*)ZAC_System_AllocateMemory(sizeof(ZAC_SwapchainImage) * ctx->_max_in_flight_frames);

 VkMemoryRequirements *memory_requirements = (VkMemoryRequirements*)ZAC_System_AllocateMemory(sizeof(VkMemoryRequirements) * ctx->_max_in_flight_frames);
 VkDeviceSize *offsets = (VkDeviceSize*)ZAC_System_AllocateMemory(sizeof(VkDeviceSize) * ctx->_max_in_flight_frames);
 VkImageCreateInfo image_create_info;
 memset(&image_create_info, 0, sizeof(VkImageCreateInfo));
 image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
 image_create_info.imageType = VK_IMAGE_TYPE_2D;
 image_create_info.extent.width = ctx->_screen_size.width;
 image_create_info.extent.height = ctx->_screen_size.height;
 image_create_info.extent.depth = 1;
 image_create_info.mipLevels = 1;
 image_create_info.arrayLayers = 1;
 image_create_info.format = ctx->_depth_format;
 image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
 image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
 image_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
 image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
 image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
 image_create_info.flags = 0;

 VkDeviceSize current_size = 0;
 for(uint32_t i = 0; i < ctx->_max_in_flight_frames; i++) {
  if(vkCreateImage(ctx->_device, &image_create_info, NULL, &ctx->_swapchain_depth_images[i]._image) != VK_SUCCESS) {
   ZAC_System_Panic("__ZAC_CreateSwapchainImages() : failed swapchain image view creation ");
  }
  vkGetImageMemoryRequirements(ctx->_device, ctx->_swapchain_depth_images[i]._image, &memory_requirements[i]);
  current_size = (current_size + memory_requirements[i].alignment - 1) & ~(memory_requirements[i].alignment - 1);
  
  offsets[i] = current_size;
  current_size += memory_requirements[i].size;
 }

 VkMemoryAllocateInfo allocate_info;
 memset(&allocate_info, 0, sizeof(VkMemoryAllocateInfo));
 allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
 allocate_info.allocationSize = current_size;
 allocate_info.memoryTypeIndex = __ZAC_FindMemoryType(
  ctx->_physical_device,
  memory_requirements[0].memoryTypeBits,
  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
 );

 if(vkAllocateMemory(ctx->_device, &allocate_info, NULL, &ctx->_depth_memory) != VK_SUCCESS) {
  ZAC_System_Panic("__ZAC_CreateSwapchainImages() : Failed to allocate memory!.");
 }

 for(uint32_t i = 0; i < ctx->_max_in_flight_frames; i++) {
  vkBindImageMemory(ctx->_device, ctx->_swapchain_depth_images[i]._image, ctx->_depth_memory, offsets[i]);
 }
 
 ZAC_System_FreeMemory(memory_requirements);
 ZAC_System_FreeMemory(offsets);

 memset(&image_create_info, 0, sizeof(VkImageCreateInfo));
 image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
 image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
 image_view_create_info.format = ctx->_depth_format;
  
 image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
 image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
 image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
 image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

 image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
 image_view_create_info.subresourceRange.baseMipLevel = 0;
 image_view_create_info.subresourceRange.levelCount = 1;
 image_view_create_info.subresourceRange.baseArrayLayer = 0;
 image_view_create_info.subresourceRange.layerCount = 1;
  

 for(uint32_t i = 0; i < ctx->_max_in_flight_frames; i++) {
  image_view_create_info.image = ctx->_swapchain_depth_images[i]._image;
  if(vkCreateImageView(ctx->_device, &image_view_create_info, NULL, &ctx->_swapchain_depth_images[i]._image_view) != VK_SUCCESS) {
    ZAC_System_Panic("__ZAC_CreateSwapchainImages() : failed swapchain depth image view creation ");
  }
 }
 
}



void __ZAC_CreateSwapchainRenderpass(ZAC_Ctxrender *ctx) { 
 VkAttachmentDescription color_attachment_description;
 memset(&color_attachment_description, 0, sizeof(VkAttachmentDescription));
 color_attachment_description.format = ctx->_surface_format.format;
 color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
 color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
 color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
 color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
 color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
 color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
 color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

 
 VkAttachmentDescription depth_attachment_description;
 memset(&depth_attachment_description, 0, sizeof(VkAttachmentDescription));
 depth_attachment_description.format = ctx->_depth_format;
 depth_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
 depth_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
 depth_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
 depth_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
 depth_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
 depth_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
 depth_attachment_description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

 VkAttachmentDescription attachment_descriptions[2] = {color_attachment_description, depth_attachment_description};
 
 VkAttachmentReference color_attachment_ref;
 memset(&color_attachment_ref, 0, sizeof(VkAttachmentReference));
 color_attachment_ref.attachment = 0;
 color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


 VkAttachmentReference depth_attachment_ref;
 memset(&depth_attachment_ref, 0, sizeof(VkAttachmentReference));
 depth_attachment_ref.attachment = 1;
 depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


 VkSubpassDescription subpass_description;
 memset(&subpass_description, 0, sizeof(VkSubpassDescription));
 subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
 subpass_description.colorAttachmentCount = 1;
 subpass_description.pColorAttachments = &color_attachment_ref;
 subpass_description.pDepthStencilAttachment = &depth_attachment_ref;
 
 VkSubpassDependency subpass_dependency;
 memset(&subpass_dependency, 0, sizeof(VkSubpassDependency));
 subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
 subpass_dependency.dstSubpass = 0;
 /*
 subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
 subpass_dependency.srcAccessMask = 0;
 subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
 subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
*/
 subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
 subpass_dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
 subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
 subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


 VkRenderPassCreateInfo render_pass_create_info;
 memset(&render_pass_create_info, 0, sizeof(VkRenderPassCreateInfo));
 render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
 render_pass_create_info.attachmentCount = 2;
 render_pass_create_info.pAttachments = attachment_descriptions;
 render_pass_create_info.subpassCount = 1;
 render_pass_create_info.pSubpasses = &subpass_description;
 render_pass_create_info.dependencyCount = 1;
 render_pass_create_info.pDependencies = &subpass_dependency;

 if(vkCreateRenderPass(ctx->_device, &render_pass_create_info, NULL, &ctx->_render_pass) != VK_SUCCESS) {
  ZAC_System_Panic("__ZAC_CreateSwapchainRenderpass() : failed render pass creation");
 }
}



void __ZAC_CreateSwapchainFramebuffer(ZAC_Ctxrender *ctx) {

 VkFramebufferCreateInfo framebuffer_create_info;
 memset(&framebuffer_create_info, 0, sizeof(VkFramebufferCreateInfo));
 framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
 framebuffer_create_info.renderPass = ctx->_render_pass;
 framebuffer_create_info.width = ctx->_screen_size.width;
 framebuffer_create_info.height = ctx->_screen_size.height;
 framebuffer_create_info.layers = 1;

 framebuffer_create_info.attachmentCount = 2;
 ctx->_swapchain_framebuffers = (VkFramebuffer*)ZAC_System_AllocateMemory(sizeof(VkFramebuffer) * ctx->_max_swapchain_images * ctx->_max_in_flight_frames);
 
 for(uint32_t i = 0; i < ctx->_max_swapchain_images; i++) {
  for(uint32_t j = 0; j < ctx->_max_in_flight_frames; j++) {

   VkImageView attachments[2] = {
    ctx->_swapchain_images[i]._image_view, // color image view 
    ctx->_swapchain_depth_images[j]._image_view // depth image view 
   };

   framebuffer_create_info.pAttachments = attachments;
  
   const uint32_t index = i * ctx->_max_in_flight_frames + j;
  
   assert(!((ctx->_max_swapchain_images * ctx->_max_in_flight_frames) < index));
  
   if(vkCreateFramebuffer(ctx->_device, &framebuffer_create_info, NULL, &ctx->_swapchain_framebuffers[index]) != VK_SUCCESS) {
    ZAC_System_Panic("__ZAC_CreateSwapchainFramebuffer() : failed framebuffer creation");
   }
  }
 }
}



void __ZAC_CreateCommandPool(ZAC_Ctxrender *ctx) {
 VkFenceCreateInfo staging_fence_create_info;
 memset(&staging_fence_create_info, 0, sizeof(VkFenceCreateInfo));
 staging_fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
 staging_fence_create_info.flags = 0;
 
 
 if(vkCreateFence(ctx->_device, &staging_fence_create_info, NULL, &ctx->_staging_fence) != VK_SUCCESS) {
  ZAC_System_Panic("failed fence creation");
 }

 VkCommandPoolCreateInfo pool_create_info;
 memset(&pool_create_info, 0, sizeof(VkCommandPoolCreateInfo));
 
 pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
 pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
 pool_create_info.queueFamilyIndex = ctx->_queue_family_indices[0];
 
 if(vkCreateCommandPool(ctx->_device, &pool_create_info, NULL, &ctx->_command_pool) != VK_SUCCESS) {
  ZAC_System_Panic("__ZAC_CreateCommandPool() : failed command pool creation.");
 }
 
 VkCommandBufferAllocateInfo command_buffer_allocate_info;
 memset(&command_buffer_allocate_info, 0, sizeof(VkCommandBufferAllocateInfo));
 
 command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
 command_buffer_allocate_info.commandPool = ctx->_command_pool;
 command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
 command_buffer_allocate_info.commandBufferCount = ctx->_max_in_flight_frames;
 
 ctx->_command_buffers = (VkCommandBuffer*)ZAC_System_AllocateMemory(sizeof(VkCommandBuffer) * ctx->_max_in_flight_frames);
 
 if(vkAllocateCommandBuffers(ctx->_device, &command_buffer_allocate_info, ctx->_command_buffers) != VK_SUCCESS) {
  ZAC_System_Panic(" failed command buffer creation");
 }
 
 
 
 
 ctx->_synchronizers = (ZAC_SwapchainSynchronizer*)ZAC_System_AllocateMemory(sizeof(ZAC_SwapchainSynchronizer) * ctx->_max_in_flight_frames);
 
 VkSemaphoreCreateInfo semaphore_create_info;
 memset(&semaphore_create_info, 0, sizeof(VkSemaphoreCreateInfo));
 semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

 VkFenceCreateInfo fence_create_info;
 memset(&fence_create_info, 0, sizeof(VkFenceCreateInfo));
 fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
 fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
 
 for(uint32_t i = 0; i < ctx->_max_in_flight_frames; i++) {
  if(vkCreateSemaphore(ctx->_device, &semaphore_create_info, NULL, &ctx->_synchronizers[i]._semaphore) != VK_SUCCESS) {
   ZAC_System_Panic("failed semaphore creation");
  }
  
  if(vkCreateFence(ctx->_device, &fence_create_info, NULL, &ctx->_synchronizers[i]._fence) != VK_SUCCESS) {
   ZAC_System_Panic("failed fence creation");
  }
 }

 ctx->_submit_semaphores = (VkSemaphore*)ZAC_System_AllocateMemory(sizeof(VkSemaphore) * ctx->_max_swapchain_images);
 for(uint32_t i = 0; i < ctx->_max_swapchain_images; i++) {
  if(vkCreateSemaphore(ctx->_device, &semaphore_create_info, NULL, &ctx->_submit_semaphores[i]) != VK_SUCCESS) {
   ZAC_System_Panic("failed semaphore creation");
  }
 }
 
  
 /*
  immediate command buffer
 */
 memset(&command_buffer_allocate_info, 0, sizeof(VkCommandBufferAllocateInfo));
 
 command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
 command_buffer_allocate_info.commandPool = ctx->_command_pool;
 command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
 command_buffer_allocate_info.commandBufferCount = 1;
  
 if(vkAllocateCommandBuffers(ctx->_device, &command_buffer_allocate_info, &ctx->_immediate_cmd_buffer) != VK_SUCCESS) {
  ZAC_System_Panic(" failed command buffer creation");
 }
}


void __ZAC_DepthBufferImageTransition(ZAC_Ctxrender *ctx) {

 VkImageMemoryBarrier image_memory_barrier;
 memset(&image_memory_barrier, 0, sizeof(VkImageMemoryBarrier));
 image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
 image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
 image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

 image_memory_barrier.srcAccessMask = 0;
 image_memory_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    
 image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
 image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

 image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
 if(__ZAC_HasStencil(ctx->_depth_format))
  image_memory_barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
 
 image_memory_barrier.subresourceRange.baseMipLevel = 0;
 image_memory_barrier.subresourceRange.levelCount = 1;
 image_memory_barrier.subresourceRange.baseArrayLayer = 0;
 image_memory_barrier.subresourceRange.layerCount = 1;
 

 VkCommandBufferBeginInfo begin_info;
 memset(&begin_info, 0, sizeof(VkCommandBufferBeginInfo));
 begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
 begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

 if(vkBeginCommandBuffer(ctx->_immediate_cmd_buffer, &begin_info) != VK_SUCCESS)
  ZAC_System_Panic("begin command buffer failed.");
 
 for(uint32_t i = 0; i < ctx->_max_in_flight_frames; i++) {
 image_memory_barrier.image = ctx->_swapchain_depth_images[i]._image;
  vkCmdPipelineBarrier(ctx->_immediate_cmd_buffer,
   VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, //src stage
   VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, //dst stage
   0,
   0, VK_NULL_HANDLE,
   0, VK_NULL_HANDLE,
   1, &image_memory_barrier
   );
 }
 
 vkEndCommandBuffer(ctx->_immediate_cmd_buffer);

 VkSubmitInfo submit_info;
 memset(&submit_info, 0, sizeof(VkSubmitInfo));
 submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
 submit_info.commandBufferCount = 1;
 submit_info.pCommandBuffers = &ctx->_immediate_cmd_buffer;

 vkQueueSubmit(ctx->_graphics_queue, 1, &submit_info, ctx->_staging_fence);
 vkWaitForFences(ctx->_device, 1, &ctx->_staging_fence, VK_TRUE, UINT64_MAX);
 vkResetFences(ctx->_device, 1, &ctx->_staging_fence);
}



void __ZAC_RecreateSwapchain(ZAC_Ctxrender *ctx) {
// vkDeviceWaitIdle(ctx->_device);
 VkFence guard_lock[2];
 guard_lock[0] = ctx->_synchronizers[0]._fence;
 guard_lock[1] = ctx->_synchronizers[1]._fence;

 vkWaitForFences(ctx->_device, ctx->_max_in_flight_frames, guard_lock, VK_TRUE, UINT64_MAX);

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

 __ZAC_CreateSwapchain(ctx);
 __ZAC_CreateSwapchainImages(ctx);
 __ZAC_CreateSwapchainFramebuffer(ctx);
 __ZAC_DepthBufferImageTransition(ctx);
}



void __ZAC_CreateDummyObjects(ZAC_Ctxrender *ctx) {
 
 VkMemoryAllocateInfo allocate_info;
 memset(&allocate_info, 0, sizeof(VkMemoryAllocateInfo));
 allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
 allocate_info.allocationSize = ctx->_physical_device_properties.limits.minMemoryMapAlignment * 4;
 allocate_info.memoryTypeIndex = __ZAC_FindMemoryTypeIndex(ctx->_physical_device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

 if(vkAllocateMemory(ctx->_device, &allocate_info, NULL, &ctx->_dummy_device_memory) != VK_SUCCESS) {
  ZAC_System_Panic("__ZAC_CreateDummyObjects() : Failed to allocate memory!.");
 }

 VkImageCreateInfo image_create_info;
 memset(&image_create_info, 0, sizeof(VkImageCreateInfo));
 image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
 image_create_info.imageType = VK_IMAGE_TYPE_2D;
 image_create_info.extent.width = 2;
 image_create_info.extent.height = 2;
 image_create_info.extent.depth = 1;
 image_create_info.mipLevels = 1;
 image_create_info.arrayLayers = 1;
 image_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
 image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
 image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
 image_create_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
 image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
 image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
 image_create_info.flags = 0;

 if(vkCreateImage(ctx->_device, &image_create_info, NULL, &ctx->_dummy_image) != VK_SUCCESS) {
  ZAC_System_Panic("__ZAC_CreateDummyObjects() : failed swapchain depth image view creation ");
 }
 
 vkBindImageMemory(ctx->_device, ctx->_dummy_image, ctx->_dummy_device_memory, 0);



/*
 image view
*/
 VkImageViewCreateInfo image_view_create_info;
 memset(&image_view_create_info, 0, sizeof(VkImageViewCreateInfo));
 image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
 image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
 image_view_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
  
 image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
 image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
 image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
 image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

 image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
 image_view_create_info.subresourceRange.baseMipLevel = 0;
 image_view_create_info.subresourceRange.levelCount = 1;
 image_view_create_info.subresourceRange.baseArrayLayer = 0;
 image_view_create_info.subresourceRange.layerCount = 1;
  

 image_view_create_info.image = ctx->_dummy_image;
 if(vkCreateImageView(ctx->_device, &image_view_create_info, NULL, &ctx->_dummy_image_view) != VK_SUCCESS) {
  ZAC_System_Panic("__ZAC_CreateDummyObjects() : failed swapchain depth image view creation ");
 }

/*
 image transition
*/


 VkCommandBufferBeginInfo begin_info;
 memset(&begin_info, 0, sizeof(VkCommandBufferBeginInfo));
 begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
 begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

 if(vkBeginCommandBuffer(ctx->_immediate_cmd_buffer, &begin_info) != VK_SUCCESS)
  ZAC_System_Panic("begin command buffer failed.");


 __ZAC_TransitionColorImageLayout(ctx->_immediate_cmd_buffer, ctx->_dummy_image, 
  VK_IMAGE_LAYOUT_UNDEFINED, 
  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 
  0, 
  VK_ACCESS_SHADER_READ_BIT, 
  VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
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


/*
 dummy buffer
*/
 
 VkBufferCreateInfo buffer_create_info;
 memset(&buffer_create_info, 0, sizeof(VkBufferCreateInfo));
 buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
 buffer_create_info.size = 32;
 buffer_create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
 buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

 if(vkCreateBuffer(ctx->_device, &buffer_create_info, NULL, &ctx->_dummy_buffer) != VK_SUCCESS) {
  ZAC_System_Panic("Failed to create buffer!");
 }

 vkBindBufferMemory(ctx->_device, ctx->_dummy_buffer, ctx->_dummy_device_memory, 0);
}



void __ZAC_CreateStagingMemory(ZAC_Ctxrender *ctx) {
/*
 staging buffer 
*/
 VkBufferCreateInfo buffer_create_info;
 memset(&buffer_create_info, 0, sizeof(VkBufferCreateInfo));
 buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
 buffer_create_info.size = 32;
 buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
 buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
 VkBuffer tmp_buffer;
 if(vkCreateBuffer(ctx->_device, &buffer_create_info, NULL, &tmp_buffer) != VK_SUCCESS) {
  ZAC_System_Panic("Failed to create buffer!");
 }

 VkMemoryRequirements requirements;
 vkGetBufferMemoryRequirements(ctx->_device, tmp_buffer, &requirements);
 vkDestroyBuffer(ctx->_device, tmp_buffer, NULL);

 ctx->_staging_memory_requirements = requirements.memoryTypeBits;


/*
 shared buffer
*/
 
 memset(&buffer_create_info, 0, sizeof(VkBufferCreateInfo));
 buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
 buffer_create_info.size = 32;
 buffer_create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
 buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
 if(vkCreateBuffer(ctx->_device, &buffer_create_info, NULL, &tmp_buffer) != VK_SUCCESS) {
  ZAC_System_Panic("Failed to create buffer!");
 }

 vkGetBufferMemoryRequirements(ctx->_device, tmp_buffer, &requirements);
 vkDestroyBuffer(ctx->_device, tmp_buffer, NULL);

 ctx->_shared_memory_requirements = requirements.memoryTypeBits;


/*
 local buffer
*/

 memset(&buffer_create_info, 0, sizeof(VkBufferCreateInfo));
 buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
 buffer_create_info.size = 32;
 buffer_create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
 buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
 if(vkCreateBuffer(ctx->_device, &buffer_create_info, NULL, &tmp_buffer) != VK_SUCCESS) {
  ZAC_System_Panic("Failed to create buffer!");
 }

 vkGetBufferMemoryRequirements(ctx->_device, tmp_buffer, &requirements);
 vkDestroyBuffer(ctx->_device, tmp_buffer, NULL);

 ctx->_local_memory_requirements = requirements.memoryTypeBits;



/*
 local image
*/

 VkImageCreateInfo image_create_info;
 memset(&image_create_info, 0, sizeof(VkImageCreateInfo));
 image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
 image_create_info.imageType = VK_IMAGE_TYPE_2D;
 image_create_info.extent.width = 5;
 image_create_info.extent.height = 5;
 image_create_info.extent.depth = 1;
 image_create_info.mipLevels = 1;
 image_create_info.arrayLayers = 1;
 image_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
 image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;

 image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
 image_create_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
 image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
 image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
 image_create_info.flags = 0;

 VkImage tmp_image;
 if(vkCreateImage(ctx->_device, &image_create_info, NULL, &tmp_image) != VK_SUCCESS) {
  ZAC_System_Panic("create_image() : failed swapchain depth image view creation ");
 }

 vkGetImageMemoryRequirements(ctx->_device, tmp_image, &requirements);
 vkDestroyImage(ctx->_device, tmp_image, NULL);

 ctx->_local_memory_requirements &= requirements.memoryTypeBits;



 if(!ctx->_shared_memory_requirements)
  ZAC_System_Panic("__ZAC_CreateDummyObjects() : Failed to allocate memory!.");

 if(!ctx->_local_memory_requirements)
  ZAC_System_Panic("__ZAC_CreateDummyObjects() : Failed to allocate memory!.");


 ctx->_staging_memory_size = 1024 * 1024 * 1; // 1 mb
 
 VkMemoryAllocateInfo allocate_info;
 memset(&allocate_info, 0, sizeof(VkMemoryAllocateInfo));
 allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
 allocate_info.allocationSize = ctx->_staging_memory_size;

 allocate_info.memoryTypeIndex = __ZAC_FindMemoryType(
  ctx->_physical_device,
  ctx->_staging_memory_requirements,
  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
 );
 
 if(vkAllocateMemory(ctx->_device, &allocate_info, NULL, &ctx->_staging_memory) != VK_SUCCESS) {
  ZAC_System_Panic("__ZAC_CreateDummyObjects() : Failed to allocate memory!.");
 }
 vkMapMemory(ctx->_device, ctx->_staging_memory, 0, ctx->_staging_memory_size, 0, &ctx->_mapped_staging_memory);
}




void __ZAC_CreateDescriptorSetLayouts(ZAC_Ctxrender *ctx) {

 VkDescriptorSetLayoutBinding texture_binding;
 memset(&texture_binding, 0, sizeof(VkDescriptorSetLayoutBinding));
 texture_binding.binding = 0;
 texture_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
 texture_binding.descriptorCount = 8;
 texture_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
 texture_binding.pImmutableSamplers = NULL;

 VkDescriptorSetLayoutCreateInfo layout_info;
 memset(&layout_info, 0, sizeof(VkDescriptorSetLayoutCreateInfo));
 layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
 layout_info.bindingCount = 1;
 layout_info.pBindings = &texture_binding;

 if(vkCreateDescriptorSetLayout(ctx->_device, &layout_info, NULL, &ctx->_textured_descriptor_layout) != VK_SUCCESS) {
 	ZAC_System_Panic(" descriptor set layout creation failed");
 }

 VkDescriptorSetLayoutBinding ubo_binding;
 memset(&ubo_binding, 0, sizeof(VkDescriptorSetLayoutBinding));
 ubo_binding.binding = 0;
 ubo_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
 ubo_binding.descriptorCount = 1;
 ubo_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
 ubo_binding.pImmutableSamplers = NULL;

 memset(&layout_info, 0, sizeof(VkDescriptorSetLayoutCreateInfo));
 layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
 layout_info.bindingCount = 1;
 layout_info.pBindings = &ubo_binding;

 if(vkCreateDescriptorSetLayout(ctx->_device, &layout_info, NULL, &ctx->_ubo_descriptor_layout) != VK_SUCCESS) {
 	ZAC_System_Panic(" descriptor set layout creation failed");
 }
}



void __ZAC_CreateSamplers(ZAC_Ctxrender *ctx) {

 VkSamplerCreateInfo sampler_create_info;
 memset(&sampler_create_info, 0, sizeof(VkSamplerCreateInfo));
 
 sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
 sampler_create_info.magFilter = VK_FILTER_LINEAR;
 sampler_create_info.minFilter = VK_FILTER_LINEAR;
 sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
 sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
 sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
 sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
 sampler_create_info.unnormalizedCoordinates = VK_FALSE;
 sampler_create_info.compareEnable = VK_FALSE;
 sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
 sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
 sampler_create_info.anisotropyEnable = VK_FALSE;
 sampler_create_info.maxAnisotropy = 1.0f;
 sampler_create_info.mipLodBias = 0.0f;
 sampler_create_info.minLod = 0.0f;
 sampler_create_info.maxLod = 0.0f;
 
 if(vkCreateSampler(ctx->_device, &sampler_create_info, NULL, &ctx->_linear_sample) != VK_SUCCESS) {
 	ZAC_System_Panic("sampler creation failed.");
 }
 
 memset(&sampler_create_info, 0, sizeof(VkSamplerCreateInfo));
 sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
 sampler_create_info.magFilter = VK_FILTER_NEAREST;
 sampler_create_info.minFilter = VK_FILTER_NEAREST;
 sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
 sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
 sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
 sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
 sampler_create_info.unnormalizedCoordinates = VK_FALSE;
 sampler_create_info.compareEnable = VK_FALSE;
 sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
 sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
 sampler_create_info.anisotropyEnable = VK_FALSE;
 sampler_create_info.maxAnisotropy = 1.0f;
 sampler_create_info.mipLodBias = 0.0f;
 sampler_create_info.minLod = 0.0f;
 sampler_create_info.maxLod = 0.0f;
 
 if(vkCreateSampler(ctx->_device, &sampler_create_info, NULL, &ctx->_nearest_sample) != VK_SUCCESS) {
 	ZAC_System_Panic("sampler creation failed.");
 }
}





