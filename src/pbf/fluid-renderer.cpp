//
// Created by Dillon Yao on 4/25/19.
//

#include <glm/gtc/type_ptr.hpp>
#include "fluid-renderer.h"
#include "../core/input.h"

namespace pbf {

gl::Program FluidRenderer::points_prog;

void FluidRenderer::init() {
    gl::VertexShader points_vert;
    gl::FragmentShader points_frag;

    points_vert.from_file("./shaders/points.vert");
    points_frag.from_file("./shaders/points.frag");

    points_prog.create();
    points_prog.attach_shader(points_vert);
    points_prog.attach_shader(points_frag);
    if (!points_prog.link())
        throw std::runtime_error("Unable to link program");
}

FluidRenderer::FluidRenderer() : _vao(true), _vbo(true) {
    _vao.bind();
    _vbo.bind();
    _vao.vertex_attr(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    _vbo.unbind();
    _vao.unbind();
}

void FluidRenderer::render(Camera &camera) {
    _vbo.bind();
    _vbo.data(3 * sizeof(float) * _fluid->_num_particles, &_fluid->_pos.front(), GL_DYNAMIC_DRAW);

    points_prog.use();

    glm::ivec2 vp = Input::poll_viewport();

    glUniform1f(points_prog.get_uniform_loc("radius"), 0.05f);
    glUniform2iv(points_prog.get_uniform_loc("vp"), 1, glm::value_ptr(vp));
    glUniform3fv(points_prog.get_uniform_loc("eye"), 1, glm::value_ptr(camera.position()));
    glUniformMatrix4fv(points_prog.get_uniform_loc("model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));
    glUniformMatrix4fv(points_prog.get_uniform_loc("view"), 1, GL_FALSE, glm::value_ptr(camera.view()));
    glUniformMatrix4fv(points_prog.get_uniform_loc("proj"), 1, GL_FALSE, glm::value_ptr(camera.proj()));

    _vao.bind();
    glDrawArrays(GL_POINTS, 0, (GLsizei) _fluid->_num_particles);
}

void FluidRenderer::set_fluid(Fluid &fluid) {
    _fluid = &fluid;
}

}
