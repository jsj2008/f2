//
// Created by Dillon Yao on 4/25/19.
//

#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"
#include "../core/input.h"

Camera::Camera() :
    _yaw(-M_PI_2), _pitch(0), _roll(0),
    _afovy(0), _aspect(0), _near(0), _far(100),
    _pos(0.f)
{
    glm::ivec2 viewport = Input::poll_viewport();
    set_perspective(60.0f, (float) viewport.x / viewport.y, 0.1f, 100.0f);
    calc_view();
}

glm::vec3 &Camera::position() { return _pos; }
glm::mat4 &Camera::proj() { return _proj; }
glm::mat4 &Camera::view() { return _view; }

void Camera::set_position(glm::vec3 &v) {
    _pos = v;
    calc_view();
}

void Camera::set_orientation(float pitch, float yaw, float roll) {
    _pitch = pitch;
    _yaw = yaw;
    _roll = roll;
    calc_view();
}

void Camera::set_aspect(float aspect) {
    _aspect = aspect;
    calc_proj();
}

void Camera::set_perspective(float fovy, float aspect, float near, float far) {
    _aspect = aspect;
    _afovy = glm::radians(fovy);
    _near = near;
    _far = far;
    calc_proj();
}

void Camera::translate(glm::vec3 &p) {
    _pos += p;
    calc_view();
}

void Camera::rotate(float pitch, float yaw, float roll, bool constrain_pitch) {
    _pitch += pitch;
    if (constrain_pitch) {
        if (_pitch > M_PI_4) _pitch = (float) M_PI_4;
        if (_pitch < -M_PI_4 + 0.001) _pitch = (float) -M_PI_4 + 0.001f;
    }
    _yaw += yaw;
    _roll += roll;
    _yaw = std::fmod(_yaw, (float) M_PI * 2.f);
    calc_view();
}

void Camera::move(CamDirection dir, float dist) {
    switch(dir) {
        case FRONT:
            _pos += dist * front();
            break;
        case BACK:
            _pos -= dist * front();
            break;
        case LEFT:
            _pos -= dist * right();
            break;
        case RIGHT:
            _pos += dist * right();
            break;
        case TOP:
            _pos.y += dist;
            break;
        case BOTTOM:
            _pos.y -= dist;
            break;
    }
    calc_view();
}

glm::vec3 Camera::front() {
    float x = std::cos(_yaw) * std::cos(_pitch);
    float y = std::sin(_pitch);
    float z = std::sin(_yaw) * std::cos(_pitch);
    glm::vec3 front(x, y, z);
    front = glm::normalize(front);
    return front;
}

glm::vec3 Camera::up() {
    return glm::normalize(glm::cross(right(), front()));
}

glm::vec3 Camera::right() {
    return glm::normalize(glm::cross(front(), glm::vec3(0, 1.f, 0)));
}

void Camera::calc_proj() {
    _proj = glm::perspective(_afovy, _aspect, _near, _far);
}

void Camera::calc_view() {
    _view = glm::lookAt(_pos, _pos + front(), up());
}
