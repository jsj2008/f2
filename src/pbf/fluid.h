//
// Created by Dillon Yao on 4/25/19.
//

#ifndef F2_DUP_FLUID_H
#define F2_DUP_FLUID_H

#include <vector>
#include <glm/vec3.hpp>
#include "../core/accel.h"

namespace pbf {

struct FluidParams {
    unsigned char solver_iterations = 2;
    float dt = 0.0167;
    float gravity = -9.8f;
    float kernel_h = 0.1f;
    float rest_density = 7000.f;
    float desity_eps = 800.f;
    float s_corr_k = 0.0005f;
    float s_corr_dq_multiplier = 0.f;
    float s_corr_n = 4;
    float vort_eps = 0.00017f;
    float visc_c = 0.000001f;

    float grid_res = 0.25f;
    float x_min = 0.f;
    float x_max = 4.f;
    float y_min = 0.f;
    float y_max = 4.f;
    float z_min = 0.f;
    float z_max = 2.f;
};

struct DeviceParams {
    FluidParams fluid_params;
    unsigned dim_x = 0;
    unsigned dim_y = 0;
    unsigned dim_z = 0;
};

class Fluid {
public:
    Fluid();
    Fluid(FluidParams &params);

    void spawn(glm::vec3 p);
    void spawn_cube(glm::vec3 ori, float length, float density=10);

    void update(float dt);
    void clear();

    FluidParams &params();
    void set_dirty();

    static void init();

private:
    DeviceParams _params;

    size_t _num_particles;

    std::vector<glm::vec3> _pos;

    cl::Buffer _cl_pos;
    cl::Buffer _cl_vel_dp;
    cl::Buffer _cl_pred_pos;
    cl::Buffer _cl_bin_of;
    cl::Buffer _cl_lambda;

    cl::Buffer _cl_ind_pos;
    cl::Buffer _cl_ind_vel;
    cl::Buffer _cl_ind_pred_pos;
    cl::Buffer _cl_ind_bin_of;

    cl::Buffer _cl_bin_offset;
    cl::Buffer _cl_bin_counts;
    cl::Buffer _cl_bin_starts;

    bool _should_resize;
    bool _params_dirty;

    void init_domain();
    void init_cl_bufs();
    void bind_params();

    void predict_pos();
    void build_grid();
    void calc_lam();
    void calc_dp();
    void update_pred_pos();
    void update_vel();
    void update_pos();

    static cl::Program _cl_prog;
    static cl::Kernel _k_pred_pos;

    static cl::Kernel _k_count_bins;
    static cl::Kernel _k_prefix_sum;
    static cl::Kernel _k_reindex_particles;

    static cl::Kernel _k_calc_lam;
    static cl::Kernel _k_calc_dp;
    static cl::Kernel _k_update_pred_pos;
    static cl::Kernel _k_update_vel;

    friend class FluidRenderer;
};

}

#endif //F2_DUP_FLUID_H
