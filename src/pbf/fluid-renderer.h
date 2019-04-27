//
// Created by Dillon Yao on 4/25/19.
//

#ifndef F2_DUP_FLUIDRENDERER_H
#define F2_DUP_FLUIDRENDERER_H

#include "fluid.h"
#include "../gl/buffer.h"
#include "../gl/program.h"
#include "../scene/thing.h"

namespace pbf {

class FluidRenderer : public Thing {
public:
    FluidRenderer();

    void render(Camera &camera) override;
    void set_fluid(Fluid &fluid);

    static void init();

private:
    gl::Buffer _vbo;
    gl::VertexArray _vao;

    Fluid *_fluid;

    static gl::Program points_prog;
};

}

#endif //F2_DUP_FLUIDRENDERER_H
