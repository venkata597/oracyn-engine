#pragma once

#include "../resources/entity.hpp"
#include "../resources/scene.hpp"
#include "../resources/camera.hpp"
#include <vector>

struct CameraData{
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec3 pos;
};


struct DrawCommand{
    unsigned int assetID;
    unsigned int instance_count;
    std::vector<Transform> transforms;
};

struct RenderState{
    CameraData cam_data;
    std::vector<DrawCommand> draw_queue;
};

class Renderer{
private:
    RenderState state;

    void _build_render_queue(const SceneMap& map);
public:
    void beginFrame(Camera cam);
    void draw(const SceneMap& map);
    void endFrame();
    RenderState getState();
};
