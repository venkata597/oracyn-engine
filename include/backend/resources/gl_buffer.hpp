#pragma once

#include <GL/glew.h>
#include <vector>
#include "../../resources/vertex.hpp"

namespace Backend{

    class VAO{
    private:
         unsigned int id = 0;
    public:
        void createVAO();
        void bind();
        void setAttribPointers();
        void unbind();
    };

    class VertexBuffer{
    private:
        unsigned int id = 0;
    public:
        void create();
        void bind();
        void setData(const std::vector<Vertex>& vertices);
        void deleteBuffer();
    };

    class IndexBuffer{
    private:
        unsigned int id = 0;
    public:
        void create();
        void bind();
        void setData(const std::vector<uint32_t>& indices);
        void deleteBuffer();
    };

    class InstanceBuffer{
    private:
        unsigned int id = 0;
    public:
        void create();
        void bind();
        void setData(const std::vector<glm::mat4>& transforms);
        void deleteBuffer();
    };
}
