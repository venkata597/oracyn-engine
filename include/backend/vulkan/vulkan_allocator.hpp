#pragma once
#include "../vma/vk_mem_alloc.h"


static struct{
    VmaAllocator allocator;
}VulkanAllocator;

static void createAllocator(const VkPhysicalDevice& pdevice,const VkDevice& ldevice,const VkInstance& instance);
