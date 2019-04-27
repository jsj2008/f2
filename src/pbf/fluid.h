//
// Created by Dillon Yao on 4/25/19.
//

#ifndef F2_DUP_FLUID_H
#define F2_DUP_FLUID_H

#include <vector>
#include <glm/vec3.hpp>

namespace pbf {

class Fluid {
public:
    Fluid();

    void spawn(glm::vec3 p);
    void spawn(glm::vec3 &p);
    void update(float dt);
    void clear();

private:
    std::vector<glm::vec3> _pos;
    std::vector<glm::vec3> _pred_pos;
    std::vector<glm::vec3> _vel;
    std::vector<glm::vec3> _temp;
    std::vector<float> _den;
    std::vector<float> _lam;

    std::vector<int> _bin_of;
    std::vector<int> _bin_starts;
    std::vector<int> _bin_counts;

    unsigned int _num_particles;

    void predict_pos();
    void build_grid();
    void calc_lam();
    void calc_dp();
    void update_pred_pos();
    void update_vel();
    void update_pos();

    friend class FluidRenderer;
};

}

#endif //F2_DUP_FLUID_H
