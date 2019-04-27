//
// Created by Dillon Yao on 4/26/19.
//

#ifndef F2_DUP_PLANE_H
#define F2_DUP_PLANE_H


#include "../thing.h"
#include "../../gl/program.h"

class Plane : public Thing {
public:
    Plane() = default;
    Plane(glm::vec3 pos, glm::vec3 n);

    void render(Camera &camera);

    void set_position(glm::vec3 &pos);
    void set_normal(glm::vec3 &n);
    void translate(glm::vec3 &dp);

    static void init();

private:
    glm::vec3 _pos;
    glm::vec3 _normal;

    glm::mat4 _model;

    static gl::Program *prog;
};


#endif //F2_DUP_PLANE_H
