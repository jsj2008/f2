//
// Created by Dillon Yao on 4/25/19.
//

#include "app.h"
#include "accel.h"
#include "../scene/geometry/plane.h"

bool App::init() {
    if (!_view.init())
        return false;

    CLContextManager::init();
    pbf::Fluid::init();

    Plane::init();
    pbf::FluidRenderer::init();

    _sim = std::make_unique<Sim>();
    _sim->init();
    return true;
}

void App::run() {
    double acc = 0;
    double sys_curr, sys_last, dt;
    sys_last = glfwGetTime();

    unsigned num_updates;
    _running = true;

    while (_running) {
        sys_curr = glfwGetTime();
        dt = sys_curr - sys_last;
        acc += dt;
        sys_last = sys_curr;

        glfwPollEvents();
        num_updates = 0;
        while (num_updates < 1 && acc > 0.0167) {
            acc -= 0.0167;
            update();
            ++num_updates;
        }

        render();

        if (_view.should_close()) {
            _running = false;
        }
    }

    Input::destroy();
    _view.destroy();
}

void App::terminate() {
    _running = false;
}

void App::update() {
    _sim->update();
}

void App::render() {
    _sim->render();
    _view.swap_buffers();
}