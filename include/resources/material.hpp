#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "../glad/glad/glad.h"

struct cgltf_data;

struct Texture{

    unsigned int texID;

    bool containsTexture;
    const char* failure_reason;

    int width,height,nchannels;
};

Texture loadTexture(const char* filepath);


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
    std::vector<MaterialData> materials;
public:
    void constructMaterial(cgltf_data* data);
    const std::vector<MaterialData>& getMaterials() const;
};