#include "../../include/resources/entity.hpp"

unsigned int Entity::_id = 0;

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


void Entity::makeEntity(std::string n,unsigned int assetid,glm::vec3 trans,glm::vec3 rotate,glm::vec3 scale,float angle){
    this->name = n;
    this->ID = _id++;
    this->assetID = assetid;
    if(trans!=glm::vec3(0.0f)){
        this->transform.apply_transform(Transform::TRANSLATE,trans);
    }
    if(rotate!=glm::vec3(0.0f)){
        this->transform.apply_transform(Transform::ROTATE,rotate,angle);
    }
    if(scale!=glm::vec3(0.0f)){
        this->transform.apply_transform(Transform::SCALE,scale);
    }
}