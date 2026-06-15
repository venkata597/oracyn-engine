#pragma once

#include <GL/glew.h>
#include <cstddef>
#include <vector>
#include "../../resources/vertex.hpp"

namespace Backend{

    static struct VAO{    
    private:
        static unsigned int id;
    public:
        static void createVAO(){
            glGenVertexArrays(1,&id);
        }

        static void bind(){
            glBindVertexArray(id);
        }

        static void setAttribPointers(){
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(Vertex),(void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1,1,GL_FLOAT,GL_FALSE,2*sizeof(Vertex),(void*)offsetof(Vertex,uv));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,3*sizeof(Vertex),(void*)offsetof(Vertex,normals));
        }

        static void unbind(){
            glBindVertexArray(0);
        }
    }VAO;

    class VertexBuffer{
    private:
        unsigned int id;
    public:
        void create();
        void bind();
        void setData(std::vector<Vertex> vertices);
    };

    class IndexBuffer{
    private:
        unsigned int id;
    public:
        void create();
        void bind();
        void setData(std::vector<uint32_t> indices);
    };
}