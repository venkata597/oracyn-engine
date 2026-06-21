#include "../../../include/backend/resources/gl_mesh.hpp"
#include <cstdint>

void Backend::GPUMesh::makeMesh(std::vector<Vertex> vertices,std::vector<uint32_t> indices,unsigned int mat_ind){
    vb.create();
    ib.create();

    vb.setData(vertices);
    ib.setData(indices);

    this->indices_count = indices.size();
    this->materialIndex = mat_ind;
}

void Backend::GPUMesh::bindMesh(){
    vb.bind();
    ib.bind();
}

void Backend::GPUMesh::deleteMesh(){
    vb.deleteBuffer();
    ib.deleteBuffer();
}

const unsigned int& Backend::GPUMesh::getMaterialIndex(){
    return materialIndex;
}
