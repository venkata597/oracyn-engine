#include "../../../include/backend/resources/gl_buffer.hpp"
#include <cstddef>
#include <cstdint>

void Backend::VAO::createVAO(){
    glGenVertexArrays(1,&id);
}

void Backend::VAO::bind(){
    glBindVertexArray(id);
}

void Backend::VAO::setAttribPointers(){
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,uv));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,normals));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,tangents));
}

void Backend::VAO::setInstanceAttribPointers(){
    unsigned int location = 4;
    for(int i = 0;i<4;i++){
        glEnableVertexAttribArray(location+i);
        glVertexAttribPointer(
            location+i,
            4,
            GL_FLOAT,
            GL_FALSE,
            sizeof(glm::mat4),
            (void*)(sizeof(glm::vec4)*i)
        );

        glVertexAttribDivisor(location+i,1);
    }
}

void Backend::VAO::unbind(){
    glBindVertexArray(0);
}

void Backend::VertexBuffer::create(){
    glGenBuffers(1,&this->id);
}

void Backend::VertexBuffer::bind(){
    glBindBuffer(GL_ARRAY_BUFFER,id);
}

void Backend::VertexBuffer::setData(const std::vector<Vertex>& vertices){
    glBufferData(GL_ARRAY_BUFFER,vertices.size() * sizeof(Vertex),vertices.data(),GL_DYNAMIC_DRAW);
}

void Backend::IndexBuffer::create(){
    glGenBuffers(1,&this->id);
}

void Backend::IndexBuffer::bind(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,id);
}

void Backend::IndexBuffer::setData(const std::vector<uint32_t>& indices){
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(uint32_t),indices.data(),GL_STATIC_DRAW);
}

void Backend::VertexBuffer::deleteBuffer(){
    glDeleteBuffers(1,&id);
}

void Backend::IndexBuffer::deleteBuffer(){
    glDeleteBuffers(1,&id);
}

void Backend::InstanceBuffer::create(){
    glGenBuffers(1,&this->id);
}

void Backend::InstanceBuffer::bind(){
    glBindBuffer(GL_ARRAY_BUFFER,id);
}

void Backend::InstanceBuffer::setData(const std::vector<glm::mat4>& transforms){
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(glm::mat4) * transforms.size(),
        transforms.data(),
        GL_STATIC_DRAW
    );
}

void Backend::InstanceBuffer::deleteBuffer(){
    glDeleteBuffers(1,&id);
}
