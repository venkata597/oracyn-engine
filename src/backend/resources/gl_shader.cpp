#include "../../../include/backend/resources/gl_shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

void Backend::ShaderProgram::createProgram(){
    id = glCreateProgram();
}

void Backend::ShaderProgram::attachShader(unsigned int shader){
    glAttachShader(id,shader);
}

void Backend::ShaderProgram::linkShader(){
    glLinkProgram(id);
    glGetProgramiv(id,GL_LINK_STATUS,&success);

    if(!success){
        glGetProgramInfoLog(id,512,NULL,log);
        std::cout << "[ORACYN (BACKEND)]: Failed to link shader program" << log << '\n';
    }
}

void Backend::ShaderProgram::useShaderProgram(){
    glUseProgram(id);
}

void Backend::ShaderProgram::deleteProgram(){
    glDeleteProgram(id);
}



std::string Backend::_read_from_file(const std::string& filepath){
    std::ifstream file;
    file.open(filepath);

    std::stringstream source_stream;

    source_stream << file.rdbuf();
    file.close();

    return source_stream.str();
}

void Backend::Shader::createShader(const std::string& source,SHADER_TYPE type){
    shader_id = glCreateShader(type);
    if(this->shader_source.empty()){
        std::cout << "[ORACYN (BACKEND)]: Shader source is empty" << '\n';
        return;
    }

    const char* code = shader_source.c_str();
    glShaderSource(shader_id,1,&code,NULL);
}

void Backend::Shader::compileShader(){
    glCompileShader(shader_id);
    glGetShaderiv(shader_id,GL_COMPILE_STATUS,&success);

    if(!success){
        glGetShaderInfoLog(shader_id,512,NULL,log);
        std::cout << "[ORACYN (BACKEND)]: Failed to compile shader: " << log << '\n';
    }
}

unsigned int Backend::Shader::getShaderID(){
    return shader_id;
}
