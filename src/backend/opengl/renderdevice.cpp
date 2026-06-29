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
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f,1.0f);
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

    glm::vec3 lightdir = glm::normalize(glm::vec3(-0.5f,-1.0f,-0.3f));
    glUniform3f(glGetUniformLocation(shader_program.getShaderID(),"ulightDir"),lightdir.x,lightdir.y,lightdir.z);
    glUniform3f(glGetUniformLocation(shader_program.getShaderID(),"ulightColor"),1.0f,1.0f,1.0f);
}

Backend::GLRenderDevice::GLRenderDevice(){
    _init_device();
    _make_shaders();
    camera_ubo.createUniformBufferObject();
    shader_program.useShaderProgram();
    _set_location_bindings();
    camera_ubo.bindUniformBufferObject();
    camera_ubo.bindUniformBufferData();
    ib.create();
}

void Backend::GLRenderDevice::clearScreen(){
    glClearColor(0.2f,0.3f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Backend::GLRenderDevice::submitState(RenderState&& state){
    this->state = std::move(state);
}

void Backend::GLRenderDevice::_draw_node(GPUNode& node,GPUScene& scene,std::vector<GPUMaterial>& materials,std::vector<glm::mat4>& eTransforms){
    node.nodeUBO.bindUniformBufferObject();
    node.nodeUBO.updateUniformBufferData();

    ib.bind();
    ib.setData(eTransforms);

    if(node.hasGPUMesh){
        for(auto& prim: scene.getMesh(node.meshIndex)){
            materials[prim.materialIndex].ubo.bindUniformBufferObject();
            materials[prim.materialIndex].ubo.updateUniformBufferData();
            materials[prim.materialIndex].bind();
            prim.vao.bind();
            ib.bind();
            glDrawElementsInstanced(GL_TRIANGLES,prim.getIndicesCount(),GL_UNSIGNED_INT,nullptr,eTransforms.size());
        }
    }
    else{
        for(auto& child_node: node.child_nodes){
            _draw_node(child_node,scene,materials,eTransforms);
        }
    }
}

void Backend::GLRenderDevice::drawScene(){
    shader_program.useShaderProgram();

    camera_ubo.data.uView = state.cam_data.view;
    camera_ubo.data.uProj = state.cam_data.proj;
    camera_ubo.data.uPos = state.cam_data.pos;

    camera_ubo.bindUniformBufferObject();
    camera_ubo.updateUniformBufferData();


    for(const auto& cmd: state.draw_queue){
        auto& [scene,materials] = RenderContext::gpuResourceMap.at(cmd.assetID);

        std::vector<glm::mat4> entityTransforms;
        entityTransforms.reserve(cmd.transforms.size());

        for(const auto& transform: cmd.transforms){
            entityTransforms.push_back(transform.getTransform());
        }

        for(auto& node: scene.gScene){
            _draw_node(node,scene,materials,entityTransforms);
        }
    }
}
