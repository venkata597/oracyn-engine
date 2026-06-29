#pragma once

#include "assetloader.hpp"
#include "entity.hpp"
#include "../../include/json/json.hpp"
#include <glm/fwd.hpp>
#include <unordered_map>

using json = nlohmann::json;

using SceneMap = std::unordered_map<Entity,std::vector<Transform>,EntityHasher>;

class Scene{
    friend class RenderContext;
private:

    AssetLoader loader;
    std::vector<Entity> entities;
    SceneMap instances;

    void _load_entity(std::string n,glm::vec3 trans,glm::quat rot,glm::vec3 scl);
    void _map_entities();

public:
    void loadScene(const std::string& filepath);
    const SceneMap& getScene();

    Scene() = default;

    Scene(const Scene&) = delete;
    Scene operator=(const Scene&) = delete;
};
