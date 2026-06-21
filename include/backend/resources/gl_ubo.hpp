#pragma once
#include <GL/glew.h>

namespace Backend{
    class UBO{
    protected:
        unsigned int id = 0;
    public:
        void createUniformBufferObject();
        void bindUniformBufferObject();
        virtual void bindUniformBufferData();
        void unbindUniformBufferObject();
        virtual void updateUniformBufferData();
    };
}
