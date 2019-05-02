//
// Created by Dillon Yao on 4/25/19.
//

#ifndef F2_DUP_FLUIDRENDERER_H
#define F2_DUP_FLUIDRENDERER_H

#include "fluid.h"
#include "../gl/buffer.h"
#include "../gl/program.h"
#include "../scene/thing.h"
#include "../gl/framebuffer.h"

namespace pbf {

class FluidRenderer : public Thing {
public:
    FluidRenderer();

    void render(Camera &camera) override;
    void set_fluid(const std::shared_ptr<Fluid> &fluid);

    static void init();

private:
    gl::Buffer _vbo;
    gl::VertexArray _vao;
    gl::Framebuffer _fbo;

    unsigned _render_buf;
    unsigned _depth_buf;

    std::shared_ptr<Fluid> _fluid;

    static gl::Buffer _screen_vbo;
    static gl::VertexArray _screen_vao;
    static gl::Program points_prog;
    static gl::Program screen_prog;
};

}

#endif //F2_DUP_FLUIDRENDERER_H
