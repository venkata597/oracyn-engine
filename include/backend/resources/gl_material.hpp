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
            glBindBufferBase(GL_UNIFORM_BUFFER,0,id);
        }
    };

    class GPUMaterial{
    private:
        MaterialData material_data;
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
