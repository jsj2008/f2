//
// Created by Dillon Yao on 4/26/19.
//

#ifndef F2_DUP_PLANE_H
#define F2_DUP_PLANE_H


#include <glm/gtx/quaternion.hpp>
#include "../thing.h"
#include "../../gl/program.h"
#include "../../gl/buffer.h"

class Plane : public Thing {
public:
    Plane();
    Plane(glm::vec3 pos, glm::vec3 orientation, glm::vec2 dim);

    void render(Camera &camera) override;

    void set_position(glm::vec3 &pos);
    void set_orientation(glm::vec3 &euler);
    void set_dimensions(glm::vec2 &dim);
    void set_width(float width);
    void set_height(float height);

    void translate(glm::vec3 &dp);

    static void init();

private:
    glm::vec2 _dim;
    glm::vec3 _pos;
    glm::quat _orientation;

    static gl::Program prog;
    static gl::Buffer vbo;
    static gl::VertexArray vao;
};


#endif //F2_DUP_PLANE_H
