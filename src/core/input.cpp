//
// Created by Dillon Yao on 4/25/19.
//

#include "input.h"

Input Input::_self;

bool Input::init(GLFWwindow *window) {
    get()->_window = window;

    glfwSetKeyCallback(get()->_window, on_key_event);
    glfwSetCursorPosCallback(get()->_window, on_cursor_event);
    glfwSetMouseButtonCallback(get()->_window, on_mouse_event);
    glfwSetScrollCallback(get()->_window, on_scroll_event);
    glfwSetFramebufferSizeCallback(get()->_window, on_resize_event);

    get()->_selection = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    return true;
}

void Input::on_key_event(GLFWwindow *window, int key, int scancode, int action, int mode) {
    for (auto cb : get()->_key_cbs) cb(key, scancode, action);
}

void Input::on_cursor_event(GLFWwindow *window, double x, double y) {
    for (auto cb : get()->_cursor_cbs) cb(x, y);
}

void Input::on_mouse_event(GLFWwindow *window, int button, int action, int mods) {
    for (auto cb : get()->_mouse_cbs) cb(button, action);
}

void Input::on_scroll_event(GLFWwindow *window, double dx, double dy) {
    for (auto cb : get()->_scroll_cbs) cb(dx, dy);
}

void Input::on_resize_event(GLFWwindow *window, int width, int height) {
    for (auto cb : get()->_resize_cbs) cb(width, height);
}
