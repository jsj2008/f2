#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_ENABLE_EXCEPTIONS

#include <iostream>
#include "core/app.h"

int main() {
    App app;
    if (!app.init())
        return 1;

    app.run();
    return 0;
}