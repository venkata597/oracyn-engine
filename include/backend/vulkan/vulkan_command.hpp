#pragma once
#include "vulkan_pipeline.hpp"
#include "vulkan_swapchain.hpp"
#include "vulkan_buffer.hpp"
#include "../be_resources/be_vertex.hpp"
#include <cstdint>
#include <vulkan/vulkan.h>
#include <vector>

class VulkanCommandStream{
private:

    const std::vector<gpuVertex> vertices = {
        {{0.0f,-0.5f},{1.0f,1.0f,1.0f}},
        {{0.5f,0.5f},{0.0f,1.0f,0.0f}},
        {{-0.5f,0.5f},{0.0f,0.0f,1.0f}}
    };

    VkCommandPool commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers;

    VkDevice ldevice;
    VulkanSwapChain swapChain;
    VulkanPipeline pipeline;
    VkQueue gqueue;

    VulkanBuffer vertexBuffer;
    VulkanBuffer stagingBuffer;
private:
    // Command Pool Creation
    void _create_command_pool(const VkDevice& ldevice,const std::pair<uint32_t,uint32_t> qf);

    // Command Buffer Creation
    void _create_command_buffers(const VkDevice& ldevice);

    // Command Buffer Recording
    void _record_command_buffer(VkCommandBuffer commandBuffer,uint32_t imageIndex);
public:
    void initCommandStream(const VkDevice& ldevice,const VulkanSwapChain& swapChain,const VulkanPipeline& pipeline,const std::pair<uint32_t,uint32_t> qf,const VkQueue& graphicsQueue);
    void initBuffers(const VkInstance& instance);
    VkCommandBuffer getCommandBuffer(int index);
    void recordDrawCommandBuffer(int buffer_index,uint32_t imageIndex);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer cmdBuffer);
    void ResetCommandBuffer();
};