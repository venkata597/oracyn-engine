#pragma once
#include <GL/glew.h>
#include "../resources/gl_material.hpp"
#include "../resources/gl_mesh.hpp"
#include "../resources/gl_shader.hpp"
#include "../../renderer/renderer.hpp"
#include "../../renderer/rendercontext.hpp"


namespace Backend{
    class GLRenderDevice{
    private:
        void _init_device();
        void _make_shaders();
        void _set_location_bindings();

        Backend::ShaderProgram shader_program;
        Backend::VAO vertex_array_obj;
        InstanceBuffer ib;

        RenderState state;
    public:
        GLRenderDevice();
        void clearScreen();
        void submitState(RenderState& state);
        void drawScene();
    };
}
