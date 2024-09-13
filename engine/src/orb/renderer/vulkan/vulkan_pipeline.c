#include "vulkan_pipeline.h"
#include "../../core/asserts.h"
#include "../../math/math_types.h"

b8 orb_vulkan_graphics_pipeline_create(orb_vulkan_context *context,
                                       orb_vulkan_renderpass *renderpass, u32 attribute_count,
                                       VkVertexInputAttributeDescription *attributes,
                                       u32 descriptor_set_layout_count,
                                       VkDescriptorSetLayout *descriptor_set_layouts,
                                       u32 stage_count, VkPipelineShaderStageCreateInfo *stages,
                                       VkViewport viewport, VkRect2D scissor, b8 is_wireframe,
                                       orb_vulkan_pipeline *out_pipeline) {
    VkPipelineViewportStateCreateInfo viewport_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor,
    };

    VkPipelineRasterizationStateCreateInfo rasterizer_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = is_wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL,
        .lineWidth = 1.0f,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,

        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,

        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
    };

    VkPipelineMultisampleStateCreateInfo multisampling = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .minSampleShading = 1.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    VkPipelineDepthStencilStateCreateInfo depth_stencil = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
    };

    VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineColorBlendStateCreateInfo color_blend_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &color_blend_attachment_state,
    };

#define ORB_PIPELINE_DYNAMIC_STATE_COUNT 3

    VkDynamicState dynamic_states[ORB_PIPELINE_DYNAMIC_STATE_COUNT] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH,
    };

    VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = ORB_PIPELINE_DYNAMIC_STATE_COUNT,
        .pDynamicStates = dynamic_states,
    };

    VkVertexInputBindingDescription vertex_binding_description = {
        .binding = 0, // index
        .stride = sizeof(orb_vertex_3d),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &vertex_binding_description,
        .vertexAttributeDescriptionCount = attribute_count,
        .pVertexAttributeDescriptions = attributes,
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = descriptor_set_layout_count,
        .pSetLayouts = descriptor_set_layouts,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges =
            &(VkPushConstantRange){
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset = sizeof(orb_mat4) * 0,
                .size = sizeof(orb_mat4) * 2,
            },
    };

    ORB_VK_EXPECT(vkCreatePipelineLayout(context->device.logical_device,
                                         &pipeline_layout_create_info, context->allocator,
                                         &out_pipeline->layout));

    VkGraphicsPipelineCreateInfo pipeline_create_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = stage_count,
        .pStages = stages,
        .pVertexInputState = &vertex_input_info,
        .pInputAssemblyState = &input_assembly,
        .pViewportState = &viewport_state,
        .pRasterizationState = &rasterizer_state,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = &depth_stencil,
        .pColorBlendState = &color_blend_state,
        .pDynamicState = &dynamic_state_create_info,
        .pTessellationState = nullptr,

        .layout = out_pipeline->layout,
        .renderPass = renderpass->handle,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    ORB_VK_EXPECT(vkCreateGraphicsPipelines(context->device.logical_device, VK_NULL_HANDLE, 1,
                                            &pipeline_create_info, context->allocator,
                                            &out_pipeline->handle));

    return true;
}

void orb_vulkan_graphics_pipeline_destroy(orb_vulkan_context *context,
                                          orb_vulkan_pipeline *pipeline) {
    if (pipeline->handle) {
        vkDestroyPipeline(context->device.logical_device, pipeline->handle, context->allocator);
        pipeline->handle = nullptr;
    }

    if (pipeline->layout) {
        vkDestroyPipelineLayout(context->device.logical_device, pipeline->layout,
                                context->allocator);
        pipeline->layout = nullptr;
    }

    *pipeline = (orb_vulkan_pipeline){0};
}

void orb_vulkan_graphics_pipeline_bind(orb_vulkan_command_buffer *command_buffer,
                                       VkPipelineBindPoint bind_point,
                                       orb_vulkan_pipeline *pipeline) {
    vkCmdBindPipeline(command_buffer->handle, bind_point, pipeline->handle);
}
