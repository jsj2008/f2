//
// Created by Dillon Yao on 4/25/19.
//

#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include "fluid.h"

namespace pbf {

cl::Program Fluid::_cl_prog;
cl::Kernel Fluid::_k_pred_pos;
cl::Kernel Fluid::_k_count_bins;
cl::Kernel Fluid::_k_prefix_sum;
cl::Kernel Fluid::_k_reindex_particles;
cl::Kernel Fluid::_k_calc_lam;
cl::Kernel Fluid::_k_calc_dp;
cl::Kernel Fluid::_k_update_pred_pos;
cl::Kernel Fluid::_k_update_vel;

void Fluid::init() {
    std::ifstream input("../kernels/pbf.cl");
    if (input.fail())
        throw std::runtime_error("Could not open file");

    std::ostringstream output;
    output << input.rdbuf();
    input.close();

    _cl_prog = cl::Program(CLContextManager::context(), output.str());

    try {
        _cl_prog.build();
    } catch (cl::Error &error) {
        if (error.err() == CL_BUILD_PROGRAM_FAILURE) {
            std::vector<cl::Device> devices;
            devices = CLContextManager::context().getInfo<CL_CONTEXT_DEVICES>();
            std::string built = _cl_prog.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]);
            std::cout << built << std::endl;
        }
        throw error;
    }

    try {
        _k_pred_pos = cl::Kernel(_cl_prog, "predict_position");
        _k_count_bins = cl::Kernel(_cl_prog, "count_bins");
        _k_prefix_sum = cl::Kernel(_cl_prog, "prefix_sum");
        _k_reindex_particles = cl::Kernel(_cl_prog, "reindex_particles");
        _k_update_vel = cl::Kernel(_cl_prog, "update_velocity");
        _k_calc_lam = cl::Kernel(_cl_prog, "calculate_lambda");
        _k_calc_dp = cl::Kernel(_cl_prog, "calculate_dp");
        _k_update_pred_pos = cl::Kernel(_cl_prog, "update_pred_position");
    } catch (cl::Error &error) {
        std::cout << error.err() << std::endl;
        exit(1);
    }
}

Fluid::Fluid() :
        _num_particles(0), _should_resize(true), _params_dirty(true) {
    init_domain();
}

Fluid::Fluid(FluidParams &params) :
        _num_particles(0), _should_resize(true), _params_dirty(true) {
    _params.fluid_params = params;
    init_domain();
}

void Fluid::spawn(glm::vec3 p) {
    _pos.push_back(p);
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
    if (_should_resize)
        init_cl_bufs();
    if (_params_dirty)
        bind_params();

    predict_pos();
    build_grid();
//
//    // calc_lam();
//    // calc_dp();
//    // update_pred_pos();
//
    update_vel();
    update_pos();
}

void Fluid::clear() {
    _pos.clear();
    _num_particles = 0;
    _should_resize = true;
}

FluidParams &Fluid::params() {
    return _params.fluid_params;
}

void Fluid::set_dirty() {
    _params_dirty = true;
}

void Fluid::init_domain() {
    FluidParams &fluid = _params.fluid_params;
    _params.dim_x = (unsigned) std::ceil((fluid.x_max - fluid.x_min) / fluid.grid_res);
    _params.dim_y = (unsigned) std::ceil((fluid.y_max - fluid.y_min) / fluid.grid_res);
    _params.dim_z = (unsigned) std::ceil((fluid.z_max - fluid.z_min) / fluid.grid_res);
}

void Fluid::init_cl_bufs() {
    size_t vector_buf_size = sizeof(cl_float3) * _num_particles;
    _cl_pos = cl::Buffer(CLContextManager::context(), CL_MEM_READ_WRITE, vector_buf_size);
    _cl_vel_dp = cl::Buffer(CLContextManager::context(), CL_MEM_READ_WRITE, vector_buf_size);
    _cl_pred_pos = cl::Buffer(CLContextManager::context(), CL_MEM_READ_WRITE, vector_buf_size);
    _cl_ind_pos = cl::Buffer(CLContextManager::context(), CL_MEM_READ_WRITE, vector_buf_size);
    _cl_ind_vel = cl::Buffer(CLContextManager::context(), CL_MEM_READ_WRITE, vector_buf_size);
    _cl_ind_pred_pos = cl::Buffer(CLContextManager::context(), CL_MEM_READ_WRITE, vector_buf_size);

    size_t fscalar_buf_size = sizeof(cl_float) * _num_particles;
    _cl_lambda = cl::Buffer(CLContextManager::context(), CL_MEM_READ_WRITE, fscalar_buf_size);

    size_t iscalar_buf_size = sizeof(cl_uint) * _num_particles;
    _cl_bin_of = cl::Buffer(CLContextManager::context(), CL_MEM_READ_WRITE, iscalar_buf_size);
    _cl_ind_bin_of = cl::Buffer(CLContextManager::context(), CL_MEM_READ_WRITE, iscalar_buf_size);
    _cl_bin_offset = cl::Buffer(CLContextManager::context(), CL_MEM_READ_WRITE, iscalar_buf_size);

    size_t num_bins = _params.dim_x *_params.dim_y * _params.dim_z;
    size_t bin_buf_size = sizeof(cl_uint) * num_bins;
    _cl_bin_starts = cl::Buffer(CLContextManager::context(), CL_MEM_READ_WRITE, bin_buf_size);
    _cl_bin_counts = cl::Buffer(CLContextManager::context(), CL_MEM_READ_WRITE, bin_buf_size);

    CLContextManager::queue().enqueueFillBuffer(_cl_vel_dp, 0, 0, vector_buf_size);
    CLContextManager::queue().enqueueFillBuffer(_cl_bin_counts, 0, 0, bin_buf_size);

    _k_pred_pos.setArg(1, _cl_pos);
    _k_pred_pos.setArg(2, _cl_vel_dp);
    _k_pred_pos.setArg(3, _cl_pred_pos);

    _k_count_bins.setArg(1, _cl_pred_pos);
    _k_count_bins.setArg(2, _cl_bin_of);
    _k_count_bins.setArg(3, _cl_bin_offset);
    _k_count_bins.setArg(4, _cl_bin_counts);

    _k_prefix_sum.setArg(0, _cl_bin_counts);
    _k_prefix_sum.setArg(1, _cl_bin_starts);

    _k_reindex_particles.setArg(0, _cl_bin_starts);
    _k_reindex_particles.setArg(1, _cl_bin_of);
    _k_reindex_particles.setArg(2, _cl_bin_offset);
    _k_reindex_particles.setArg(3, _cl_pos);
    _k_reindex_particles.setArg(4, _cl_vel_dp);
    _k_reindex_particles.setArg(5, _cl_pred_pos);
    _k_reindex_particles.setArg(6, _cl_ind_bin_of);
    _k_reindex_particles.setArg(7, _cl_ind_pos);
    _k_reindex_particles.setArg(8, _cl_ind_vel);
    _k_reindex_particles.setArg(9, _cl_ind_pred_pos);

//    _k_calc_lam.setArg(1, _cl_bin_starts);
//    _k_calc_lam.setArg(2, _cl_bin_counts);
//    _k_calc_lam.setArg(3, _cl_ind_pred_pos);
//    _k_calc_lam.setArg(4, _cl_lambda);
//
//    _k_calc_dp.setArg(1, _cl_bin_starts);
//    _k_calc_dp.setArg(2, _cl_bin_counts);
//    _k_calc_dp.setArg(3, _cl_ind_pred_pos);
//    _k_calc_dp.setArg(4, _cl_lambda);
//    _k_calc_dp.setArg(5, _cl_vel_dp);
//
//    _k_update_pred_pos.setArg(1, _cl_pred_pos);
//    _k_update_pred_pos.setArg(2, _cl_vel_dp);
//    _k_update_pred_pos.setArg(3, _cl_ind_pred_pos);

    _k_update_vel.setArg(1, _cl_ind_pos);
    _k_update_vel.setArg(2, _cl_ind_pred_pos);
    _k_update_vel.setArg(3, _cl_vel_dp);

    _should_resize = false;
}

void Fluid::bind_params() {
    _k_pred_pos.setArg(0, sizeof(DeviceParams), &_params);
    _k_count_bins.setArg(0, sizeof(DeviceParams), &_params);
    _k_calc_lam.setArg(0, sizeof(DeviceParams), &_params);
    _k_calc_dp.setArg(0, sizeof(DeviceParams), &_params);
    _k_update_pred_pos.setArg(0, sizeof(DeviceParams), &_params);
    _k_update_vel.setArg(0, sizeof(DeviceParams), &_params);

    _params_dirty = false;
}

void Fluid::predict_pos() {
    size_t size = 3 * sizeof(float) * _num_particles;
    CLContextManager::queue().enqueueWriteBuffer(_cl_pos, CL_FALSE, 0, size, _pos.data());
    CLContextManager::queue().enqueueNDRangeKernel(_k_pred_pos, cl::NullRange, cl::NDRange(_num_particles), cl::NullRange);
}

void Fluid::build_grid() {
    size_t num_bins = _params.dim_x *_params.dim_y * _params.dim_z;
    size_t bin_buf_size = sizeof(cl_uint) * num_bins;
    CLContextManager::queue().enqueueFillBuffer(_cl_bin_counts, 0, 0, bin_buf_size);
    CLContextManager::queue().enqueueNDRangeKernel(_k_count_bins, cl::NullRange, cl::NDRange(_num_particles), cl::NullRange);
    CLContextManager::queue().enqueueNDRangeKernel(_k_prefix_sum, cl::NullRange, cl::NDRange(num_bins), cl::NullRange);
    CLContextManager::queue().enqueueNDRangeKernel(_k_reindex_particles, cl::NullRange, cl::NDRange(_num_particles), cl::NullRange);

//    std::vector<unsigned> counts, starts, bin_of;
//    counts.resize(_params.dim_x *_params.dim_y * _params.dim_z);
//    starts.resize(_params.dim_x *_params.dim_y * _params.dim_z);
//    bin_of.resize(_num_particles);
//
//    CLContextManager::queue().enqueueReadBuffer(_cl_bin_of, CL_TRUE, 0, sizeof(cl_uint) * _num_particles, bin_of.data());
//    for (int i = 0; i < 100; ++i)
//        std::cout << bin_of[i] << ", ";
//    std::cout << std::endl;
//
//    CLContextManager::queue().enqueueReadBuffer(_cl_bin_counts, CL_TRUE, 0, bin_buf_size, counts.data());
//    CLContextManager::queue().enqueueReadBuffer(_cl_bin_starts, CL_TRUE, 0, bin_buf_size, starts.data());
//
//    for (int i = 0; i < _params.dim_x *_params.dim_y * _params.dim_z; ++i) {
//        std::cout << i << " > " << "start: " << starts[i] << ", " << "count: " << counts[i] << std::endl;
//    }
//
//    exit(1);
}

void Fluid::calc_lam() {
    CLContextManager::queue().enqueueNDRangeKernel(_k_calc_lam, cl::NullRange, cl::NDRange(_num_particles), cl::NullRange);
}

void Fluid::calc_dp() {
    CLContextManager::queue().enqueueNDRangeKernel(_k_calc_lam, cl::NullRange, cl::NDRange(_num_particles), cl::NullRange);
}

void Fluid::update_pred_pos() {
    CLContextManager::queue().enqueueNDRangeKernel(_k_calc_lam, cl::NullRange, cl::NDRange(_num_particles), cl::NullRange);
}

void Fluid::update_vel() {
    CLContextManager::queue().enqueueNDRangeKernel(_k_update_vel, cl::NullRange, cl::NDRange(_num_particles), cl::NullRange);
}

void Fluid::update_pos() {
    CLContextManager::queue().enqueueReadBuffer(_cl_ind_pred_pos, CL_TRUE, 0, 3 * sizeof(float) * _num_particles, _pos.data());
}

}