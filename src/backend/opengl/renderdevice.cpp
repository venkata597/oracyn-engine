#include "../../../include/backend/opengl/renderdevice.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

void Backend::GLRenderDevice::_init_device(){
    GLenum err = glewInit();
    if(err!=GLEW_OK){
        std::cout << "[ORACYN (BACKEND)]: Failed to initialize glew";
    }

    glEnable(GL_DEPTH_TEST);
}

void Backend::GLRenderDevice::_make_shaders(){
    Shader vertex_shader;
    Shader frag_shader;

    vertex_shader.createShader("shaders/vert.glsl",VERTEX_SHADER);
    frag_shader.createShader("shaders/frag.glsl",FRAGMENT_SHADER);
    vertex_shader.compileShader(); frag_shader.compileShader();

    shader_program.createProgram();
    shader_program.attachShader(vertex_shader.getShaderID());
    shader_program.attachShader(frag_shader.getShaderID());

    shader_program.linkShader();

    vertex_shader.deleteShader();
    frag_shader.deleteShader();
}

void Backend::GLRenderDevice::_set_location_bindings(){
    shader_program.useShaderProgram();
    glUniform1i(glGetUniformLocation(shader_program.getShaderID(),"albedo"),0);
    glUniform1i(glGetUniformLocation(shader_program.getShaderID(),"normal"),1);
    glUniform1i(glGetUniformLocation(shader_program.getShaderID(),"emissive"),2);
    glUniform1i(glGetUniformLocation(shader_program.getShaderID(),"occlusion"),3);
    glUniform1i(glGetUniformLocation(shader_program.getShaderID(),"mr"),4);
}

Backend::GLRenderDevice::GLRenderDevice(){
    _init_device();
    _make_shaders();
    _set_location_bindings();
    vertex_array_obj.createVAO();
    vertex_array_obj.bind();
    ib.create();
    ib.bind();
    vertex_array_obj.setAttribPointers();
}

void Backend::GLRenderDevice::clearScreen(){
    glClearColor(0.2f,0.3f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Backend::GLRenderDevice::submitState(RenderState& state){
    this->state = state;
}

void Backend::GLRenderDevice::drawScene(){
    vertex_array_obj.bind();
    ib.bind();
    for(const auto& cmd: state.draw_queue){
        auto& [meshes,materials] = RenderContext::gpuResourceMap.at(cmd.assetID);

        std::vector<glm::mat4> transforms;
        transforms.reserve(cmd.transforms.size());

        for(const auto& transform: cmd.transforms){
            transforms.push_back(transform.matrix);
        }
        ib.setData(transforms);

        for(auto& mesh: meshes){
            if(mesh.getMaterialIndex() == UINT32_MAX){
                continue;
            }
            materials[mesh.getMaterialIndex()].ubo.bindUniformBufferObject();
            materials[mesh.getMaterialIndex()].ubo.updateUniformBufferData();
            materials[mesh.getMaterialIndex()].bind();
            mesh.bindMesh();
            glDrawElementsInstanced(GL_TRIANGLES,mesh.getIndicesCount(),GL_UNSIGNED_INT,nullptr,transforms.size());
        }
    }
}
