#include "../../include/resources/camera.hpp"
#include <SDL_keycode.h>
#include <glm/geometric.hpp>

void Camera::_update_camera(){
    float cameraSpeed = 1.5f;
    if(input.keys[SDLK_w] == true){
        _camera_pos += cameraSpeed * _camera_front;
    }
    else if(input.keys[SDLK_s] == true){
        _camera_pos -= cameraSpeed * _camera_front;
    }
    else if(input.keys[SDLK_a] == true){
        _camera_pos -= glm::normalize(glm::cross(_camera_front,_camera_up)) * cameraSpeed;
    }
    else if(input.keys[SDLK_d] == true){
        _camera_pos += glm::normalize(glm::cross(_camera_front,_camera_up)) * cameraSpeed;
    }
}


void Camera::update(){
    _update_camera();
}
