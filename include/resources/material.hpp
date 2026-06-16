#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

#include "../../include/stb_image/stb_image.h"


struct cgltf_data;


struct Deleter{
    void operator()(unsigned char* p) {stbi_image_free(p);}
};

struct Texture{

    unsigned int texID;

    bool containsTexture;
    const char* failure_reason;

    int width,height,nchannels;

    std::unique_ptr<unsigned char [],Deleter> img_data;
};

Texture loadTexture(std::string filepath);


struct PBR{
    glm::vec4 baseColorFactor;
    float metallicFactor;
    float roughnessFactor;
    Texture metallicRoughnessTexture;
};

enum AlphaMode{
    OPAQUE,
    MASK,
    BLEND
};

struct MaterialData{

    const char* name;

    PBR pbr;
    Texture albedo;

    Texture normal;
    float normalScale = 1.0f;

    Texture occlusion;
    float occlusionStrength = 1.0f;

    Texture emissive;
    float emissiveStrength = 1.0f;
    glm::vec3 emissiveFactor;

    AlphaMode mode = AlphaMode::OPAQUE;
    bool doubleSided;
};

class MaterialLoader{   
private:
public:
    std::vector<MaterialData> constructMaterial(cgltf_data* data,std::string p);

    MaterialLoader() = default;
    MaterialLoader(const MaterialLoader&) = delete;
    MaterialLoader operator=(const MaterialLoader&) = delete;
};