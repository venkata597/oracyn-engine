#pragma once

#include <glm/glm.hpp>
#include "gl_ubo.hpp"

namespace Backend{

    struct CameraUBOData{
        glm::mat4 uView;
        glm::mat4 uProj;
        glm::vec3 uPos;
    };

    class CameraUBO: public UBO{
    public:
        CameraUBOData data;
    public:
        void bindUniformBufferData() override;
        void updateUniformBufferData() override;
    };
}
