#include "../../include/resources/camera.hpp"
#include <SDL2/SDL_scancode.h>
#include <cmath>
#include <glm/common.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>

void Camera::_update_camera(float delta_time){
    yaw += input.mouse_delta_x * sensitivity;
    pitch -= input.mouse_delta_y * sensitivity;

    pitch = glm::clamp(pitch,-89.0f,89.0f);
    glm::vec3 new_front;
    new_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    new_front.y = sin(glm::radians(pitch));
    new_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    _camera_front = glm::normalize(new_front);

    float cameraSpeed = 0.5f * delta_time;
    if(input.keys[SDL_SCANCODE_W] == true){
        _camera_pos += cameraSpeed * _camera_front;
    }
    if(input.keys[SDL_SCANCODE_S] == true){
        _camera_pos -= cameraSpeed * _camera_front;
    }
    if(input.keys[SDL_SCANCODE_A] == true){
        _camera_pos -= glm::normalize(glm::cross(_camera_front,_camera_up)) * cameraSpeed;
    }
    if(input.keys[SDL_SCANCODE_D] == true){
        _camera_pos += glm::normalize(glm::cross(_camera_front,_camera_up)) * cameraSpeed;
    }

    view = glm::lookAt(_camera_pos,_camera_pos + _camera_front,_camera_up);
}


void Camera::update(float delta_time){
    _update_camera(delta_time);
}
