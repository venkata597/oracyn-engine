#include "../../../include/backend/resources/gl_shader.hpp"
#include <fstream>
#include <sstream>

std::string Backend::_read_from_file(const std::string& filepath){
    std::ifstream file;
    file.open(filepath);

    std::stringstream source_stream;

    source_stream << file.rdbuf();
    file.close();

    return source_stream.str();
}

void Backend::Shader::createShader(){
    shader_id = glCreateShader(GL_VERTEX_SHADER);
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