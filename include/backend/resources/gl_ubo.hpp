#pragma once
#include <GL/glew.h>

namespace Backend{
    class UBO{
    protected:
        unsigned int id = 0;
    public:

        UBO() = default;

        UBO(UBO&& other) noexcept {
            id = other.id;
            other.id = 0;
        }

        UBO& operator=(UBO&& other) noexcept {
            if(this!=&other){
                id = other.id;
                other.id = 0;
            }
            return *this;
        }

        void createUniformBufferObject();
        void bindUniformBufferObject();
        virtual void bindUniformBufferData() = 0;
        void unbindUniformBufferObject();
        virtual void updateUniformBufferData() = 0;
        virtual ~UBO() = 0;
    };
}
