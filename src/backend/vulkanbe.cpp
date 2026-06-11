#include "../../include/backend/vulkan/vulkanbe.hpp"
#include "../../include/backend/be_resources/be_shader.hpp"
#include "../../include/backend/be_resources/be_vertex.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <sys/types.h>
#include <vulkan/vulkan_core.h>

bool VulkanBackend::_check_device_extension_support(VkPhysicalDevice device){
    uint32_t ec;
    vkEnumerateDeviceExtensionProperties(device,nullptr,&ec,nullptr);

    std::vector<VkExtensionProperties> available_extensions(ec);
    vkEnumerateDeviceExtensionProperties(device,nullptr,&ec,available_extensions.data());

    std::set<std::string> required_extension(device_extensions.begin(),device_extensions.end());

    for(const auto& extension: available_extensions){
        required_extension.erase(extension.extensionName);
    }
    return required_extension.empty();
}




void VulkanBackend::_cleanup_swap_chain(){
    
    
}

void VulkanBackend::_recreate_swap_chain(){
    
}

uint32_t VulkanBackend::_find_memory_type(uint32_t type_filter,VkMemoryPropertyFlags flags){
    VkPhysicalDeviceMemoryProperties mem_props;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &mem_props);
    for(uint32_t i = 0;i<mem_props.memoryTypeCount;i++){
        if(type_filter & (1<<i) && (mem_props.memoryTypes[i].propertyFlags & flags) == flags){
            return i;
        }
    }
    std::cout << "[ORACYN (BACKEND)]: Failed to find suitable memory type" << '\n';
    return -1;
}

void VulkanBackend::_create_vertex_buffer(){
    VkDeviceSize size = sizeof(vertices[0]) * vertices.size();

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory = VK_NULL_HANDLE;
    _create_buffer(size,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        ,staging_buffer,staging_buffer_memory
    );

    if(staging_buffer_memory == VK_NULL_HANDLE){
        std::cout << "Failed to create staging buffer" << '\n';
        return;
    }

    void* data;
    vkMapMemory(ldevice,staging_buffer_memory,0,size,0,&data);
        memcpy(data,vertices.data(),(size_t)size);
    vkUnmapMemory(ldevice,staging_buffer_memory);


    _create_buffer(size,VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        this->vertexBuffer,this->vertexBufferMemory
    );

    _copy_to_buffer(staging_buffer,vertexBuffer,size);
    

    vkDestroyBuffer(ldevice,staging_buffer,nullptr);
    vkFreeMemory(ldevice,staging_buffer_memory,nullptr);
}

void VulkanBackend::_copy_to_buffer(VkBuffer src_buffer,VkBuffer dest_buffer,VkDeviceSize size){
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = commandPool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(ldevice,&alloc_info,&command_buffer);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(command_buffer,&begin_info);
        VkBufferCopy copy_region{};
        copy_region.srcOffset = 0;
        copy_region.dstOffset = 0;
        copy_region.size = size;
        vkCmdCopyBuffer(command_buffer,src_buffer,dest_buffer,1,&copy_region);
    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo s_info{};
    s_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    s_info.commandBufferCount = 1;
    s_info.pCommandBuffers = &command_buffer;

    vkQueueSubmit(graphicsQueue,1,&s_info,VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(ldevice,commandPool,1,&command_buffer);
}

void VulkanBackend::initBackend(std::vector<const char*> extensions,AppWindow& window){
    this->window = &window;
    
    surface = window.createVulkanSurface(this->instance);
    
   
    _create_swapchain(window);
    _create_image_views();
    _create_render_pass();
    _create_graphics_pipeline();
    _create_frame_buffers();
    _create_command_pool();
    _create_vertex_buffer();
    _create_command_buffers();
    _create_sync_objects();
}

void VulkanBackend::drawFrame(){
    vkWaitForFences(ldevice,1,&inFlightFences[currentFrame],VK_TRUE,UINT64_MAX);
    
    if(windowResized){
        windowResized = false;
        _recreate_swap_chain();
        return;
    }

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(ldevice,swapChain,UINT64_MAX,imageAvailableSemaphores[currentFrame],VK_NULL_HANDLE,&imageIndex);

    if(result==VK_ERROR_OUT_OF_DATE_KHR || result==VK_SUBOPTIMAL_KHR || windowResized){
        windowResized = false;
        _recreate_swap_chain();
        return;
    }else if(result!= VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to acquire swapchain image" << '\n';
        return;
    }

    if(imagesInFlight[imageIndex]!=VK_NULL_HANDLE){
        vkWaitForFences(ldevice,1,&imagesInFlight[imageIndex],VK_TRUE,UINT64_MAX);
    }

    imagesInFlight[imageIndex] =  inFlightFences[currentFrame];

    vkResetFences(ldevice,1,&inFlightFences[currentFrame]);

    vkResetCommandBuffer(commandBuffers[imageIndex],0);
    _record_command_buffer(commandBuffers[imageIndex],imageIndex);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = {
        imageAvailableSemaphores[currentFrame]
    };

    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &commandBuffers[imageIndex];

    VkSemaphore signal_semaphores[] = {
        renderFinishedSemaphores[imageIndex]
    };
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    if(vkQueueSubmit(graphicsQueue,1,&submit_info,inFlightFences[currentFrame])!=VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to submit to the graphics queue" << '\n';
        return;
    }   

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;
    
    VkSwapchainKHR swap_chains[] = {swapChain};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swap_chains;
    present_info.pImageIndices = &imageIndex;
    present_info.pResults = nullptr;

    result = vkQueuePresentKHR(presentQueue,&present_info);
    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || windowResized){
        windowResized = false;
        _recreate_swap_chain();
        
    }else if(result != VK_SUCCESS){
        std::cout << "[ORACYN (BACKEND)]: Failed to present swap chain image" << '\n';
        return;
    }

    currentFrame = (currentFrame+1)%MAX_FRAMES_IN_FLIGHT;
}

void VulkanBackend::deviceWait(){
    vkDeviceWaitIdle(ldevice);
}

void VulkanBackend::resize(){
    windowResized = true;
}

VulkanBackend::~VulkanBackend(){

    _cleanup_swap_chain();

    if(vertexBuffer!=VK_NULL_HANDLE){
        vkDestroyBuffer(ldevice,vertexBuffer,nullptr);
    }
    if(vertexBufferMemory!=VK_NULL_HANDLE){
        vkFreeMemory(ldevice,vertexBufferMemory,nullptr);
    }

    for(size_t i = 0;i<swapChainImages.size();i++){
        if(renderFinishedSemaphores[i]!=VK_NULL_HANDLE){
            vkDestroySemaphore(ldevice,renderFinishedSemaphores[i],nullptr);
        }
    }
    for(size_t i = 0;i<MAX_FRAMES_IN_FLIGHT;i++){
        if(inFlightFences[i]!=VK_NULL_HANDLE){
            vkDestroyFence(ldevice,inFlightFences[i],nullptr);
        }
        if(imageAvailableSemaphores[i]!=VK_NULL_HANDLE){
            vkDestroySemaphore(ldevice,imageAvailableSemaphores[i],nullptr);
        }
    }
    if(commandPool!=VK_NULL_HANDLE){
        vkDestroyCommandPool(ldevice,commandPool,nullptr);
    }
    
    if(ldevice!=VK_NULL_HANDLE){
        vkDestroyDevice(ldevice,nullptr);
    }
    if(surface!=VK_NULL_HANDLE){
        vkDestroySurfaceKHR(instance,surface,nullptr);
    }
    if(enableValidationLayers){
        _destroy_debug_messenger(instance,debugMessenger,nullptr);
    }
    if(instance!=VK_NULL_HANDLE){
        vkDestroyInstance(instance,nullptr);
    }
}