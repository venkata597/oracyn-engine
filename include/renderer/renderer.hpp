#pragma once

#include "../resources/entity.hpp"
#include "../resources/scene.hpp"
#include <vector>

struct DrawCommand{
    Entity entity;
    unsigned int instance_count;
    std::vector<Transform> transforms;
};

class Renderer{
private:
    Scene scene;
public:
    void draw(Scene scene);
};