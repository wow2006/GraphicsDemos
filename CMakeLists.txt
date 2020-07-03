# ${CMAKE_SOURCE_DIR}/CMakeLists.txt
project(GraphicsDemo CXX)
cmake_minimum_required(VERSION 3.9)

option(ENABLE_SFML "Build SFML examples" OFF)
option(ENABLE_GLFW "Build GLFW examples" OFF)
option(ENABLE_Qt5  "Build Qt5 examples"  OFF)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(options)

add_subdirectory(window)
add_subdirectory(opengl)
add_subdirectory(cameras)
add_subdirectory(mimicOpenGL)
add_subdirectory(shaders)

