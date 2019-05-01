//
// Created by Dillon Yao on 4/25/19.
//

#ifndef F2_DUP_FLUID_H
#define F2_DUP_FLUID_H

#include <vector>
#include <glm/vec3.hpp>
#include "../core/accel.h"

namespace pbf {

static const size_t INIT_POS_BUF_CAP = 16384;

struct FluidParams {
    unsigned char solver_iterations = 2;
    float dt = 0.0083f;
    float gravity = -9.8f;
    float kernel_h = 0.1f;
    float rest_density = 8000.f;
    float desity_eps = 800.f;
    float s_corr_k = 0.0001f;
    float s_corr_dq_multiplier = 0.01f;
    float s_corr_n = 4.f;
    float vort_eps = 0.0001f;
    float visc_c = 0.00001f;

    float grid_res = 0.1f;
    float x_min = 0.f;
    float x_max = 2.f;
    float y_min = 0.f;
    float y_max = 2.f;
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

    inline void spawn(glm::vec3 p);
    void spawn_cube(glm::vec3 ori, float length, float density=10);

    void update();
    void clear();

    FluidParams &params() {
        return _params.fluid_params;
    }

    void set_dirty() {
        _params_dirty = true;
    }

    static void init();

private:
    DeviceParams _params;

    size_t _num_particles;
    size_t _buf_size;

    float *_pos;

    cl::Buffer _cl_int_pos;
    cl::Buffer _cl_int_vel;
    cl::Buffer _cl_int_pred_pos;
    cl::Buffer _cl_int_lambda;

    cl::Buffer _cl_int_bin_offset;
    cl::Buffer _cl_int_bin_counts;
    cl::Buffer _cl_int_bin_starts;

    cl::Buffer _cl_int_temp_0;
    cl::Buffer _cl_int_temp_1;

    cl::Buffer *_b_pos;
    cl::Buffer *_b_pred_pos;
    cl::Buffer *_b_vel_dp;
    cl::Buffer *_b_lambda;

    cl::Buffer *_b_temp_0;
    cl::Buffer *_b_temp_1;

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

    void swap(cl::Buffer *&a, cl::Buffer *&b) {
        cl::Buffer *temp = a;
        a = b;
        b = temp;
    }

    static cl::Program _cl_prog;
    static cl::Kernel _k_pred_pos;
    static cl::Kernel _k_count_bins;
    static cl::Kernel _k_prefix_sum;
    static cl::Kernel _k_reindex_particles;
    static cl::Kernel _k_calc_lam;
    static cl::Kernel _k_calc_dp;
    static cl::Kernel _k_update_pred_pos;
    static cl::Kernel _k_update_vel;
    static cl::Kernel _k_calc_vort;
    static cl::Kernel _k_apply_visc_vort;

    friend class FluidRenderer;
};

}

#endif //F2_DUP_FLUID_H
