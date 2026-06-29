#include "../../include/resources/entity.hpp"
#include <glm/gtc/quaternion.hpp>

unsigned int Entity::_id = 0;

void Entity::makeEntity(std::string n,unsigned int assetid,glm::vec3 trans,glm::quat rotate,glm::vec3 scale){
    this->name = n;
    this->ID = _id++;
    this->assetID = assetid;
    if(trans!=glm::vec3(0.0f)){
        this->transform.translate = trans;
    }
    if(rotate!=glm::quat(0.0f,0.0f,0.0f,0.0f)){
        this->transform.rotation = rotate;
    }
    if(scale!=glm::vec3(0.0f)){
        this->transform.scale = scale;
    }
}
