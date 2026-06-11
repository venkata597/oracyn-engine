#include "../../../include/backend/vulkan/vulkan_command.hpp"
#include <cstdint>
#include <iostream>
#include <vulkan/vulkan_core.h>

void VulkanCommandStream::_create_command_pool(const VkDevice& ldevice,const std::pair<uint32_t,uint32_t> qf){
    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = qf.first;

    if(vkCreateCommandPool(ldevice,&pool_info,nullptr,&commandPool)!=VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to create Command Pool" << '\n';
        return;
    }
    
    // Hardcoding buffer creation to test all classes functioning
    VkDeviceSize size = sizeof(vertices[0]) * vertices.size();
    vertexBuffer.createBuffer(size, VERTEX);
    stagingBuffer.createBuffer(size,STAGING);
    stagingBuffer.assignData(vertices.data(),size);
}



void VulkanCommandStream::_create_command_buffers(const VkDevice& ldevice){
    uint32_t image_c;
    vkGetSwapchainImagesKHR(ldevice,swapChain.getswapChain(),&image_c,nullptr);
    commandBuffers.resize(image_c);

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = commandPool;
    alloc_info.level  = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
    
    if(vkAllocateCommandBuffers(ldevice,&alloc_info,commandBuffers.data())!=VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to allocate command buffers" << '\n';
        return;
    }
}

void VulkanCommandStream::_record_command_buffer(VkCommandBuffer commandBuffer,uint32_t imageIndex){
    VkCommandBufferBeginInfo beg_info{};
    beg_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beg_info.flags = 0;
    beg_info.pInheritanceInfo = nullptr;

    if(vkBeginCommandBuffer(commandBuffer,&beg_info)!=VK_SUCCESS){
        std::cout << "[ORACYN (Backend)]: Failed to begin recording command buffer" << '\n';
        return;
    }

    VkRenderPassBeginInfo rp_beg_info{};
    rp_beg_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_beg_info.renderPass = pipeline.getrenderPass();
    rp_beg_info.framebuffer = swapChain.getswapChainFrameBuffers()[imageIndex];
    rp_beg_info.renderArea.offset = {0,0};
    rp_beg_info.renderArea.extent = swapChain.getswapChainExtent();

    VkClearValue clearcolor = {{{0.0f,0.0f,0.0f,1.0f}}};
    rp_beg_info.clearValueCount = 1;
    rp_beg_info.pClearValues = &clearcolor;

    vkCmdBeginRenderPass(commandBuffer,&rp_beg_info,VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,pipeline.getgraphicsPipeline());
        vkCmdSetViewport(commandBuffer,0,1,&pipeline.getviewPort());
        vkCmdSetScissor(commandBuffer,0,1,&pipeline.getscissor());
        
        VkBuffer vertexBuffers[] = {vertexBuffer.getBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer,0,1,vertexBuffers,offsets);
        vkCmdDraw(commandBuffer,static_cast<uint32_t>(vertices.size()),1,0,0);

    vkCmdEndRenderPass(commandBuffer);

    if(vkEndCommandBuffer(commandBuffer)!=VK_SUCCESS){
        std::cout << "[ORACYN (Backend)]: Failed to record command buffer" << '\n';
    }
    
}

void VulkanCommandStream::initCommandStream(const VkDevice& ldevice,const VulkanSwapChain& swapChain,const VulkanPipeline& pipeline,const std::pair<uint32_t,uint32_t> qf,const VkQueue& graphicsQueue){
    _create_command_pool(ldevice,qf);
    this->swapChain = swapChain;
    this->pipeline = pipeline;
    this->ldevice = ldevice;
    this->gqueue = graphicsQueue;
}

void VulkanCommandStream::recordDrawCommandBuffer(int buffer_index,uint32_t imageIndex){
    _record_command_buffer(commandBuffers.at(buffer_index),imageIndex);
}

VkCommandBuffer VulkanCommandStream::getCommandBuffer(int index){
    return commandBuffers.at(index);
}

VkCommandBuffer VulkanCommandStream::beginSingleTimeCommands(){
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = commandPool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer single_time_commandBuffer;
    if(vkAllocateCommandBuffers(ldevice,&alloc_info,&single_time_commandBuffer)!=VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to allocate single time command buffer" << '\n';
    }

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if(vkBeginCommandBuffer(single_time_commandBuffer,&begin_info)!=VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to begin single time command buffer" << '\n';
    }
    return single_time_commandBuffer;
}

void VulkanCommandStream::endSingleTimeCommands(VkCommandBuffer cmdBuffer){
    vkEndCommandBuffer(cmdBuffer);
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmdBuffer;

    vkQueueSubmit(gqueue,1,&submit_info,VK_NULL_HANDLE);  
    vkQueueWaitIdle(gqueue);
    
    vkFreeCommandBuffers(ldevice,commandPool,1,&cmdBuffer);
}