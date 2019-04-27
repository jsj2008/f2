project(deps)

add_subdirectory(glm)
add_subdirectory(glfw)

find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
include_directories(glad/include)

add_library(${PROJECT_NAME} STATIC glad/src/glad.c)
target_link_libraries(deps glfw glm ${OpenCL_LIBRARIES})

set(${PROJECT_NAME}_INCLUDE_DIRS
        ${OpenCL_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/glad/include
        ${PROJECT_SOURCE_DIR}/cl/build/include
        CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)