//
// Created by Dillon Yao on 4/27/19.
//

#include <iostream>
#include "accel.h"

cl::Context CLContextManager::_context;
cl::CommandQueue CLContextManager::_queue;

void CLContextManager::init() {
    _context = cl::Context(CL_DEVICE_TYPE_GPU);
    _queue = cl::CommandQueue(_context);
}