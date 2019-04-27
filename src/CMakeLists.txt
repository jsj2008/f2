include_directories(${deps_INCLUDE_DIRS})

add_executable(f2_dup main.cpp core/app.cpp core/app.h pbf/fluid.cpp pbf/fluid.h core/view.cpp core/view.h core/input.cpp core/input.h pbf/fluid-renderer.cpp pbf/fluid-renderer.h gl/buffer.cpp gl/buffer.h gl/framebuffer.cpp gl/framebuffer.h gl/program.cpp gl/program.h gl/texture.cpp gl/texture.h core/sim.cpp core/sim.h scene/camera.cpp scene/camera.h scene/scene.cpp scene/scene.h scene/thing.cpp scene/thing.h scene/geometry/plane.cpp scene/geometry/plane.h)
target_link_libraries(f2_dup deps)
set(EXECUTABLE_OUTPUT_PATH ..)