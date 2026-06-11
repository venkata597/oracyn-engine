#include "../../../include/backend/vulkan/vulkan_sync.hpp"
#include <iostream>


void VulkanSynchronizer::_create_sync_objects(const VkDevice& ldevice){
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(swapChain.getswapChainImages().size());
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);    
    uint32_t image_c;
    vkGetSwapchainImagesKHR(ldevice,swapChain.getswapChain(),&image_c,nullptr);

    imagesInFlight.resize(image_c,VK_NULL_HANDLE);
    
    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(size_t i = 0;i<MAX_FRAMES_IN_FLIGHT;i++){
        if(vkCreateSemaphore(ldevice,&semaphore_info,nullptr,&imageAvailableSemaphores[i]) ||
            vkCreateFence(ldevice,&fence_info,nullptr,&inFlightFences[i])){
                std::cout << "[ORACYN (BACKEND)]: Failed to create Semaphores or Fence" << '\n';
                return;
        }
    }

    for(size_t i = 0;i<swapChain.getswapChainImages().size();i++){
        if(vkCreateSemaphore(ldevice,&semaphore_info,nullptr,&renderFinishedSemaphores[i]) != VK_SUCCESS){
            std::cout << "[ORACYN (BACKEND)]: Failed to create Semaphores or Fence" << '\n';
            return;
        }
    }
}
