#pragma once
#include <GL/glew.h>
#include "../resources/gl_material.hpp"
#include "../resources/gl_mesh.hpp"
#include "../resources/gl_camera.hpp"
#include "../resources/gl_shader.hpp"
#include "../../renderer/renderer.hpp"
#include "../../renderer/rendercontext.hpp"


namespace Backend{
    class GLRenderDevice{
        friend class RenderContext;
    private:
        void _init_device();
        void _make_shaders();
        void _set_location_bindings();

        Backend::ShaderProgram shader_program;
        InstanceBuffer ib;

        Backend::CameraUBO camera_ubo;

        RenderState state;

        void _draw_node(GPUNode& node,GPUScene& scene,std::vector<GPUMaterial>& materials,std::vector<glm::mat4>& eTransforms);
    public:
        GLRenderDevice();
        const unsigned int& getInstanceBufferID() {return ib.getID();}
        void clearScreen();
        void submitState(RenderState&& state);
        void drawScene();
    };
}
