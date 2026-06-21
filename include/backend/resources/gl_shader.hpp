#pragma once

#include <GL/glew.h>
#include <string>

namespace Backend{

    enum SHADER_TYPE{
        VERTEX_SHADER = GL_VERTEX_SHADER,
        FRAGMENT_SHADER = GL_FRAGMENT_SHADER
    };

    class ShaderProgram{
    private:
        unsigned int id;
        int success;
        char log[512];
    public:
        void createProgram();
        void attachShader(unsigned int shader);
        void linkShader();
        void useShaderProgram();
        void deleteProgram();
        const unsigned int& getShaderID() {return id;}
    };

    std::string _read_from_file(const std::string& filepath);

    class Shader{
    private:
        std::string shader_source;

        unsigned int shader_id;

        int success;
        char log[512];
    public:
        void createShader(const std::string& source,SHADER_TYPE type);
        void compileShader();
        unsigned int getShaderID();

        void deleteShader();
    };
}
