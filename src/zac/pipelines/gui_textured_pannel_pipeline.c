#define __ZAC_INTERNAL__

#include "zac/pipelines/gui_textured_pannel_pipeline.h"
#include "zac/system/system.h"
#include <string.h>


void __ZAC_CreateTexturedPannelPipeline(ZAC_Ctxrender *ctx, ZAC_Pipelines *p, uintptr_t shaders[2], size_t shader_size[2], size_t pc_size) {
	VkShaderModule vs, fs;
	VkShaderModuleCreateInfo vertex_module_create_info;
	memset(&vertex_module_create_info, 0, sizeof(VkShaderModuleCreateInfo));
 vertex_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
 vertex_module_create_info.codeSize = (VkDeviceSize)shader_size[0];
 vertex_module_create_info.pCode = (uint32_t*)shaders[0];

	VkShaderModuleCreateInfo fragment_module_create_info;
	memset(&fragment_module_create_info, 0, sizeof(VkShaderModuleCreateInfo));
 fragment_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
 fragment_module_create_info.codeSize = (VkDeviceSize)shader_size[1];
 fragment_module_create_info.pCode = (uint32_t*)shaders[1];

 if(vkCreateShaderModule(ctx->_device, &vertex_module_create_info, NULL, &vs) != VK_SUCCESS) {
 	ZAC_System_Panic(" failed creating shader module");
 }
 
 if(vkCreateShaderModule(ctx->_device, &fragment_module_create_info, NULL, &fs) != VK_SUCCESS) {
 	ZAC_System_Panic(" failed creating shader module");
 }
 
 
 VkPipelineShaderStageCreateInfo shader_stage_create_info[2];
 memset(shader_stage_create_info, 0, sizeof(VkPipelineShaderStageCreateInfo) * 2);
 
 shader_stage_create_info[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
 shader_stage_create_info[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
 shader_stage_create_info[0].module = vs;
 shader_stage_create_info[0].pName = "main";

 shader_stage_create_info[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
 shader_stage_create_info[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
 shader_stage_create_info[1].module = fs;
 shader_stage_create_info[1].pName = "main";

 VkDynamicState dynamic_states[2] = {
  VK_DYNAMIC_STATE_VIEWPORT,
  VK_DYNAMIC_STATE_SCISSOR
 };

 VkPipelineDynamicStateCreateInfo dynamic_state_create_info;
 memset(&dynamic_state_create_info, 0, sizeof(VkPipelineDynamicStateCreateInfo));
 dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
 dynamic_state_create_info.dynamicStateCount = 2;
 dynamic_state_create_info.pDynamicStates = dynamic_states;

 VkPipelineInputAssemblyStateCreateInfo input_assembly_state;
 memset(&input_assembly_state, 0, sizeof(VkPipelineInputAssemblyStateCreateInfo));
 input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
 input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
 input_assembly_state.primitiveRestartEnable = VK_FALSE;
	
	VkPipelineViewportStateCreateInfo viewport_state;
	memset(&viewport_state, 0, sizeof(VkPipelineViewportStateCreateInfo));
 viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
 viewport_state.viewportCount = 1;
 viewport_state.pViewports = &ctx->_viewport;
 viewport_state.scissorCount = 1;
 viewport_state.pScissors = &ctx->_scissor;
 
 VkPipelineRasterizationStateCreateInfo rasterization_state;
 memset(&rasterization_state, 0, sizeof(VkPipelineRasterizationStateCreateInfo));
 rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
 rasterization_state.depthClampEnable = VK_FALSE;
	rasterization_state.rasterizerDiscardEnable = VK_FALSE;
	rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
	rasterization_state.lineWidth = 1.0f;
	rasterization_state.cullMode = VK_CULL_MODE_NONE;
 rasterization_state.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterization_state.depthBiasEnable = VK_FALSE;
 rasterization_state.depthBiasConstantFactor = 0.0f; 
 rasterization_state.depthBiasClamp = 0.0f;
 rasterization_state.depthBiasSlopeFactor = 0.0f; 

 VkPipelineMultisampleStateCreateInfo multisample_state;
 memset(&multisample_state, 0, sizeof(VkPipelineMultisampleStateCreateInfo));
 multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
 multisample_state.sampleShadingEnable = VK_FALSE;
 multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
 multisample_state.minSampleShading = 1.0f; 
 multisample_state.pSampleMask = VK_NULL_HANDLE;
 multisample_state.alphaToCoverageEnable = VK_FALSE; 
 multisample_state.alphaToOneEnable = VK_FALSE; 
 
 VkPipelineDepthStencilStateCreateInfo depth_stencil_state;
 memset(&depth_stencil_state, 0, sizeof(VkPipelineDepthStencilStateCreateInfo));
 depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
 depth_stencil_state.depthTestEnable = VK_FALSE; 
 depth_stencil_state.depthWriteEnable = VK_FALSE;
 depth_stencil_state.depthCompareOp = VK_COMPARE_OP_LESS;
 depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
 depth_stencil_state.minDepthBounds = 0.0f;
 depth_stencil_state.maxDepthBounds = 1.0f;
 depth_stencil_state.stencilTestEnable = VK_FALSE;

 VkPipelineColorBlendAttachmentState color_blend_attachment_state;
 memset(&color_blend_attachment_state, 0, sizeof(VkPipelineColorBlendAttachmentState));
 color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	color_blend_attachment_state.blendEnable = VK_TRUE;
 color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
 color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
 color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
 color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
 color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
 color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
 
 VkPipelineColorBlendStateCreateInfo color_blend_state;
 memset(&color_blend_state, 0, sizeof(VkPipelineColorBlendStateCreateInfo));
 color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
 color_blend_state.logicOpEnable = VK_FALSE;
 color_blend_state.logicOp = VK_LOGIC_OP_COPY;
 color_blend_state.attachmentCount = 1;
 color_blend_state.pAttachments = &color_blend_attachment_state;
 color_blend_state.blendConstants[0] = 0.0f;
 color_blend_state.blendConstants[1] = 0.0f; 
 color_blend_state.blendConstants[2] = 0.0f;
 color_blend_state.blendConstants[3] = 0.0f;

  
 VkPipelineVertexInputStateCreateInfo vertex_input_state;
 memset(&vertex_input_state, 0, sizeof(VkPipelineVertexInputStateCreateInfo));
 vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
 vertex_input_state.vertexBindingDescriptionCount = 0;
 vertex_input_state.pVertexBindingDescriptions = NULL;
 vertex_input_state.vertexAttributeDescriptionCount = 0;
 vertex_input_state.pVertexAttributeDescriptions = NULL;


 VkPushConstantRange pushConstantRange;
 memset(&pushConstantRange, 0, sizeof(VkPushConstantRange));
 pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
 pushConstantRange.offset = 0;
 pushConstantRange.size = pc_size;

 VkPipelineLayoutCreateInfo pipeline_layout_create_info;
 memset(&pipeline_layout_create_info, 0, sizeof(VkPipelineLayoutCreateInfo));
 pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
 pipeline_layout_create_info.setLayoutCount = 1; 
 pipeline_layout_create_info.pSetLayouts = &ctx->_textured_descriptor_layout; 
 pipeline_layout_create_info.pushConstantRangeCount = 1; 
 pipeline_layout_create_info.pPushConstantRanges = &pushConstantRange; 

 if(vkCreatePipelineLayout(ctx->_device, &pipeline_layout_create_info, NULL, &p->gui_textured_pannel_pipeline_layout) != VK_SUCCESS) {
  ZAC_System_Panic("failed creating pipeline layout");
 }
 
 VkGraphicsPipelineCreateInfo graphics_pipeline_create_info;
 memset(&graphics_pipeline_create_info, 0, sizeof(VkGraphicsPipelineCreateInfo));
 graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

 graphics_pipeline_create_info.stageCount = 2;
 graphics_pipeline_create_info.pStages = shader_stage_create_info;

 graphics_pipeline_create_info.pVertexInputState = &vertex_input_state;
 graphics_pipeline_create_info.pInputAssemblyState = &input_assembly_state;
 graphics_pipeline_create_info.pViewportState = &viewport_state;
 graphics_pipeline_create_info.pRasterizationState = &rasterization_state;
 graphics_pipeline_create_info.pMultisampleState = &multisample_state;
 graphics_pipeline_create_info.pDepthStencilState = &depth_stencil_state;
 graphics_pipeline_create_info.pColorBlendState = &color_blend_state;
 graphics_pipeline_create_info.pDynamicState = &dynamic_state_create_info;
 
 graphics_pipeline_create_info.layout = p->gui_textured_pannel_pipeline_layout;
 graphics_pipeline_create_info.renderPass = ctx->_render_pass;
 graphics_pipeline_create_info.subpass = 0;
 graphics_pipeline_create_info.basePipelineHandle = NULL;
 graphics_pipeline_create_info.basePipelineIndex = -1;

 if(vkCreateGraphicsPipelines(ctx->_device, NULL, 1, &graphics_pipeline_create_info, NULL, &p->gui_textured_pannel_pipeline) != VK_SUCCESS) {
 	ZAC_System_Panic(" pipeline derrive creation failed.");
 } 

 vkDestroyShaderModule(ctx->_device, vs, NULL);
 vkDestroyShaderModule(ctx->_device, fs, NULL);
}

