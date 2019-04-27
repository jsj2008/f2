cd deps

if [ ! -d "cl/build" ]; then
  cd cl
  mkdir build && cd build
  cmake ..
  make
  cd ../..
fi

if [ ! -d "glfw/build" ]; then
  cd glfw
  mkdir build && cd build
  cmake .. -DBUILD_SHARED_LIBS=ON -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF -DGLFW_INSTALL=OFF
  make
  cd ../..
fi