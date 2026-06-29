#include "../../../include/backend/resources/gl_camera.hpp"
#include <cstring>

void Backend::CameraUBO::bindUniformBufferData(){
    glBufferData(
        GL_UNIFORM_BUFFER,
        sizeof(CameraUBOData),
        nullptr,
        GL_DYNAMIC_DRAW
    );

    glBindBufferBase(GL_UNIFORM_BUFFER,1,id);
}

void Backend::CameraUBO::updateUniformBufferData(){
    this->bindUniformBufferObject();
    void* ptr = glMapBuffer(GL_UNIFORM_BUFFER,GL_WRITE_ONLY);
    std::memcpy(ptr,&data,sizeof(CameraUBOData));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
}
