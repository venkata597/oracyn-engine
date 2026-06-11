#include "../../../include/backend/vulkan/vulkan_pipeline.hpp"
#include "../../../include/backend/be_resources/be_shader.hpp"
#include "../../../include/backend/be_resources/be_vertex.hpp"
#include <vulkan/vulkan_core.h>
#include <iostream>

VkShaderModule VulkanPipeline::_create_shader_module(const VkDevice& ldevice,const std::vector<char>& code){
    VkShaderModuleCreateInfo s_create_info{};
    s_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    s_create_info.codeSize = code.size();
    s_create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shad_mod;
    if(vkCreateShaderModule(ldevice,&s_create_info,nullptr,&shad_mod)!=VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to create Shader Module" << '\n';
    }
    return shad_mod;
}


void VulkanPipeline::_create_render_pass(const VkDevice& ldevice,const VkFormat& swapChainImageFormat){
    VkAttachmentDescription ca{};
    ca.format = swapChainImageFormat;
    ca.samples = VK_SAMPLE_COUNT_1_BIT;
    ca.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    ca.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    ca.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    ca.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    ca.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ca.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference ca_attach_ref{};
    ca_attach_ref.attachment = 0;
    ca_attach_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription sub_pass{};
    sub_pass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    sub_pass.colorAttachmentCount = 1;
    sub_pass.pColorAttachments = &ca_attach_ref;

    VkSubpassDependency subpass_depend{};
    subpass_depend.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpass_depend.dstSubpass = 0;
    subpass_depend.srcStageMask =  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_depend.srcAccessMask = 0;
    subpass_depend.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_depend.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo rp_create_info{};
    rp_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rp_create_info.attachmentCount = 1;
    rp_create_info.pAttachments = &ca;
    rp_create_info.subpassCount = 1;
    rp_create_info.pSubpasses = &sub_pass;
    rp_create_info.dependencyCount = 1;
    rp_create_info.pDependencies = &subpass_depend;

    if(vkCreateRenderPass(ldevice,&rp_create_info,nullptr,&renderPass)!=VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to create Render Pass" << '\n';
        return;
    }
}

void VulkanPipeline::_create_graphics_pipeline(const VkDevice& ldevice,const VkExtent2D& swapChainExtent){
    Shader vertex_code;
    Shader fragment_code;
    vertex_code.readFromFile("shaders/vert.spv");
    fragment_code.readFromFile("shaders/frag.spv");

    std::vector<char> vert_raw_code = vertex_code.getShaderCode();
    std::vector<char> frag_raw_code = fragment_code.getShaderCode();
    VkShaderModule vertex_shader_mod = _create_shader_module(ldevice,vert_raw_code);
    VkShaderModule frag_shader_mod = _create_shader_module(ldevice,frag_raw_code);

    VkPipelineShaderStageCreateInfo vert_stage_create_info{};
    vert_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_stage_create_info.module = vertex_shader_mod;
    vert_stage_create_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_stage_create_info{};
    frag_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_stage_create_info.module = frag_shader_mod;
    frag_stage_create_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_stages[] = {
        vert_stage_create_info,
        frag_stage_create_info
    };

    std::vector<VkDynamicState> dyn_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dyn_create_info{};
    dyn_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dyn_create_info.dynamicStateCount = static_cast<uint32_t>(dyn_states.size());
    dyn_create_info.pDynamicStates = dyn_states.data();

    VkPipelineVertexInputStateCreateInfo vinp_create_info{};
    auto bnd_dsc = gpuVertex::getBindingDesc();
    auto attr_dsc = gpuVertex::getAttributeDesc();
    vinp_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vinp_create_info.vertexBindingDescriptionCount = 1;
    vinp_create_info.pVertexBindingDescriptions = &bnd_dsc;
    vinp_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attr_dsc.size());
    vinp_create_info.pVertexAttributeDescriptions = attr_dsc.data();
    
    VkPipelineInputAssemblyStateCreateInfo ias_create_info{};
    ias_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ias_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    ias_create_info.primitiveRestartEnable = VK_FALSE;

    
    viewPort.x = 0.0f;
    viewPort.y = 0.0f;
    viewPort.width = (float)swapChainExtent.width;
    viewPort.height = (float)swapChainExtent.height;
    viewPort.minDepth = 0.0f;
    viewPort.maxDepth = 1.0f;

    scissor.offset = {0,0};
    scissor.extent = swapChainExtent;

    VkPipelineViewportStateCreateInfo vp_create_info{};
    vp_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vp_create_info.viewportCount = 1;
    vp_create_info.pViewports = &viewPort;
    vp_create_info.scissorCount = 1;
    vp_create_info.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rs_create_info{};
    rs_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs_create_info.depthClampEnable = VK_FALSE;
    rs_create_info.rasterizerDiscardEnable = VK_FALSE;
    rs_create_info.polygonMode = VK_POLYGON_MODE_FILL;
    rs_create_info.lineWidth = 1.0f;
    rs_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
    rs_create_info.frontFace =VK_FRONT_FACE_CLOCKWISE;
    rs_create_info.depthBiasEnable = VK_FALSE;
    rs_create_info.depthBiasConstantFactor = 0.0f;
    rs_create_info.depthBiasClamp = 0.0f;
    rs_create_info.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo ms_create_info{};
    ms_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ms_create_info.sampleShadingEnable = VK_FALSE;
    ms_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    ms_create_info.minSampleShading = 1.0f;
    ms_create_info.pSampleMask = nullptr;
    ms_create_info.alphaToCoverageEnable = VK_FALSE;
    ms_create_info.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState cba_create_info{};
    cba_create_info.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    cba_create_info.blendEnable = VK_FALSE;
    cba_create_info.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    cba_create_info.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    cba_create_info.colorBlendOp = VK_BLEND_OP_ADD;
    cba_create_info.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    cba_create_info.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    cba_create_info.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo cbs_create_info{};
    cbs_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    cbs_create_info.logicOpEnable = VK_FALSE;
    cbs_create_info.logicOp = VK_LOGIC_OP_COPY;
    cbs_create_info.attachmentCount = 1;
    cbs_create_info.pAttachments = &cba_create_info;

    VkPipelineLayoutCreateInfo pp_create_info{};
    pp_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pp_create_info.setLayoutCount = 0;
    pp_create_info.pSetLayouts = nullptr;
    pp_create_info.pushConstantRangeCount = 0;
    pp_create_info.pPushConstantRanges = nullptr;

    if(vkCreatePipelineLayout(ldevice,&pp_create_info,nullptr,&pipelineLayout)!=VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to create graphics pipeline layout" << '\n';
    }
    
    VkGraphicsPipelineCreateInfo gp_create_info{};
    gp_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    gp_create_info.stageCount = 2;
    gp_create_info.pStages = shader_stages;
    gp_create_info.pVertexInputState = &vinp_create_info;
    gp_create_info.pInputAssemblyState = &ias_create_info;
    gp_create_info.pViewportState = &vp_create_info;
    gp_create_info.pRasterizationState = &rs_create_info;
    gp_create_info.pMultisampleState = &ms_create_info;
    gp_create_info.pDepthStencilState = nullptr;
    gp_create_info.pColorBlendState = &cbs_create_info;
    gp_create_info.pDynamicState = &dyn_create_info;
    gp_create_info.layout = pipelineLayout;
    gp_create_info.renderPass = renderPass;
    gp_create_info.subpass = 0;
    gp_create_info.basePipelineHandle = VK_NULL_HANDLE;
    gp_create_info.basePipelineIndex = -1;

    if(vkCreateGraphicsPipelines(ldevice,VK_NULL_HANDLE,1,&gp_create_info, nullptr,&graphicsPipeline)!=VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to create Graphics Pipeline" << '\n';
        return;
    }

    vkDestroyShaderModule(ldevice,vertex_shader_mod,nullptr);
    vkDestroyShaderModule(ldevice,frag_shader_mod,nullptr);
}

void VulkanPipeline::initRenderPass(const VkDevice& ldevice,const VkFormat& swapChainImageFormat){
    _create_render_pass(ldevice,swapChainImageFormat);
}

void VulkanPipeline::initPipeline(const VkDevice& ldevice,const VkExtent2D& swapChainExtent){
    _create_graphics_pipeline(ldevice,swapChainExtent);
}

void VulkanPipeline::destroyPipeline(const VkDevice& ldevice){
    vkDestroyPipeline(ldevice,graphicsPipeline,nullptr);
    vkDestroyPipelineLayout(ldevice,pipelineLayout,nullptr);
    vkDestroyRenderPass(ldevice,renderPass, nullptr);
}


const VkRenderPass& VulkanPipeline::getrenderPass(){
    return this->renderPass;
}
    
const VkPipelineLayout& VulkanPipeline::getpipelineLayout(){
    return this->pipelineLayout;
}
    
const VkViewport& VulkanPipeline::getviewPort(){
    return this->getviewPort();
}
    
const VkRect2D& VulkanPipeline::getscissor(){
    return this->scissor;
}
    
const VkPipeline& VulkanPipeline::getgraphicsPipeline(){
    return this->graphicsPipeline;
}