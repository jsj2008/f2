//
// Created by Dillon Yao on 4/25/19.
//

#include "app.h"

bool App::init() {
    if (!_view.init())
        return false;

    pbf::FluidRenderer::init();

    _sim = new Sim;
    return true;
}

void App::run() {
    double acc = 0;
    double sys_curr, sys_last, dt;
    sys_last = glfwGetTime();

    unsigned int num_updates;
    _running = true;

    while (_running) {
        sys_curr = glfwGetTime();
        dt = sys_curr - sys_last;
        acc += dt;
        sys_last = sys_curr;

        glfwPollEvents();
        num_updates = 0;
        while (num_updates < 10 && acc > 0.0083) {
            acc -= 0.0083;
            update();
            ++num_updates;
        }

        render();

        if (_view.should_close()) {
            _running = false;
        }
    }
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