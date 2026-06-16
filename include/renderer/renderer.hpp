#pragma once

#include "../resources/entity.hpp"
#include "../resources/scene.hpp"
#include <vector>

struct DrawCommand{
    unsigned int assetID;
    unsigned int instance_count;
    std::vector<Transform> transforms;
};

struct RenderState{
    SceneMap map;
    std::vector<DrawCommand> draw_queue;
};

class Renderer{
private:
    RenderState state;

    void _build_render_queue(const SceneMap& map);
public:
    void draw(const SceneMap& map);
    const std::vector<DrawCommand>& getQueue();
};