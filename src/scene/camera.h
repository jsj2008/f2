//
// Created by Dillon Yao on 4/25/19.
//

#ifndef F2_DUP_CAMERA_H
#define F2_DUP_CAMERA_H

#include <glm/glm.hpp>

enum CamDirection {
    FRONT,
    BACK,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
};


class Camera {
public:
    Camera();

    glm::vec3 &position();
    glm::mat4 &proj();
    glm::mat4 &view();

    void set_position(glm::vec3 &v);
    void set_orientation(float pitch, float yaw, float roll);

    void set_aspect(float aspect);
    void set_perspective(float fovy, float aspect, float near, float far);

    void translate(glm::vec3 &p);
    void rotate(float pitch, float yaw, float roll, bool constrain_pitch=false);
    void move(CamDirection dir, float dist);

private:
    glm::vec3 _pos;
    glm::mat4 _proj;
    glm::mat4 _view;

    float _aspect, _afovy, _near, _far;
    float _pitch, _yaw, _roll;

    glm::vec3 front();
    glm::vec3 up();
    glm::vec3 right();

    void calc_proj();
    void calc_view();
};


#endif //F2_DUP_CAMERA_H
