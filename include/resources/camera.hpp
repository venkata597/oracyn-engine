#pragma once

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include "inputhandler.hpp"

class Camera{
private:
    glm::vec3 _camera_pos = glm::vec3(0.0f,0.0f,3.0f);
    glm::vec3 _camera_front = glm::vec3(0.0f,0.0f,-1.0f);
    glm::vec3 _camera_up = glm::vec3(0.0f,1.0f,0.0f);

    glm::mat4 view = glm::lookAt(_camera_pos,_camera_pos + _camera_front,_camera_up);

    float fov = 45.0f;
    float aspect = 16.0f/9.0f;
    float near_plane = 0.1f;
    float far_plane = 100.0f;

    glm::mat4 projection = glm::perspective(fov,aspect,near_plane,far_plane);

private:
    void _update_camera();
public:
    void update();
    const glm::mat4& getViewMatrix() {return view;}
    const glm::mat4& getProjectionMatrix() {return projection;}
};
