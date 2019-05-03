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
    ~FluidRenderer();

    void render(Camera &camera) override;
    void set_fluid(const std::shared_ptr<Fluid> &fluid);

    void set_blur_scale(float s) {
        _blur_scale = s;
    }

    void set_blur_depth_fall_off(float fo) {
        _blur_depth_fall_off = fo;
    }

    float &blur_scale() {
        return _blur_scale;
    }

    float &blur_depth_fall_off() {
        return _blur_depth_fall_off;
    }

    static void init();

private:
    gl::Buffer _vbo;
    gl::VertexArray _vao;

    gl::Framebuffer _fbo_0;
    gl::Framebuffer _fbo_1;

    gl::Framebuffer *_fbo;
    gl::Framebuffer *_t_fbo;

    unsigned _depth_buf;
    unsigned _t_depth_buf;

    float _blur_scale;
    float _blur_depth_fall_off;

    std::shared_ptr<Fluid> _fluid;

    gl::Framebuffer *swap_fbo() {
        gl::Framebuffer *temp = _t_fbo;
        _t_fbo = _fbo;
        _fbo = temp;
        return temp;
    }

    static gl::Buffer _screen_vbo;
    static gl::VertexArray _screen_vao;
    static gl::Program points_prog;
    static gl::Program screen_prog;
    static gl::Program depth_prog;
};

}

#endif //F2_DUP_FLUIDRENDERER_H
