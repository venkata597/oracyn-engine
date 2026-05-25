#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Transform{

    enum Mode{
        TRANSLATE,
        ROTATE,
        SCALE
    };

    glm::mat4 matrix = glm::mat4(1.0f);
    
    void apply_transform(Transform::Mode mode,glm::vec3 param,float angle);
};

class Entity{
public:
    unsigned int ID;
    unsigned int assetID;

    Transform transform;

    void makeEntity(unsigned int id,unsigned int assetid);
};