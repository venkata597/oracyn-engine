#pragma once

#include "gl_buffer.hpp"
#include <cstdint>

namespace Backend{
    class GPUMesh{
    private:
        VertexBuffer vb;
        IndexBuffer ib;
        unsigned int materialIndex;
        unsigned int indices_count;
    public:
        void makeMesh(const std::vector<Vertex>& vertices,const std::vector<uint32_t>& indices,unsigned int mat_ind);
        const unsigned int& getMaterialIndex();
        const unsigned int& getIndicesCount() {return indices_count;}
        void bindMesh();
        void deleteMesh();
    };
}
