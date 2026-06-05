#pragma once

#include "assetloader.hpp"
#include "entity.hpp"
#include "../../include/json/json.hpp"
#include <unordered_map>

using json = nlohmann::json;

class Scene{
private:

    AssetLoader loader;
    std::vector<Entity> entities;
    std::unordered_map<Entity,std::vector<Transform>,EntityHasher> instances;

    void _load_entity(std::string n,glm::vec3 trans,glm::vec3 rot,glm::vec3 scl,float angle = 0.0f);
    void _map_entities();
    
public:
    void loadScene(const std::string& filepath);

    Scene() = default;

    Scene(const Scene&) = delete;
    Scene operator=(const Scene&) = delete;
};