#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "vertex.hpp"

struct cgltf_data;

typedef struct{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
}Primitive;

struct Mesh{
    std::vector<Primitive> primitives;
    const char* name;
};

class MeshLoader{
private:
    std::vector<Mesh> meshes;
    Primitive data;
public:
    void constructMesh(cgltf_data* data);
    const std::vector<Mesh>& getMeshes() const;

    MeshLoader() = default;
    MeshLoader(const MeshLoader&) = delete;
    MeshLoader operator=(const MeshLoader&) = delete;
};