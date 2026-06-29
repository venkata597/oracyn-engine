#include "../../../include/backend/resources/gl_texture.hpp"
#include <cstdint>
#include <iostream>
#include <unordered_map>

std::unordered_map<std::string,std::vector<uint8_t>> default_textures = {
    {"white",{255,255,255,255}},
    {"normal",{128,128,255,255}},
    {"emissive",{0,0,0,255}},
    {"mr",{0,255,0,255}},
    {"occlusion",{255,255,255,255}}
};

std::string Backend::GPUTexture::_tex_type_to_string(TEXTURE_TYPE type){
    switch (type) {
    case ALBEDO:
        return "white";
    case NORMAL:
        return "normal";
    case EMISSIVE:
        return "emissive";
    case METALLIC_ROUGNESS:
        return "mr";
    case OCCLUSION:
        return "occlusion";
    default:
        return "white";
    }
}

void Backend::GPUTexture::createTexture(){
    glGenTextures(1,&id);
    glBindTexture(GL_TEXTURE_2D,id);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}

void Backend::GPUTexture::bindTexture(TEXTURE_TYPE tex_type){
    this->type = tex_type;
    glActiveTexture(tex_type);
    glBindTexture(GL_TEXTURE_2D,id);
}

void Backend::GPUTexture::uploadTextureData(Texture&& texture_data,TEXTURE_TYPE type){
    if(texture_data.img_data.get()!= nullptr){

        GLenum format;
        switch(texture_data.nchannels){
            case 1:
                format = GL_RED;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:
                format = GL_RGB;
                break;
        }

        glTexImage2D(GL_TEXTURE_2D,0,format,texture_data.width,texture_data.height,0,format,GL_UNSIGNED_BYTE,texture_data.img_data.get());
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,1,1,0,GL_RGBA,GL_UNSIGNED_BYTE,default_textures.at(_tex_type_to_string(type)).data());
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}
