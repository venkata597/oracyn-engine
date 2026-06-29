#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "vertex.hpp"
#include "../../include/resources/transform.hpp"

struct cgltf_data;
struct cgltf_node;
struct cgltf_mesh;


typedef struct{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    uint32_t material_index;
}Primitive;

struct Mesh{
    std::vector<Primitive> primitives;
    const char* name;
    bool hasMesh;
};

struct NodeData{
    Mesh mesh;
    Transform localTransform;
    std::vector<NodeData> children;
};

class MeshLoader{
private:
    cgltf_data* _current_model_data = nullptr;
    Mesh _construct_mesh(cgltf_mesh* mesh);
    NodeData _construct_node(cgltf_node* node);
public:
    std::vector<NodeData> constructModel(cgltf_data* data);

    MeshLoader() = default;
    MeshLoader(const MeshLoader&) = delete;
    MeshLoader operator=(const MeshLoader&) = delete;
};
