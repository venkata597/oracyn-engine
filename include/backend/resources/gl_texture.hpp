#pragma once

#include <GL/glew.h>
#include "../../resources/material.hpp"

enum TEXTURE_TYPE{
    ALBEDO = GL_TEXTURE0,
    NORMAL = GL_TEXTURE1,
    EMISSIVE = GL_TEXTURE2,
    OCCLUSION = GL_TEXTURE3,
    METALLIC_ROUGNESS = GL_TEXTURE4
};


namespace Backend{

    class GPUTexture{
    private:
        unsigned int id = 0;

        std::string _tex_type_to_string(TEXTURE_TYPE type);
    public:
        TEXTURE_TYPE type;
    public:

        void createTexture();
        void bindTexture(TEXTURE_TYPE tex_type);
        void uploadTextureData(Texture texture_data,TEXTURE_TYPE type);
    };
}
