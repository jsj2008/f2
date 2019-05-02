//
// Created by Dillon Yao on 4/25/19.
//

#include <glm/gtc/type_ptr.hpp>
#include "fluid-renderer.h"
#include "../core/input.h"

namespace pbf {

gl::Buffer FluidRenderer::_screen_vbo;
gl::VertexArray FluidRenderer::_screen_vao;
gl::Program FluidRenderer::points_prog;
gl::Program FluidRenderer::screen_prog;

void FluidRenderer::init() {
    gl::VertexShader points_vert;
    gl::FragmentShader points_frag;

    points_vert.from_file("./shaders/points.vert");
    points_frag.from_file("./shaders/points.frag");

    points_prog.create();
    points_prog.attach_shader(points_vert);
    points_prog.attach_shader(points_frag);
    if (!points_prog.link())
        throw std::runtime_error("Unable to link points program");

    gl::VertexShader screen_vert;
    gl::FragmentShader screen_frag;

    screen_vert.from_file("./shaders/screen.vert");
    screen_frag.from_file("./shaders/screen.frag");

    screen_prog.create();
    screen_prog.attach_shader(screen_vert);
    screen_prog.attach_shader(screen_frag);
    if (!screen_prog.link())
        throw std::runtime_error("Unable to link screen program");

    _screen_vao.create();
    _screen_vbo.create();

    float points[8] = {
            0.f, 0.f,
            1.f, 0.f,
            0.f, 1.f,
            1.f, 1.f,
    };

    _screen_vao.bind();
    _screen_vbo.bind();
    _screen_vao.vertex_attr(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    _screen_vbo.data(8 * sizeof(float), points, GL_STATIC_DRAW);
    _screen_vbo.unbind();
    _screen_vao.unbind();
}

FluidRenderer::FluidRenderer() : _vao(true), _vbo(true), _fbo(true) {
    _vao.bind();
    _vbo.bind();
    _vao.vertex_attr(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    _vbo.unbind();
    _vao.unbind();

    glm::ivec2 vp = Input::poll_viewport();

    glGenTextures(1, &_render_buf);
    glBindTexture(GL_TEXTURE_2D, _render_buf);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, vp.x, vp.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glGenTextures(1, &_depth_buf);
    glBindTexture(GL_TEXTURE_2D, _depth_buf);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, vp.x, vp.y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    _fbo.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _render_buf, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth_buf, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("framebuffer not complete");

    _fbo.unbind();
}

void FluidRenderer::render(Camera &camera) {
    _fbo.bind();

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _vao.bind();
    _vbo.bind();
    _vbo.data(3 * sizeof(float) * _fluid->_num_particles, _fluid->_pos, GL_DYNAMIC_DRAW);
    _vao.vertex_attr(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    points_prog.use();

    glm::ivec2 vp = Input::poll_viewport();

    glUniform1f(points_prog.get_uniform_loc("radius"), 0.025f);
    glUniform2iv(points_prog.get_uniform_loc("vp"), 1, glm::value_ptr(vp));
    glUniformMatrix4fv(points_prog.get_uniform_loc("model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));
    glUniformMatrix4fv(points_prog.get_uniform_loc("view"), 1, GL_FALSE, glm::value_ptr(camera.view()));
    glUniformMatrix4fv(points_prog.get_uniform_loc("proj"), 1, GL_FALSE, glm::value_ptr(camera.proj()));

    glDrawArrays(GL_POINTS, 0, (GLsizei) _fluid->_num_particles);

    _fbo.unbind();
    _vao.unbind();
    _vbo.unbind();

    screen_prog.use();
    _screen_vao.bind();

    glUniform2iv(screen_prog.get_uniform_loc("vp"), 1, glm::value_ptr(vp));
    glUniformMatrix4fv(screen_prog.get_uniform_loc("inv_proj"), 1, GL_FALSE, glm::value_ptr(glm::inverse(camera.proj())));
    glUniform1i(screen_prog.get_uniform_loc("screen_tex"), 0);
    glUniform1i(screen_prog.get_uniform_loc("depth_tex"), 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _render_buf);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _depth_buf);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei) 4);

    _screen_vao.unbind();
}

void FluidRenderer::set_fluid(const std::shared_ptr<Fluid> &fluid) {
    _fluid = fluid;
}

}
