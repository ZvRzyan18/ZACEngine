#ifndef ZAC_CTXINTERNAL_H
#define ZAC_CTXINTERNAL_H

#include "volk.h"
#include "zac/renderer/ctxrender.h"

/*
 NOTE : this is internal/core part and should not be 
 included outside of engine
*/

void __ZAC_CreateInstance(ZAC_Ctxrender *ctx);
void __ZAC_QueryPhysicalDevice(ZAC_Ctxrender *ctx);
void __ZAC_CreateDeviceAndQueues(ZAC_Ctxrender *ctx);
void __ZAC_QuerySurfaceCapabilities(ZAC_Ctxrender *ctx);
void __ZAC_CreateSwapchain(ZAC_Ctxrender *ctx);
void __ZAC_CreateSwapchainImages(ZAC_Ctxrender *ctx);
void __ZAC_CreateSwapchainRenderpass(ZAC_Ctxrender *ctx);
void __ZAC_CreateSwapchainFramebuffer(ZAC_Ctxrender *ctx);
void __ZAC_CreateCommandPool(ZAC_Ctxrender *ctx);
void __ZAC_DepthBufferImageTransition(ZAC_Ctxrender *ctx);
void __ZAC_RecreateSwapchain(ZAC_Ctxrender *ctx);
void __ZAC_CreateDummyObjects(ZAC_Ctxrender *ctx);
void __ZAC_CreateStagingMemory(ZAC_Ctxrender *ctx);
void __ZAC_CreateDescriptorSetLayouts(ZAC_Ctxrender *ctx);
void __ZAC_CreateSamplers(ZAC_Ctxrender *ctx);

#endif


