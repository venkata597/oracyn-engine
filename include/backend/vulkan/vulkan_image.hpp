#pragma once

#include "../vma/vk_mem_alloc.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>

class VulkanImage{
private:
    VkImage image;
    VmaAllocation allocation;
    VmaAllocationInfo allocationInfo;
    VkImageView imageView;
public:
    void createImage(const VkDevice& ldevice,uint32_t width,uint32_t height,VkFormat format,VkImageTiling tiling,VkImageUsageFlags usage,VkMemoryPropertyFlags mmflags);
    void createImageView(const VkDevice& ldevice,VkImageAspectFlags flags);

};