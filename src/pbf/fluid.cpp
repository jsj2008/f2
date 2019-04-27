//
// Created by Dillon Yao on 4/25/19.
//

#include "fluid.h"

namespace pbf {

Fluid::Fluid() : _num_particles(0) {
    _bin_counts.resize(1);
    _bin_starts.resize(1);
}

void Fluid::spawn(glm::vec3 p) {
    _pos.push_back(p);
    _vel.emplace_back(0.f);
    _pred_pos.emplace_back(0.f);
    _temp.emplace_back(0.f);
    _den.push_back(0.f);
    _lam.push_back(0.f);
    _bin_of.emplace_back();

    ++_num_particles;
}

void Fluid::spawn_cube(glm::vec3 ori, float length, float density) {
    float stride = 1.f / density;

    for (float x = 0; x <= length; x += stride) {
        for (float y = 0; y <= length; y += stride) {
            for (float z = 0; z <= length; z += stride) {
                spawn(ori + glm::vec3(x, y, z));
            }
        }
    }
}

void Fluid::update(float dt) {
    predict_pos();
    build_grid();
    calc_lam();
    calc_dp();
    update_pred_pos();
    update_vel();
    update_pos();
}

void Fluid::clear() {
    _pos.clear();
    _vel.clear();
    _pred_pos.clear();
    _temp.clear();
    _den.clear();
    _lam.clear();
    _num_particles = 0;
}

void Fluid::predict_pos() {

}

void Fluid::build_grid() {

}

void Fluid::calc_lam() {

}

void Fluid::calc_dp() {

}

void Fluid::update_pred_pos() {

}

void Fluid::update_vel() {

}

void Fluid::update_pos() {

}

}