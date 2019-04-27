//
// Created by Dillon Yao on 4/25/19.
//

#ifndef F2_DUP_INPUT_H
#define F2_DUP_INPUT_H

#include <vector>
#include <glm/glm.hpp>
#include <functional>
#include "view.h"

class Input {
public:
    static bool init(GLFWwindow *window);

    static void on_key(const std::function<void(int, int, int)> &cb) {
        get()->_key_cbs.push_back(cb);
    }

    static void on_cursor(const std::function<void(double, double)> &cb) {
        get()->_cursor_cbs.push_back(cb);
    }

    static void on_mouse(const std::function<void(int, int)> &cb) {
        get()->_mouse_cbs.push_back(cb);
    }

    static void on_scroll(const std::function<void(double, double)> &cb) {
        get()->_scroll_cbs.push_back(cb);
    }

    static void on_resize(const std::function<void(int, int)> &cb) {
        get()->_resize_cbs.push_back(cb);
    }

    static int poll_key(int key) {
        return glfwGetKey(get()->_window, key);
    }

    static int poll_mouse(int button) {
        return glfwGetMouseButton(get()->_window, button);
    }

    static glm::dvec2 poll_cursor() {
        glm::dvec2 cursor;
        glfwGetCursorPos(get()->_window, &cursor.x, &cursor.y);
        return cursor;
    }

    static glm::ivec2 poll_viewport() {
        glm::ivec2 vp;
        glfwGetFramebufferSize(get()->_window, &vp.x, &vp.y);
        return vp;
    }

    static void set_cursor_type(int cursor) {
        GLFWcursor *c = nullptr;
        switch (cursor) {
            case GLFW_HAND_CURSOR:
                c = get()->_selection;
                break;
            default:
                break;
        }
        glfwSetCursor(get()->_window, c);
    }

private:
    GLFWwindow *_window;
    GLFWcursor *_selection;

    std::vector<std::function<void(int, int, int)>>     _key_cbs;
    std::vector<std::function<void(double, double)>>    _cursor_cbs;
    std::vector<std::function<void (int, int)>>         _mouse_cbs;
    std::vector<std::function<void(double, double)>>    _scroll_cbs;
    std::vector<std::function<void(int, int)>>          _resize_cbs;

    Input() {}

    static Input *get() { return &_self; }

    static void on_key_event(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void on_cursor_event(GLFWwindow *window, double x, double y);
    static void on_mouse_event(GLFWwindow *window, int button, int action, int mods);
    static void on_scroll_event(GLFWwindow *window, double dx, double dy);
    static void on_resize_event(GLFWwindow *window, int width, int height);

    static Input _self;
};


#endif //F2_DUP_INPUT_H
