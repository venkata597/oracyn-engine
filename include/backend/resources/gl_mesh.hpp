#pragma once

#include "gl_buffer.hpp"

namespace Backend{
    class GPUMesh{
    private:
        VertexBuffer vb;
        IndexBuffer ib;
    public:
        void makeMesh(std::vector<Vertex> vertices,std::vector<uint32_t> indices);
        void bindMesh();
        void deleteMesh();
    };
}