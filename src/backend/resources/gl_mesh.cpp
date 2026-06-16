#include "../../../include/backend/resources/gl_mesh.hpp"
#include <cstdint>

void Backend::GPUMesh::makeMesh(std::vector<Vertex> vertices,std::vector<uint32_t> indices){
    vb.create();
    ib.create();
    
    vb.setData(vertices);
    ib.setData(indices);
}

void Backend::GPUMesh::bindMesh(){
    vb.bind();
    ib.bind();
}

void Backend::GPUMesh::deleteMesh(){
    vb.deleteBuffer();
    ib.deleteBuffer();
}

