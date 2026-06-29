#include "../../include/resources/transform.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

glm::mat4 Transform::getTransform() const {
    if(hasMatrix){
        return transform;
    }
    else{
        return glm::translate(glm::mat4(1.0f),translate) *
            glm::mat4_cast(rotation) *
            glm::scale(glm::mat4(1.0f),scale);
    }
}
