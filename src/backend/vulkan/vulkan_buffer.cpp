#include "../../../include/backend/vulkan/vulkan_buffer.hpp"
#include "../../../include/backend/vulkan/vulkan_allocator.hpp"
#include <cstring>
#include <iostream>
#include <vulkan/vulkan_core.h>

void VulkanBuffer::_create_buffer(VkDeviceSize size,VkBufferUsageFlags flags){
    VkBufferCreateInfo bfc_info{};
    bfc_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bfc_info.size = size;
    bfc_info.flags = flags;
    
    VmaAllocationCreateInfo alc_info{};
    if(flags != VK_BUFFER_USAGE_TRANSFER_SRC_BIT){
        alc_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    }else{
        alc_info.usage = VMA_MEMORY_USAGE_AUTO;
        alc_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
    }

    vmaCreateBuffer(
        VulkanAllocator.allocator,
        &bfc_info,
        &alc_info,
        &buffer,
        &allocation,
        &allocationInfo
    );
}

void VulkanBuffer::createBuffer(VkDeviceSize size,BufferType bftype){
    switch(bftype){
        case VERTEX:
            _create_buffer(size,VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
            break;
        case INDEX:
            _create_buffer(size,VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
            break;
        case STAGING:
            _create_buffer(size,VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
            break;
        default:
            std::cout << "[ORACYN (Backend)]: Failed to create a buffer" << '\n';
    }
}

void VulkanBuffer::assignData(const void* __restrict data,size_t size){
    std::memcpy(allocationInfo.pMappedData,data,size);
}   

const VkBuffer& VulkanBuffer::getBuffer(){
    return this->buffer;
}