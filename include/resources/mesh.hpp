#pragma once

#include <glm/glm.hpp>
#include <vector>


struct cgltf_data;

typedef struct{
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normals;
} Vertex;

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