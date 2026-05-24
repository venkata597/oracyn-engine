#include "../../include/resources/shader.hpp"
#include "../../include/glinclude.hpp"
#include <fstream>
#include <iostream>


std::string Shader::_read_from_file(const std::string& path){
    std::ifstream file;
    file.open(path,std::ios::in | std::ios::ate);
    std::streamsize filesize = file.tellg();
    if(filesize==-1){
        std::cout << "[ORACYN (SHADER)]: Shader file empty" << '\n';
    }
    file.seekg(0,std::ios::beg);


    std::string contents(filesize,'\0');
    file.read(&contents[0],filesize);

    return contents;
}

void Shader::_create_shader(){
    unsigned int vertex,fragment;
    int success;
    char infolog[512];

    const char* vcode = _vertex_shader.c_str();
    const char* fcode = _fragment_shader.c_str();

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex,1,&vcode,NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex,GL_COMPILE_STATUS,&success);
    if(!success){
        glGetShaderInfoLog(vertex,512,NULL,infolog);
        std::cout << "[ORACYN (SHADER)]: Failed to compile vertex shader: " << infolog << '\n';
        return;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment,1,&fcode,NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment,GL_COMPILE_STATUS,&success);
    if(!success){
        glGetShaderInfoLog(fragment,512,NULL,infolog);
        std::cout << "[ORACYN (SHADER)]: Failed to compile fragment shader: " << infolog << '\n';
        return;
    }

    _program_id = glCreateProgram();
    glAttachShader(_program_id,vertex);
    glAttachShader(_program_id,fragment);
    glLinkProgram(_program_id);

    glGetProgramiv(_program_id,GL_LINK_STATUS,&success);
    if(!success){
        glGetProgramInfoLog(_program_id,512,NULL,infolog);
        std::cout << "[ORACYN (SHADER)]: Failed to link shader program " << infolog << '\n';
        return;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::bind(){
    glUseProgram(_program_id);
}

void Shader::unbind(){
    glUseProgram(0);
}


Shader::Shader(std::string vertexPath,std::string fragmentPath){
    _vertex_shader = _read_from_file(vertexPath);
    _fragment_shader = _read_from_file(fragmentPath);

    _create_shader();
}


Shader::~Shader(){
    glDeleteProgram(_program_id);
}