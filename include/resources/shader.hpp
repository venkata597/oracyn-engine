#pragma once

#include <string>


class Shader{
private:
    unsigned int _program_id;

    std::string _vertex_shader;
    std::string _fragment_shader;

private:
    std::string _read_from_file(const std::string& path);
    void _create_shader();
public:
    Shader(std::string vertexPath,std::string fragmentPath);
    void bind();
    void unbind();
    ~Shader();
    
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
};