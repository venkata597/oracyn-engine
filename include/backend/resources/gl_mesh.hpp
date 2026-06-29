#pragma once

#include "gl_buffer.hpp"
#include "../../resources/mesh.hpp"
#include "gl_ubo.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace Backend{

    struct GPUPrimitive{
        VAO vao;
        VertexBuffer vb;
        IndexBuffer ib;
        unsigned int materialIndex;
        unsigned int indices_count;


        GPUPrimitive() = default;
        GPUPrimitive(const GPUPrimitive&) = delete;
        GPUPrimitive& operator=(const GPUPrimitive&) = delete;
        GPUPrimitive(GPUPrimitive&&) = default;
        GPUPrimitive& operator=(GPUPrimitive&&) = default;

        const unsigned int& getMaterialIndex() {return materialIndex;}
        const unsigned int& getIndicesCount() {return indices_count;}
    };

    class GPUMesh{
    public:
        std::vector<GPUPrimitive> mesh;
    public:
        void makeMesh(Mesh m,unsigned int ibid);

        GPUMesh() = default;
        GPUMesh(const GPUMesh&) = delete;
        GPUMesh& operator=(const GPUMesh&) = delete;
        GPUMesh(GPUMesh&&) = default;
        GPUMesh& operator=(GPUMesh&&) = default;
    };

    struct GPUNodeUBOData{
        glm::mat4 globalTransform;
    };

    class NodeUBO: public UBO{
    public:
        GPUNodeUBOData data;
    public:
        void bindUniformBufferData() override;
        void updateUniformBufferData() override;
    };

    class GPUNode{
    public:
        bool hasGPUMesh;
        unsigned int meshIndex = UINT32_MAX;
        glm::mat4 localTransform;
        glm::mat4 globalTransform;
        std::vector<GPUNode> child_nodes;

        NodeUBO nodeUBO;

        GPUNode();
        GPUNode(const GPUNode&) = delete;
        GPUNode& operator=(const GPUNode&) = delete;
        GPUNode(GPUNode&& other){
            hasGPUMesh = other.hasGPUMesh;
            meshIndex = other.meshIndex;
            localTransform = other.localTransform;
            globalTransform = other.globalTransform;
            nodeUBO = std::move(other.nodeUBO);
            child_nodes = std::move(other.child_nodes);
            other.meshIndex = UINT32_MAX;
        }
        GPUNode& operator=(GPUNode&&) = default;
    };

    class GPUScene{
    private:
        std::unordered_map<std::string,unsigned int> mesh_cache;
        std::vector<GPUMesh> gMeshes;

    private:
        std::vector<GPUNode> _construct_scene_recur(const std::vector<NodeData>& scene,glm::mat4 globalTransform,unsigned int instance_buffer_id);
    public:
        std::vector<GPUNode> gScene;
        void constructScene(std::vector<NodeData> scene,glm::mat4 globalTransform,unsigned int instance_buffer_id);
        std::vector<GPUPrimitive>& getMesh(unsigned int index);
    };

}
