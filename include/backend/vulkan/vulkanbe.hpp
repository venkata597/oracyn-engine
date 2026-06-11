#pragma once

#include <cstdint>
#include <sys/types.h>
#include <vector>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <iostream>
#include <optional>
#include "../../windowing/window.hpp"
#include "../be_resources/be_vertex.hpp"




class VulkanBackend{
private:
    

    
    

private:

    bool windowResized = false;

private:
    
    

    
    // Swapchain Recreation
    void _cleanup_swap_chain();
    void _recreate_swap_chain();
    // Vertex Buffer Creation
    uint32_t _find_memory_type(uint32_t type_filter,VkMemoryPropertyFlags flags);
    void _create_buffer(VkDeviceSize size,VkBufferUsageFlags flags,VkMemoryPropertyFlags mem_flags,VkBuffer& buffer,VkDeviceMemory& device_mem);
    void _create_vertex_buffer();
    void _copy_to_buffer(VkBuffer src_buffer,VkBuffer dest_buffer,VkDeviceSize size);
public:
    void initBackend(std::vector<const char*> extensions,AppWindow& window);
    void resize();
    void drawFrame();
    void deviceWait();
    ~VulkanBackend();
};