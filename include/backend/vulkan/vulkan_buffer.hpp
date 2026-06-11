#pragma once
#include <vulkan/vulkan.h>
#include "../vma/vk_mem_alloc.h"

enum BufferType{
    VERTEX,
    INDEX,
    STAGING
};

class VulkanBuffer{
private:
    static VmaAllocator allocator;
private:
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo allocationInfo{};
private:
    void _create_buffer(VkDeviceSize size,VkBufferUsageFlags flags);
public:
    void createBuffer(VkDeviceSize size,BufferType bftype);
    static void createAllocator(const VkPhysicalDevice& pdevice,const VkDevice& ldevice,const VkInstance& instance);
    void assignData(const void* __restrict data,size_t size);

    const VkBuffer& getBuffer();

};