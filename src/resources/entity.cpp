#include "../../include/resources/entity.hpp"

void Transform::apply_transform(Transform::Mode mode,glm::vec3 param,float angle){
    switch(mode){
        case Transform::Mode::TRANSLATE:
            matrix = glm::translate(matrix,param);
            break;
        case Transform::Mode::ROTATE:
            matrix = glm::rotate(matrix,angle,param);
            break;
        case Transform::Mode::SCALE:
            matrix = glm::scale(matrix,param);
            break;
    }
}


void Entity::makeEntity(unsigned int id,unsigned int assetid){
    this->ID = id;
    this->assetID = assetid;
}