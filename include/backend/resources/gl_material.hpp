#pragma once

#include "gl_texture.hpp"
#include "../../resources/material.hpp"
#include "gl_ubo.hpp"
#include <glm/glm.hpp>

namespace Backend{

    struct MaterialUBOData{
        glm::vec4 uBaseColor;
        float uMetallicFactor;
        float uRoughnessFactor;
        float uOcclusionStrength;
        float uEmissiveStrength;
        glm::vec3 uEmissiveFactor;
    };

    class MaterialUBO: public UBO{
    public:
        MaterialUBOData data;
    public:

        void bindUniformBufferData() override;
        void updateUniformBufferData() override;

        MaterialUBO(){
            bindUniformBufferData();
        }
    };

    class GPUMaterial{
    private:
        GPUTexture albedo;
        GPUTexture normal;
        GPUTexture occlusion;
        GPUTexture emissive;
        GPUTexture metallic_roughness;
    public:
        MaterialUBO ubo;
    public:
        GPUMaterial();
        void makeGPUMaterial(MaterialData material_data);
        void bind();
    };
}
