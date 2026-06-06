#pragma once

#include <array>
#include <cstddef>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

struct gpuVertex{
    glm::vec2 pos;
    glm::vec3 colors;

    static VkVertexInputBindingDescription getBindingDesc(){
        VkVertexInputBindingDescription bnd_dsc{};
        bnd_dsc.binding = 0;
        bnd_dsc.stride = sizeof(gpuVertex);
        bnd_dsc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bnd_dsc;
    }

    static std::array<VkVertexInputAttributeDescription,2> getAttributeDesc(){
        std::array<VkVertexInputAttributeDescription,2> attr_desc{};
        attr_desc[0].binding = 0;
        attr_desc[0].location = 0;
        attr_desc[0].format = VK_FORMAT_R32G32_SFLOAT;
        attr_desc[0].offset = offsetof(gpuVertex, pos);

        attr_desc[1].binding = 0;
        attr_desc[1].location = 1;
        attr_desc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attr_desc[1].offset = offsetof(gpuVertex, colors);
        return attr_desc;
    }
};