#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "vertex.hpp"

struct cgltf_data;

typedef struct{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    uint32_t material_index;
}Primitive;

struct Mesh{
    std::vector<Primitive> primitives;
    const char* name;
};

class MeshLoader{
private:
public:
    std::vector<Mesh> constructMesh(cgltf_data* data);

    MeshLoader() = default;
    MeshLoader(const MeshLoader&) = delete;
    MeshLoader operator=(const MeshLoader&) = delete;
};