#include "../../../include/backend/resources/gl_ubo.hpp"

void Backend::UBO::createUniformBufferObject(){
    glGenBuffers(1,&id);
}

void Backend::UBO::bindUniformBufferObject(){
    glBindBuffer(GL_UNIFORM_BUFFER,id);
}

void Backend::UBO::unbindUniformBufferObject(){
    glBindBuffer(GL_UNIFORM_BUFFER,0);
}
