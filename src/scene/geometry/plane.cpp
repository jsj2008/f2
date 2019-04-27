//
// Created by Dillon Yao on 4/26/19.
//

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include "plane.h"

gl::Program Plane::prog;
gl::Buffer Plane::vbo;
gl::VertexArray Plane::vao;

void Plane::init() {
    gl::VertexShader v_shader;
    gl::FragmentShader f_shader;

    v_shader.from_file("./shaders/plane.vert");
    f_shader.from_file("./shaders/plane.frag");

    prog.create();
    prog.attach_shader(v_shader);
    prog.attach_shader(f_shader);
    if (!prog.link())
        throw std::runtime_error("Unable to link program");

    vbo.create();
    vao.create();

    float points[12] = {
            1.f, 0.f, 1.f,
            1.f, 0.f, 0.f,
            0.f, 0.f, 1.0f,
            0.f, 0.f, 0.f
    };

    vao.bind();
    vbo.bind();
    vbo.data(12 * sizeof(float), points, GL_STATIC_DRAW);
    vao.vertex_attr(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    vbo.unbind();
    vao.unbind();
}

Plane::Plane() :
    _pos(0.f),
    _orientation(1.f, 0.f, 0.f, 0.f),
    _dim(1.f) { }

Plane::Plane(glm::vec3 pos, glm::vec3 orientation, glm::vec2 dim) :
    _pos(pos),
    _orientation(1.f, 0.f, 0.f, 0.f),
    _dim(dim) {
    set_orientation(orientation);
}

void Plane::render(Camera &camera) {
    prog.use();

    glm::mat4 model(1.f);
    model = glm::translate(model, _pos) * glm::mat4_cast(_orientation);
    model = glm::scale(model, glm::vec3(_dim.x, 1.f, _dim.y));

    glUniform3fv(prog.get_uniform_loc("eye"), 1, glm::value_ptr(camera.position()));
    glUniformMatrix4fv(prog.get_uniform_loc("model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(prog.get_uniform_loc("view"), 1, GL_FALSE, glm::value_ptr(camera.view()));
    glUniformMatrix4fv(prog.get_uniform_loc("proj"), 1, GL_FALSE, glm::value_ptr(camera.proj()));

    vao.bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei) 4);
}

void Plane::set_position(glm::vec3 &pos) {
    _pos = pos;
}

void Plane::set_orientation(glm::vec3 &euler) {
    _orientation = glm::quat(euler);
}

void Plane::set_dimensions(glm::vec2 &dim) {
    _dim = dim;
}

void Plane::set_width(float width) {
    _dim.x = width;
}

void Plane::set_height(float height) {
    _dim.y = height;
}

void Plane::translate(glm::vec3 &dp) {
    _pos = _pos + dp;
}
