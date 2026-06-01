#include "../../include/resources/material.hpp"
#include "../../include/gltf/cgltf/cgltf.h"
#include <iostream>
#include <memory>
#include <vector>
#include <cstring>

Texture loadTexture(std::string filepath){
    int width,height,channels;
    Texture t;
    unsigned char* data = stbi_load(filepath.c_str(),&width,&height,&channels,0);
    if(data == NULL){
        t.containsTexture = false;
        t.failure_reason = stbi_failure_reason();
    }
    else{
        t.height = height;
        t.width = width;
        t.nchannels = channels;
        t.containsTexture = true;
        t.img_data = std::unique_ptr<unsigned char [],Deleter>(data);
    }
    return std::move(t);
}


void MaterialLoader::constructMaterial(cgltf_data* data,std::string p){
    for(int c = 0;c<data->materials_count;c++){
        cgltf_material* material = &(data->materials[c]);
        MaterialData data;
        data.name = material->name;
        char* filepath;
        int width,height,channels;

        // PBR Factors
        auto pbr = material->pbr_metallic_roughness;
        data.pbr.metallicFactor = pbr.metallic_factor;
        data.pbr.roughnessFactor = pbr.roughness_factor;
        data.pbr.baseColorFactor = glm::vec4(
            pbr.base_color_factor[0],
            pbr.base_color_factor[1],
            pbr.base_color_factor[2],
            pbr.base_color_factor[3]
        );

        if(pbr.metallic_roughness_texture.texture){
            filepath = pbr.metallic_roughness_texture.texture->image->uri;
            std::string cf(filepath);
            Texture mtrt = loadTexture(p+cf);

            if(!mtrt.containsTexture){
                std::cout << "[ORACYN (Material)]: Error Loading Metallic Roughness Texture" << mtrt.failure_reason << '\n';
            }
            else{
                data.pbr.metallicRoughnessTexture = std::move(mtrt);
            }
        }

        // Albedo Texture
        if(pbr.base_color_texture.texture){
            filepath = pbr.base_color_texture.texture->image->uri;
            std::string cf(filepath);
            Texture base = loadTexture(p+cf);

            if(!base.containsTexture){
                std::cout << "[ORACYN (Material)]: Error Loading Albedo Texture" << base.failure_reason << '\n';
            }
            else{
                data.albedo = std::move(base);
            }
        }

        // Normal Map
        if(material->normal_texture.texture){
            filepath = material->normal_texture.texture->image->uri;
            std::string cf(filepath);
            Texture normal = loadTexture(p+cf);
            if(!normal.containsTexture){
                std::cout << "[ORACYN (Material)]: Error Loading Normal Texture" << normal.failure_reason << '\n';
            }
            else{
                data.normal = std::move(normal);
                data.normalScale = material->normal_texture.scale;
            }
        }

        if(material->occlusion_texture.texture){
            filepath = material->occlusion_texture.texture->image->uri;
            std::string cf(filepath);
            Texture occlusion = loadTexture(p+cf);
            if(!occlusion.containsTexture){
                std::cout << "[ORACYN (Material)]: Error Loading Occulsion Texture" << occlusion.failure_reason << '\n';
            }
            else{
                data.occlusion = std::move(occlusion);
                data.occlusionStrength = material->occlusion_texture.scale;   
            }
        }

        if(material->emissive_texture.texture){
            filepath = material->emissive_texture.texture->image->uri;
            std::string cf(filepath);
            Texture emissive = loadTexture(p+cf);
            if(!emissive.containsTexture){
                std::cout << "[ORACYN (Material)]: Error Loading Emmisive Texture" << emissive.failure_reason << '\n';
            }
            else{
                data.emissive = std::move(emissive);
                data.emissiveStrength = material->emissive_strength.emissive_strength;
                data.emissiveFactor = glm::vec3(
                    material->emissive_factor[0],
                    material->emissive_factor[1],
                    material->emissive_factor[2]
                );
                
            }
        }

        data.doubleSided = material->double_sided;
        switch(material->alpha_mode){
            case cgltf_alpha_mode_opaque:
                data.mode = AlphaMode::OPAQUE;
                break;
            case cgltf_alpha_mode_mask:
                data.mode = AlphaMode::MASK;
                break;
            case cgltf_alpha_mode_blend:
                data.mode = AlphaMode::BLEND;
                break;
            case cgltf_alpha_mode_max_enum:
            default:
                data.mode = AlphaMode::OPAQUE; // For Now
        }

        materials.push_back(std::move(data));
    }
}

std::vector<MaterialData> MaterialLoader::getMaterials(){
    return std::move(this->materials);
}