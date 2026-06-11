#pragma once
#include "vulkan_swapchain.hpp"
#include <vector>
#include <vulkan/vulkan.h>


const int MAX_FRAMES_IN_FLIGHT = 2; 

class VulkanSynchronizer{
private:
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;

    uint32_t currentFrame = 0;

    VulkanSwapChain swapChain; 
private:
    // Create Synchronization Objects
    void _create_sync_objects(const VkDevice& ldevice);

public:
    void initSynchronizer(const VkDevice& ldevice,const VulkanSwapChain& swapChain);
};