//
// Created by Dillon Yao on 4/25/19.
//

#include <iostream>
#include "view.h"
#include "input.h"

void View::destroy() {
    glfwDestroyWindow(_window);
    glfwTerminate();
}

bool View::init() {
    if (!glfwInit()) {
        std::cerr << "Initialization of GLFW failed" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    _width = 800;
    _height = 600;
    _title = "f2";

    _window = glfwCreateWindow(_width, _height, _title, nullptr, nullptr);
    if (!_window) {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(_window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glfwGetFramebufferSize(_window, &_width, &_height);
    glViewport(0, 0, _width, _height);

    Input::init(_window);
    Input::on_resize([this](int width, int height) {
        this->_width = width;
        this->_height = height;
        glViewport(0, 0, width, height);
    });

    return true;
}

void View::swap_buffers() {
    glfwSwapBuffers(_window);
}

bool View::should_close() {
    return (bool) glfwWindowShouldClose(_window);
}
