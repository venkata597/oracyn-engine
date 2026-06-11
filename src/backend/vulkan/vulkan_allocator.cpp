#include "../../../include/backend/vulkan/vulkan_allocator.hpp"

void createAllocator(const VkPhysicalDevice &pdevice, const VkDevice &ldevice, const VkInstance &instance){
    VmaAllocatorCreateInfo allocator_info{};
    allocator_info.physicalDevice = pdevice;
    allocator_info.device = ldevice;
    allocator_info.instance = instance;

    vmaCreateAllocator(&allocator_info,&VulkanAllocator.allocator);

}