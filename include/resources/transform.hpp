#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform{

    glm::vec3 translate = glm::vec3(1.0f);
    glm::quat rotation{1.0f,0.0f,0.0f,0.0f};
    glm::vec3 scale = glm::vec3(1.0f);

    glm::mat4 transform = glm::mat4(1.0f);
    bool hasMatrix = false;

    glm::mat4 getTransform() const;
};
