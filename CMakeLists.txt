cmake_minimum_required(VERSION 3.10)
project(f2_dup)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(deps)
add_subdirectory(src)

file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})