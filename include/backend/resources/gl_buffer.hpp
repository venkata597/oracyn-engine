#pragma once

#include <GL/glew.h>
#include <vector>
#include "../../resources/vertex.hpp"

namespace Backend{

    class VAO{
    private:
         unsigned int id = 0;
    public:

        VAO() = default;

        VAO(VAO&& other) noexcept {
            id = other.id;
            other.id = 0;
        }

        VAO& operator=(VAO&& other) noexcept {
            if(this!=&other){
                id = other.id;
                other.id = 0;
            }
            return *this;
        }

        void createVAO();
        void bind();
        void setAttribPointers();
        void setInstanceAttribPointers();
        void unbind();
    };

    class VertexBuffer{
    private:
        unsigned int id = 0;
    public:

        VertexBuffer() = default;

        VertexBuffer(VertexBuffer&& other) noexcept {
            id = other.id;
            other.id = 0;
        }

        VertexBuffer& operator=(VertexBuffer&& other) noexcept {
            if(this!=&other){
                id = other.id;
                other.id = 0;
            }
            return *this;
        }

        void create();
        void bind();
        void setData(const std::vector<Vertex>& vertices);
        void deleteBuffer();
    };

    class IndexBuffer{
    private:
        unsigned int id = 0;
    public:
        IndexBuffer() = default;

        IndexBuffer(IndexBuffer&& other) noexcept {
            id = other.id;
            other.id = 0;
        }

        IndexBuffer& operator=(IndexBuffer&& other) noexcept {
            if(this!=&other){
                id = other.id;
                other.id = 0;
            }
            return *this;
        }

        void create();
        void bind();
        void setData(const std::vector<uint32_t>& indices);
        void deleteBuffer();
    };

    class InstanceBuffer{
    private:
        unsigned int id = 0;
    public:

        InstanceBuffer() = default;

        InstanceBuffer(InstanceBuffer&& other) noexcept {
            id = other.id;
            other.id = 0;
        }

        InstanceBuffer& operator=(InstanceBuffer&& other) noexcept {
            if(this!=&other){
                id = other.id;
                other.id = 0;
            }
            return *this;
        }

        void create();
        void bind();
        void setData(const std::vector<glm::mat4>& transforms);
        void deleteBuffer();
        const unsigned int& getID(){return id;}
    };
}
