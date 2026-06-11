#pragma once

#include <vector>
#include <string>

class Shader{
private:
    std::vector<char> shader_code;
public:
    void readFromFile(const std::string& filepath);
    const std::vector<char>& getShaderCode();
};