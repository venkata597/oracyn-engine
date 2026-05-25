#pragma once

#include "assetloader.hpp"
#include "entity.hpp"

class Scene{
private:

    AssetLoader loader;

    std::vector<Entity> entities;
public:
    void loadScene(const std::string& filepath);
};