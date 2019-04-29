//
// Created by Dillon Yao on 4/27/19.
//

#ifndef F2_DUP_ACCEL_H
#define F2_DUP_ACCEL_H

#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_ENABLE_EXCEPTIONS

#include <CL/cl2.hpp>

class CLContextManager {
public:
    static void init();

    static cl::Context &context() {
        return _context;
    }

    static cl::CommandQueue &queue() {
        return _queue;
    }

private:
    static cl::Context _context;
    static cl::CommandQueue _queue;

};

#endif //F2_DUP_ACCEL_H
