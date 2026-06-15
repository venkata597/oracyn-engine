#include "../../../include/backend/opengl/gl_buffer.hpp"
#include <cstdint>

void Backend::VertexBuffer::create(){
    glGenBuffers(1,&this->id);
}

void Backend::VertexBuffer::bind(){
    glBindBuffer(GL_ARRAY_BUFFER,id);
}

void Backend::VertexBuffer::setData(std::vector<Vertex> vertices){
    glBufferData(GL_ARRAY_BUFFER,vertices.size() * sizeof(Vertex),vertices.data(),GL_STATIC_DRAW);
}

void Backend::IndexBuffer::create(){
    glGenBuffers(1,&this->id);
}

void Backend::IndexBuffer::bind(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,id);
}

void Backend::IndexBuffer::setData(std::vector<uint32_t> indices){
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(uint32_t),indices.data(),GL_STATIC_DRAW);
}