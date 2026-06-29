#include "../../../include/backend/resources/gl_material.hpp"
#include <cstring>


void Backend::MaterialUBO::bindUniformBufferData(){
    glBufferData(
        GL_UNIFORM_BUFFER,
        sizeof(MaterialUBOData),
        nullptr,
        GL_DYNAMIC_DRAW
    );

    glBindBufferBase(GL_UNIFORM_BUFFER,0,id);
}

void Backend::MaterialUBO::updateUniformBufferData(){
    this->bindUniformBufferObject();
    void* ptr = glMapBuffer(GL_UNIFORM_BUFFER,GL_WRITE_ONLY);
    std::memcpy(ptr,&data,sizeof(MaterialUBOData));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
}

Backend::GPUMaterial::GPUMaterial(){
    ubo.createUniformBufferObject();
    ubo.bindUniformBufferObject();
    ubo.bindUniformBufferData();

    albedo.createTexture();
    normal.createTexture();
    occlusion.createTexture();
    emissive.createTexture();
    metallic_roughness.createTexture();
}

void Backend::GPUMaterial::makeGPUMaterial(MaterialData material_data){
    albedo.bindTexture(ALBEDO);
    albedo.uploadTextureData(std::move(material_data.albedo),ALBEDO);

    normal.bindTexture(NORMAL);
    normal.uploadTextureData(std::move(material_data.normal),NORMAL);

    emissive.bindTexture(EMISSIVE);
    emissive.uploadTextureData(std::move(material_data.emissive),EMISSIVE);

    occlusion.bindTexture(OCCLUSION);
    occlusion.uploadTextureData(std::move(material_data.occlusion),OCCLUSION);

    metallic_roughness.bindTexture(METALLIC_ROUGNESS);
    metallic_roughness.uploadTextureData(std::move(material_data.pbr.metallicRoughnessTexture),METALLIC_ROUGNESS);

    ubo.data.uBaseColor = std::move(material_data.pbr.baseColorFactor);
    ubo.data.uRoughnessFactor = std::move(material_data.pbr.roughnessFactor);
    ubo.data.uMetallicFactor = std::move(material_data.pbr.metallicFactor);
    ubo.data.uEmissiveFactor = std::move(material_data.emissiveFactor);
    ubo.data.uEmissiveStrength = std::move(material_data.emissiveStrength);
    ubo.data.uOcclusionStrength = std::move(material_data.occlusionStrength);
}

void Backend::GPUMaterial::bind(){
    albedo.bindTexture(ALBEDO);
    normal.bindTexture(NORMAL);
    occlusion.bindTexture(OCCLUSION);
    emissive.bindTexture(EMISSIVE);
    metallic_roughness.bindTexture(METALLIC_ROUGNESS);
}
