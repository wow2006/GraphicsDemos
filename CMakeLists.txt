# ${CMAKE_SOURCE_DIR}/CMakeLists.txt
project(GraphicsDemo CXX)
cmake_minimum_required(VERSION 3.9)

option(ENABLE_SFML "" OFF)
option(ENABLE_GLFW "" OFF)
option(ENABLE_Qt5  "" OFF)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(options)

set(CONAN_EXTRA_REQUIRES
  ${CONAN_EXTRA_REQUIRES}
  glm/0.9.9.5@g-truc/stable
  sdl2/2.0.10@bincrafters/stable
  #sdl2_image/2.0.5@bincrafters/stable
  #Assimp/4.1.0@jacmoe/stable
)

include(conan)
run_conan()

#add_subdirectory(window)
#add_subdirectory(opengl)
#add_subdirectory(cameras)
#add_subdirectory(mimicOpenGL)
add_subdirectory(scene)

