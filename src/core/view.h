//
// Created by Dillon Yao on 4/25/19.
//

#ifndef F2_DUP_VIEW_H
#define F2_DUP_VIEW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class View {
public:
    void destroy();
    bool init();
    void swap_buffers();
    bool should_close();

    GLFWwindow *window() {
        return _window;
    }

private:
    const char *_title;
    int _height, _width;

    GLFWwindow *_window;
};


#endif //F2_DUP_VIEW_H
