#pragma once

#include <GL/glew.h>
#include <iostream>
#include <string>

namespace Backend{
    static struct ShaderProgram{
    private:
        static unsigned int id;
        static int success;
        static char log[512];
    public:
        static void createProgram(){
            id = glCreateProgram();
        }

        static void attachShader(unsigned int shader){
            glAttachShader(id,shader);
        }

        static void linkShader(){
            glLinkProgram(id);
            glGetProgramiv(id,GL_LINK_STATUS,&success);
            
            if(!success){
                glGetProgramInfoLog(id,512,NULL,log);
                std::cout << "[ORACYN (BACKEND)]: Failed to link shader program" << log << '\n';
            }
        }


        static void deleteProgram(){
            glDeleteProgram(id);
        }

    }ShaderProgram;

    std::string _read_from_file(const std::string& filepath);

    class Shader{
    private:
        std::string shader_source;

        unsigned int shader_id;

        int success;
        char log[512];
    public:
        void createShader();
        void compileShader();
        unsigned int getShaderID();

        void deleteShader();
    };
}