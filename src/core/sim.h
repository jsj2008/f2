//
// Created by Dillon Yao on 4/25/19.
//

#ifndef F2_DUP_SIM_H
#define F2_DUP_SIM_H

#include "../pbf/fluid.h"
#include "../pbf/fluid-renderer.h"
#include "../scene/scene.h"

class Sim {
public:
    Sim();

    void update();
    void render();

private:
    Scene _scene;
    Camera _camera;

    pbf::Fluid _fluid;
    pbf::FluidRenderer *_renderer;

    glm::vec2 _last_mouse;
    glm::vec3 _cam_trajectory;
    float _cam_inertia;

    void attach_input_cbs();
    void handle_input();
};


#endif //F2_DUP_SIM_H
