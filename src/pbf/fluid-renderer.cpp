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
gl::Program FluidRenderer::depth_prog;

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

    gl::VertexShader depth_vert;
    gl::FragmentShader depth_frag;

    depth_vert.from_file("./shaders/bilateral.vert");
    depth_frag.from_file("./shaders/bilateral.frag");

    depth_prog.create();
    depth_prog.attach_shader(depth_vert);
    depth_prog.attach_shader(depth_frag);
    if (!depth_prog.link())
        throw std::runtime_error("Unable to link depth program");

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

FluidRenderer::FluidRenderer() :
        _vao(true), _vbo(true), _fbo_0(true), _fbo_1(true),
        _blur_scale(64.f), _blur_depth_fall_off(64.f) {

    _vao.bind();
    _vbo.bind();
    _vao.vertex_attr(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    _vbo.unbind();
    _vao.unbind();

    glm::ivec2 vp = Input::poll_viewport();

    glGenTextures(1, &_depth_buf);
    glBindTexture(GL_TEXTURE_2D, _depth_buf);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, vp.x, vp.y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glGenTextures(1, &_t_depth_buf);
    glBindTexture(GL_TEXTURE_2D, _t_depth_buf);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, vp.x, vp.y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    _fbo = &_fbo_0;
    _t_fbo = &_fbo_1;

    Input::on_resize([&](int width, int height) {
        glBindTexture(GL_TEXTURE_2D, _depth_buf);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);

        glBindTexture(GL_TEXTURE_2D, _t_depth_buf);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);

        glBindTexture(GL_TEXTURE_2D, 0);
    });
}

FluidRenderer::~FluidRenderer() {
    glDeleteTextures(1, &_depth_buf);
    glDeleteTextures(1, &_t_depth_buf);
}

void FluidRenderer::render(Camera &camera) {

    gl::Framebuffer *fbo = swap_fbo();

    // Render Particle Depths
    fbo->bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth_buf, 0);

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_DEPTH_BUFFER_BIT);

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

    _vao.unbind();
    _vbo.unbind();

    // Apply Bilateral Filter Approximation to depths
    fbo = swap_fbo();
    fbo->bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _t_depth_buf, 0);
    glClear(GL_DEPTH_BUFFER_BIT);

    depth_prog.use();
    _screen_vao.bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _depth_buf);

    glUniform1i(depth_prog.get_uniform_loc("depth_tex"), 0);
    glUniform1i(depth_prog.get_uniform_loc("filter_radius"), 16);
    glUniform1f(depth_prog.get_uniform_loc("blur_scale"), _blur_scale);
    glUniform1f(depth_prog.get_uniform_loc("blur_depth_fall_off"), _blur_depth_fall_off);
    glUniform2f(depth_prog.get_uniform_loc("filter_dir"), 1.f / vp.x, 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei) 4);

    fbo = swap_fbo();
    fbo->bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth_buf, 0);
    glClear(GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _t_depth_buf);

    glUniform1i(depth_prog.get_uniform_loc("depth_tex"), 0);
    glUniform1i(depth_prog.get_uniform_loc("filter_radius"), 16);
    glUniform1f(depth_prog.get_uniform_loc("blur_scale"), _blur_scale);
    glUniform1f(depth_prog.get_uniform_loc("blur_depth_fall_off"), _blur_depth_fall_off);
    glUniform2f(depth_prog.get_uniform_loc("filter_step"), 0, 1.f / vp.y);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei) 4);

    fbo->unbind();

    // Render screen space fluid
    screen_prog.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _t_depth_buf);

    glUniform2iv(screen_prog.get_uniform_loc("vp"), 1, glm::value_ptr(vp));
    glUniformMatrix4fv(screen_prog.get_uniform_loc("inv_proj"), 1, GL_FALSE, glm::value_ptr(glm::inverse(camera.proj())));
    glUniform1i(screen_prog.get_uniform_loc("depth_tex"), 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei) 4);

    _screen_vao.unbind();
}

void FluidRenderer::set_fluid(const std::shared_ptr<Fluid> &fluid) {
    _fluid = fluid;
}

}
