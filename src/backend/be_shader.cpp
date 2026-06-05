#include "../../include/backend/be_shader.hpp"
#include <iostream>
#include <fstream>


void Shader::readFromFile(const std::string& filepath){
    std::ifstream file(filepath,std::ios::ate | std::ios::binary);
    if(!file.is_open()){
        std::cout << "[ORACYN (Backend {Shader})]: Couldn't find shader file (PATH: " << filepath << " )" << '\n';
        return;
    }

    size_t filesize = (size_t)file.tellg();
    file.seekg(0);
    shader_code.resize(filesize);
    file.read(shader_code.data(),filesize);
    file.close();
}

const std::vector<char>& Shader::getShaderCode(){
    return shader_code;
}