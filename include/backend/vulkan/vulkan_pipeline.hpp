#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "vulkan_image.hpp"

class VulkanPipeline{
private:
    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

    VkViewport viewPort{};
    VkRect2D scissor{};

    VkPipeline graphicsPipeline = VK_NULL_HANDLE;

    VulkanImage depthImage;
private:
    // Render Pass Creation
    void _create_render_pass(const VkDevice& ldevice,const VkFormat& swapChainImageFormat);

    // Graphics Pipeline Creation Helpers
    VkShaderModule _create_shader_module(const VkDevice& ldevice,const std::vector<char>& code);

    // Graphics Pipeline Creation
    void _create_graphics_pipeline(const VkDevice& ldevice,const VkExtent2D& swapChainExtent);

    void _create_depth_resources();
public:
    void initRenderPass(const VkDevice& ldevice,const VkFormat& swapChainImageFormat);
    void initPipeline(const VkDevice& ldevice,const VkExtent2D& swapChainExtent);
    void destroyPipeline(const VkDevice& ldevice);


    const VkRenderPass& getrenderPass();
    const VkPipelineLayout& getpipelineLayout();

    const VkViewport& getviewPort();
    const VkRect2D& getscissor();
    const VkPipeline& getgraphicsPipeline();
};