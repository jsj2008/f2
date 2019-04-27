//
// Created by Dillon Yao on 4/25/19.
//

#include "sim.h"
#include "input.h"

Sim::Sim() :
    _last_mouse(0.f), _cam_trajectory(0.f), _cam_inertia(0.f)
{
    attach_input_cbs();

    _fluid.spawn(glm::vec3(0.f));
    _fluid.spawn(glm::vec3(0.25f, 0.25f, 0.f));
    _fluid.spawn(glm::vec3(-0.25f, 0.25f, 0.f));

    _renderer = new pbf::FluidRenderer();
    _renderer->set_fluid(_fluid);
    _scene.add_thing(_renderer);

    glm::vec3 p(0.f, 0.f, 2.f);
    _camera.set_position(p);
}

void Sim::update() {
    handle_input();
    _fluid.update(1.f / 60);
}

void Sim::render() {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_PROGRAM_POINT_SIZE);

    glClearColor(0.9f, 0.9f, 0.9f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _scene.render(_camera);
}

void Sim::attach_input_cbs() {
    Input::on_resize([this](int width, int height) {
        _camera.set_aspect((float) width / height);
    });

    Input::on_mouse([this](int button, int action) {
        if (button != GLFW_MOUSE_BUTTON_LEFT)
            return;

        if (action == GLFW_PRESS) {
            Input::set_cursor_type(GLFW_HAND_CURSOR);
            _last_mouse = Input::poll_cursor();
        } else {
            Input::set_cursor_type(-1);
        }
    });

    Input::on_cursor([this](float x, float y) {
        if (Input::poll_mouse(GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS)
            return;

        glm::vec2 p(x, y);
        glm::vec2 dp = 0.002f * (p - _last_mouse);
        _camera.rotate(-dp.y, dp.x, 0, true);
        _last_mouse = p;
    });

    Input::on_key([this](int key, int scancode, int action) {
        if (action != GLFW_PRESS)
            return;

        switch (key) {
            case GLFW_KEY_P:
                break;
            case GLFW_KEY_S:
                break;
            case GLFW_KEY_R:
                break;
            default:
                break;
        }
    });
}

void Sim::handle_input() {
    bool movement = false;

    glm::vec3 orig_pos = _camera.position();

    if (Input::poll_key(GLFW_KEY_W) == GLFW_PRESS) {
        _camera.move(FRONT, 0.02);
        movement = true;
    }

    if (Input::poll_key(GLFW_KEY_S) == GLFW_PRESS) {
        _camera.move(BACK, 0.02);
        movement = true;
    }

    if (Input::poll_key(GLFW_KEY_A) == GLFW_PRESS) {
        _camera.move(LEFT, 0.02);
        movement = true;
    }

    if (Input::poll_key(GLFW_KEY_D) == GLFW_PRESS) {
        _camera.move(RIGHT, 0.02);
        movement = true;
    }

    if (Input::poll_key(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        _camera.move(BOTTOM, 0.02);
        movement = true;
    }

    if (Input::poll_key(GLFW_KEY_SPACE) == GLFW_PRESS) {
        _camera.move(TOP, 0.02);
        movement = true;
    }

    glm::vec3 dp;
    if (movement) {

        dp = glm::normalize(_camera.position() - orig_pos);
        _cam_trajectory = glm::normalize(0.8f * dp + 0.2f * _cam_trajectory);

        if (_cam_inertia < 1.f)
            _cam_inertia += 0.1f;

    } else if (_cam_inertia > 0.f) {
        dp = _cam_inertia * 0.02f * _cam_trajectory;
        _camera.translate(dp);

        if (_cam_inertia > 0.f)
            _cam_inertia -= 0.02f;
        else
            _cam_inertia = 0.f;

    }
}