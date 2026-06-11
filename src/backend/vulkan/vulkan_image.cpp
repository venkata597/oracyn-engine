#include "../../../include/backend/vulkan/vulkan_image.hpp"
#include "../../../include/backend/vulkan/vulkan_allocator.hpp"
#include <iostream>
#include <vulkan/vulkan_core.h>

void VulkanImage::createImage(const VkDevice& ldevice,uint32_t width,uint32_t height,VkFormat format,VkImageTiling tiling,VkImageUsageFlags usage,VkMemoryPropertyFlags mmflags){
    VkImageCreateInfo img_c_info{};
    img_c_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    img_c_info.imageType = VK_IMAGE_TYPE_2D;
    img_c_info.extent.width = width;
    img_c_info.extent.height = height;
    img_c_info.extent.depth = 1;
    img_c_info.mipLevels = 1;
    img_c_info.arrayLayers = 1;
    img_c_info.format = format;
    img_c_info.tiling = tiling;
    img_c_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    img_c_info.usage = usage;
    img_c_info.samples = VK_SAMPLE_COUNT_1_BIT;
    img_c_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo al_c_info{};
    al_c_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

    vmaCreateImage(
        VulkanAllocator.allocator,
        &img_c_info,
        &al_c_info,
        &image,
        &allocation,
        &allocationInfo
    );
}

void VulkanImage::createImageView(const VkDevice& ldevice,VkImageAspectFlags flags){
    VkImageViewCreateInfo img_vc_info{};
    img_vc_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    img_vc_info.image = image;
    img_vc_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    img_vc_info.format = VK_FORMAT_R8G8B8A8_SRGB;

    img_vc_info.subresourceRange.aspectMask = flags;
    img_vc_info.subresourceRange.baseMipLevel = 0;
    img_vc_info.subresourceRange.levelCount = 1;
    img_vc_info.subresourceRange.baseArrayLayer = 0;
    img_vc_info.subresourceRange.layerCount = 1;

    if(vkCreateImageView(ldevice,&img_vc_info,nullptr,&imageView)!=VK_SUCCESS){
        std::cout << "[ORACYN (Backend)]: Failed to create Image View" << '\n';
        return;
    }
}